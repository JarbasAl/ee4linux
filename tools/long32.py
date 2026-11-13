"""The game's long as Windows' 32-bit long, for the x86-64 target (python3 -I tools/long32.py --help).

The game's source is written for Windows, where long is 32 bits wide on every target. Its data files, saves and
network packets are records of longs read and written whole, and its headers give their byte offsets. On x86-64
Linux, long is 64 bits wide, so every one of those records changes shape.

The x86-64 build therefore compiles a copy of the game's source in which the keyword long names a 32-bit type:
`long` and `long int` become `int`, and `unsigned long` becomes `unsigned int`. `long long` and `long double` keep
their meaning. An integer literal with the suffix L, UL or LU loses its L, so 0L is an int and 0UL an unsigned
int; LL and floating literals keep theirs. Comments, string literals and character literals are copied unchanged, and each line keeps its line
number, so a compiler message names the line of the original file.

The copy is build/port-linux64/long32/<path>, with the same relative paths as the source tree. It holds the game's
units and headers: include/ apart from include/port, and src/ apart from the Win32 layer (src/Port/Linux), whose code
is written for the target's own types. A file is written again only when its rewritten text changes, so the build's
dependency check stays incremental.

  python3 -I tools/long32.py FILE      prints FILE as the x86-64 build compiles it
"""
import pathlib
import re
import sys

ROOT = pathlib.Path(__file__).resolve().parent.parent
SUFFIXES = {".c", ".cpp", ".h", ".hpp", ".inl", ".cxx", ".cc"}
# The folders whose files are copied, and the folders inside them that are not: the port's own headers and the Win32
# layer use long as the target defines it
TREES = [ROOT / "include", ROOT / "src"]
EXCLUDED = [ROOT / "include" / "port", ROOT / "src" / "Port" / "Linux"]

# One token of C or C++ source, in the order the alternatives are tried: a comment, a string or character literal
# (with an optional encoding prefix), a preprocessor line continuation, an identifier or keyword, anything else
TOKEN = re.compile(r"""
    (?P<comment>//[^\n]*|/\*.*?\*/)
  | (?P<literal>(?:u8|[uUL])?"(?:\\.|[^"\\\n])*"|(?:u8|[uUL])?'(?:\\.|[^'\\\n])*')
  | (?P<number>\.?[0-9](?:[eEpP][+-]|[A-Za-z_0-9.'])*)
  | (?P<word>[A-Za-z_][A-Za-z_0-9]*)
  | (?P<space>\s+)
  | (?P<other>.)
""", re.VERBOSE | re.DOTALL)


def rewrite(text):
    """The source text with each keyword long that names a single long turned into a 32-bit int."""
    tokens = [(m.lastgroup, m.group()) for m in TOKEN.finditer(text)]
    out = [value for _, value in tokens]

    def neighbour(i, step):
        i += step
        while 0 <= i < len(tokens) and tokens[i][0] in ("space", "comment"):
            i += step
        return tokens[i][1] if 0 <= i < len(tokens) else ""

    for i, (kind, value) in enumerate(tokens):
        if kind == "number":
            out[i] = _int_literal(value)
            continue
        if kind != "word" or value != "long":
            continue
        around = (neighbour(i, -1), neighbour(i, 1))
        if "long" in around or "double" in around:
            continue
        # long int and int long are one int already
        out[i] = "" if "int" in around else "int"
    return "".join(out)


def _int_literal(value):
    """An integer literal with its single L suffix removed; any other number unchanged."""
    hexadecimal = value[:2].lower() == "0x"
    if (not hexadecimal and any(c in value for c in ".eE")) or (hexadecimal and any(c in value for c in "pP")):
        return value
    match = re.fullmatch(r"(.*?)([uU]?)([lL])([uU]?)", value)
    if not match or match.group(1)[-1:] in ("l", "L") or not match.group(1):
        return value
    return match.group(1) + match.group(2) + match.group(4)


def copied(path):
    """True for a file or folder that the copy holds."""
    path = path.resolve()
    return ((path.is_dir() or path.suffix.lower() in SUFFIXES) and any(path.is_relative_to(t) for t in TREES)
            and not any(path.is_relative_to(e) for e in EXCLUDED))


def shadow(out, path):
    """The copy's path for a source file or include folder; paths outside the copied trees are their own."""
    path = path.resolve()
    return out / path.relative_to(ROOT) if copied(path) else path


def sync(out):
    """Brings the copy in out up to date; returns the files in and the files written."""
    seen = written = 0
    for tree in TREES:
        for path in tree.rglob("*"):
            if not path.is_file() or not copied(path):
                continue
            seen += 1
            target = out / path.relative_to(ROOT)
            text = rewrite(path.read_text(encoding="latin-1"))
            if target.exists() and target.read_text(encoding="latin-1") == text:
                continue
            target.parent.mkdir(parents=True, exist_ok=True)
            target.write_text(text, encoding="latin-1")
            written += 1
    return seen, written


if __name__ == "__main__":
    if len(sys.argv) != 2 or sys.argv[1].startswith("-"):
        sys.exit(__doc__)
    sys.stdout.write(rewrite(pathlib.Path(sys.argv[1]).read_text(encoding="latin-1")))
