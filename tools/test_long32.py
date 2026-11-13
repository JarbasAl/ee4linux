"""Tests of the x86-64 build's long rewrite, tools/long32.py (python3 -I tools/test_long32.py).

Each case is a piece of C++ source and the text the rewrite must give for it, worked out by hand from the C++ rules
for the keyword long and for integer literal suffixes. A last check rewrites every file of the copied trees and
compares the line counts, because a compiler message must name the line of the original file. The report gives the
tests in and the tests that passed.
"""
import pathlib
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import long32

CASES = [
    # the keyword
    ("long x;", "int x;"),
    ("unsigned long x;", "unsigned int x;"),
    ("signed long x;", "signed int x;"),
    ("long int x;", " int x;"),
    ("unsigned long int x;", "unsigned  int x;"),
    ("int long x;", "int  x;"),
    ("long long x;", "long long x;"),
    ("unsigned long long x;", "unsigned long long x;"),
    ("long double x;", "long double x;"),
    ("double long x;", "double long x;"),
    ("long /* c */ long x;", "long /* c */ long x;"),
    ("const long& f(long a, long *b);", "const int& f(int a, int *b);"),
    ("sizeof(long)", "sizeof(int)"),
    ("(unsigned long)(p)", "(unsigned int)(p)"),
    ("std::vector<long> v;", "std::vector<int> v;"),
    ("longer = long_name + mylong;", "longer = long_name + mylong;"),
    ("#define LONG long\n", "#define LONG int\n"),
    # what is not source
    ("// a long comment\nlong x;", "// a long comment\nint x;"),
    ("/* long\nlong */ long x;", "/* long\nlong */ int x;"),
    ('"long 0L"', '"long 0L"'),
    ("L\"long\"", "L\"long\""),
    ("'L'", "'L'"),
    # integer literals
    ("0L", "0"),
    ("10UL + 3LU + 7ul", "10U + 3U + 7u"),
    ("0xFFL", "0xFF"),
    ("0x7fffffffUL", "0x7fffffffU"),
    ("5LL + 6ULL + 7llu", "5LL + 6ULL + 7llu"),
    ("1.0L + 2.5e3L", "1.0L + 2.5e3L"),
    ("0x1p3L", "0x1p3L"),
    ("1e-5L", "1e-5L"),
    ("x0L + L1", "x0L + L1"),
    ("a[2L]", "a[2]"),
    ("10", "10"),
]


def main():
    passed = 0
    for source, expected in CASES:
        got = long32.rewrite(source)
        if got == expected:
            passed += 1
        else:
            print(f"FAIL {source!r}: {got!r}, expected {expected!r}")
    files = moved = 0
    for tree in long32.TREES:
        for path in tree.rglob("*"):
            if path.is_file() and long32.copied(path):
                files += 1
                text = path.read_text(encoding="latin-1")
                if long32.rewrite(text).count("\n") != text.count("\n"):
                    moved += 1
                    print(f"FAIL line count changes: {path.relative_to(long32.ROOT)}")
    print(f"line counts: {files} files in, {files - moved} unchanged")
    passed += moved == 0 and files > 0
    print(f"tests: {len(CASES) + 1} in, {passed} passed")
    return passed != len(CASES) + 1


if __name__ == "__main__":
    sys.exit(main())
