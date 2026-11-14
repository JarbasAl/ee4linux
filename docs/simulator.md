# The headless simulator

`ee-aoc --sim` plays a seeded random-map game between computer players, with no window, no sound and no wall clock.
It runs the game's own code, so a change to the game or to the AI shows in the result. A 30-minute game on a small
map takes about 10 seconds. The same seed gives the same game, tick for tick, so two runs can be compared.

The simulator runs on the native builds only. It needs a run folder from `tools/port_linux_folder.py` ([run](run.md)).
The game must find the base Empire Earth folder from the run folder's settings. A `C:` path there resolves through
`WINEPREFIX`, so set it when the prefix is not `~/.wine`. Without the base game, every run stops at the start with
the verification `theResult == keseOK` in `EEInitializeGameObjects`, because the mouse pointers are missing.

## Running it

`tools/sim.py` runs one or more seeds, each in a stage folder of its own, and compares repeated runs:

```
python3 -I tools/sim.py --seeds 1-10 --jobs 4 -- --game-seconds 1800
python3 -I tools/sim.py --seeds 2 --repeat 2 -- --game-seconds 3600
```

The options before `--` belong to the tool, and the options after it go to `ee-aoc --sim`.

| Tool option | Effect |
| --- | --- |
| `--target linux64` | the build to use, `linux` or `linux64` (default `linux64`) |
| `--run DIR`, `--bin DIR` | the run folder and the images (default `build/port-<target>/run` and `bin`) |
| `--seeds LIST` | seeds as `3`, `1-10` or `1,4,7` (default 2) |
| `--repeat N` | runs per seed. With 2 or more, the tool compares the runs of each seed. |
| `--jobs N` | runs at once (default 1) |
| `--wall-timeout S` | wall seconds per run (default 3600). The tool kills a run 60 seconds after that. |
| `--out DIR` | the output folder (default `build/sim/<time>`) |
| `--ageofai FILE` | the `ageofai.ini` of every run, which selects the AI engine ([Age of AI](ageofai.md)). Without it, a run uses the run folder's file. |
| `--preset fights` | three computer players on a Tiny map, started in a later age (lobby control 183 at line 8) with many resources (control 181 at line 4), for 3600 game seconds. The options after `--` come last and win. |

The default game (a Small map, two computer players, the Prehistoric start) has almost no fights in an hour: the
computer players average 1.5 army units. With `--preset fights` they average about 10, and several lose most of their
population, so changes to combat, aiming and movement show in the scorecard.

After the runs, the tool prints a scorecard per AI engine: for each engine, the computer players, the games they
won, their defeats, and the mean epoch, population, army, buildings and value at the end. It counts each seed once,
by its first run that played. Each player in a report has an `engine` field: `classic`, `ageofai` or `human`. The game option
`--classic-players 3,4` keeps those players on the classic AI whatever `ageofai.ini` says, so one game holds both
engines and the scorecard compares them. A scorecard of one engine against itself shows the noise between the
players' starting positions.

The report of an Age of AI player has an `intel` object, its memory of its enemies ([Age of AI](ageofai.md)): `records`,
`buildings`, `soldiers`, `seen_now`, `cells_looked` and `army`.

A stage is the run folder linked file by file. The tool copies the files the game writes (settings, logs, saves)
and the build's images into it. The run folder therefore stays unchanged, and runs do not share files.

Each run writes three files to the output folder:

- `report-<seed>-<run>.json`: the setup, the outcome, each player's state at the end, and samples during the game;
- `trace-<seed>-<run>.txt`: one line per communications tick;
- `log-<seed>-<run>.txt`: the game's standard error, which holds the AI's own log.

The tool prints a line per run and, with `--repeat`, a line per seed. That line says `agree` when every run wrote a
report and a trace and all of them match apart from the wall-clock times. It says `DIFFER` when they do not match, and
`INCOMPLETE` when a run wrote no report or no trace. The last line gives the runs started and the reports read. The
tool exits 1 when a run fails, a seed does not agree, or a report is missing.

## The game options

