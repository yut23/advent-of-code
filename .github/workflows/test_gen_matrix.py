from gen_matrix import ROOT, WORKFLOWS_DIR, Matrix, get_dependencies

ACTIONS_DIR = ROOT / ".github/actions"


def test_get_dependencies_cpp():
    base_dir = ROOT / "2023"
    src = base_dir / "src"
    aoc_lib = base_dir.parent / "aoc_lib"
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


def matrix_helper(*changed_files: str) -> dict[str, set[str]]:
    result = {}
    for mode in ("build", "answer", "unit"):
        matrix = Matrix(mode)
        for f in changed_files:
            matrix.process_changed_file(ROOT / f)
        result[mode] = {
            "{directory}/{name}".format(**target.to_dict()) for target in matrix.targets
        }
    return result


def test_matrix():
    # pylint: disable=use-dict-literal
    everything = matrix_helper("2022/Makefile", "2023/Makefile")

    # C++ source files
    assert matrix_helper("2023/src/day05.cpp") == dict(
        build={"2023/day05"}, answer={"2023/day05"}, unit=set()
    )
    assert matrix_helper("2023/src/day05.hpp") == dict(
        build={"2023/day05", "2023/test05"}, answer={"2023/day05"}, unit={"2023/test05"}
    )
    assert matrix_helper("2023/src/test05.cpp") == dict(
        build={"2023/test05"}, answer=set(), unit={"2023/test05"}
    )

    # answer tests
    assert matrix_helper("2023/answer_tests/day01/example2.txt") == dict(
        build=set(), answer={"2023/day01"}, unit=set()
    )

    # workflow files
    assert matrix_helper(".github/workflows/gen_matrix.py") == everything
    assert matrix_helper(".github/workflows/generate-matrix.yml") == everything
    assert matrix_helper(".github/workflows/docker-build.yml") == everything

    assert matrix_helper(".github/workflows/test-build.yml") == dict(
        build=everything["build"], answer=set(), unit=set()
    )
    assert matrix_helper(".github/workflows/answer-tests.yml") == dict(
        build=set(), answer=everything["answer"], unit=set()
    )
    assert matrix_helper(".github/workflows/unit-tests.yml") == dict(
        build=set(), answer=set(), unit=everything["unit"]
    )

    # workflow action helpers
    assert matrix_helper(".github/actions/build-action/action.yml") == matrix_helper(
        ".github/workflows/test-build.yml"
    )
    assert matrix_helper(
        ".github/actions/answer-test-action/action.yml"
    ) == matrix_helper(".github/workflows/answer-tests.yml")
    assert matrix_helper(
        ".github/actions/unit-test-action/action.yml"
    ) == matrix_helper(".github/workflows/unit-tests.yml")
