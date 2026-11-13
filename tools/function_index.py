"""Age of AI's hook sites in this tree (python3 -I tools/function_index.py --ageofai DIR [--out FILE]).

Age of AI (github.com/Grille/Empire-Earth-Age-of-AI) changes the retail game by patching its code: a call redirected,
a vtable slot replaced, an import slot hooked. Its spec (spec/game/*.cs) names each place by an address or an offset
per exe build, and its generator writes them into dll/game/api/gen/*.c, one block of values per build. The Art of
Conquest column is the exe this tree reconstructs, so each value has a place here:

- a function address: the function whose "// FUNCTION: EXE", "// SYNTHETIC: EXE" or "// TEMPLATE: EXE" comment
  gives that address, else a comment that names the address (an inline function the linker kept, for example);
- a place in the code (a call site, an instruction operand): the function that starts nearest below it;
- a data address: the comment that names it, written here without the image base (0x931710 is 0x531710);
- a member offset: the member whose "// +0x" comment covers it, in the spec's class or one of its bases;
- a vtable: the class of that name, or the class its spec comment names;
- an import slot: the engine function it imports, the Win32 function of the port's layer, or the C library's.

The tool reads every patch_* and iat_hook call in the mod's code outside the generated files, expands the generated
patch_* wrappers, takes the spec values each call uses, and writes one row per value with the place it has here. It
does the same for the values the mod's code reads and calls through the generated accessors. An address of 0, or an
offset or slot of NA, is a place the Art of Conquest exe does not have.

Each generated patch_call names the function its call site calls. The tool checks that the function it places the
site in calls that function. It prints the calls and values read, how many it placed and how many sites passed the
check ("N in, N out").
"""
import argparse
import bisect
import collections
import pathlib
import re
import subprocess
import sys

ROOT = pathlib.Path(__file__).resolve().parent.parent
IMAGE_BASE = 0x400000
NA = 0xFFFFFFFF                          # the generator's value for an offset or slot a build has not got
CODE_END = 0x800000                      # the exe's code section ends below this; data and .rdata lie above
NEAR = 0x4000                            # a code place further than this past its function start is reported as far
ANNOTATION = re.compile(r"//\s*(?:FUNCTION|SYNTHETIC|TEMPLATE):\s*(\w+)\s+0x([0-9a-fA-F]+)")
DEFINITION = re.compile(r"([~\w]+(?:<[^()]*>)?(?:::[~\w]+(?:<[^()]*>)?)*)\s*\(")
CLASS = re.compile(r"^(?:class|struct)\s+(?:[A-Z0-9]+_API\s+)?(\w+)\s*(?::\s*([^{;]+))?\{", re.M)
MEMBER = re.compile(r"(\w+)\s*(?:\[[^\]]*\]\s*)*;\s*//\s*\+0x([0-9a-fA-F]+)")
SECTION = re.compile(r"^// (?:static )?class (\w+)")
CASE = re.compile(r"^\s*case (\d+):\s*//\s*(\w+)")
ASSIGN = re.compile(r"^\s*((?:fn|data|of|value|vt|size|slot|calls?)_\w+)(?:\[(\d+)\])?\s*=\s*(?:\([^)]*\))?\s*"
                    r"(0x[0-9A-Fa-f]+|\d+|NA);")
VARIABLE = re.compile(r"\b((?:fn|data|of|value|vt|size|slot|calls?)_\w+)")
LABEL = re.compile(r"\bspec_(\w+)\(&?(\w+)(?:\[\d+\])?,\s*((?:\"[^\"]*\",\s*)*)\"([\w.\[\]]+)\"\)")
ACCESSOR = re.compile(r"^internal inline [^(]*?\b(\w+)\([^)]*\)\s*\{(.*)\}\s*$")
WRAPPER = re.compile(r"^internal inline int (patch_\w+)\([^)]*\)\s*\{(.*)\}\s*$")
HOOK_CALL = re.compile(r"\b(patch_\w+|iat_hook)\s*\(")
SYSTEM_MODULES = ("kernel32.dll", "user32.dll", "gdi32.dll", "winmm.dll", "msvcrt.dll", "advapi32.dll", "ole32.dll",
                  "dsound.dll", "ddraw.dll", "dinput.dll", "ws2_32.dll", "wsock32.dll", "shell32.dll", "version.dll")


