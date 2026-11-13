"""Headless simulation runs: seeded computer-against-computer games of ee-aoc --sim (python3 -I tools/sim.py --help).

  python3 -I tools/sim.py [--target linux64] [--seeds 1-10] [--repeat 2] [--jobs 4] [--out DIR] -- [--sim options]

Each run plays one seed in a stage of its own: the run folder (tools/port_linux_folder.py) hard-linked file by file,
with the files and folders the game writes copied, and the build's images copied over it. The game therefore never
writes into the run folder, and several runs go at once. A run writes report-<seed>-<n>.json, trace-<seed>-<n>.txt
(one line per communications tick) and log-<seed>-<n>.txt (the game's standard error) into the output folder.

With --repeat 2 or more, each seed runs that many times and the tool compares the runs: the same seed must give the
same trace and the same report, apart from the wall-clock times. The tool prints one line per run, the comparison
for each seed (agree, DIFFER, or INCOMPLETE when a run wrote no report or trace), and "N in, N out" for the runs
started and the reports read. It exits 1 when a run fails or the runs of a seed do not agree.

The game runs headless: no DISPLAY or WAYLAND_DISPLAY, SDL's offscreen video and dummy audio, Mesa's software
renderer, a wall-clock limit with SIGKILL after it (the game does not stop on SIGTERM).
"""
import argparse
import concurrent.futures
import json
import os
import pathlib
import shutil
import subprocess
import sys
import time

ROOT = pathlib.Path(__file__).resolve().parent.parent
# what the game writes in its folder: copied into a stage, never hard-linked
WRITTEN_DIRS = ["Users", "Data/Saved Games"]
WRITTEN_SUFFIXES = {".reg", ".log", ".cfg"}
# report fields that depend on the wall clock
WALL_FIELDS = {"wall_seconds", "wall_seconds_game", "wall"}


def parse_seeds(text):
    seeds = []
    for part in text.split(","):
        first, _, last = part.partition("-")
        seeds += range(int(first), int(last or first) + 1)
    return seeds


def stage(run_dir, bin_dir, slot_dir):
    if slot_dir.exists():
        shutil.rmtree(slot_dir)
    slot_dir.mkdir(parents=True)
    written = [run_dir / d for d in WRITTEN_DIRS]
    for path in sorted(run_dir.rglob("*")):
        target = slot_dir / path.relative_to(run_dir)
        if path.is_dir():
            target.mkdir(parents=True, exist_ok=True)
        elif path.suffix.lower() in WRITTEN_SUFFIXES or any(d in path.parents for d in written):
            shutil.copy2(path, target)
        else:
            os.link(path, target)
    for image in bin_dir.iterdir():
        target = slot_dir / image.name
        if target.exists():
            target.unlink()
        shutil.copy2(image, target)


def run_one(args, seed, repeat, slot):
    slot_dir = args.stage / str(slot)
    stage(args.run, args.bin, slot_dir)
    name = f"{seed}-{repeat}"
    report = args.out / f"report-{name}.json"
    trace = args.out / f"trace-{name}.txt"
    log = args.out / f"log-{name}.txt"
    env = {k: v for k, v in os.environ.items() if k not in ("DISPLAY", "WAYLAND_DISPLAY")}
    env.update(SDL_VIDEODRIVER="offscreen", SDL_AUDIODRIVER="dummy", LIBGL_ALWAYS_SOFTWARE="1")
    command = ["timeout", "-k", "10", str(args.wall_timeout + 60), "./ee-aoc", "--sim", "--seed", str(seed),
               "--wall-timeout", str(args.wall_timeout), "--report", str(report), "--sync-trace", str(trace)]
    command += args.sim_options
    started = time.monotonic()
    with open(log, "w") as out:
        code = subprocess.run(command, cwd=slot_dir, env=env, stdout=out, stderr=subprocess.STDOUT).returncode
    wall = time.monotonic() - started
    result = {"seed": seed, "repeat": repeat, "code": code, "wall": wall, "report": None, "trace": trace}
    if report.exists():
        result["report"] = json.loads(report.read_text())
    return result


def without_wall(value):
    if isinstance(value, dict):
        return {k: without_wall(v) for k, v in value.items() if k not in WALL_FIELDS}
    if isinstance(value, list):
        return [without_wall(v) for v in value]
    return value


