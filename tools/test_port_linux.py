"""Native port tests: build and run the tests of the Win32 layer (python3 -I tools/test_port_linux.py).

Each tests/port/linux/test_*.cpp compiles with the native unit flags of tools/build_port.py, links against the
layer's objects (build/port-linux/obj/win32, made by build_port.py --target linux win32) with fopen wrapped, as the
game's images are, and runs in a temporary folder that is removed afterwards. More checks compare the layer with an
independent source: every string of the installed Language.dll through LoadStringA against ee-toolkit's PE reader
(EE_TOOLKIT, EE_LANGUAGE_DLL name others), code page conversion against Python's codecs, font metrics against a
Windows 11 capture, Bink playback against ffmpeg, and Miles' MP3 decoding of the base game's music (EE_SOUNDS) and
AoC's data.ssa (EE_AOC_DATA) against ffmpeg. The report gives the tests in and the tests that passed.
"""
import collections
import json
import os
import pathlib
import re
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
AOC_DATA = pathlib.Path(os.environ.get("EE_AOC_DATA", LANGUAGE_DLL.parent / "Data" / "data.ssa"))
SOUNDS = pathlib.Path(os.environ.get(
    "EE_SOUNDS", pathlib.Path.home() / ".wine" / "drive_c" / "Program Files (x86)" / "Empire Earth" / "Empire Earth" /
    "Data" / "Sounds"))


def build(source, binary, layer):
    cfg = bp.TARGETS["linux"]
    cmd = [str(cfg["cxx"]), *[f for f in cfg["flags"] if f != "-w"], *cfg["cxxflags"],
           *bp.unit_defines(source, "win32"), "-include", str(bp.COMPAT),
           *(f"-I{d}" for d in cfg["includes"]), *(f"-I{d}" for d in bp.INCLUDES),
           *(f"-isystem{d}" for d in cfg["system_includes"]), str(source), *layer, *cfg["libs"], "-o", str(binary)]
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

def windows_muldiv(a, b, c):
    """MulDiv's documented result: a*b/c rounded half away from zero, -1 for c == 0 or a result past 32 bits."""
    from fractions import Fraction
    if c == 0:
        return -1
    exact = Fraction(a * b, c)
    result = int(abs(exact) + Fraction(1, 2)) * (1 if exact >= 0 else -1)
    return result if -2**31 <= result < 2**31 else -1


def check_text(scratch, layer):
    """Windows-1252 and UTF-8 conversion against Python's codecs, MulDiv against exact arithmetic."""
    binary = pathlib.Path(scratch) / "dump_text"
    made = build(TESTS / "dump_text.cpp", binary, layer)
    if made.returncode:
        print(f"dump_text.cpp: does not build\n{made.stderr}")
        return False
    run = subprocess.run([str(binary)], capture_output=True, text=True, cwd=scratch)
    if run.returncode:
        print(f"dump_text: exit {run.returncode}\n{run.stderr}")
        return False
    wrong, counts = [], {"byte": 0, "back": 0, "muldiv": 0, "utf8": 0}
    for line in run.stdout.splitlines():
        kind, *values = line.split()
        counts[kind] += 1
        numbers = list(map(int, values))
        if kind == "byte":
            byte, code = numbers
            try:
                expected = ord(bytes([byte]).decode("cp1252"))
            except UnicodeDecodeError:
                expected = byte     # Windows maps the five unassigned bytes to the same code point
            if code != expected:
                wrong.append(line)
        elif kind == "back":
            byte, back, used_default = numbers
            if back != byte or used_default:
                wrong.append(line)
        elif kind == "utf8":
            expected = list("café € 🏰\0".encode("utf-16-le"))
            expected = [expected[i] | expected[i + 1] << 8 for i in range(0, len(expected), 2)]
            if numbers != expected:
                wrong.append(f"{line} != {expected}")
        else:
            a, b, c, result = numbers
            if result != windows_muldiv(a, b, c):
                wrong.append(f"{line} != {windows_muldiv(a, b, c)}")
    print(f"text: {counts['byte']} bytes decoded, {counts['back']} encoded back, {counts['utf8']} UTF-8 sample, "
          f"{counts['muldiv']} MulDiv cases; {len(wrong)} wrong")
    for line in wrong[:5]:
        print(f"  {line}")
    return not wrong and counts == {"byte": 255, "back": 255, "muldiv": 16, "utf8": 1}


