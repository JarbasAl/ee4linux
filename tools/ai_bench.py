"""The AI benchmark: named AIs play complete games against each other in a set of scenarios, and get a rating.

  python3 -I tools/ai_bench.py run --run GAME_FOLDER --out DIR [--ais a,b,c] [--scenarios s,t] [--seeds 1-10] [--jobs 8]
  python3 -I tools/ai_bench.py table --out DIR [--anchor classic]

An AI is a file in config/bench/ais/: the settings in which it differs from config/ageofai.ini, in the same sections.
The file name is the AI's name. The tool lays each file over config/ageofai.ini and gives the result to its player in
full (ee-aoc --sim --player-ini). A scenario is a section of config/bench/scenarios.ini whose "options" line holds the
ee-aoc --sim options of its games.

"run" plays every pair of AIs in every scenario on every seed, once with each AI in slot 2, through tools/sim.py. Each
game goes as one line into DIR/games.jsonl. A game already in that file is not played again, so a run can stop and
continue, and a new AI plays only its own games. A game that reaches its scenario's game time without a winner is a
draw. A run the sim did not finish (a lobby that did not start, a crash) does not go into the file, and the next "run"
plays it again. `--jobs` is the games played at once: a pair fills one slot per seed, and the pairs that the seeds of
one call cannot fill play side by side, each in a stage folder of its own under DIR/stages.

"table" fits the ratings and prints them as markdown: one rating per AI over all games, one per scenario, and the
measures of each AI's games (wins, time to win, epoch, army, population, value at the end). The rating is a
Bradley-Terry fit of all games at once on the Elo scale: 400 points are odds of ten to one, and the anchor AI is 1000.
A draw counts half a win for each side. Each AI also gets one virtual draw against the anchor, so an AI that won or
lost every game has a finite rating. The interval is the 2.5 and 97.5 percentile of 200 fits over seeds drawn with
replacement.
"""
import argparse
import collections
import concurrent.futures
import configparser
import fcntl
import itertools
import json
import math
import pathlib
import random
import re
import shlex
import shutil
import subprocess
import sys
import threading

ROOT = pathlib.Path(__file__).resolve().parent.parent
BASE_INI = ROOT / "config" / "ageofai.ini"
AIS_DIR = ROOT / "config" / "bench" / "ais"
SCENARIOS = ROOT / "config" / "bench" / "scenarios.ini"
GAME_SECONDS = 28800                  # 8 game hours: a game still running then is a draw
FINISHED = ("last-computer-standing", "game-end", "time-cap")   # the outcomes of a game the sim played to its end
SLOTS = (2, 3)                        # the computer players; player 1 is the sim's idle human
MEASURES = ["epoch", "army", "population", "value"]
BOOTSTRAP_FITS = 200


def merged_ini(base_text, overlay_text):
    """base_text with each key of overlay_text set to the overlay's value, in its section; new keys and sections added."""
    overlay = configparser.ConfigParser(interpolation=None, strict=False)
    overlay.optionxform = str
    overlay.read_string(overlay_text)
    wanted = {(s.lower(), k.lower()): (k, v) for s in overlay.sections() for k, v in overlay.items(s)}
    lines, section, placed = base_text.splitlines(), None, set()
    out = []

    def close_section():
        for (s, k), (key, value) in wanted.items():
            if s == section and (s, k) not in placed:
                out.append(f"{key}={value}")
                placed.add((s, k))

    for line in lines:
        header = re.match(r"\s*\[([^\]]+)\]", line)
        if header:
            close_section()
            section = header.group(1).strip().lower()
        else:
            key = re.match(r"\s*([^;#=\s][^=]*?)\s*=", line)
            if key and (section, key.group(1).lower()) in wanted:
                name, value = wanted[(section, key.group(1).lower())]
                out.append(f"{name}={value}")
                placed.add((section, key.group(1).lower()))
                continue
        out.append(line)
    close_section()
    for s in sorted({s for s, _ in wanted} - {s for s, _ in placed}):
        out.append(f"[{s}]")
        out += [f"{key}={value}" for (ws, _), (key, value) in wanted.items() if ws == s]
    return "\n".join(out) + "\n"


def load_ais(names=None):
    ais = {p.stem: p.read_text() for p in sorted(AIS_DIR.glob("*.ini"))}
    if names:
        missing = [n for n in names if n not in ais]
        if missing:
            sys.exit(f"ai_bench: no AI named {', '.join(missing)} in {AIS_DIR}")
        ais = {n: ais[n] for n in names}
    return ais


