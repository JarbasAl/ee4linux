# Age of AI in the native build

[Age of AI](https://github.com/Grille/Empire-Earth-Age-of-AI) by Grille is a mod for the retail game. It gives the
computer players a new AI and fixes many of the game's own bugs. On Windows it is a DLL that patches the running exe:
it redirects calls, replaces vtable slots and hooks import slots, at addresses that its spec gives for each exe
build.

The native build has the game's source. So the port of Age of AI is C++ in that source, with explicit calls where
the mod has hooks. It does not patch addresses. The port's files are in `src/Empire Earth/AI Thread/AgeOfAI/`. Each
file names the upstream files it comes from, at commit `39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07`.

## The AI engine

Each computer player plays one of two AI engines: the classic AI, which is the game's own, or Age of AI. The
classic AI is the default. `EEComputerPlayer::Update` asks `AgeOfAI::EngineOf` for the player's engine, by player number.

The engine comes from `ageofai.ini` in the game folder, the same file and keys as the mod's. The game reads it once,
when the first computer player updates. `tools/port_linux_folder.py` copies `config/ageofai.ini` into the run folder
when the game folder has no `ageofai.ini`. That copy sets `enabled=0`. A missing file gives every key its default,
and the classic AI.

| Key | Effect in the port |
| --- | --- |
| `[general] enabled` | `1`: every computer player plays Age of AI. `0` (the default): every computer player plays the classic AI. |
| `[general] scenario_ai` | `1` (the default): an Age of AI player updates with the scenario flag clear. A scenario or campaign started from the menu sets that flag, and several classic AI routines then skip their planning. |

The game reads and checks every other key of the file, and the other keys do nothing yet. The module PRs give them
their effect. A key's text is a number, a name from the key's list, or a constant expression: numbers, `%` (`35%`
is 0.35), `+ - * / ( )`, `min`, `max`, `abs`, `round` and `floor`. A value that cannot be used keeps the key's
default. The game writes one line to the standard error for each such value, for each value outside the key's range,
and for each key or section that is not a setting:

```
ageofai: ageofai.ini [general] scenario_ai=maybe: no names in a setting: numbers, %, + - * / ( ), min, max, abs, round, floor - the default instead
ageofai: ageofai.ini [general] bogus: no such setting (a misspelt key?) - ignored
```

`tools/sim.py --ageofai FILE` gives every simulated game that file ([simulator](simulator.md)).

A computer player can also have a settings file of its own: the simulator's `--player-ini P=FILE` gives player P that
file in full, in place of `ageofai.ini`. The player's engine (`[general] enabled`), its `[ai_base]`, `[ai_easy]` and
`[ai_hard]` rows, and the shot rules of its units (`lead_targets`, `lob_each_shot`) then come from that file. Every
other player keeps `ageofai.ini`. The [AI benchmark](benchmark.md) uses this to make two named AIs play each other. A
profile that cannot be read is reported on the standard error, and the player is left on the defaults, whose
`enabled` is 0: the classic AI. Unlike `ageofai.ini`, a profile is named, so a missing one is a mistake and not a
setting.

An Age of AI player takes these keys into its own fields on its first update: `cost_percent` (the price it pays; `0%`, the default, leaves the game's own setting, whose cheat flag gives computer players a reduced price; `100%` is the real price, which makes the player weaker than the classic AI until the defence, army and budget modules are ported), `epoch_pacing` (every value but `classic` makes it ignore the humans' epoch and population, so it never waits for
them and its population limit is the game's), `epoch_min` and `epoch_max` (the epoch timer), `first_attack_min` and
`first_attack_max` (minutes until the first attack, a range the game draws from; a timer the game already drew is drawn
again inside the new range while the player has not attacked yet), `attack_interval_min` and
`attack_interval_max`, `attack_group_percent`, `attack_on_contact`, `attack_for_resources`, `attack_revenge`,
`last_stand_buildings` and `citizen_share` (the citizens' share of the population cap; the game's own when it is 0 or
1). `attention_activities` is not applied yet: its field is not mapped in this tree.

## What an Age of AI player knows

The classic AI sees every unit of the human everywhere: the game keeps, for each unit, how many units of each player
see it, and every unit of the human carries a floor of that count in the computer players' slots. With
`[ai_base] honest_vision` (on by default, per difficulty) an Age of AI player sees a unit only when its count is above
the least count among that owner's units, or when the unit is hitting one of the player's own units
(`reveal_attackers`, on by default). The player keeps a memory of what it saw:

- A building stays known until its place is looked at and it is gone. A building that leaves the game within six
  seconds of its last sighting died in sight, and is dropped at once.
- A soldier counts for 10 minutes at the place where it was seen. When a unit of the player or an ally comes within six
  tiles of that place and the soldier is not seen, the soldier is "somewhere": it still counts in the army the player
  knows of, but no longer at a place.
- A place is looked at when a unit of the player or an ally is within eight tiles. The map is cut into cells of 6x6 tiles.
- The memory holds 1024 records; a new sighting in a full memory replaces the oldest record.

The memory is `AgeOfAIIntel.cpp`, and it knows no game types. `AgeOfAIIntelScan.cpp` reads the game into a scan every
three game seconds, for each Age of AI player, from `EEComputerPlayer::Update`. A unit is known by its unit id, so the
memory does not depend on where the heap puts the unit. The report of a simulated game gives each Age of AI player an
`intel` object: records, buildings and soldiers remembered, units seen at the last scan, cells looked at, and the
soldiers in the army it knows of.

`AgeOfAIRoles.cpp` says what a unit class is (tower, wall, gate, siege weapon, ram, citizen, transport ship, fishing
boat, building, soldier) from its category, family and range, so a data mod's new tower or ram needs no name here.

Nothing reads the memory yet: the modules that judge an enemy's strength, the defence of a place or the danger of a
route ask it, and each arrives in its own change. A soldier is any unit of a military class: land, naval, air, heroes,
priests and transports; not siege weapons, citizens, rams, animals, shots or buildings. The memory counts siege weapons
with the soldiers, as upstream's `intel.c` does. Upstream's own test, an "army family" that includes naval buildings
and leaves out its separate ship and aircraft kinds, is not copied.

The rest of this page describes the tool that finds, for each of the mod's hooks, the function, member or class that
the hook changes here.

## The early game

An Age of AI player (with `[ai_base] early_towers` and `tower_bait`, both on) plays the playtester's opening. In the
first two epochs it queues two guard towers by the town centre, four tiles out: one towards the nearest seen enemy building (or the map's centre when none is seen), one on
the side where its citizens work. It adds up to two builders to every unfinished tower within 20 tiles of the town
centre, every 10 game seconds. Up to the Bronze Age, a citizen that has an enemy soldier within five tiles, or one
attacking it, and stands outside its towers' reach, walks to the nearest finished tower within 25 tiles and stands 1.5
tiles behind it from the enemy: the raiders that follow come under the tower. `AgeOfAIEarly.cpp` is the module and
`AgeOfAIEarlyGeometry.cpp` holds the places. The report of a simulated game gives the player `early_towers_queued`,
`early_builders` and `early_baits`. The first army (`early_army`), the militia (`early_militia`) and the stone of the
core towers in the worker plan wait for the files that hold them.

## The economy before farms

The game gives each resource gatherers by its demand, and its food demand is a phantom. An Age of AI player with
`[ai_base] copper_plan` `on` (the default) or `next_epoch` replaces the game's count of wanted gatherers, every five
game seconds, in the Prehistoric and Stone Ages. For each resource it asks what is still needed: the game's demand
(for stone, gold and iron only up to the reserve plus `mineral_worker_margin`), less the stock; the reserve is what the
Copper Age costs (with `next_epoch` only the next epoch's); the stone of the first two towers; and with `food_plan`
`count_trained` the food of the citizens still to come (50 each, at most 10 at a time, up to `min_workforce`) and a ceiling
on wood by the next three queued buildings. It gives the gatherers in proportion to the time each need takes to
gather: the need divided by the income per worker, measured every 30 game seconds. A few stay on food and wood, and with
`next_epoch` half the workers (up to `food_floor`) go to food. From the Copper Age on the game's targets stand. The
military's reserves, the fields' wood and the late rules wait for their files.

The game plans its attempt at the next epoch by a timer, often long after the player could pay. With the copper plan,
before the Copper Age, and in every epoch with `epoch_pacing` `rush`, an Age of AI player moves the attempt to now as
soon as its stock pays the next epoch (the game then researches the requirements it asks for first). A request for them
that is still pending after five minutes is asked again. The `follow` pacing (waiting for the humans) and the catch-up of
the `timer` pacing are not ported.
`AgeOfAIWorkers.cpp` holds the arithmetic and `AgeOfAIEconomy.cpp` the hooks.

The game's research manager schedules gathering upgrades rarely. With `[ai_base] gather_research` on (the default) an Age of
AI player with ten or more gatherers takes, every 15 game seconds, one available, unresearched tech from the manager's
candidates whose event raises the gathering rate of citizens (or of all units) and schedules it as the game does for its
own choice: it orders the building that researches it, if missing, and reserves the cost.

The game gives a computer player the line upgrades of an epoch (the techs of kind 7) the moment it enters the epoch, with no
cost, building or time. With `[ai_base] paid_upgrades` on (the default) an Age of AI player schedules them with its
research manager instead, so they are researched at their buildings and paid for, as a human player does. The army plan's
`upgrade` bit, which holds back lines the player does not use, and the upgrade reserve are not ported.

A building that leaves the map loses its research queue, and the research manager does not ask again. Every 30 game seconds an
Age of AI player schedules again each available, unresearched line upgrade that is in no research list and no building's
queue or production, when its stock pays the price. The upgrade that the game marked as not queueable is not handled.

## Mines and the worker rebalance

The game's assignment hands out idle citizens only and never takes a gatherer back, and its mine search takes the first
acceptable mine in the order of the resource centers. An Age of AI player (`mine_choice`, `mine_crowds`, `rebalance_workers`,
all on) searches differently for stone, gold and iron: the free mine nearest to one of its drop sites (Town Center,
Capitol, Settlement), with the game's checks (an active resource center, explored, a free gatherer slot, no enemy near,
the citizen's continent) and no known enemy on the straight way from that drop site; with nothing found, the game's own
search. A mine counts as full when the working gatherers or the AI's own citizens aiming at it reach the cap, since the
game counts only those that work. Every 10 seconds the citizens beyond the cap at a mine go to the nearest free mine of
the kind, 20 at most. After each assignment pass the rebalance moves up to 2 gatherers (4 with `citizen_plan`
`mine_slots`) from the resource with the most gatherers above its wanted count to a stone, gold or iron mine that is
short by 2 or more (wood too from the Copper Age on). `AgeOfAISites.cpp` holds it. With `[ai_base] local_mines` (off in
the shipped file) a mine whose drop site is more than 60 tiles from the citizen is another town's and is refused, and one
within 40 tiles comes before the rest: the game's assignment otherwise sends citizens across the map and back.
`AgeOfAISites.h` holds those two distances. For the ferry/island-expansion search with no citizen, `[patches] overseas_mines` (on by default) prefers a mine on another continent than the base so citizens are not ferried to a home-island mine. Not ported yet: the test of the mine's tile that upstream makes and `town_work` (off by default).

The game's assignment never moves a gatherer back to food either. With `food_plan` `back_to_food` (on in upstream's default, off in the shipped
`ageofai.ini` until the [benchmark](benchmark.md) rates it as the AI `aoai-foodpull`), after
each assignment pass and the rebalance, while food is 2 or more gatherers short of its wanted count, up to 4 gatherers
move to food. Each comes from the resource of wood, stone, gold and iron with the most gatherers above its wanted count
(2 or more), and goes to the free forage bush nearest to a drop site that takes food, within six times the player's
drop-off distance of the base. The bush passes the game's own checks: explored, a free gatherer slot, the gatherer's
land, no enemy near, no known danger. The sim report's `intel.food_pulled` counts the moves.

With `food_plan` `forage_near` (off in the shipped file) the game's own bush search takes the free bush nearest a drop
site that takes food, within two times the player's drop-off distance of the town centre for the pass that names a
citizen and six times for the far one, where the game takes the first bush in centre order. The reach of the two passes
is the game's own; only the choice of bush changes. `AgeOfAISites.h` holds the two multiples.

With `food_plan` `hunt_near` (off in the shipped file), before each assignment in the Stone Age idle citizens hunt
animals within `[ai_base] food_hunt_range` tiles (15 by default) of a Town Center, Capitol or Settlement. The animal is
the free one nearest to such a drop site: food, an animal, fewer than three gatherers, on the citizen's land, with no
enemy near and not a dangerous site. Upstream leaves tiger and wolf alone; this port does not resolve those type names,
so the danger and enemy-near checks are the guard.

With `food_plan` `wild_settlement` (off in the shipped file), before the Copper Age an Age of AI player may place a
Settlement by rich wild food far from its drop sites, at most twice in a game and no more than once every two minutes.
Bushes and animals within 30 tiles of the base count as wild food; a spot 10 or more tiles from every food drop site
with 1500 or more food within 6 tiles gets a Settlement placed 2 to 4 tiles from it, with no enemy near and no raiders
near. Upstream excludes tiger and wolf by type; this port does not resolve those type names.

The game puts no drop site by the forest its woodcutters work. With `[ai_base] wood_drop` on (the default), an Age of AI player
looks every game minute for woodcutters whose tree is 15 or more tiles from a drop site. When 8 or more of them work within 8
tiles of one tree, and no Settlement is going up and the player can build one, it lays a Settlement at the placeable spot
nearest the middle of their trees (2 to 12 tiles away), with no enemy building or soldiers near. The cost reserve that holds
the Settlement's price back from training waits for the training budget.

## What is dangerous for the citizens

The game's searches send citizens through the enemy's defences, to sites raiders stand at, and to places where citizens
were just killed. An Age of AI player (`avoid_towers_citizens`, `citizen_threats`, `threat_memory`, `hurt_seen_short`,
all on) refuses a mine, field or forest in these cases: a known enemy soldier it sees or remembers (for a minute) within
its reach, which is its range plus 3 tiles and at most 15 (27 with `citizen_threats`: artillery, ships); citizens hurt
within 6 tiles lately (1 for food and wood, 3 for stone, gold and iron; for two minutes, or twenty seconds when the raider
was in sight); and the straight way from the nearest drop site to the site crossing the range plus 2 tiles of a known enemy
defence (a building or fortress with a ground attack). Every 4 seconds, before the assignment, the player refreshes the
defences it has seen, compares each citizen's hit points with the last look to mark where one lost some, sends home the strays of
an attack (unassigned citizens marching to an enemy building), and sends gatherers and builders back to their drop site when their
site has raiders near, citizens hurt near it, or lies behind a known enemy defence; builders only at fields, since the wall
manager sends the builders of walls and towers again and again. The repair round's order for a building is refused while enemy soldiers it
knows stand within 8 tiles, and (`repair_one_walker`) while another of its citizens already holds a build goal on the building. With
`repair_safe_way` (off by default), it is also refused when known enemy soldiers stand on the way or known enemy towers cover the route.
`AgeOfAIDanger.cpp` holds it, and `SeenBy` (what the player knows of a unit, from the honest vision of the memory) is shared. With
`ship_reach_exact` (off by default), a naval or submerged raider counts only within its own range plus one tile, in a
round distance. With `raid_held` (ported, set to `0` in the shipped config to preserve default behaviour and benchmark-ledger comparability until re-rated), a raided or hurt site the town holds on the fight estimate is not treated as dangerous. Not ported yet: `behind_walls` (off by default),
the danger map the route searches read, and the shelter in a fortress (`bunker.c`).

An enemy army in the base is met as one force. Every game three seconds, with `[ai_base] muster` on (the default), an
Age of AI player counts the enemy soldiers within 22 tiles of its base against its own soldiers within 35 tiles and its towers
within 22. With 8 or more enemies and the home side under half of their strength, every own soldier there walks to a rally
point 18 tiles beyond the base, away from the enemy, and new soldiers join it. Once the soldiers at the rally point are 1.3
times the enemy in the base, each attacks the nearest enemy soldier. The muster ends when fewer than 3 enemies are left,
when soldiers and towers can hold the base again, or when the enemy follows to the rally point a third time; a new muster
waits 30 or 60 seconds. While the base is mustering, citizens in danger flee to the rally point instead of working through the overrun. The shelter in a fortress is not ported.

## Outdated units attack

A unit type's tech has a last epoch. The game offers the type up to that epoch, and a line upgrade turns the units of a
line into the next type. Units of a type whose last epoch is behind the player's epoch are units that no upgrade reaches
again. With `[ai_base] outdated_attack` on (the default), an Age of AI player looks every 5 game minutes at its land units
of class Land, citizens excluded. If 8 or more of them are outdated, are in none of its attack subgroups and are free for
the combat manager, they attack as one of the game's own attack groups. The group is the one that the scenario effect
"Create AI Military Group" makes. The target is the enemy building nearest the units' middle, walls excluded, and a
tower or a fortress only when no other building is left. The units fight to the end: the staging, the falling back and
the rally (`stage.c`, `cover.c`, `mass.c`) are not ported. The sim report counts the attacks, the units sent, the
launches the combat manager refused and those it took with no group made (`intel.outdated_*`).

The classic combat manager takes free soldiers into its own attack groups within a second, so in the sim games the rule
finds no free outdated units and the game does not change. It acts for a player whose army is larger than its groups
take, as on the late-game save the rule was written for.

## Counter-siege and the citizen militia

Upstream's default turns `[ai_base] citizen_militia` and `counter_siege` on. The shipped `ageofai.ini` turns both off
until the [benchmark](benchmark.md) rates them as the AI `aoai-militia`.

Every two game seconds an Age of AI player looks for two kinds of threat. With `citizen_militia` on, its citizens attack enemy citizens who build a defence within 12 tiles of its buildings or citizens, and, up
to the Bronze Age, enemy units that attack one of its units near its citizens. Only gathering or idle citizens within
10 tiles go, at most 3 per enemy and against at most 6 enemies, and only when their hit points times attack are 3 times
those of the enemies within 6 tiles. With `early_militia` (the default) they fight soldiers only next to their own
soldiers or under their own towers, and only where the fight estimate has them win without the loss of a citizen.

With `counter_siege` on, up to 5 soldiers within 20 tiles attack an enemy siege weapon or ram in reach of
its buildings (its range plus 4 tiles), or an enemy defence going up within 12 tiles: the builders first, spread over
them. They go only when they are at least twice the enemy soldiers within 8 tiles plus 3. Against a defence going up
with too few soldiers, the citizens go at the builders. Counter-fire runs just before counter-siege, as
upstream's `counter_siege` starts with it. The chat lines are not ported.

## The fight estimate

`AgeOfAIStats.cpp` estimates how long each of two sides needs to destroy the other, from the game's own unit data, as
Grille's `stats.c` does. Each unit deals the game's shot damage: the attack of its owner's stats, times the hit factor
and the weapon table, less the target's armour, rounded and at least 1. A shot takes the reload time plus 2 seconds.
A unit that cannot move never hits one that outranges it, and an outranged unit walks the difference under fire first.
Artillery cannot hit a unit that has closed inside its minimum range, and area damage counts up to 2 neighbours of the
target. The ratio of two sides is the square root of the time the enemy needs over the time the own side needs, so twice
the units of one type give 2. The citizen militia uses it (`early_militia`, `militia_estimate`, and `fight_estimate=1`),
and so does counter-fire. Muster and hold-the-town do not use it yet.

## Counter-fire

`AgeOfAICounterFire.cpp` ports Grille's `counterfire.c`, with `[ai_base] counter_fire` on. Upstream's default turns it on; the shipped `ageofai.ini`
turns it off until the [benchmark](benchmark.md) rates it as the AI `aoai-counterfire`. Every two game
seconds an Age of AI player looks for enemy shooters: enemies it sees that attack one of its units or buildings from beyond
that victim's reach (further off than its range plus 1 tile), with a range of 5 tiles at least. Shooters within 6 tiles
of each other are one target. Its soldiers within 24 tiles join, nearest first: those on the shooter's land (helicopters
anywhere), able to hurt it, not inside a building or transport, and not fighting an enemy within 4 tiles. A unit whose
class's AI script has no attack action of its own is left out even when its damage says it can hurt the target, as a
Prophet's is: the goal's start would leave the unit without a command, and the game's own step reads that command. They
join four at a time until the fight estimate against the enemy fighters within 10 tiles of the target gives 1.5, with
at most 60 on the target. If they cannot reach 1.5, nobody goes. Each soldier sent attacks one of the shooters and is
committed to it for 45 seconds. The sim report's `intel` object counts the targets attacked
(`counter_fire_sorties`), the soldiers sent (`counter_fire_sent`) and the targets left because the soldiers were too
weak (`counter_fire_short`).

Upstream calls counter-fire at the start of `counter_siege` (`defence.c`), and the port calls it just before counter-siege. The
skirmish spotter that upstream holds back (`skirmish.c`) and the staging rules that read the commitment (`stage.c`) are
not ported yet.

## Idle citizens home

The game sends a computer player's idle citizens to one base point for the whole empire. After a game is loaded, that
point is where the first unit stood, which can be a ship at sea or a soldier far away. With `[ai_base] citizens_home` on,
every 3 game seconds an Age of AI player checks this. A base point at sea, or with no finished town within 20 tiles,
moves to the town with the most finished buildings within 20 tiles. A citizen idle on two checks in a row and more than
15 tiles from its nearest finished town goes home. So does a citizen whose walk ends within 3 tiles of the base point
while a town is 10 or more tiles nearer. It goes to the nearest finished Town Center, Capitol or Settlement on its land
with no enemy soldiers in sight within 12 tiles, and stops 4 tiles from the town on its own side. With no such town, it
stays. `AgeOfAIHome.h` holds the rules, and the sim report's `intel` object counts the citizens sent home
(`home_sent`) and the base points that moved to a town (`home_base_moved`). Upstream's default turns `citizens_home` on.
The shipped `ageofai.ini` turns it off until the [benchmark](benchmark.md) rates it as the AI `aoai-citizenshome`. Not
ported: the citizens that a landing, an outpost or a new town holds (`landing.c`, `expand.c`, `towns.c`), and the
removal of unfinished buildings stacked on one tile.

## Extra towers

The game's own plan asks for few towers. With `[ai_base] defence_towers` on, and with `defence_layout` and
`defence_walls` on, every 20 game seconds an Age of AI player keeps a layout of tower slots. The layout has a front
`defence_distance` tiles (10) out from the base towards each of its two nearest enemies, by their buildings it has seen,
or towards the map's centre. On each front the slots are the middle, then 3 tiles to each side across the front, then 6
tiles to each side. A spot 3 tiles out from each town 12 or more tiles from the base comes before the outer slots. With
no tower in its build queue, the player queues the best tower of the line it can build at the first free slot, at most
once a minute, when its stone pays the tower with 100 left. A slot is free with no tower within 2 tiles and no tower
given there in the last 90 seconds, and a tile near it is used when the route from the base passes no known enemy
defence, no enemy raiders are in sight within 10 tiles, and the tower fits. `AgeOfAITowers.h` holds the slot order, and
the sim report's `intel` object counts the towers queued (`towers_queued`) and the looks that queued none
(`towers_skipped`). Upstream's default turns `defence_towers` on. The shipped `ageofai.ini` turns it off until the
[benchmark](benchmark.md) rates it as the AI `aoai-towers`. Not ported: the fronts' walls and gates, the site forts, the enclosure, the coast towers, and
the slots that the game's own tower requests take.

## Game fixes

`[general] lead_targets` (on in the shipped `ageofai.ini`, for every player and every AI engine) makes a unit or tower
that shoots a plain projectile (an arrow, a stone, a cannon ball) aim where a moving target will be when the shot comes
down. Before, the aim was the place the target stood when the shot was fired, and a slow projectile missed nearly
everything that walked across its line of fire. The flight time comes from the game's own arc solver. Missiles steer
by themselves and are left alone. In a multiplayer game every player needs the same setting.

`[general] lob_each_shot` (on) changes how archers, slingers, the pilum and the trebuchet fire over walls. The game lets
them shoot flat until one shot hits a wall, then lobs every later shot until they get a new order, wall gone or not. Now
a unit looks along the line to its target before each shot: a wall in the way gets the high arc, else the shot is flat.
After the game's own flat shot hit a wall, the unit lobs four more shots before it looks again. In a multiplayer game
every player needs the same setting.

## The hook-site index

```
python3 -I tools/function_index.py --ageofai DIR [--out FILE]
```

`DIR` is a checkout of Age of AI. The tool reads the mod's generated spec values in `dll/game/api/gen/` for the Art
of Conquest exe, the exe that this tree reconstructs. It then places each value in this tree:

| Value | Place |
| --- | --- |
| a function address | the function whose `// FUNCTION: EXE`, `// SYNTHETIC: EXE` or `// TEMPLATE: EXE` comment gives it, else a comment that names the address |
| a place in the code | the function that starts nearest below it |
| a data address | the comment that names it. Comments here give data addresses without the image base: the exe's `0x931710` is `0x531710`. |
| a member offset | the member whose `// +0x` comment covers the offset, in the spec's class or one of its bases |
| a vtable | the class that the spec gives the vtable to, or that the spec comment names |
| an import slot | the engine function it imports, the Win32 function of the port's layer, or the C library's |

The tool reads two sets of values. The first set is every value that a `patch_*` or `iat_hook` call of the mod uses,
generated wrappers included. Those are the hook sites. The second set is every value that the mod's code reads or
calls through the generated accessors. Those are the members and functions that a C++ module uses directly.

Each generated `patch_call` gives the function that its call site calls. The tool checks that the function in which
it places the site calls that function. A site that fails the check is listed.

`--out` writes a Markdown listing: the values the tool cannot place, the calls without a spec value, and one row per
value with its place here. Without `--out`, the tool prints the counts only.

Some values have no place, and that is expected:

- members of structures that the spec names itself (`ee_draw_record`, `ee_scen_effect`) and that are not classes
  here;
- functions of the C runtime that the exe links in, such as `operator new`;
- data that no comment here names yet.

A module PR maps the values it needs by hand, and adds the missing names to the tree.