def check_gdi_metrics(scratch, layer):
    """Font metrics against a Windows 11 capture (tests/port/linux/data, from ee-macos): True when every face resolves,
    Tahoma's line metrics are exact and Courier New's widths and extents are exact.

    Only those two have an installed stand-in with Windows' measurements: Wine's own Tahoma, and Liberation Mono, whose
    advances are all one width. Liberation Sans and Serif have no VDMX or hdmx table, so their line heights and some
    advances differ from Arial and Times New Roman by a pixel; the report gives those numbers without gating on them.
    The Windows faces in the Wine prefix (winetricks corefonts) are used first when they are there.
    """
    binary = pathlib.Path(scratch) / "dump_gdi_metrics"
    made = build(TESTS / "dump_gdi_metrics.cpp", binary, layer)
    if made.returncode:
        print(f"dump_gdi_metrics.cpp: does not build\n{made.stderr}")
        return False
    output = pathlib.Path(scratch) / "gdi_metrics.jsonl"
    run = subprocess.run([str(binary), str(TESTS / "data" / "cases_windows11.txt"), str(output)],
                         capture_output=True, text=True, cwd=scratch)
    if run.returncode:
        print(f"dump_gdi_metrics: exit {run.returncode}\n{run.stderr}")
        return False
    def load(path):
        rows = [json.loads(line) for line in path.read_text().splitlines()]
        return {(r["face"], r["height"], r["weight"], r["italic"]): r for r in rows}
    windows, ours = load(TESTS / "data" / "gdi_metrics_windows11.jsonl"), load(output)
    drawn = dict(re.findall(r'font "([^"]+)" is drawn with (.+)', run.stderr))
    faces = {}
    for key, want in windows.items():
        got = ours.get(key)
        counts = faces.setdefault(key[0], collections.Counter())
        counts["requests"] += 1
        if not got:
            continue
        counts["answered"] += 1
        counts["line exact"] += want["tm"][:5] == got["tm"][:5]
        counts["widths"] += len(want["widths"])
        counts["widths exact"] += sum(a == b for a, b in zip(want["widths"], got["widths"]))
        counts["extents"] += len(want["extents"])
        counts["extents exact"] += sum(a[0] == b[0] for a, b in zip(want["extents"], got["extents"]))
    print(f"gdi metrics: {len(windows)} Windows 11 requests, {len(ours)} answered")
    for face, c in faces.items():
        print(f"  {face} (drawn with {drawn.get(face, 'nothing logged')}): line metrics exact {c['line exact']}/"
              f"{c['requests']}, widths exact {c['widths exact']}/{c['widths']}, "
              f"extent widths exact {c['extents exact']}/{c['extents']}")
    resolved = all(face in drawn and not drawn[face].startswith("no face") for face in faces)
    tahoma, courier = faces["Tahoma"], faces["Courier New"]
    return (resolved and len(ours) == len(windows) and all(c["answered"] == c["requests"] for c in faces.values())
            and tahoma["line exact"] == tahoma["requests"]
            and courier["widths exact"] == courier["widths"] > 0
            and courier["extents exact"] == courier["extents"] > 0)


def check_linkage(layer):
    """The layer defines every Windows function with C linkage. A definition whose parameter types differ from the
    Windows header's declaration is a C++ overload instead, and the game's calls to the Windows name stay undefined."""
    overloads = []
    for obj in layer:
        symbols = subprocess.run(["nm", "-C", "--defined-only", obj], capture_output=True, text=True).stdout
        for line in symbols.splitlines():
            parts = line.split(" ", 2)
            kind, name = (parts[1], parts[2]) if len(parts) == 3 else ("", "")
            if kind == "T" and "(" in name and not name.startswith(("eeport::", "ee_port_", "(anonymous")):
                overloads.append(f"{pathlib.Path(obj).name}: {name}")
    print(f"linkage: {len(layer)} objects, {len(overloads)} functions with C++ linkage outside eeport")
    for name in overloads[:10]:
        print(f"  {name}")
    return not overloads


MOVIES = bp.ROOT / "media" / "Movies"


