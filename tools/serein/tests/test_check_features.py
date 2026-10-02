import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

import check_features


class CheckFeaturesTest(unittest.TestCase):
    def test_accepts_defined_values(self):
        text = (
            "| ID | 功能 | 来源 | 状态 | 优先级 |\n"
            "| SG-MENU-01 | Select `a | b` range | Ad Na | Implemented | P2 |\n"
            "| SG-CORE-02 | Codegen | D | Verified | P0 |\n"
        )
        self.assertEqual(check_features.problems(text), [])

    def test_rejects_undefined_status_priority_and_source(self):
        text = "| SG-MENU-01 | Feature | Xx | Done | P5 |\n"
        found = check_features.problems(text)
        self.assertEqual(len(found), 3)
        self.assertIn("undefined status 'Done'", found[0])

    def test_rejects_duplicate_and_malformed_ids(self):
        text = (
            "| SG-MENU-01 | One | Ad | Planned | P1 |\n"
            "| SG-MENU-01 | Two | Ad | Planned | P1 |\n"
            "| SG-menu-3 | Three | Ad | Planned | P1 |\n"
        )
        found = check_features.problems(text)
        self.assertTrue(any("already used on line 1" in item for item in found))
        self.assertTrue(any("malformed ID 'SG-menu-3'" in item for item in found))

    def test_repository_matrix_is_valid(self):
        root = Path(__file__).resolve().parents[3]
        text = (root / "docs/serein/features.md").read_text(encoding="utf-8")
        self.assertEqual(check_features.problems(text), [])


if __name__ == "__main__":
    unittest.main()
