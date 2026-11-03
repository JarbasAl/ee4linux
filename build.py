"""Build entry point: runs tools/build.py with the same arguments (python -I build.py --help)."""
import pathlib
import runpy
import sys

sys.argv[0] = str(pathlib.Path(__file__).resolve().parent / "tools" / "build.py")
sys.path.insert(0, str(pathlib.Path(sys.argv[0]).parent))
runpy.run_path(sys.argv[0], run_name="__main__")
