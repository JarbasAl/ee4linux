"""tools/sim.py: the paths of the options it hands the game, against the stage folder a run plays in.

  python3 -I tests/tools/test_sim.py
"""
import pathlib
import sys
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parents[2] / "tools"))
import sim  # noqa: E402


class PlayerIni(unittest.TestCase):
    def test_a_relative_profile_is_named_in_full(self):
        # a run plays in a stage folder of its own, so "bench/aoai.ini" names nothing there
        options = sim.absolute_profiles(["--player-ini", "3=bench/aoai.ini"])
        self.assertEqual(options[0], "--player-ini")
        self.assertTrue(pathlib.Path(options[1].split("=", 1)[1]).is_absolute())
        self.assertTrue(options[1].endswith("/bench/aoai.ini"))

    def test_a_profile_already_named_in_full_is_kept(self):
        self.assertEqual(sim.absolute_profiles(["--player-ini", "2=/tmp/a b.ini"]), ["--player-ini", "2=/tmp/a b.ini"])

    def test_every_player_ini_is_named_in_full(self):
        options = sim.absolute_profiles(["--player-ini", "2=a.ini", "--ai", "2", "--player-ini", "3=b.ini"])
        self.assertTrue(options[1].endswith("/a.ini"))
        self.assertEqual(options[4], "--player-ini")
        self.assertTrue(options[5].endswith("/b.ini"))

    def test_the_other_options_are_left_alone(self):
        options = ["--ai", "2", "--lobby", "181=Standard - Low", "--map-size", "Tiny"]
        self.assertEqual(sim.absolute_profiles(options), options)

    def test_a_player_ini_with_no_file_is_left_alone(self):
        # the game answers "takes a player number 1..15" for the pairs it cannot use
        self.assertEqual(sim.absolute_profiles(["--player-ini"]), ["--player-ini"])
        self.assertEqual(sim.absolute_profiles(["--player-ini", "2"]), ["--player-ini", "2"])


if __name__ == "__main__":
    unittest.main()