def load_scenarios(names=None):
    parser = configparser.ConfigParser(interpolation=None)
    parser.read(SCENARIOS)
    scenarios = {s: shlex.split(parser[s].get("options", "")) for s in parser.sections()}
    if names:
        missing = [n for n in names if n not in scenarios]
        if missing:
            sys.exit(f"ai_bench: no scenario named {', '.join(missing)} in {SCENARIOS}")
        scenarios = {n: scenarios[n] for n in names}
    return scenarios


def parse_seeds(text):
    seeds = []
    for part in text.split(","):
        first, _, last = part.partition("-")
        seeds += range(int(first), int(last or first) + 1)
    return seeds


def read_games(out):
    path = out / "games.jsonl"
    return [json.loads(line) for line in path.read_text().splitlines() if line.strip()] if path.exists() else []


def game_row(report, scenario, seed, seats):
    """One ledger line from a sim report: the AIs by slot, the winning AI (None: a draw), the end of each player.
    None when the sim did not finish the game."""
    if report.get("outcome") not in FINISHED:
        return None
    last = report["samples"][-1] if report.get("samples") else {"players": []}
    players = {p["index"]: p for p in last["players"]}
    winner = report.get("winner")
    return {
        "scenario": scenario, "seed": seed, "seats": {str(s): ai for s, ai in seats.items()},
        "winner": seats.get(winner) if winner is not None else None,
        "outcome": report.get("outcome"), "game_s": report.get("game_ms", 0) // 1000,
        "end": {ai: {m: players.get(s, {}).get(m) for m in MEASURES + ["engine"]} for s, ai in seats.items()},
    }


def engine_of(settings):
    """The engine a merged settings file selects: "ageofai" when its [general] enabled is on, else "classic"."""
    parser = configparser.ConfigParser(interpolation=None, strict=False)
    parser.optionxform = str
    parser.read_string(settings)
    value = parser.get("general", "enabled", fallback="").strip().lower()
    if value in ("1", "true", "yes", "on"):
        return "ageofai"
    if value in ("", "0", "false", "no", "off"):
        return "classic"
    sys.exit(f"ai_bench: [general] enabled={value} is not a setting: the engine of an AI cannot be told")


def wrong_engines(row, engines):
    """The seats of a finished game that ran an engine their own settings file does not select, by AI name.

    A game runs in a stage folder of the sim's own, so a player's settings file is named in full, and the report says
    which engine each player ran. A seat that ran another engine read no file, and a game of two AIs that were not
    those AIs must not reach the ledger.
    """
    return {ai: end.get("engine") for ai, end in row["end"].items() if end.get("engine") != engines[ai]}


class Budget:
    """The games in flight: a pair-call takes the slots of its games before it plays and gives them back at the end."""

    def __init__(self, games):
        self.mGames = games
        self.mUsed = 0
        self.mCondition = threading.Condition()

    def take(self, games):
        with self.mCondition:
            while self.mUsed + games > self.mGames:
                self.mCondition.wait()
            self.mUsed += games

    def give(self, games):
        with self.mCondition:
            self.mUsed -= games
            self.mCondition.notify_all()