def index_functions(root):
    """The annotated functions: exe start address -> (name, file, line), and name -> (module, file, line) for every
    module. An annotation with no definition after it (a template instantiation, code the linker shared, an inline
    function defined in a header) has the name None and the annotation's line."""
    functions, names = {}, {}
    for path in sorted((root / "src").rglob("*.cpp")) + sorted((root / "include").rglob("*.h")):
        lines = path.read_text(errors="replace").splitlines()
        for number, line in enumerate(lines):
            match = ANNOTATION.search(line)
            if not match:
                continue
            defined = False
            for offset, following in enumerate(lines[number + 1:number + 12]):
                stripped = following.strip()
                if ANNOTATION.search(stripped):
                    break
                if not stripped or stripped.startswith("//"):
                    continue
                name = DEFINITION.search(stripped)
                if name:
                    where = (path.relative_to(root), number + 2 + offset)
                    if match.group(1) == "EXE":
                        functions[int(match.group(2), 16)] = (name.group(1), *where)
                    names.setdefault(re.sub(r"<[^()]*>", "", name.group(1)), (match.group(1), *where))
                    defined = True
                break
            if not defined and match.group(1) == "EXE":
                functions.setdefault(int(match.group(2), 16), (None, path.relative_to(root), number + 1))
    return functions, names


def index_classes(root):
    """Each class of the headers: name -> (bases, [(offset, member, file, line)])"""
    classes = {}
    for path in sorted((root / "include").rglob("*.h")):
        text = path.read_text(errors="replace")
        for match in CLASS.finditer(text):
            depth, end = 1, match.end()
            while depth and end < len(text):
                depth += {"{": 1, "}": -1}.get(text[end], 0)
                end += 1
            bases = [re.sub(r"^(public|protected|private|virtual)\s+", "", b.strip()).split("<")[0].strip()
                     for b in (match.group(2) or "").split(",") if b.strip()]
            first_line = text.count("\n", 0, match.start()) + 1
            members = []
            for number, line in enumerate(text[match.start():end].splitlines()):
                member = MEMBER.search(line)
                if member:
                    members.append((int(member.group(2), 16), member.group(1), path.relative_to(root),
                                    first_line + number))
            if members or match.group(1) not in classes:
                classes[match.group(1)] = (bases, sorted(members))
    return classes


class Generated:
    """What dll/game/api/gen/*.c says of each variable: its class (owner), its Art of Conquest values, its spec comment
    (doc), its spec label and, for an import slot, the module and the name; and the variables that each patch_*
    wrapper and each accessor uses"""

    def __init__(self, ageofai):
        self.owners, self.docs, self.labels, self.imports = {}, {}, {}, {}
        self.values, self.wrappers, self.accessors, self.column = collections.defaultdict(list), {}, {}, None
        for path in sorted((ageofai / "dll" / "game" / "api" / "gen").glob("*.c")):
            owner, case, doc = None, None, []
            for line in path.read_text(errors="replace").splitlines():
                section = SECTION.match(line)
                if section:
                    owner, doc = section.group(1), []
                    continue
                case_match = CASE.match(line)
                if case_match:
                    case = case_match.group(2)
                    continue
                assign = ASSIGN.match(line)
                if assign and case == "aoc":
                    self.column = "aoc"
                    self.values[assign.group(1)].append(NA if assign.group(3) == "NA" else int(assign.group(3), 0))
                    continue
                for label in LABEL.finditer(line):
                    self.labels.setdefault(label.group(2), label.group(4))
                    if label.group(1) == "import":
                        module, name = re.findall(r'"([^"]*)"', label.group(3))[:2]
                        self.imports[label.group(2)] = (module, name)
                wrapper = WRAPPER.match(line)
                if wrapper:
                    self.wrappers[wrapper.group(1)] = sorted(set(VARIABLE.findall(wrapper.group(2))))
                accessor = ACCESSOR.match(line)
                if accessor and not wrapper:
                    self.accessors[accessor.group(1)] = sorted(set(VARIABLE.findall(accessor.group(2))))
                if line.startswith("//"):
                    doc.append(line[2:].strip())
                    continue
                if owner and case is None:
                    for variable in VARIABLE.findall(line):
                        self.owners.setdefault(variable, owner)
                        self.docs.setdefault(variable, " ".join(doc))
                doc = []


