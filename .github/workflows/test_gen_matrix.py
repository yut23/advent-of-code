from pathlib import Path

from gen_matrix import (
    ROOT,
    WORKFLOWS_DIR,
    Matrix,
    get_dependencies,
    get_transitive_dependencies,
)

ACTIONS_DIR = ROOT / ".github/actions"


def test_get_dependencies_cpp():
    base_dir = ROOT / "2023"
    src = base_dir / "src"
    aoc_lib = base_dir.parent / "aoc_lib/src"
    include_dirs = (src, aoc_lib)
    assert get_dependencies(src / "day01.cpp", *include_dirs) == {aoc_lib / "lib.hpp"}
    assert get_dependencies(src / "day05.cpp", *include_dirs) == {
        src / "day05.hpp",
        aoc_lib / "lib.hpp",
    }
    assert get_dependencies(src / "day05.hpp", *include_dirs) == {aoc_lib / "lib.hpp"}
    assert get_dependencies(src / "test05.cpp", *include_dirs) == {
        src / "day05.hpp",
        aoc_lib / "unit_test/unit_test.hpp",
    }

    assert get_dependencies(ROOT / "2022/src/day16.cpp", *include_dirs) == {
        ROOT / "2022/src/lib.h"
    }


def test_get_transitive_dependencies():
    base_dir = ROOT / "2023"
    src = base_dir / "src"
    aoc_lib = base_dir.parent / "aoc_lib/src"
    include_dirs = (src, aoc_lib)
    assert get_transitive_dependencies(src / "day22.cpp", *include_dirs) == frozenset(
        {
            src / "day22.hpp",
            aoc_lib / "lib.hpp",
            aoc_lib / "util/hash.hpp",
            aoc_lib / "util/concepts.hpp",
            aoc_lib / "graph_traversal.hpp",
        }
    )

    assert get_transitive_dependencies(aoc_lib / "test_ds.cpp", aoc_lib) == frozenset(
        {
            aoc_lib / "ds/grid.hpp",
            aoc_lib / "ds/pairing_heap.hpp",
            aoc_lib / "unit_test/pretty_print.hpp",
            aoc_lib / "unit_test/unit_test.hpp",
            aoc_lib / "util/util.hpp",
            aoc_lib / "lib.hpp",
            aoc_lib / "util/hash.hpp",
            aoc_lib / "util/concepts.hpp",
        }
    )


def test_get_dependencies_workflows():
    assert get_dependencies(WORKFLOWS_DIR / "unit-tests.yml") == {
        WORKFLOWS_DIR / "generate-matrix.yml",
        WORKFLOWS_DIR / "docker-build.yml",
        ACTIONS_DIR / "unit-test-action/action.yml",
    }
    assert get_dependencies(WORKFLOWS_DIR / "test-build.yml") == {
        WORKFLOWS_DIR / "generate-matrix.yml",
        WORKFLOWS_DIR / "docker-build.yml",
        ACTIONS_DIR / "build-action/action.yml",
    }
    assert get_dependencies(WORKFLOWS_DIR / "generate-matrix.yml") == {
        WORKFLOWS_DIR / "gen_matrix.py"
    }
    assert get_dependencies(WORKFLOWS_DIR / "answer-tests.yml") == {
        WORKFLOWS_DIR / "generate-matrix.yml",
        WORKFLOWS_DIR / "docker-build.yml",
        ACTIONS_DIR / "answer-test-action/action.yml",
    }


class MatrixHelper:
    def __init__(self) -> None:
        self.matrices = {mode: Matrix(mode) for mode in ("build", "answer", "unit")}

    def get_targets(self, *changed_files: str | Path) -> dict[str, set[str]]:
        result = {}
        for mode in ("build", "answer", "unit"):
            matrix = self.matrices[mode]
            for f in changed_files:
                matrix.process_changed_file(ROOT / f)
            result[mode] = {
                "{directory}/{name}".format(**target.to_dict())
                for target in matrix.targets
            }
            matrix.targets.clear()
        return result


def test_matrix():
    # pylint: disable=use-dict-literal
    helper = MatrixHelper()
    everything = helper.get_targets(*ROOT.glob("*/Makefile"), "tools/cpp/Makefile")

    # C++ source files
    assert helper.get_targets("2023/src/day05.cpp") == dict(
        build={"2023/day05"}, answer={"2023/day05"}, unit=set()
    )
    assert helper.get_targets("2023/src/day05.hpp") == dict(
        build={"2023/day05", "2023/test05"},
        answer={"2023/day05"},
        unit={"2023/test05"},
    )
    assert helper.get_targets("2023/src/test05.cpp") == dict(
        build={"2023/test05"}, answer=set(), unit={"2023/test05"}
    )

    # 2022 doesn't use aoc_lib/src/lib.hpp
    matrix = Matrix("build")
    matrix.process_changed_file(ROOT / "aoc_lib/src/lib.hpp")
    assert all(target.base_dir != ROOT / "2022" for target in matrix.targets)

    # unit tests for aoc_lib
    assert helper.get_targets("aoc_lib/src/test_graph.cpp") == dict(
        build={"aoc_lib/test_graph"},
        answer=set(),
        unit={"aoc_lib/test_graph"},
    )
    assert "aoc_lib/test_lib" in helper.get_targets("aoc_lib/src/lib.hpp")["unit"]
    assert (
        "aoc_lib/test_graph"
        in helper.get_targets("aoc_lib/src/graph_traversal.hpp")["unit"]
    )
    assert "aoc_lib/test_ds" in helper.get_targets("aoc_lib/src/ds/grid.hpp")["unit"]
    assert (
        "aoc_lib/test_ds"
        in helper.get_targets("aoc_lib/src/ds/pairing_heap.hpp")["unit"]
    )
    assert (
        "aoc_lib/test_unit_test"
        in helper.get_targets("aoc_lib/src/unit_test/unit_test.hpp")["unit"]
    )

    # answer tests
    assert helper.get_targets("2023/answer_tests/day01/example2.txt") == dict(
        build=set(), answer={"2023/day01"}, unit=set()
    )

    # workflow files
    assert helper.get_targets(".github/workflows/gen_matrix.py") == everything
    assert helper.get_targets(".github/workflows/generate-matrix.yml") == everything
    assert helper.get_targets(".github/workflows/docker-build.yml") == everything

    assert helper.get_targets(".github/workflows/test-build.yml") == dict(
        build=everything["build"], answer=set(), unit=set()
    )
    assert helper.get_targets(".github/workflows/answer-tests.yml") == dict(
        build=set(), answer=everything["answer"], unit=set()
    )
    assert helper.get_targets(".github/workflows/unit-tests.yml") == dict(
        build=set(), answer=set(), unit=everything["unit"]
    )

    # workflow action helpers
    assert helper.get_targets(
        ".github/actions/build-action/action.yml"
    ) == helper.get_targets(".github/workflows/test-build.yml")
    assert helper.get_targets(
        ".github/actions/answer-test-action/action.yml"
    ) == helper.get_targets(".github/workflows/answer-tests.yml")
    assert helper.get_targets(
        ".github/actions/unit-test-action/action.yml"
    ) == helper.get_targets(".github/workflows/unit-tests.yml")