def ffmpeg_frame(movie, number, width, height):
    """Frame `number` (from 1) of a movie as BGRA, decoded by the system's FFmpeg: the second decoder."""
    run = subprocess.run(["ffmpeg", "-v", "error", "-i", str(movie), "-vf", f"select=eq(n\\,{number - 1})",
                          "-frames:v", "1", "-pix_fmt", "bgra", "-f", "rawvideo", "-"], capture_output=True)
    return run.stdout if len(run.stdout) == width * height * 4 else None


def mean_difference(first, second):
    return sum(abs(a - b) for a, b in zip(first, second)) / len(first)


def play(scratch, layer, name, wait, frames, env=None):
    """Runs dump_movie on a movie name; returns its report as a dict and the folder of its frames."""
    binary = pathlib.Path(scratch) / "dump_movie"
    if not binary.exists():
        made = build(TESTS / "dump_movie.cpp", binary, layer)
        if made.returncode:
            print(f"dump_movie.cpp: does not build\n{made.stderr}")
            return None, None
    out = pathlib.Path(tempfile.mkdtemp(dir=scratch))
    run = subprocess.run([str(binary), name, str(int(wait)), str(out), *map(str, frames)], capture_output=True,
                         text=True, cwd=scratch, env=dict(os.environ, SDL_AUDIODRIVER="dummy", **(env or {})))
    if run.returncode:
        print(f"dump_movie {name}: exit {run.returncode}\n{run.stderr}")
        return None, None
    report = {}
    for line in run.stdout.splitlines():
        key, _, value = line.partition(" ")
        report[key] = value
    return report, out


def check_frames(movie, report, out, frames):
    width, height = map(int, report["size"].split())
    good = True
    for number in frames:
        ours = (out / f"frame-{number}.bgra").read_bytes()
        theirs = ffmpeg_frame(movie, number, width, height)
        difference = mean_difference(ours, theirs) if theirs else None
        print(f"  frame {number}: mean difference {difference if difference is None else round(difference, 3)}")
        good = good and difference is not None and difference < 2
    return good


def check_movies(scratch, layer):
    """A made movie in the game's Movies folder replaces its .bik, plays in time and decodes as FFmpeg decodes it.
    The downloaded intro in media/Movies, when present, plays to its last frame."""
    folder = pathlib.Path(scratch) / "Movies"
    folder.mkdir()
    movie = folder / "Test.webm"
    made = subprocess.run(["ffmpeg", "-v", "error", "-f", "lavfi", "-i", "testsrc2=size=320x240:rate=15:duration=2",
                           "-f", "lavfi", "-i", "sine=frequency=440:duration=2", "-c:v", "libvpx-vp9", "-c:a",
                           "libopus", str(movie)], capture_output=True, text=True)
    if made.returncode:
        print(f"movies: the test movie is not made\n{made.stderr}")
        return False
    report, out = play(scratch, layer, "movies\\TEST.bik", True, [1, 15, 30])
    if report is None:
        return False
    seconds = float(report["seconds"])
    print(f"movies: Test.webm as movies\\TEST.bik: size {report['size']}, frames {report['frames']}, played "
          f"{report['played']}, last {report['last']}, {seconds:.3f} s")
    good = (report["size"] == "320 240" and report["frames"] == "30" and report["played"] == "30"
            and report["last"] == "30 of 30" and 1.85 < seconds < 2.3)
    good = check_frames(movie, report, out, [1, 15, 30]) and good
    intro = next((f for f in sorted(MOVIES.glob("Empire Earth.*")) if f.suffix != ".bik"), None)
    if intro is None:
        print(f"movies: no intro in {MOVIES}; the intro check did not run")
        return good
    probe = subprocess.run(["ffprobe", "-v", "error", "-count_packets", "-select_streams", "v:0", "-show_entries",
                            "stream=width,height,nb_read_packets", "-of", "csv=p=0", str(intro)],
                           capture_output=True, text=True).stdout.strip().split(",")
    width, height, frames = probe
    report, out = play(scratch, layer, "Movies\\Empire Earth.bik", False, [700],
                       env={"EE_PORT_MOVIES": str(MOVIES)})
    if report is None:
        return False
    print(f"movies: {intro.name}: size {report['size']}, frames {report['frames']}, played {report['played']}, "
          f"last {report['last']}, decode rate {report['rate']} frames per second; ffprobe: {width}x{height}, "
          f"{frames} frames")
    good = check_frames(intro, report, out, [700]) and good
    return (good and report["size"] == f"{width} {height}" and report["played"] == frames
            and report["last"] == f"{frames} of {frames}")