def hook_calls(ageofai, patches):
    """Each call of a patching function (patches) in the mod's own code: (file, line, call, [variables])"""
    calls = []
    for path in sorted((ageofai / "dll").rglob("*.c")):
        relative = path.relative_to(ageofai)
        if relative.parts[:4] == ("dll", "game", "api", "gen") or relative == pathlib.Path("dll/base/patch.c"):
            continue
        lines = path.read_text(errors="replace").splitlines()
        for number, line in enumerate(lines):
            stripped = line.strip()
            if stripped.startswith("//") or stripped.startswith("#define"):
                continue
            for match in HOOK_CALL.finditer(line):
                if match.group(1) not in patches:
                    continue
                text, depth, row = "", 0, number
                position = match.end() - 1
                while row < len(lines):
                    segment = lines[row][position:] if row == number else lines[row]
                    for character in segment:
                        text += character
                        depth += {"(": 1, ")": -1}.get(character, 0)
                        if depth == 0:
                            break
                    if depth == 0:
                        break
                    row += 1
                calls.append((relative, number + 1, match.group(1), sorted(set(VARIABLE.findall(text)))))
    return calls


def mod_uses(ageofai, generated):
    """The spec variables the mod's own code reaches through the generated accessors and calls (a member's get, set
    and _at, a game function's wrapper): variable -> the number of places that use it"""
    uses = collections.Counter()
    name = re.compile(r"\b(\w+)\s*\(")
    for path in sorted((ageofai / "dll").rglob("*.c")):
        if path.relative_to(ageofai).parts[:4] == ("dll", "game", "api", "gen"):
            continue
        for line in path.read_text(errors="replace").splitlines():
            for called in name.findall(line):
                for variable in generated.accessors.get(called, []):
                    if variable in generated.values:
                        uses[variable] += 1
    return uses


def expand(calls, generated):
    """The calls with each generated wrapper's variables added to the call's own"""
    out = []
    for path, line, call, variables in calls:
        found = set(variables) | set(generated.wrappers.get(call, []))
        out.append((path, line, call, sorted(v for v in found if v in generated.values)))
    return out


def read_spec(ageofai):
    """From spec/game/*.cs: the Art of Conquest vtable address of each class -> its name, and each Art of Conquest
    address -> the spec comment right above it"""
    vtables, docs = {}, {}
    vtable = re.compile(r"vtable\(\(ee\)(?:0x[0-9A-Fa-f]+|none),\s*\(aoc\)(0x[0-9A-Fa-f]+)\)")
    address = re.compile(r"address\(\(ee\)(?:0x[0-9A-Fa-f]+|none),\s*\(aoc\)(0x[0-9A-Fa-f]+)\)")
    for path in sorted((ageofai / "spec" / "game").glob("*.cs")):
        doc, pending = [], None
        for line in path.read_text(errors="replace").splitlines():
            stripped = line.strip()
            if stripped.startswith("///"):
                doc.append(stripped[3:].strip())
                continue
            if stripped.startswith("["):
                found = vtable.search(stripped)
                if found:
                    pending = int(found.group(1), 16)
                for found in address.finditer(stripped):
                    docs.setdefault(int(found.group(1), 16), " ".join(doc))
                continue
            declared = re.search(r"\b(?:struct|class)\s+(\w+)", stripped)
            if declared and pending is not None:
                vtables[pending] = declared.group(1)
            if stripped:
                doc, pending = [], None
    return vtables, docs


