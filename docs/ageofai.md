# Age of AI in the native build

[Age of AI](https://github.com/Grille/Empire-Earth-Age-of-AI) by Grille is a mod for the retail game. It gives the
computer players a new AI and fixes many of the game's own bugs. On Windows it is a DLL that patches the running exe:
it redirects calls, replaces vtable slots and hooks import slots, at addresses that its spec gives for each exe
build.

The native build has the game's source. So the port of Age of AI is C++ in that source, with explicit calls where
the mod has hooks. It does not patch addresses. This page describes the tool that finds, for each of the mod's
hooks, the function, member or class that the hook changes here.

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
