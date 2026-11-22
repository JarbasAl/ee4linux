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
| `--stage DIR` | the folder of a run's linked copy (default `build/sim/stage`). A stage of its own lets two sims of one tree run side by side; `<stage>.lock` keeps two off the same one. |
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
`buildings`, `soldiers`, `seen_now`, `cells_looked` and `army`. It also holds the counters of the early game (`early_*`),
of the outdated units' attack (`outdated_*`), of the food pull (`food_pulled`), of counter-fire (`counter_fire_*`), of
the mine cap (`mine_calls`, `mine_capped`, `mine_mines`, `mine_cap`), the army tether (`tether_calls`, `tether_skipped`),
the army's wait for a stronger moment (`mass_checks`, `mass_waits`, `mass_goes`, `mass_waited`, with `mass_waited`
adding five per 20-second extension as upstream counts it), the catch-up for other AIs' epochs (`catch_checks`,
`catch_calls`), the wall breach (`breach_calls`, `breach_crossed`, `breach_picked`, `breach_walls`), the build queue fix
(`build_attempts`, `build_fails`, `build_dropped`, `build_top_type`, `build_top_fails`), the economy recovery
(`recover_checks`, `recover_starts`, `recover_ends`, `recover_moves`, `recover_on`, `recover_had_economy`), the citizen militia
(`militia_orders`,
`militia_refused`, `militia_caught`), of counter-siege (`counter_siege_orders`,
`counter_siege_skipped`), of the idle citizens home (`home_sent`, `home_base_moved`), of the settlement upgrades
(`garrison_checks`, `garrison_orders`, `garrison_citizens`, `garrison_idle`, `garrison_wood`, `garrison_holds`), of
anti-air buildings (`anti_air_checks`, `anti_air_near`, `anti_air_any`, `anti_air_ahead`, `anti_air_queued`,
`anti_air_skipped`), of the answer-war stance rule (`answer_war_checks`, `answer_war_transitions`,
`answer_war_scheduled`, `answer_war_answers`, `answer_war_cancelled`), of the hospital fall-back bit
(`hospital_checks`, `hospitals`, `hospital_sent`, `hospital_holds`, `hospital_done`), of the tower-cover fall-back bit
(`cover_checks`, `cover_near`, `cover_fights`, `cover_sent`, `cover_holds`, `cover_back`), of the coast towers
(`coast_checks`, `coast_sites`, `coast_cells`, `coast_sea`, `coast_placed`), of the unseen-land scout (`scout_checks`,
`scout_towns`, `scout_foes`, `scout_cells`, `scout_picked`, `scout_sent`, `scout_back`), of the site forts
(`fort_checks`, `fort_sites`, `fort_picked`, `fort_towers`, `fort_busy`, `fort_wait`, `fort_skipped`, `fort_exposed`),
of the packed-building search (`tight_build_calls`, `tight_build_sites`), of the surrender rule (`surrender_checks`, `surrender_citizens`,
`surrender_no_citizen`, `surrender_can_train`, `surrender_at_war`, `surrender_resigned`), of the lost-fight rule
(`lost_checks`, `lost_skipped`, `lost_in_reach`, `lost_refreshes`, `lost_cells`, `lost_zones`), of the remembered foes
in that rule (`lost_mem_cells`, `lost_mem_foes`) and of the defence layout's towers (`towers_queued`,
`towers_skipped`).

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

