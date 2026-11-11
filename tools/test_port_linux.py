"""Native port tests: build and run the tests of the Win32 layer (python3 -I tools/test_port_linux.py).

Each tests/port/linux/test_*.cpp compiles with the native unit flags of tools/build_port.py, links against the
layer's objects (build/port-linux/obj/win32, made by build_port.py --target linux win32) with fopen wrapped, as the
game's images are, and runs in a temporary
folder that is removed afterwards. One more test reads every string of the installed Language.dll through
LoadStringA and compares it with ee-toolkit's PE reader (EE_TOOLKIT, EE_LANGUAGE_DLL name others). The report gives the
tests in and the tests that passed.
"""
import os
import pathlib
import subprocess
import sys
import tempfile

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import build_port as bp

TESTS = bp.ROOT / "tests" / "port" / "linux"
OBJECTS = bp.ROOT / "build" / "port-linux" / "obj" / "win32"
TOOLKIT = pathlib.Path(os.environ.get("EE_TOOLKIT", pathlib.Path.home() / "AgentWorkspaces" / "games" / "ee-toolkit"))
LANGUAGE_DLL = pathlib.Path(os.environ.get(
    "EE_LANGUAGE_DLL", pathlib.Path.home() / ".wine" / "drive_c" / "Program Files (x86)" / "Empire Earth" /
    "Empire Earth - The Art of Conquest" / "Language.dll"))


def build(source, binary, layer):
    cfg = bp.TARGETS["linux"]
    cmd = [str(cfg["cxx"]), *[f for f in cfg["flags"] if f != "-w"], *cfg["cxxflags"],
           *bp.unit_defines(source, "win32"), "-include", str(bp.COMPAT),
           *(f"-I{d}" for d in cfg["includes"]), *(f"-I{d}" for d in bp.INCLUDES),
           *(f"-isystem{d}" for d in cfg["system_includes"]), str(source), *layer, "-lpthread", "-ldl",
           "-Wl,--wrap=fopen", "-o", str(binary)]
    return subprocess.run(cmd, capture_output=True, text=True)


def check_strings(scratch, layer):
    """Every string of Language.dll as LoadStringA reads it, against ee-toolkit's PE reader: True when they agree."""
    sys.path.insert(0, str(TOOLKIT))
    from ee_toolkit.container.pe_strings import ResourceDLL
    binary = pathlib.Path(scratch) / "dump_strings"
    made = build(TESTS / "dump_strings.cpp", binary, layer)
    if made.returncode:
        print(f"dump_strings.cpp: does not build\n{made.stderr}")
        return False
    run = subprocess.run([str(binary), str(LANGUAGE_DLL)], capture_output=True, text=True, cwd=scratch)
    if run.returncode:
        print(f"dump_strings: exit {run.returncode}\n{run.stderr}")
        return False
    dll = ResourceDLL(LANGUAGE_DLL.read_bytes())
    expected = {i: t for i, t in dll.strings().items() if t}
    got, language, tables = {}, None, None
    for line in run.stdout.splitlines():
        kind, _, rest = line.partition(" ")
        if kind == "language":
            language = int(rest)
        elif kind == "tables":
            tables = int(rest)
        elif kind == "string":
            number, _, data = rest.partition(" ")
            got[int(number)] = bytes.fromhex(data).decode("latin-1")
    want = {i: "".join(c if ord(c) < 256 else "?" for c in t)[:8191] for i, t in expected.items()}
    wrong = [i for i in want if got.get(i) != want[i]]
    extra = sorted(set(got) - set(want))
    print(f"strings: {len(want)} in Language.dll by ee-toolkit, {len(got)} read by LoadStringA, "
          f"{len(wrong)} differ, {len(extra)} extra; language {language} against {dll.language}; {tables} tables")
    for i in (wrong + extra)[:5]:
        print(f"  {i}: {got.get(i)!r} against {want.get(i)!r}")
    return not wrong and not extra and language == dll.language and len(got) == len(want) and tables > 0


def main():
    layer = sorted(str(o) for o in OBJECTS.rglob("*.o"))
    if not layer:
        sys.exit(f"no objects in {OBJECTS}: run python3 -I tools/build_port.py --target linux win32")
    tests = sorted(TESTS.glob("test_*.cpp"))
    passed = 0
    with tempfile.TemporaryDirectory(prefix="ee-port-test-") as scratch:
        for test in tests:
            binary = pathlib.Path(scratch) / test.stem
            data = pathlib.Path(scratch) / f"{test.stem}-data"
            data.mkdir()
            made = build(test, binary, layer)
            if made.returncode:
                print(f"{test.name}: does not build\n{made.stderr}")
                continue
            run = subprocess.run([str(binary)], capture_output=True, text=True, timeout=120, cwd=data,
                                 env={"EE_PORT_TEST_DIR": str(data), "PATH": "/usr/bin:/bin"})
            ended = f"ended by signal {-run.returncode}" if run.returncode < 0 else ""
            print(f"{test.name}: {run.stdout.strip()} {ended}".rstrip())
            if run.stderr.strip():
                print(run.stderr.rstrip())
            passed += run.returncode == 0
        strings_ok = check_strings(scratch, layer)
    print(f"tests: {len(tests) + 1} in, {passed + strings_ok} passed")
    return 0 if passed == len(tests) and strings_ok else 1


if __name__ == "__main__":
    sys.exit(main())