def check_miles(scratch, layer):
    """MP3 images through the Miles calls decode as FFmpeg's fixed-point MP3 decoder decodes them, at their own rate,
    with Miles' length in milliseconds (the decoded length) and the position at the end in the image's bytes: a made
    mono MP3 with an ID3 tag, the base game's music and the MP3s in AoC's data.ssa."""
    binary = pathlib.Path(scratch) / "dump_miles"
    made = build(TESTS / "dump_miles.cpp", binary, layer)
    if made.returncode:
        print(f"dump_miles.cpp: does not build\n{made.stderr}")
        return False
    tone = pathlib.Path(scratch) / "tone.mp3"
    made = subprocess.run(["ffmpeg", "-v", "error", "-f", "lavfi", "-i", "sine=frequency=440:sample_rate=22050:"
                           "duration=1.5", "-c:a", "libmp3lame", "-b:a", "64k", "-write_xing", "0", "-metadata",
                           "title=tone", str(tone)], capture_output=True, text=True)
    if made.returncode:
        print(f"miles: the test MP3 is not made\n{made.stderr}")
        return False
    music = sorted(SOUNDS.glob("*.mp3"))
    sys.path.insert(0, str(TOOLKIT))
    from ee_toolkit.container.ssa import Archive
    archive = Archive(AOC_DATA)
    for name in archive.names("*.mp3"):
        music.append(pathlib.Path(scratch) / pathlib.PureWindowsPath(name).name)
        music[-1].write_bytes(archive.read(name))
    if len(music) < 2:
        print(f"miles: {len(music)} MP3s in {SOUNDS} and {AOC_DATA}")
        return False
    good = True
    for sound in [tone, *music]:
        out = pathlib.Path(scratch) / f"{sound.stem}.pcm"
        run = subprocess.run([str(binary), "Z:" + str(sound).replace("/", "\\"), str(out)], capture_output=True,
                             text=True, timeout=300)
        report = dict(line.split(" ", 1) for line in run.stdout.splitlines())
        probe = subprocess.run(["ffprobe", "-v", "error", "-select_streams", "a:0", "-show_entries",
                                "stream=sample_rate,channels,duration", "-of", "csv=p=0", str(sound)],
                               capture_output=True, text=True).stdout.strip().split(",")
        rate, channels, seconds = int(probe[0]), int(probe[1]), float(probe[2])
        theirs = subprocess.run(["ffmpeg", "-v", "error", "-c:a", "mp3", "-i", str(sound), "-f", "s16le", "-"],
                                capture_output=True).stdout
        ours = out.read_bytes() if out.exists() else b""
        if channels == 1:
            same = ours[0::4] + ours[1::4] == theirs[0::2] + theirs[1::2] == ours[2::4] + ours[3::4]
        else:
            same = ours == theirs
        size = sound.stat().st_size
        frames = len(theirs) // (2 * channels)
        print(f"miles: {sound.name}: exit {run.returncode}, rate {report.get('rate')}, granularity "
              f"{report.get('granularity')}, {report.get('ms')} ms, {report.get('frames')} frames, end "
              f"{report.get('end')}; ffprobe: {rate} Hz, {channels} channels, {seconds} s; PCM "
              f"{'identical to' if same else 'differs from'} FFmpeg's")
        good = good and (run.returncode == 0 and same and report.get("rate") == str(rate)
                         and report.get("granularity") == "1" and report.get("ms") == str(round(frames * 1000 / rate))
                         and report.get("frames") == str(frames)
                         and report.get("end") == f"{size} of {size}")
    return good


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
        text_ok = check_text(scratch, layer)
        metrics_ok = check_gdi_metrics(scratch, layer)
        movies_ok = check_movies(scratch, layer)
        miles_ok = check_miles(scratch, layer)
    linkage_ok = check_linkage(layer)
    checks = [strings_ok, text_ok, metrics_ok, movies_ok, miles_ok, linkage_ok]
    print(f"tests: {len(tests) + len(checks)} in, {passed + sum(checks)} passed")
    return 0 if passed == len(tests) and all(checks) else 1


if __name__ == "__main__":
    sys.exit(main())