One `tools/sim.py` runs at a time in a checkout: its runs use the stage folders under `build/sim/stage`, and a
second one refuses to start while the first holds them.

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
| `--army-air NAME:COUNT` | the same with the first flying class whose name has NAME in it. Use it with `--army` to put aircraft among land soldiers in a probe. |
| `--army-at S` | the game second the armies appear (default 120) |
| `--army-every S` | the armies appear again every S game seconds, a raid after raid, so an AI's spending on defence is tested against pressure that lasts |
| `--army-raid` | the armies appear eight tiles from the building each one attacks, on the side of its own base, instead of beside their own: a raid on every base, at the same distance from what it came for |
| `--army-spot X,Y` | the armies appear at this tile, even off the map: a test of units put off it |
| `--player-ini P=FILE` | player P reads its Age of AI settings from FILE in full, not from `ageofai.ini`: its engine, its AI rows and its units' shot rules ([Age of AI](ageofai.md)). Repeat it for more players. |
| `--lobby CONTROL=VALUE` | any lobby list, by line name or `#index`, for example `209=#2` |
| `--stance S,F,T,V` | at game second S, player F's stance toward player T becomes V: 0 ally, 1 enemy. Repeat it for more changes. This drives the diplomacy rule the way a player's declaration does ([Age of AI](ageofai.md)). |
| `--wall S[,N]` | at game second S each defender gets N wall segments (default 9) three tiles from the next attacker's base, laid across the direction to that attacker's enemy. The attacker sees them and its land routes cross them, which exercises the breach rule ([Age of AI](ageofai.md)). |
| `--starve P,S` | at game second S kill every citizen of player P, the way a lost town does. Repeat it for more times. This drives the surrender rule ([Age of AI](ageofai.md)). |
| `--raze P,S` | as `--starve`, and also kill P's Town Centers, Capitols and Settlements so it cannot train a citizen. Use `--keep-alive` with it so the game does not end when the last centre falls. |
| `--keep-alive P` | keep P in the game when its last Town Center falls, so the surrender rule's own timer can run. Repeat it for more players. |
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

## Diplomacy

Ordinary games set no stance change, so `answer_war` records checks but never a transition. To exercise it, make two
computer players allies and then declare war for one of them with
`--stance 1,2,3,0 --stance 1,3,2,0 --stance 60,2,3,1`. Player 3 then answers player 2's declaration, and its
`intel.answer_war_transitions` and `intel.answer_war_answers` become 1 where a run with `answer_war=0` stays at 0.
Use a 600-second game: the two scores and traces separate after the answer, not at the declaration.

## Walls

`--wall 60` gives each defender a line of nine wall segments three tiles from the next attacker's base, laid across the
direction to that attacker's enemy. The attacker sees the walls beside its own base, and its land attack routes cross
them. With `--preset fights --wall 60 --game-seconds 1200`, a run with `attack_breach=1` records crossings and chosen
segments in `intel.breach_crossed` and `intel.breach_picked`; the same run with `attack_breach=0` records none. The
segments and sightings are in `intel.breach_walls` and the route checks in `intel.breach_calls`.

## Surrender

`--starve P,S` kills player P's citizens at a game second, and `--raze P,S` also kills its Town Centers, Capitols and
Settlements so it cannot train more. An AI that retrains between strikes resets the surrender timer, so `--raze` is the
way to reach the 60-second no-centre rule. The game ends as soon as a player loses its last Town Center, so add
`--keep-alive P` to hold that player in until the surrender rule's own result ends it.

With `--lobby 183=#2 --map-size Tiny --game-seconds 300 --raze 2,30 --keep-alive 2`, a run whose `ageofai.ini` has
`surrender=1` records `intel.surrender_no_citizen=61`, `surrender_at_war=61` and `surrender_resigned=1` for player 2,
and the game ends at millisecond 91875. The same run with `surrender=0` records zero for every `surrender_*` counter
and plays to the time cap. The traces match until the resign. In an ordinary game the rule never fires, so the setting
changes nothing in the default benchmark.

The report of every computer player has `gatherers` and `wanted`: the citizens on food, wood, stone, gold and iron, and
the number the player's worker plan wants on each. A large gap, as with iron at 33 wanted and 11 working, means the
citizens cannot be placed at the mines.

Each computer player's report also has `drop_sites` (its finished drop-site buildings by the resource they take, index 0 to 6;
the Town Center and Settlement take type 1, everything) and `drop_distance_x10` (the gatherers' mean tile distance to the
nearest drop site that takes their resource, times 10; -1 for no gatherers or no site) and `techs` (the techs it has researched).

The report's `overlap` counts the land units, soldiers and citizens not at work and not held, whose circles (as wide as the game's
size for them) overlap: `pairs_mean` is the mean number of pairs overlapping by 0.1 tile or more over the samples, and
`deepest_x100` the deepest overlap in hundredths of a tile.
