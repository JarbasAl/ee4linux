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
`early_builders` and `early_baits`. With `[ai_base] early_army` on (off in the shipped file), from the Stone Age to the
Bronze Age the player asks for one Archery Range a minute when none stands or goes up and none is queued, so the
food-free ranged soldiers have a place to train. The army's training budget, the militia (`early_militia`) and the
stone of the core towers in the worker plan wait for the files that hold them.

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
that is still pending after five minutes is asked again. With `epoch_pacing` `follow`, while no human is ahead the attempt is kept a
minute off and no resources are held back for the next epoch, though its requirements are still asked for; once a
human is ahead it moves up as soon as the epoch can be paid. With no human in the game the timer alone counts. With
`epoch_pacing` `timer` an Age of AI player ignores the game's catch-up when a human reaches an epoch, so the timer alone
counts; every other pacing keeps the catch-up. `AgeOfAIWorkers.cpp` holds the arithmetic and `AgeOfAIEconomy.cpp` the
hooks.

The game's research manager schedules gathering upgrades rarely. With `[ai_base] gather_research` on (the default) an Age of
AI player with ten or more gatherers takes, every 15 game seconds, one available, unresearched tech from the manager's
candidates whose event raises the gathering rate of citizens (or of all units) and schedules it as the game does for its
own choice: it orders the building that researches it, if missing, and reserves the cost.

The game gives a computer player the line upgrades of an epoch (the techs of kind 7) the moment it enters the epoch, with no
cost, building or time. With `[ai_base] paid_upgrades` on (the default) an Age of AI player schedules them with its
research manager instead, so they are researched at their buildings and paid for, as a human player does. With the army
plan's `used_lines` bit a line the player neither has nor is asked for waits until it comes into use; the upgrade reserve
is not ported.

A building that leaves the map loses its research queue, and the research manager does not ask again. Every 30 game seconds an
Age of AI player schedules again each available, unresearched line upgrade that is in no research list and no building's
queue or production, when its stock pays the price. With `used_lines` a line still out of use stays back. The upgrade that
the game marked as not queueable is not handled.

## Mines and the worker rebalance