def demangle(name):
    """An MSVC name's class and function: ?UpdateVisibleTerrain@GETerrainMesh@@QAEX... is
    GETerrainMesh::UpdateVisibleTerrain"""
    if not name.startswith("?"):
        return name
    parts = name[1:].split("@@", 1)[0].split("@")
    return "::".join(reversed(parts))


def base_patches(ageofai):
    """The patching functions of dll/base/patch.c"""
    text = (ageofai / "dll" / "base" / "patch.c").read_text(errors="replace")
    return set(re.findall(r"^internal \w+\s+\*?(patch_\w+|iat_hook)\s*\(", text, re.M))


class Places:
    def __init__(self, root, generated, spec):
        self.generated = generated
        self.spec_vtables, self.spec_docs = spec
        self.functions, self.names = index_functions(root)
        self.starts = sorted(self.functions)
        self.classes = index_classes(root)
        self.sources = [(p.relative_to(root), p.read_text(errors="replace").splitlines())
                        for p in sorted((root / "src").rglob("*.cpp")) + sorted((root / "include").rglob("*.h"))]
        self.layer = [(path, lines) for path, lines in self.sources if path.parts[:3] == ("src", "Port", "Linux")]
        self.lines = dict(self.sources)

    def function(self, address):
        if address in self.functions:
            name, path, line = self.functions[address]
            if name:
                return f"`{name}` ({path}:{line})"
            lines = self.lines[path]
            hint = lines[line].strip().lstrip("/ ") if line < len(lines) else ""
            return f"an annotation without a definition here ({path}:{line}): {hint[:80]}"
        return None

    def code(self, address):
        i = bisect.bisect_right(self.starts, address) - 1
        if i < 0:
            return None
        start = self.starts[i]
        name, path, line = self.functions[start]
        far = " FAR" if address - start > NEAR else ""
        return f"`{name or 'unnamed'}` +0x{address - start:x}{far} ({path}:{line})"

    def search(self, pattern, sources):
        needle = re.compile(pattern)
        for path, lines in sources:
            for number, line in enumerate(lines):
                if needle.search(line):
                    return f"{path}:{number + 1}: `{line.strip()[:90]}`"
        return None

    def mentioned(self, address):
        found = self.search(r"(?i)//.*\b0x0*%x\b" % address, self.sources)
        return f"named in a comment: {found}" if found else None

    def data(self, address):
        return self.search(r"(?i)\b0x0*%x\b" % (address - IMAGE_BASE), self.sources)

    def member(self, owner, offset, seen=()):
        if owner not in self.classes or owner in seen:
            return None
        bases, members = self.classes[owner]
        covering = [m for m in members if m[0] <= offset]
        if covering and (covering[-1][0] == offset or not bases or offset >= members[0][0]):
            start, name, path, line = covering[-1]
            within = f" +0x{offset - start:x}" if offset != start else ""
            return f"`{owner}::{name}`{within} ({path}:{line})"
        for base in bases:
            found = self.member(base, offset, seen + (owner,))
            if found:
                return found
        return None

    def imported(self, module, name):
        if module.lower().startswith("msvcr"):
            return f"the C library's `{name}`"
        if module.lower() in SYSTEM_MODULES:
            found = self.search(r"\b%s\s*\(" % re.escape(name), self.layer)
            return f"the layer's `{name}` ({found.split(': ', 1)[0]})" if found else None
        function = demangle(name)
        if function in self.names:
            module_name, path, line = self.names[function]
            return f"`{function}` ({module_name}, {path}:{line})"
        owner, _, method = function.rpartition("::")
        header = [(path, lines) for path, lines in self.sources if path.name == f"{owner}.h"]
        found = self.search(r"\b%s\s*\(" % re.escape(method), header) if owner else None
        return f"`{function}`, inline ({found.split(': ', 1)[0]})" if found else None

    def vtable(self, address):
        name = self.spec_vtables.get(address)
        named = re.match(r"(\w+)", self.spec_docs.get(address, ""))
        if name is None and named and named.group(1) in self.classes:
            name = named.group(1)
        if name is None:
            return None
        return f"the vtable of class `{name}`" + ("" if name in self.classes else " (not a class of this tree)")

    def constant(self, address):
        doc = self.spec_docs.get(address, "")
        return f"a constant in .rdata: {doc[:80]}" if re.match(r"(the )?float\b", doc) and address < 0x900000 else None

    def place(self, variable, value):
        """(category, place): the category is placed, absent (not in the Art of Conquest exe), number, or None"""
        kind = variable.split("_", 1)[0]
        owner = self.generated.owners.get(variable)
        if kind in ("value", "size", "slot") and value != NA:
            return "number", "a number"
        if value == NA or (value == 0 and kind not in ("of",)):
            return "absent", "not in the Art of Conquest exe"
        if variable in self.generated.imports:
            place = self.imported(*self.generated.imports[variable])
        elif kind == "fn":
            place = self.function(value) or self.mentioned(value)
        elif kind in ("data", "call", "calls"):
            if value < CODE_END:
                place = self.function(value) or self.code(value)
            else:
                place = self.vtable(value) or self.data(value) or self.constant(value)
        elif kind == "of":
            place = self.member(owner, value) if owner else None
        else:   # vt
            place = self.vtable(value)
        return ("placed" if place else None), place


