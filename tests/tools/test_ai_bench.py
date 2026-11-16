"""tools/ai_bench.py: the profile merge and the rating fit, against values worked out by hand.

  python3 -I tests/tools/test_ai_bench.py
"""
import math
import pathlib
import sys
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parents[2] / "tools"))
import ai_bench  # noqa: E402


class Merge(unittest.TestCase):
    BASE = "; comment\n[general]\nenabled=0\nlead_targets=1\n\n[ai_base]\nmuster=1\n; muster=1 in a comment\n"

    def test_replaces_a_key_in_its_section(self):
        merged = ai_bench.merged_ini(self.BASE, "[general]\nenabled=1\n")
        self.assertIn("[general]\nenabled=1\nlead_targets=1\n", merged)
        self.assertNotIn("enabled=0", merged)

    def test_keeps_comments_and_other_sections(self):
        merged = ai_bench.merged_ini(self.BASE, "[ai_base]\nmuster=0\n")
        self.assertIn("; comment\n", merged)
        self.assertIn("; muster=1 in a comment\n", merged)
        self.assertIn("[ai_base]\nmuster=0\n", merged)

    def test_adds_a_new_key_at_the_end_of_its_section(self):
        merged = ai_bench.merged_ini(self.BASE, "[general]\nlob_each_shot=0\n")
        self.assertIn("lead_targets=1\n\nlob_each_shot=0\n[ai_base]", merged)

    def test_adds_a_new_section(self):
        merged = ai_bench.merged_ini(self.BASE, "[ai_hard]\nmuster=0\n")
        self.assertTrue(merged.endswith("[ai_hard]\nmuster=0\n"))

    def test_section_and_key_names_ignore_case(self):
        merged = ai_bench.merged_ini(self.BASE, "[AI_Base]\nMuster=0\n")
        self.assertIn("[ai_base]\nMuster=0\n", merged)
        self.assertNotIn("\nmuster=1\n", merged)


class Fit(unittest.TestCase):
    def test_three_wins_of_four(self):
        # a: 3 wins + half the virtual draw = 3.5 of 5 games, so a's odds are 3.5 / 1.5 and its rating 400 log10(7/3)
        ratings = ai_bench.fit([("a", "b", 1.0)] * 3 + [("a", "b", 0.0)], anchor="b")
        self.assertAlmostEqual(ratings["b"], 1000.0, places=6)
        self.assertAlmostEqual(ratings["a"], 1000 + 400 * math.log10(7 / 3), places=6)

    def test_draws_rate_equal(self):
        ratings = ai_bench.fit([("a", "b", 0.5)] * 6, anchor="b")
        self.assertAlmostEqual(ratings["a"], 1000.0, places=6)

    def test_slot_order_does_not_matter(self):
        one = ai_bench.fit([("a", "b", 1.0), ("a", "b", 0.0), ("a", "b", 1.0)], anchor="b")
        other = ai_bench.fit([("b", "a", 0.0), ("b", "a", 1.0), ("b", "a", 0.0)], anchor="b")
        self.assertAlmostEqual(one["a"], other["a"], places=6)

    def test_an_unbeaten_ai_has_a_finite_rating(self):
        # a: 10 wins + 0.5 of 11, odds 10.5 / 0.5 = 21
        ratings = ai_bench.fit([("a", "b", 1.0)] * 10, anchor="b")
        self.assertAlmostEqual(ratings["a"], 1000 + 400 * math.log10(21), places=6)

    def test_ratings_add_up_through_a_third_ai(self):
        # a beats b at odds 3, b beats c at odds 3 (prior aside): a is above c by about two steps
        games = [("a", "b", 1.0)] * 30 + [("a", "b", 0.0)] * 10 + [("b", "c", 1.0)] * 30 + [("b", "c", 0.0)] * 10
        ratings = ai_bench.fit(games, anchor="b")
        step = 400 * math.log10(3)
        self.assertAlmostEqual(ratings["a"] - 1000, step, delta=12)
        self.assertAlmostEqual(1000 - ratings["c"], step, delta=12)


class Pairs(unittest.TestCase):
    AIS = ["aoai", "aoai-honest", "aoai-towers", "classic"]

    def test_every_pair_without_opponents(self):
        self.assertEqual(ai_bench.pairs(self.AIS, None),
                         [("aoai", "aoai-honest"), ("aoai", "aoai-towers"), ("aoai", "classic"),
                          ("aoai-honest", "aoai-towers"), ("aoai-honest", "classic"), ("aoai-towers", "classic")])

    def test_opponents_play_everyone_and_the_rest_play_only_them(self):
        # 4 AIs, opponents aoai and classic: aoai-classic, and each of the 2 others against both, 5 pairs of 6
        self.assertEqual(ai_bench.pairs(self.AIS, ["aoai", "classic"]),
                         [("aoai", "aoai-honest"), ("aoai", "aoai-towers"), ("aoai", "classic"),
                          ("aoai-honest", "classic"), ("aoai-towers", "classic")])

    def test_ten_ais_against_two_opponents(self):
        ais = ["a%d" % i for i in range(8)] + ["aoai", "classic"]
        self.assertEqual(len(ai_bench.pairs(ais, ["aoai", "classic"])), 8 * 2 + 1)

    def test_an_opponent_that_is_not_an_ai_is_refused(self):
        with self.assertRaises(SystemExit):
            ai_bench.pairs(self.AIS, ["nobody"])


class Ledger(unittest.TestCase):
    def test_a_game_row_names_the_winning_ai(self):
        report = {"winner": 3, "outcome": "last-computer-standing", "game_ms": 7200000,
                  "samples": [{"players": [{"index": 2, "epoch": 3, "army": 5, "population": 50, "value": 900, "engine": "ageofai"},
                                           {"index": 3, "epoch": 4, "army": 40, "population": 200, "value": 9000, "engine": "classic"}]}]}
        row = ai_bench.game_row(report, "standard", 7, {2: "aoai", 3: "classic"})
        self.assertEqual(row["winner"], "classic")
        self.assertEqual(row["game_s"], 7200)
        self.assertEqual(row["end"]["aoai"]["army"], 5)
        self.assertEqual(ai_bench.scored([row]), [("aoai", "classic", 0.0)])

    def test_no_winner_is_a_draw(self):
        report = {"winner": None, "outcome": "time-cap", "game_ms": 28800000, "samples": [{"players": []}]}
        row = ai_bench.game_row(report, "standard", 7, {2: "aoai", 3: "classic"})
        self.assertIsNone(row["winner"])
        self.assertEqual(ai_bench.scored([row]), [("aoai", "classic", 0.5)])

    def test_a_failed_run_is_not_a_game(self):
        report = {"winner": None, "outcome": "error", "error": "timed out waiting for the game (top form 26)",
                  "game_ms": 0, "samples": []}
        self.assertIsNone(ai_bench.game_row(report, "standard", 6, {2: "aoai", 3: "classic"}))


if __name__ == "__main__":
    unittest.main()