def compare(same):
    """The runs of one seed: "agree" when every run wrote a report and a trace and all match apart from the wall
    clock, "INCOMPLETE" when a run wrote no report or no trace, else "DIFFER"; then the distinct traces and reports,
    and the trace's line count when they agree"""
    traces = {r["trace"].read_text() if r["trace"].exists() else None for r in same}
    reports = {json.dumps(without_wall(r["report"]), sort_keys=True) for r in same}
    if None in traces or any(r["report"] is None for r in same):
        return "INCOMPLETE", len(traces), len(reports), 0
    if len(traces) > 1 or len(reports) > 1:
        return "DIFFER", len(traces), len(reports), 0
    return "agree", 1, 1, len(next(iter(traces)).splitlines())


def summary(result):
    report = result["report"]
    if report is None:
        return f"seed {result['seed']} run {result['repeat']}: exit {result['code']}, no report"
    epochs = " ".join(f"p{p['index']}:e{p['epoch']}" for p in report["players"])
    return (f"seed {result['seed']} run {result['repeat']}: exit {result['code']}, {report['outcome']}, winner "
            f"{report['winner']}, game {report['game_ms'] // 1000} s, PT {report['pt']}, wall {result['wall']:.0f} s,"
            f" {epochs}")


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("--target", default="linux64", choices=["linux", "linux64"])
    ap.add_argument("--run", type=pathlib.Path, help="the run folder (default build/port-<target>/run)")
    ap.add_argument("--bin", type=pathlib.Path, help="the images (default build/port-<target>/bin)")
    ap.add_argument("--seeds", default="2", help="seeds: 3, 1-10 or 1,4,7 (default 2)")
    ap.add_argument("--repeat", type=int, default=1, help="runs per seed, compared when 2 or more (default 1)")
    ap.add_argument("--jobs", type=int, default=1, help="runs at once (default 1)")
    ap.add_argument("--wall-timeout", type=int, default=3600, help="wall seconds per run (default 3600)")
    ap.add_argument("--out", type=pathlib.Path, help="the output folder (default build/sim/<time>)")
    ap.add_argument("sim_options", nargs="*", help="after --: options of ee-aoc --sim (--ai, --game-seconds, ...)")
    args = ap.parse_args()

    build = ROOT / "build" / f"port-{args.target}"
    args.run = (args.run or build / "run").resolve()
    args.bin = (args.bin or build / "bin").resolve()
    args.out = (args.out or ROOT / "build" / "sim" / time.strftime("%Y%m%d-%H%M%S")).resolve()
    args.stage = ROOT / "build" / "sim" / "stage"
    if not (args.bin / "ee-aoc").exists():
        sys.exit(f"sim: no {args.bin / 'ee-aoc'}; run tools/build_port.py --target {args.target} --link first")
    if not args.run.is_dir():
        sys.exit(f"sim: no run folder {args.run}; run tools/port_linux_folder.py --target {args.target} first")
    args.out.mkdir(parents=True, exist_ok=True)

    runs = [(seed, n) for seed in parse_seeds(args.seeds) for n in range(1, args.repeat + 1)]
    results = []
    with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as pool:
        slots = list(range(args.jobs))
        pending = {}
        for seed, n in runs:
            if len(pending) == args.jobs:
                done, _ = concurrent.futures.wait(pending, return_when=concurrent.futures.FIRST_COMPLETED)
                for future in done:
                    slots.append(pending.pop(future))
                    results.append(future.result())
                    print(summary(results[-1]), flush=True)
            slot = slots.pop(0)
            pending[pool.submit(run_one, args, seed, n, slot)] = slot
        for future in concurrent.futures.as_completed(pending):
            results.append(future.result())
            print(summary(results[-1]), flush=True)

    failed = [r for r in results if r["code"] != 0 or r["report"] is None]
    differ = []
    if args.repeat > 1:
        for seed in parse_seeds(args.seeds):
            same = [r for r in results if r["seed"] == seed]
            status, traces, reports, lines = compare(same)
            print(f"seed {seed}: {len(same)} runs {status}"
                  f" (traces {traces} distinct, {lines} lines; reports {reports} distinct)")
            if status != "agree":
                differ.append(seed)
    reports_read = sum(r["report"] is not None for r in results)
    print(f"runs: {len(runs)} in, {reports_read} out; {len(failed)} failed; output {args.out}")
    return 1 if failed or differ or reports_read != len(runs) else 0


if __name__ == "__main__":
    sys.exit(main())
