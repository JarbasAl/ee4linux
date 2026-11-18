# The AI benchmark

`tools/ai_bench.py` rates AI configurations by complete games against each other on the headless
[simulator](simulator.md). Each configuration is a named AI. The named AIs play every scenario of
`config/bench/scenarios.ini` against each other, to a win, and the tool fits a rating per AI on the Elo scale.

## Run it

```
python3 -I tools/ai_bench.py run --run GAME_FOLDER --out build/bench --seeds 1-10 --jobs 8
python3 -I tools/ai_bench.py table --out build/bench
```

`run` plays the games that are not yet in `build/bench/games.jsonl`. Stop it at any time: the next `run` continues
where it stopped. A run that the sim did not finish, such as a lobby that did not start, stays out of the ledger, and the
next `run` plays it again. Give `--ais` or `--scenarios` (names, comma apart) to play part of the set. A new AI then plays only
its own games against the others. `table` prints the ratings and the measures as markdown.

A game is in the ledger only when both players ran the engine their own settings file selects. A player whose file
cannot be read runs the classic AI, so the tool names that seat on the standard error and plays the game again on the
next `run`, rather than log a game of two AIs that was not one.

A game takes 1 to 2 wall minutes on an 8-core box. `--jobs` is the games played at once: a pair fills one slot per
seed, and the pairs its seeds cannot fill play side by side, each in a stage folder of its own under `DIR/stages`. With
ten seeds one pair fills the eight slots; with two seeds four pairs do. Ten AIs, ten scenarios and ten seeds, in both
seat orders, give 9000 games.

## Named AIs

An AI is a file in `config/bench/ais/`. It holds only the settings in which it differs from `config/ageofai.ini`, in
the same sections. The file name is the AI's name. The tool lays each file over `config/ageofai.ini` and gives the
result to its player in full (`ee-aoc --sim --player-ini`), so each AI keeps its own settings in a shared game.

| AI | What it is |
| --- | --- |
| `classic` | the game's own AI |
| `aoai` | Age of AI with the shipped settings |
| `aoai-honest` | Age of AI at the price a human pays (`cost_percent=100%`) |
| `aoai-sighted` | Age of AI with the vision the game gives a computer player (`honest_vision=0`) |
| `aoai-plainshot` | Age of AI without aiming ahead or lobbing shot by shot |
| `aoai-foodpull` | Age of AI with the food pull of `food.c` (`back_to_food`), as upstream's default has it |
| `aoai-counterfire` | Age of AI with the counter-fire of `counterfire.c` (`counter_fire=1`), as upstream's default has it |
| `aoai-militia` | Age of AI with the citizen militia and counter-siege of `defence.c`, as upstream's default has them |
| `aoai-citizenshome` | Age of AI with the idle citizens' way home of `econ.c` (`citizens_home=1`), as upstream's default has it |
| `aoai-towers` | Age of AI with the extra towers of the defence layout of `defence.c` (`defence_towers=1`), as upstream's default has it |

The game fixes of `[general]` act for the units of each player as that player's file sets them, so `classic` units
also aim ahead, as they do in a normal game with `ageofai.ini`.

## Scenarios

Every game has two computer players and the sim's idle human, and runs to a win or 8 game hours. The lobby values are
line names of the lobby's own lists.

| Scenario | What it changes |
| --- | --- |
| `standard` | nothing: a Small Continental map, "Standard - High" resources, 5 citizens, Prehistoric to Space Age |
| `poor` | "Standard - Low" resources, 3 citizens |
| `rich` | "Deathmatch" resources, 10 citizens |
| `tournament` | "Tournament - Low" resources |
| `middle` | start in the Middle Ages |
| `industrial` | start in the Industrial Age |
| `tiny` | a Tiny map |
| `plains` | a Medium Plains map |
| `highlands` | a Highlands map |
| `raids` | 20 stone throwers and 20 bowmen raid each base at 400 seconds and every 600 seconds after |

## The rating

Each pair of AIs plays each scenario on the same seeds twice, once with each AI as player 2. The slot advantage
therefore cancels. A game that reaches 8 game hours is a draw.

`table` also gives the games of each pair of AIs and the row AI's wins in them, which is the direct
"does this setting help against that AI" answer that a rating only summarises.

`table` also gives, per scenario, how many of its decided games the first seat (player 2) won. Both seat orders are
played, so a scenario whose games go to one seat whatever AI plays it is decided by the starting position and rates
nothing; the table marks one that is more than 15 points from half over ten or more decided games.

The rating is a Bradley-Terry fit of all games at once, on the Elo scale. 400 points mean odds of ten to one, and
`classic` is fixed at 1000. A draw counts half a win for each side. Each AI also has one virtual draw against
`classic`, so an AI that won or lost every game still has a finite rating. The interval is the 95 % range of 200 fits
over seeds drawn with replacement. The table also gives a rating per scenario, and for each AI and scenario the win
rate, the mean time to win and the mean epoch, army, population and value at the end.