def play_task(args, budget, options, profiles, engines, scenario, first, second, todo, node):
    """One pair of AIs in one scenario: its sim, then the ledger rows of the games it played and those it could not.

    Each call gets a stage folder of its own, so two pairs play side by side, and it holds its game slots until it
    has read its reports.
    """
    seats = {SLOTS[0]: first, SLOTS[1]: second}
    games = args.out / "runs" / f"{scenario}-{first}-{second}"
    stage = args.out / "stages" / str(node)
    at_once = min(len(todo), max(1, args.jobs))
    budget.take(at_once)
    try:
        cmd = [sys.executable, "-I", str(ROOT / "tools" / "sim.py"), "--target", args.target, "--run", str(args.run),
               "--seeds", ",".join(map(str, todo)), "--repeat", "1", "--jobs", str(at_once),
               "--wall-timeout", str(args.wall_timeout), "--ageofai", str(BASE_INI), "--out", str(games),
               "--stage", str(stage), "--",
               "--game-seconds", str(GAME_SECONDS), *options,
               *[x for s, ai in seats.items() for x in ("--player-ini", f"{s}={profiles / (ai + '.ini')}")]]
        subprocess.run(cmd, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        rows, problems = [], []
        for seed in todo:
            report = games / f"report-{seed}-1.json"
            if not report.exists():
                problems.append(f"{scenario} seed {seed} {first} against {second}: no report")
                continue
            result = json.loads(report.read_text())
            row = game_row(result, scenario, seed, seats)
            if row is None:
                problems.append(f"{scenario} seed {seed} {first} against {second}: not finished: {result.get('error')}")
                continue
            wrong = wrong_engines(row, engines)
            if wrong:
                ran = "; ".join(f"{ai} ran {engine}, its settings file selects {engines[ai]}"
                                for ai, engine in sorted(wrong.items()))
                problems.append(f"{scenario} seed {seed} {first} against {second}: {ran}: the file was not read")
                continue
            rows.append(row)
    finally:
        budget.give(at_once)
    shutil.rmtree(stage, ignore_errors=True)
    return rows, problems


def play(args):
    ais, scenarios, seeds = load_ais(args.ais), load_scenarios(args.scenarios), parse_seeds(args.seeds)
    args.out = args.out.resolve()
    args.run = args.run.resolve()
    args.out.mkdir(parents=True, exist_ok=True)
    profiles = args.out / "ais"
    profiles.mkdir(exist_ok=True)
    base = BASE_INI.read_text()
    engines = {}
    for name, overlay in ais.items():
        settings = merged_ini(base, overlay)
        (profiles / f"{name}.ini").write_text(settings)
        engines[name] = engine_of(settings)
    # one run at a time per ledger: two would play the same games and write the same line twice. The lock names the
    # folder, and lives as long as the run.
    lock = open(args.out / "games.lock", "w")
    try:
        fcntl.flock(lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
    except BlockingIOError:
        sys.exit(f"ai_bench: another run uses {args.out}; wait for it to end")
    done = {(g["scenario"], g["seed"], g["seats"]["2"], g["seats"]["3"]) for g in read_games(args.out)}
    tasks = []
    for scenario, options in scenarios.items():
        for a, b in itertools.combinations(ais, 2):
            for first, second in ((a, b), (b, a)):
                todo = [s for s in seeds if (scenario, s, first, second) not in done]
                if todo:
                    tasks.append((scenario, options, first, second, todo))
    planned = sum(len(task[4]) for task in tasks)
    played = 0
    if tasks:
        budget = Budget(max(1, args.jobs))
        with (args.out / "games.jsonl").open("a") as ledger, \
                concurrent.futures.ThreadPoolExecutor(max_workers=min(len(tasks), max(1, args.jobs))) as pool:
            futures = [pool.submit(play_task, args, budget, options, profiles, engines, scenario, first, second,
                                   todo, node)
                       for node, (scenario, options, first, second, todo) in enumerate(tasks)]
            for future in concurrent.futures.as_completed(futures):
                rows, problems = future.result()
                for problem in problems:
                    print(f"ai_bench: {problem}", file=sys.stderr)
                for row in rows:
                    ledger.write(json.dumps(row) + "\n")
                ledger.flush()
                played += len(rows)
    print(f"games: {planned} in, {played} out; ledger {args.out / 'games.jsonl'}")
    return 0 if played == planned else 1


def fit(games, anchor, iterations=2000):
    """Elo-scale ratings of a Bradley-Terry fit over games (each a (first, second, score of first)), anchor at 1000."""
    names = sorted({n for a, b, _ in games for n in (a, b)} | {anchor})
    wins = collections.Counter()
    meetings = collections.Counter()
    for a, b, score in games:
        wins[a] += score
        wins[b] += 1 - score
        meetings[frozenset((a, b))] += 1
    for n in names:                   # the prior: one virtual draw against the anchor
        if n != anchor:
            wins[n] += 0.5
            wins[anchor] += 0.5
            meetings[frozenset((n, anchor))] += 1
    strength = {n: 1.0 for n in names}
    for _ in range(iterations):
        new = {}
        for n in names:
            denominator = sum(count / (strength[n] + strength[m])
                              for pair, count in meetings.items() if n in pair
                              for m in pair - {n})
            new[n] = wins[n] / denominator if denominator else strength[n]
        scale = new[anchor]
        new = {n: s / scale for n, s in new.items()}
        settled = max(abs(math.log(new[n] / strength[n])) for n in names) < 1e-12
        strength = new
        if settled:
            break
    return {n: 1000 + 400 * math.log10(strength[n]) for n in names}


def scored(rows):
    return [(r["seats"]["2"], r["seats"]["3"],
             1.0 if r["winner"] == r["seats"]["2"] else 0.0 if r["winner"] == r["seats"]["3"] else 0.5) for r in rows]


def interval(rows, anchor, fits=BOOTSTRAP_FITS):
    by_seed = collections.defaultdict(list)
    for r in rows:
        by_seed[(r["scenario"], r["seed"])].append(r)
    keys = sorted(by_seed)
    rng = random.Random(1)
    samples = collections.defaultdict(list)
    for _ in range(fits):
        drawn = [r for _ in keys for r in by_seed[rng.choice(keys)]]
        for name, rating in fit(scored(drawn), anchor).items():
            samples[name].append(rating)
    return {n: (sorted(v)[int(0.025 * len(v))], sorted(v)[int(0.975 * len(v)) - 1]) for n, v in samples.items()}


def seat_shares(rows):
    """Per scenario, the decided games and the ones the first seat (player 2) won.

    Both seat orders are played, so a scenario whose games go to the first seat whatever AI plays it is decided by the
    starting position: its games carry no information about the AIs.
    """
    shares = {}
    for r in rows:
        counts = shares.setdefault(r["scenario"], [0, 0])
        if r["winner"] is None:
            continue
        counts[0] += 1
        counts[1] += r["winner"] == r["seats"]["2"]
    return shares


def table(args):
    rows = read_games(args.out)
    if not rows:
        sys.exit(f"ai_bench: no games in {args.out / 'games.jsonl'}")
    ratings, spans = fit(scored(rows), args.anchor), interval(rows, args.anchor)
    order = sorted(ratings, key=lambda n: -ratings[n])
    print(f"Games: {len(rows)}. Anchor: {args.anchor} at 1000.\n")
    print("| AI | Rating | 95 % interval | Games | Wins | Draws |")
    print("| --- | ---: | --- | ---: | ---: | ---: |")
    for n in order:
        mine = [r for r in rows if n in r["seats"].values()]
        wins = sum(r["winner"] == n for r in mine)
        draws = sum(r["winner"] is None for r in mine)
        print(f"| {n} | {ratings[n]:.0f} | {spans[n][0]:.0f} to {spans[n][1]:.0f} | {len(mine)} | {wins} | {draws} |")
    scenarios = sorted({r["scenario"] for r in rows})
    print("\n| AI | " + " | ".join(scenarios) + " |")
    print("| --- |" + " ---: |" * len(scenarios))
    per = {s: fit(scored([r for r in rows if r["scenario"] == s]), args.anchor) for s in scenarios}
    for n in order:
        print(f"| {n} | " + " | ".join(f"{per[s][n]:.0f}" if n in per[s] else "" for s in scenarios) + " |")
    shares = seat_shares(rows)
    print("\n| Scenario | Decided | First seat won | Share | |")
    print("| --- | ---: | ---: | ---: | --- |")
    for s in scenarios:
        decided, first = shares[s]
        share = first / decided if decided else 0.0
        seat = "the seat decides" if decided >= 10 and abs(share - 0.5) > 0.15 else ""
        print(f"| {s} | {decided} | {first} | {share:.0%} | {seat} |")
    print("\n| Scenario | AI | Games | Win rate | Time to win (h) | " + " | ".join(MEASURES) + " |")
    print("| --- | --- | ---: | ---: | ---: |" + " ---: |" * len(MEASURES))
    for s in scenarios:
        for n in order:
            mine = [r for r in rows if r["scenario"] == s and n in r["seats"].values()]
            if not mine:
                continue
            won = [r["game_s"] / 3600 for r in mine if r["winner"] == n]
            means = [sum(r["end"][n][m] or 0 for r in mine) / len(mine) for m in MEASURES]
            print(f"| {s} | {n} | {len(mine)} | {len(won) / len(mine):.2f} | "
                  f"{sum(won) / len(won) if won else float('nan'):.2f} | " + " | ".join(f"{v:.1f}" for v in means) + " |")
    return 0


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest="command", required=True)
    run = sub.add_parser("run", help="play the games not yet in the ledger")
    run.add_argument("--run", type=pathlib.Path, required=True, help="the game folder (tools/sim.py --run)")
    run.add_argument("--target", default="linux64")
    run.add_argument("--out", type=pathlib.Path, required=True)
    run.add_argument("--ais", type=lambda t: t.split(","), help="the AIs, comma apart (default all)")
    run.add_argument("--scenarios", type=lambda t: t.split(","), help="the scenarios, comma apart (default all)")
    run.add_argument("--seeds", default="1-10")
    run.add_argument("--jobs", type=int, default=8, help="the games played at once (default 8)")
    run.add_argument("--wall-timeout", type=int, default=1800)
    tab = sub.add_parser("table", help="the ratings and measures of the games in the ledger")
    tab.add_argument("--out", type=pathlib.Path, required=True)
    tab.add_argument("--anchor", default="classic")
    args = ap.parse_args()
    return play(args) if args.command == "run" else table(args)


if __name__ == "__main__":
    sys.exit(main())
