"""Compile resource scripts with RC and link resource-only DLLs."""
import os
import pathlib
import subprocess
import sys
sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
from ee import TOOLCHAIN
PKG = TOOLCHAIN / "msvc6.0"


def run(cmd, cwd, path_dir):
    env = dict(os.environ, PATH=str(path_dir) + os.pathsep + os.environ["PATH"])
    r = subprocess.run(cmd, cwd=cwd, env=env, capture_output=True, text=True,
                       creationflags=getattr(subprocess, "CREATE_NO_WINDOW", 0))
    return r.returncode, (r.stdout + r.stderr).strip()


RC_EXE = TOOLCHAIN / "msvc7.0" / "Bin" / "rc.exe"


def compile_rc(rc, out, rc_exe=RC_EXE):
    rc, out = pathlib.Path(rc).resolve(), pathlib.Path(out).resolve()
    out.mkdir(parents=True, exist_ok=True)
    res = out / f"{rc.stem}.res"
    code, log = run([str(rc_exe), "/fo", str(res), rc.name], rc.parent, pathlib.Path(rc_exe).parent)
    if code:
        sys.exit(f"rc {rc}: exit {code}\n{log}")
    return res


def build_dll(rc, out, dll, rc_exe=RC_EXE):
    out = pathlib.Path(out).resolve()
    res = compile_rc(rc, out, rc_exe)
    stem = pathlib.Path(dll).stem
    code, log = run([str(PKG / "Bin" / "LINK.EXE"), "/nologo", "/DLL", "/NOENTRY", "/DEBUG", "/INCREMENTAL:NO",
                     "/MACHINE:I386", f"/PDB:{stem}.pdb", f"/OUT:{dll}", res.name], out, PKG / "Bin")
    print(f"link: exit {code}" + (f"\n{log}" if log else ""))
    if code:
        sys.exit(1)
    return out / dll