def check_call_sites(places, generated, ageofai):
    """Each generated patch_call(site, expected function, hook): the function the site is placed in must call the
    expected function. Returns the sites checked and the ones whose function does not call it."""
    pattern = re.compile(r"patch_call\((\w+),\s*\(uintptr_t\)(fn_\w+)")
    checked, failed = 0, []
    for path in sorted((ageofai / "dll" / "game" / "api" / "gen").glob("*.c")):
        for match in pattern.finditer(path.read_text(errors="replace")):
            target = (generated.values.get(match.group(2)) or [0])[0]
            if target not in places.functions or places.functions[target][0] is None:
                continue
            method = places.functions[target][0].split("::")[-1]
            for site in generated.values.get(match.group(1), []):
                i = bisect.bisect_right(places.starts, site) - 1
                if not site or i < 0 or places.functions[places.starts[i]][0] is None:
                    continue
                name, path_here, line = places.functions[places.starts[i]]
                body = []
                for text in (ROOT / path_here).read_text(errors="replace").splitlines()[line:]:
                    if ANNOTATION.search(text):
                        break
                    body.append(text)
                checked += 1
                if not re.search(r"\b%s\s*\(" % re.escape(method), "\n".join(body)):
                    failed.append((match.group(1), site, name, method))
    return checked, failed


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("--ageofai", type=pathlib.Path, required=True, help="a checkout of Empire-Earth-Age-of-AI")
    ap.add_argument("--out", type=pathlib.Path, help="the Markdown listing (default: the summary only)")
    args = ap.parse_args()
    if not (args.ageofai / "dll" / "game" / "api" / "gen").is_dir():
        sys.exit(f"function_index: {args.ageofai} has no dll/game/api/gen; give a checkout of Age of AI")

    generated = Generated(args.ageofai)
    if generated.column != "aoc":
        sys.exit("function_index: no Art of Conquest column (case N: // aoc) in the generated files")
    patches = set(generated.wrappers) | base_patches(args.ageofai)
    calls = expand(hook_calls(args.ageofai, patches), generated)
    places = Places(ROOT, generated, read_spec(args.ageofai))

    rows = []
    for path, line, call, variables in calls:
        for variable in variables:
            for value in generated.values[variable]:
                category, place = places.place(variable, value)
                rows.append((f"{path}:{line}", call, variable, generated.labels.get(variable, ""), value, category,
                             place, generated.docs.get(variable, "")))
    counts = collections.Counter(r[5] for r in rows)
    with_values = sum(bool(c[3]) for c in calls)
    summary = (f"{len(calls)} hook calls in, {with_values} with spec values; {len(rows)} values in: "
               f"{counts['placed']} placed, {counts['number']} numbers, {counts['absent']} not in the AoC exe, "
               f"{counts[None]} not placed; {len(places.functions)} exe functions and {len(places.classes)} classes "
               f"indexed")
    uses = mod_uses(args.ageofai, generated)
    used = []
    for variable in sorted(uses):
        for value in generated.values[variable]:
            category, place = places.place(variable, value)
            used.append((variable, generated.labels.get(variable, ""), value, uses[variable], category, place))
    used_counts = collections.Counter(u[4] for u in used)
    summary += (f"; {len(used)} values the mod reads or calls: {used_counts['placed']} placed, "
                f"{used_counts['number']} numbers, {used_counts['absent']} not in the AoC exe, "
                f"{used_counts[None]} not placed")
    checked, failed = check_call_sites(places, generated, args.ageofai)
    summary += (f"; {checked} patch_call sites checked, {checked - len(failed)} in a function that calls the "
                f"expected one")
    print("function_index: " + summary)

    if args.out:
        commit = subprocess.run(["git", "-C", str(args.ageofai), "rev-parse", "--short", "HEAD"], capture_output=True,
                                text=True).stdout.strip() or "an unknown commit"
        text = ["# Age of AI's hook sites in ee4linux", "",
                f"Generated by `tools/function_index.py` from Age of AI at `{commit}`: its `dll/` and its generated "
                f"spec values (the `{generated.column}` column). {summary[0].upper()}{summary[1:]}.", "",
                "## Not placed", "", "| Call | Patch | Spec | Value | Spec comment |",
                "| --- | --- | --- | --- | --- |"]
        text += [f"| {r[0]} | `{r[1]}` | `{r[3] or r[2]}` | 0x{r[4]:x} | {r[7][:120]} |" for r in rows if r[5] is None]
        text += ["", "## Call sites whose function does not call the expected one", "",
                 "| Site | Value | Function here | Expected call |", "| --- | --- | --- | --- |"]
        text += [f"| `{f[0]}` | 0x{f[1]:x} | `{f[2]}` | `{f[3]}` |" for f in failed]
        text += ["", "## Hook calls without a spec value", "", "| Call | Patch |", "| --- | --- |"]
        text += [f"| {c[0]}:{c[1]} | `{c[2]}` |" for c in calls if not c[3]]
        text += ["", "## Values the mod reads or calls, not placed", "", "| Spec | Value | Uses | Spec comment |",
                 "| --- | --- | --- | --- |"]
        text += [f"| `{u[1] or u[0]}` | 0x{u[2]:x} | {u[3]} | {generated.docs.get(u[0], '')[:100]} |"
                 for u in used if u[4] is None]
        text += ["", "## Every value the mod reads or calls", "", "| Spec | Value | Uses | Place here |",
                 "| --- | --- | --- | --- |"]
        text += [f"| `{u[1] or u[0]}` | 0x{u[2]:x} | {u[3]} | {u[5] or '**not placed**'} |" for u in used]
        text += ["", "## Every value of a hook call", "", "| Call | Patch | Spec | Value | Place here |",
                 "| --- | --- | --- | --- | --- |"]
        text += [f"| {r[0]} | `{r[1]}` | `{r[3] or r[2]}` | 0x{r[4]:x} | {r[6] or '**not placed**'} |" for r in rows]
        args.out.write_text("\n".join(text) + "\n")
        print(f"function_index: {len(rows) + len(used)} rows written to {args.out}")


if __name__ == "__main__":
    main()