| `ee-aoc --sim` option | Effect |
| --- | --- |
| `--seed N` | the start seed (default 2) |
| `--ai N` | computer players, 1 to 7 (default 2) |
| `--map-size NAME` | the lobby's map size line (default `Small`) |
| `--map-type NAME` | the lobby's map type line (default the first, `Continental`) |
| `--difficulty NAME` | the lobby's difficulty line (default `Hard`) |
| `--classic-players LIST` | player numbers, comma apart, that play the classic AI whatever `ageofai.ini` says |
| `--army NAME:COUNT` | at `--army-at`, each computer player gets COUNT units of the first land class whose name has NAME in it (without case), six tiles from its buildings towards the next computer player, each sent to attack that player's nearest building. Repeat it for more kinds. Every player gets the same units, so any map gives a fair fight. |
| `--army-at S` | the game second the armies appear (default 120) |
| `--army-spot X,Y` | the armies appear at this tile, even off the map: a test of units put off it |
| `--lobby CONTROL=VALUE` | any lobby list, by line name or `#index`, for example `209=#2` |
| `--game-seconds S` | the game time at which the run stops (default 2700) |
| `--sample-seconds S` | the game time between report samples (default 30) |
| `--wall-timeout S` | the wall time after which the run gives up (default 3600) |
| `--pt-rate N` | physics ticks per second (default 40) |
| `--human-defeat` | lets the idle human player lose. By default the human stays in the game. |
| `--report FILE` | the JSON report (default standard output) |
| `--sync-trace FILE` | the trace file |
| `--autosave-ms N` | the game's autosave period in game milliseconds (default 0: none) |
| `--port-offset N` | moves the game's TCP ports 33334 and 33335 by N (default from the process id) |
| `--quiet` | no progress lines on standard error |

The game also has a human player in slot 1, because the lobby needs one. That player does nothing. Its town stays in
the game, so a run ends at `--game-seconds`, at the game's own end, or when one computer player is left.

## The trace

A trace line holds six fields:

```
<physics tick> <game ms> <random seed> <stock hash> <units> <unit hash>
```

The stock hash is FNV-1a over every player's resources. The unit hash adds up an FNV-1a hash of each unit's owner,
id, type, tile and hit points, so the order of the units does not change it. Two runs that differ at some tick differ in that line, so the first differing line shows
where two games part.

## How it works

The simulator changes the game at a few places, each behind the flag `gPlatSimMode` (`include/platform/PlatSim.h`):

- `WinMain` skips the single-instance check, the splash screen and the movies, and starts the driver thread
  (`src/Port/EESim.cpp`).
- The driver clicks through the menus in-process: main menu, single player, random map, the lobby, Start.
- `EEMStartLoading::Execute` takes the simulator's seed instead of the lobby's.
- The graphics thread draws nothing.
- The physics thread sends the next communications tick itself as soon as it has run the last one
  (`NESynchronousServer::SimCommunicationsTick`). The send thread sends none while the game runs. The ticks per
  communications tick are fixed, so every run cuts the game into the same ticks.
- After each communications tick, the driver writes the trace line and takes the samples.
- The idle human is never removed from the game or defeated, unless `--human-defeat` is given.
- The TCP ports move by `--port-offset`, so several runs share one machine.

The classic AI times its epoch advances from the game's AI behaviour data, and it stays at most one epoch ahead of
the most advanced human. With the idle human in the first epoch, both computer players of seeds 2 and 3 on Hard
reached the second epoch between 47 and 53 game minutes. A shorter run ends in the first epoch.

## Credits

The simulator is ported from [Protryon/empire_earth_next](https://github.com/Protryon/empire_earth_next)
(`src/Empire Earth/Empire Earth/EESim.cpp`, `include/platform/PlatSim.h` and `tools/sim.py` at `ddb437ab`), without
its AI-engine, rules and settings options. The trace follows the sync trace of Grille's
[Age of AI](https://github.com/Grille/Empire-Earth-Age-of-AI) (`dll/test/trace.c` at `39d8ee42`).

## Fights and projectiles

The default game has almost no fights in an hour. `--army thrower:20 --army bow:20 --army-at 60` puts 20 ranged
units of each kind on every computer player at game second 60, and they fight at once. Their armies fall from about 40
to 7 within ten game minutes. The report has a `projectiles` object: the shots of plain projectiles (`fired`), how many
the aim-ahead fix led (`led`), and the projectiles that came down on a unit (`hit_unit`) and on the ground (`hit_ground`).
A change to aiming or combat shows as a change of the share `hit_unit / (hit_unit + hit_ground)`.
