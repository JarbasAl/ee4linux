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
priests and transports; not citizens, rams, animals, shots or buildings. Upstream's own test, an "army family" that
includes naval buildings and leaves out its separate ship and aircraft kinds, is not copied.

The rest of this page describes the tool that finds, for each of the mod's hooks, the function, member or class that
the hook changes here.

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