The game's assignment hands out idle citizens only and never takes a gatherer back, and its mine search takes the first
acceptable mine in the order of the resource centers. An Age of AI player (`mine_choice`, `mine_crowds`, `rebalance_workers`,
all on) searches differently for stone, gold and iron: the free mine nearest to one of its drop sites (Town Center,
Capitol, Settlement), with the game's checks (an active resource center, explored, a free gatherer slot, no enemy near,
the citizen's continent) and no known enemy on the straight way from that drop site; with nothing found, the game's own
search. A mine counts as full when the working gatherers or the AI's own citizens aiming at it reach the cap, since the
game counts only those that work. The cap is the game table's gatherer limit, the same value upstream reads; it does not
follow a civilization power or scenario effect that changes the player's own mining limit. Every 10 seconds the citizens
beyond the cap at a mine go to the nearest free mine of the kind, 20 at most. After each assignment pass the rebalance
moves up to 2 gatherers (4 with `citizen_plan`
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
enemy near and not a dangerous site. As upstream, a tiger or a wolf is left alone; the port resolves it by its name string.
With `food_plan` `back_to_food` also on, before farms, a gatherer pulled back to food goes to a free animal first and
takes a forage bush only when none is in reach.

With `food_plan` `wild_settlement` (off in the shipped file), before the Copper Age an Age of AI player may place a
Settlement by rich wild food far from its drop sites, at most twice in a game and no more than once every two minutes.
Bushes and animals within 30 tiles of the base count as wild food; a spot 10 or more tiles from every food drop site
with 1500 or more food within 6 tiles gets a Settlement placed 2 to 4 tiles from it, with no enemy near and no raiders
near. As upstream, a tiger or a wolf is excluded; the port resolves it by its name string.

The game puts no drop site by the forest its woodcutters work. With `[ai_base] wood_drop` on (the default), an Age of AI player
looks every game minute for woodcutters whose tree is 15 or more tiles from a drop site. When 8 or more of them work within 8
tiles of one tree, and no Settlement is going up and the player can build one, it lays a Settlement at the placeable spot
nearest the middle of their trees (2 to 12 tiles away), with no enemy building or soldiers near. The cost reserve that holds
the Settlement's price back from training waits for the training budget.

With `[ai_base] mine_fill` `mine_settlement` or `citizen_plan` `mine_settlement` (both off in the shipped file), an Age of
AI player may place a Settlement by a far mine. From the Copper Age on, with 30 or more gatherers, every two game minutes
it queues a Settlement 3 to 6 tiles from the free far mine on the home continent: a stone, gold or iron mine more than 12
tiles from every drop site, explored, active, with no enemy near and not a dangerous site. At most six are queued in a
game, and none while another Settlement is queued or going up; the player must be able to build and pay for one. Only the
far mine is read for this Settlement pass; the fill pass reads the mine-slot tally described below.

With `[ai_base] mine_fill` `fill_slots` (off in the shipped file), an Age of AI player sends spare woodcutters and farmers to
metal mines that have a free slot near a drop site. From the Copper Age on, after three game minutes and with 30 or more
gatherers, it counts the stone, gold and iron mines within 12 tiles of a drop site. Each mine adds the game's mine-capacity
value to the tally of its resource. While the population cap is under 120, only mines on the home continent count. A metal
whose stock is more than 1500 above the cost held back for the next epoch gets no workers. The player takes workers first
from wood above a floor of four plus a share held for the wood demand, then from food above three fifths of its target.
`mine_fill` `mines_first` also takes from wood above a fifth and food above a third of all gatherers. The pass changes the
wanted-gatherer targets, and the game's own assignment moves the citizens.

With `[ai_base] late_targets` `food_to_metals` (off in the shipped file), from the Copper Age on an Age of AI player trims
the farmers' wanted count while food piles up. Every 10 game seconds, with at least six gatherers wanted in all, the count
falls by 2 when the food stock is more than 4000 above the cost held back for the next epoch, and rises by 6 when it is
under 2000. Under 500 the count returns to the fields plus 2. No more than half the fields stand idle. The wanted workers
taken from food go to stone, gold and iron by their demand beyond stock, with a small share for each metal, and the game's
own assignment moves the citizens. The other bits of `late_targets` are not ported: the wood gap and the army's refusal
pull need the unported training budget.

## Granaries

`[ai_base] more_granaries` is on in the shipped file and is now active. A granary feeds eight fields. Once a game minute,
from the Copper Age on, an Age of AI player asks the build manager for a granary when no farm waits to be built or worked
and no granary is going up. Without `farm_first` it asks when the food target exceeds the food workers by 2 with no
granary, or by 6 with at least one. With `farm_first`, also on in the shipped file, it asks for the first two granaries
whatever the food target. After that it asks for one per eight food workers wanted, plus one, up to eight granaries. The
build manager's queued check stops repeated requests. Upstream also tries a granary spot of its own 50 seconds after a
request that brought nothing; that fallback needs the port's missing placement and raider searches, so the ordinary
request is repeated instead.

The shipped 1 is a deliberate behaviour change: the earlier port parsed the setting but never asked for another granary.
Setting 0 restores that. The 300-second benchmark runs one check and makes no request, so its trace stays the same and its
score stays at 4020. In a 30-minute Tiny map game with three Age of AI players, gate on logged 33 checks and 10 requests
and scored 6154.7; gate off logged none and scored 6779.3. The traces differ. The sim report counts the passes and the
requests in `intel.granary_checks` and `intel.granary_requested`.

## Fishing boats

`[ai_base] fishing_boat` is on in the shipped file and is now active. The game picks a fishing boat from the epoch alone:
the Stone boat before the Bronze Age, the Bronze boat until the World War I age and the Modern boat after that. From the
World War I age the Modern boat cannot be trained yet, so the game asks for a boat it cannot build and fishing stops.
For an Age of AI player the port walks the class table and returns the newest fishing boat class the player can build
now. An upgraded line's old class has no available tech and drops out. When no boat is available, the game's pick
stands. Setting 0 restores that pick; the setting was parsed but inert before this change.

The shipped 1 changes behaviour on water maps from the World War I age on. The 300-second benchmark is a land game
whose two fishing-boat calls both pick the Stone boat, so its trace stays byte-identical and its score stays at 4020.
In a 30-minute Small Islands game with three Age of AI players, gate on picked the Imperial boat (class 365) on all
104 calls, and every pick differed from the game's; it scored 15750.7 with a population of 111.0. Gate off kept the
Modern boat (class 211) on all 127 calls and scored 15504.7 with a population of 100.3. The gate-off trace is
byte-identical to the earlier baseline and the gate-on trace differs. The sim report counts calls, newest-boat picks,
changed picks and the last class in `intel.fish_calls`, `intel.fish_picked`, `intel.fish_changed` and `intel.fish_class`.

## Mine cap

`[ai_base] citizen_mine_cap` is on in the shipped file and is now active. The game asks for the population cap times the
citizen share. From the Copper Age on the port refreshes a cap every 30 game seconds: it counts the stone, gold and iron
mines the player knows, allows the game's gatherers per mine (table value 0x16, six) times 1.5, and limits the result to
half the population cap. A request above the cap is reduced to it. This is upstream's `cz_mine_cap`; the setting was
parsed but inert before this change. Setting 0 restores the game's own count.

The shipped 1 changes behaviour from the Copper Age on. The 300-second benchmark reaches the Copper Age but its cap
stays at 200 and does not bind, so its trace stays byte-identical and its score stays at 4020. In a 30-minute Tiny map
game with three Age of AI players the cap counted 11 mines and limited the count to 99: 49 of 53 requests, 3 of 7 and
5 of 9 were reduced in the three players. Gate on scored 7089.3; gate off logged no reductions and scored 7000.3, with
a trace byte-identical to the earlier baseline. The sim report counts queries, reductions, the last mine count and the
last cap in `intel.mine_calls`, `intel.mine_capped`, `intel.mine_mines` and `intel.mine_cap`.

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

With `[ai_base] hold_town` on (off by default), home soldiers within 35 tiles of the base hold the town against seen enemies (soldiers and siege) instead of trickling out. When no enemy is near they are freed; when strong enough they sortie together; when weak they retreat to a guard point and hold. Soldiers in attack groups beyond 35 tiles are freed as in upstream. While mustering, hold-the-town does not interfere. Landing and garrison exceptions are not ported.

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

## Citizen attacks

`[ai_base] citizen_attacks` follows upstream's meaning: 1 allows the original citizen attack, 0 calls it off. The shipped
file sets 0, so every five game seconds the AI looks at its attack groups and sends any group made at least half of
citizens home. The game hands those units back and disbands the group. This gate is inverted: the shipped 0 enables the
scan, while 1 leaves citizen groups alone. Upstream also calls citizen attacks off at 0, and the port had no such
call-off before, so this change brings the port in line with upstream at the shipped setting. Normal sim games form no
citizen attack groups, so the scan has no effect on play there. The sim report counts the scans and the groups sent home
in `intel.citizen_calls` and `intel.citizen_called`.

## The game's walls

`[ai_base] game_walls` has the same inverted meaning as upstream: 1 leaves the game's automatic wall manager running for
the AI, 0 turns it off. The shipped file sets 0, so an Age of AI player does not build the game's ring of wall segments;
the mod's own defences come from `defence_layout` instead, which builds through the wall builder directly. Classic AI
players keep the game's walls either way. This is another parsed row whose shipped value turns a rule on here: upstream
also turns the game's walls off at 0, and the port had no such switch before, so this change brings the port in line with
upstream at the shipped setting. In the default benchmark game the change is visible: citizens that the game's wall
manager would have sent to wall segments stay on other work, so population and army rise a little and the score moves
from 3920 to 4020 value. Setting 1 restores the old trace exactly. The sim report counts the wall updates skipped and
run in `intel.wall_skipped` and `intel.wall_ran`.

## Retargeting an attack group

With `[ai_base] attack_retarget` on (off in the shipped file), every ten game seconds the AI checks its attack groups.
A group whose target player has left the game or is no longer an enemy takes the enemy building nearest to the group's
middle as its new target, provided it has seen that building. The group and each subgroup still pointed at the old
player take the new one, so their next march and aim use it. Groups that still fight an active enemy are left alone.
This is upstream's `wave_retarget`; the sim report counts the scans, the groups retargeted and the subgroups re-pointed
in `intel.retarget_scans`, `intel.retarget_picked` and `intel.retarget_groups`. Normal games form no group with a dead
target, so the scan runs but does not change play there; the counter proves it is active when the gate is on.

## A wave's goal

With `[ai_base] wave_focus` on (off in the shipped file), the land routes of an attack group use one goal. When a subgroup asks for a route, the first route request sends it to an enemy building of the target player that the AI has seen. The building is the nearest one on the land mass of the route: nearest to the route's start at first, then nearest to the last goal after a building falls. A return mission keeps its own goal. The group holds the building until it dies, the route moves to another land mass, or three minutes pass with no route for the group. The AI then picks a new building, or leaves the game's own goal when it knows no enemy building. The sim report counts the routes seen, the goals chosen, the goals changed, the routes already at the goal and the routes with no goal found (`intel.wave_*`).

The rest of `stage.c` is not ported. There is no staging point, no falling back and no rally.

## Weak spots

`[ai_base] attack_weak_spots` is on in the shipped file, as upstream, and is now active. When a land route's goal is an enemy building under known defences, the AI looks for a weaker enemy building within `attack_weak_spot_radius` tiles (60 by default). The building must be one the AI has seen, on the same land mass as the route, and outside every known defence's range. Walls do not count. The AI picks the building nearest the route's start and goal, sends the route there and records it as the subgroup's goal. A weak spot also becomes the wave's goal when `wave_focus` is on. Upstream searches a grid box around the goal and compares the map areas of the start and the candidate; the port scans the enemy players' buildings and compares their land mass. The score and the goal write are the same.

The shipped 1 is a deliberate behaviour change: the earlier port read the setting but never used it, so an attack kept its goal behind the defences. Setting 0 restores those routes. The 300-second benchmark makes six route checks and finds no threatened goal, so its trace stays byte-identical and its score stays at 4020. In a 30-minute Tiny map game with three Age of AI players, gate on changed two goals for one player and six for another and scored 7000.3; gate off changed none and scored 6154.7, with a different trace that is byte-identical to the earlier baseline. The sim report counts the routes seen, the threatened goals and the goals changed in `intel.weak_calls`, `intel.weak_threatened` and `intel.weak_picked`.

## Attack routing

`[ai_base] avoid_towers` is on in the shipped file, as upstream. A land attack route pays a cost for each known enemy defence that covers the subsector the route enters. The cost is the defence's attack divided by 50, times `avoid_towers_weight` (2000); `avoid_towers_margin` adds 6 tiles to the defence's reach. Only defences the player has seen count, as in the citizen danger test. Upstream charges this for every tile of its route planner. The game's planner works on subsectors, so the port charges one step at the subsector's centre. `AgeOfAIDanger.cpp` computes the cost and the land attack route planner adds it for Age of AI players only.

The shipped 1 is a deliberate behaviour change: the earlier port read the setting but never used it, so an attack marched through known tower range. Setting 0 restores those original routes. The 300-second benchmark game has no covered subsector on its routes, so its trace and score stay as before. In a 30-minute Tiny map game with three Age of AI players, the gate-on run paid the extra cost on 408 route steps and finished at 7273 value; the gate-off run paid none and finished at 7701. The two traces differ. The sim report counts the route steps seen, the steps in range and the cost added in `intel.avoid_calls`, `intel.avoid_penalised` and `intel.avoid_cost`.

`[ai_base] attack_siege` is on in the shipped file. Its ram rule is now active. A land ram that would score a guarded target gives the target a score of zero and waits with the army. An enemy land soldier within 5 tiles of the target counts as a guard. Citizens, rams and siege weapons do not count. The test reads all enemy land soldiers, seen or not, as upstream does. `AgeOfAIDanger.cpp` owns the engine gate and the counters; `EECPAttackSubGroupUnit::ScoreTarget` calls it for land rams only.

The shipped 1 is a deliberate behaviour change: the earlier port parsed the setting but never used it, so rams attacked guarded targets. Setting 0 restores those attacks. The 300-second benchmark game has no ram, so its trace stays the same and its score stays at 4020. In a 300-second Tiny map probe with six rams per Age of AI player at 120 s, gate on refused 18 of 407 ram target tests and scored 3197.5; gate off refused none and scored 3006.0. The traces differ. The sim report counts the guarded tests and the refusals in `intel.ram_calls` and `intel.ram_waits`.

`[ai_base] attack_leash` is on in the shipped file and is now active. A land attack skips an enemy standing in known enemy defence range when the attack's own goal tile lies outside that range. The range is a defence's range plus `avoid_towers_margin`; the leash reads the same known defences as the route cost, even when `avoid_towers` is off. The staging rules upstream checks first are not ported, so they do not hold the leash back. `AgeOfAIDanger.cpp` owns the engine gate and the counters; `EECPAttackSubGroupUnit::ScoreTarget` passes the target and the subgroup goal tiles.

The shipped 1 is a deliberate behaviour change: the earlier port parsed the setting but never used it, so soldiers chased defenders into known tower range. Setting 0 restores those attacks. The 300-second benchmark game runs 21 leash tests but skips no target, so its trace stays the same and its score stays at 4020. In a 30-minute Tiny map game with three Age of AI players, gate on skipped 724 of 18597 tests and scored 6779.3; gate off skipped none and scored 7272.7. The traces differ. The sim report counts the tests and the refusals in `intel.leash_calls` and `intel.leash_skipped`.

`[ai_base] attack_ignore_air` is on in the shipped file and is now active. When a land attack subgroup scores an enemy aircraft, a soldier whose range plus 3 tiles does not reach the aircraft gives it a score of zero. Soldiers then stay with the attack instead of chasing planes circling over their own town and holding the whole march. Aircraft in reach stay targets. The rule covers airplanes, helicopters and transport helicopters. `AgeOfAIDanger.cpp` owns the engine gate and the counters; `EECPAttackSubGroupUnit::ScoreTarget` calls it for every target a land subgroup scores, including targets with a zero score, as upstream does.

The shipped 1 is a deliberate behaviour change: the earlier port parsed the setting but never used it, so soldiers chased aircraft they could not hit. Setting 0 restores those attacks. The 300-second benchmark and ordinary games have no aircraft, so their traces stay byte-identical and the score stays at 4020. The sim's `--army-air` option spawns aircraft for a probe. In a 30-minute Tiny map game with three Age of AI players, repeated land raids and 12 aircraft per player every three minutes, gate on skipped 12040 of 12580 aircraft tests for one player and 6145 of 6379 for another; gate off scored 21291 and 5929 aircraft targets and skipped none. The gate-on score was 25857.3 against 25715.3 with the gate off, and the traces differ. The sim report counts the aircraft tests and the refusals in `intel.air_calls` and `intel.air_skipped`.

`[ai_base] army_mass=tether` is upstream's mass rule 16 and is off in the shipped file (`army_mass=` is empty). A land soldier does not pick a target more than `army_mass_tether` tiles (12 by default) from its subgroup's pack unless the target lies within its own range plus 2 tiles. The pack is the middle of the subgroup's own members; upstream uses the members that are away from the base. `army_mass=only_buildings` limits the rule to enemy buildings and forts. `AgeOfAIDanger.cpp` owns the engine gate and the counters; `EECPAttackSubGroupUnit::ScoreTarget` calls it for land targets that already have a score.

Setting the flag on is an opt-in from the shipped off state; setting it off keeps the game's own target choice. The 300-second benchmark has no `army_mass` flag, so its trace stays byte-identical and its score stays at 4020. In a 30-minute Tiny map game with three Age of AI players, gate on skipped 220 of 543, 1480 of 4529 and 7595 of 12413 target tests in the three players and scored 6795.3; gate off skipped none and scored 7089.3, with a trace byte-identical to the earlier baseline. The traces differ. The sim report counts the tests and the refusals in `intel.tether_calls` and `intel.tether_skipped`.

`[ai_base] army_mass=wait_stronger` is upstream's mass rule 2 and is off in the shipped file. When an attack is due within 15 seconds, the AI compares the hit points times attack of its own land soldiers with the strongest enemy army it has seen in the last 10 minutes. If that enemy army is stronger than `army_mass_ratio` percent (130 by default), the AI pushes the next attack time out 20 seconds. It checks again every five seconds and holds the attack for at most `army_mass_wait` minutes (10 by default). A game-launched or rescheduled attack ends an active wait. `AgeOfAIDanger.cpp` owns the rule and its counters; `EEComputerPlayer::Update` calls it once per tick.

This is an opt-in from the shipped off state, so no default rating changes. The 300-second benchmark has no attack due in its window and its trace stays byte-identical at 4020. In a 30-minute Tiny map game with three Age of AI players, gate on held the first attacks and scored 3210.7 against 7089.3 with the gate off; the traces differ. Gate on counted 1601, 1601 and 1600 due-time checks, three new waits and two or three releases per player. Gate off counted none. The sim report counts checks, new waits, releases and extensions in `intel.mass_checks`, `intel.mass_waits`, `intel.mass_goes` and `intel.mass_waited`.

## Counter-siege and the citizen militia

Upstream's default turns `[ai_base] citizen_militia` and `counter_siege` on. The shipped `ageofai.ini` turns both off
until the [benchmark](benchmark.md) rates them as the AI `aoai-militia`.

Every two game seconds an Age of AI player looks for two kinds of threat. With `citizen_militia` on, its citizens attack enemy citizens who build a defence within 12 tiles of its buildings or citizens, and, up
to the Bronze Age, enemy units that attack one of its units near its citizens. Only gathering or idle citizens within
10 tiles go, at most 3 per enemy and against at most 6 enemies, and only when the hit points times attack of every
citizen within 10 tiles (idle, gathering or busy) are 3 times those of the enemies within 6 tiles. With `early_militia` (the default) they fight soldiers only next to their own
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
and so do counter-fire, muster, and hold-the-town when `fight_estimate` is on.

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
ported: the citizens that a landing, an outpost or a new town holds (`landing.c`, `expand.c`, `towns.c`). It also removes
duplicate unfinished buildings stacked on one tile, keeping the first of each type and tile and destroying the rest.

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

`[general] lead_missiles` (off in the shipped `ageofai.ini`) makes a homing missile steer to where its moving target will
be when the missile gets there, instead of trailing the target in a curve. The missile measures the target's speed from
its position changes between steps and aims at the point where it can arrive at the same time as the target, up to ten
seconds away. A target that stands still or moves faster than twenty tiles a second is left to the game's own pursuit.
The sim report counts the steps that led a missile in `projectiles.missiles_led`. As with `lead_targets`, the setting
reads each player's own rows, so every player needs the same setting in a multiplayer game.

`[general] spread_missiles` (on in the shipped `ageofai.ini`) stops several missile launchers from wasting their shots on
one target. Each launcher picks its own target, so a group of anti-air towers can all fire at the same aircraft and most
of the missiles hit a wreck. Every launched missile is counted with the damage its launcher will do to its target. Before
a launcher fires, if the missiles already on the way cover the target's hit points, the shot is held or sent at the
nearest other enemy of the same family in range that the launcher's owner can see and that is not covered yet. A missile
that misses frees its share again when it stops steering. Like `lead_missiles`, this setting reads each player's own
rows, so every player needs the same value in a multiplayer game. The sim report counts launches, held shots and
switched shots in `projectiles.spread_launched`, `spread_held` and `spread_switched`.

`[general] lob_each_shot` (on) changes how archers, slingers, the pilum and the trebuchet fire over walls. The game lets
them shoot flat until one shot hits a wall, then lobs every later shot until they get a new order, wall gone or not. Now
a unit looks along the line to its target before each shot: a wall in the way gets the high arc, else the shot is flat.
After the game's own flat shot hit a wall, the unit lobs four more shots before it looks again. In a multiplayer game
every player needs the same setting.

`[ai_base] production_sync` (off in the shipped file) repairs the AI's in-production count after a game load. A save
keeps the buildings' production queues but not that count, so an AI can count training room it does not have. Every
10 game seconds the port counts the units queued or in the making in the player's buildings and sets the count to that
number when the two differ. The game subtracts this count from the population limit before it asks for more citizens.

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
