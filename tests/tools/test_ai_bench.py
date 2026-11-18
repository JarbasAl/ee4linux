"""tools/ai_bench.py: the profile merge and the rating fit, against values worked out by hand.

  python3 -I tests/tools/test_ai_bench.py
"""
import math
import concurrent.futures
import pathlib
import sys
import threading
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


class Engines(unittest.TestCase):
    def test_the_shipped_profile_selects_the_classic_ai(self):
        self.assertEqual(ai_bench.engine_of(ai_bench.merged_ini(ai_bench.BASE_INI.read_text(), "[ai_base]\nmuster=1\n")),
                         "classic")

    def test_an_ai_profile_selects_age_of_ai(self):
        self.assertEqual(ai_bench.engine_of(ai_bench.merged_ini(ai_bench.BASE_INI.read_text(),
                                                                "[general]\nenabled=1\n")), "ageofai")

    def test_a_name_for_a_flag_reads_like_the_number(self):
        self.assertEqual(ai_bench.engine_of("[general]\nenabled=true\n"), "ageofai")
        self.assertEqual(ai_bench.engine_of("[general]\nenabled=off\n"), "classic")

    def test_enabled_in_another_section_selects_nothing(self):
        # [ai_base] has a member row named like the general flag: only [general] says which engine a player runs
        self.assertEqual(ai_bench.engine_of("[ai_base]\nenabled=1\n"), "classic")

    def test_a_value_it_cannot_read_is_an_error_not_a_guess(self):
        with self.assertRaises(SystemExit):
            ai_bench.engine_of("[general]\nenabled=maybe\n")


class Budget(unittest.TestCase):
    def test_slots_go_back_when_a_call_ends(self):
        budget = ai_bench.Budget(8)
        budget.take(2)
        budget.take(2)
        budget.give(2)
        budget.take(2)
        self.assertEqual(budget.mUsed, 4)

    def test_a_stopped_run_fails_the_calls_that_wait(self):
        # a run that ends must not leave a worker waiting for slots for ever: the pool would never close
        budget = ai_bench.Budget(2)
        budget.take(2)
        budget.stop()
        with self.assertRaises(concurrent.futures.CancelledError):
            budget.take(1)

    def test_stop_wakes_a_call_that_already_waits(self):
        budget = ai_bench.Budget(1)
        budget.take(1)
        waited = []
        def waiting():
            try:
                budget.take(1)
            except concurrent.futures.CancelledError:
                waited.append(True)
        thread = threading.Thread(target=waiting)
        thread.start()
        thread.join(0.2)
        budget.stop()
        thread.join(2)
        self.assertEqual(waited, [True])

    def test_a_call_waits_while_the_slots_are_taken(self):
        # two seeds with eight slots: the fifth call has to wait for one to end
        budget = ai_bench.Budget(8)
        for _ in range(4):
            budget.take(2)
        waited = threading.Thread(target=budget.take, args=(2,))
        waited.start()
        waited.join(0.2)
        self.assertTrue(waited.is_alive(), "a fifth call must wait")
        budget.give(2)
        waited.join(2)
        self.assertFalse(waited.is_alive(), "and go when a call ends")


class HeadToHead(unittest.TestCase):
    def test_a_pair_counts_both_seat_orders(self):
        rows = [{"scenario": "s", "seats": {"2": "a", "3": "b"}, "winner": "a"},
                {"scenario": "s", "seats": {"2": "b", "3": "a"}, "winner": "a"}]
        self.assertEqual(ai_bench.head_to_head(rows), {("a", "b"): [2, 2, 0]})

    def test_the_pair_is_named_with_the_lower_name_first(self):
        rows = [{"scenario": "s", "seats": {"2": "b", "3": "a"}, "winner": "b"}]
        self.assertEqual(ai_bench.head_to_head(rows), {("a", "b"): [1, 0, 0]})

    def test_a_draw_is_a_game_without_a_win(self):
        rows = [{"scenario": "s", "seats": {"2": "a", "3": "b"}, "winner": None}]
        self.assertEqual(ai_bench.head_to_head(rows), {("a", "b"): [1, 0, 1]})


class SeatShares(unittest.TestCase):
    def test_a_scenario_whose_games_go_to_one_seat_says_so(self):
        # the same map and AI order, twice: the first seat wins both, so the seat decided them
        rows = [{"scenario": "raids", "seats": {"2": "a", "3": "b"}, "winner": "a"},
                {"scenario": "raids", "seats": {"2": "b", "3": "a"}, "winner": "b"}]
        self.assertEqual(ai_bench.seat_shares(rows), {"raids": [2, 2]})

    def test_it_counts_the_first_seat_by_its_number(self):
        rows = [{"scenario": "tiny", "seats": {"2": "a", "3": "b"}, "winner": "b"},
                {"scenario": "tiny", "seats": {"2": "a", "3": "b"}, "winner": "a"}]
        self.assertEqual(ai_bench.seat_shares(rows), {"tiny": [2, 1]})

    def test_a_draw_is_not_a_decided_game(self):
        rows = [{"scenario": "poor", "seats": {"2": "a", "3": "b"}, "winner": None}]
        self.assertEqual(ai_bench.seat_shares(rows), {"poor": [0, 0]})

    def test_every_scenario_has_a_row(self):
        rows = [{"scenario": s, "seats": {"2": "a", "3": "b"}, "winner": "a"} for s in ("tiny", "raids")]
        self.assertEqual(sorted(ai_bench.seat_shares(rows)), ["raids", "tiny"])


class SeatEngines(unittest.TestCase):
    def row(self, first_engine, second_engine):
        report = {"winner": 2, "outcome": "last-computer-standing", "game_ms": 60000,
                  "samples": [{"players": [{"index": 2, "engine": first_engine},
                                           {"index": 3, "engine": second_engine}]}]}
        return ai_bench.game_row(report, "standard", 1, {2: "aoai", 3: "classic"})

    def test_a_game_of_the_profiles_it_claims_is_kept(self):
        self.assertEqual(ai_bench.wrong_engines(self.row("ageofai", "classic"), {"aoai": "ageofai", "classic": "classic"}), {})

    def test_an_ai_that_ran_the_classic_one_names_itself(self):
        # the profile path did not resolve: the player read no file and took the classic engine
        self.assertEqual(ai_bench.wrong_engines(self.row("classic", "classic"), {"aoai": "ageofai", "classic": "classic"}),
                         {"aoai": "classic"})

    def test_a_player_with_no_engine_names_itself(self):
        self.assertEqual(ai_bench.wrong_engines(self.row(None, "classic"), {"aoai": "ageofai", "classic": "classic"}),
                         {"aoai": None})


if __name__ == "__main__":
    unittest.main()
