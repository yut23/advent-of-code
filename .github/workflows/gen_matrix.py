from __future__ import annotations

import functools
import json
import re
import sys
from collections import defaultdict
from dataclasses import dataclass
from pathlib import Path


@functools.cache
def get_includes(path: Path) -> frozenset[Path]:
    assert path.suffix in {".cpp", ".hpp", ".h"}
    include_pat = re.compile(r'#include\s+"(.*?)"')
    includes: set[Path] = set()
    with open(path, "r") as f:
        for line in f:
            if (m := include_pat.search(line)) is not None:
                includes.add(path.parent / m[1])
    return frozenset(includes)


@functools.cache
def get_transitive_includes(path: Path) -> frozenset[Path]:
    # simple DFS on get_includes(), using functools.cache for memoization
    all_includes: set[Path] = set()
    for p in get_includes(path):
        all_includes.add(p)
        all_includes.update(get_transitive_includes(p))
    return frozenset(all_includes)


@dataclass(order=True, frozen=True, kw_only=True)
class Target:
    base_dir: Path
    day: int
    prefix: str
    extra: str

    @property
    def suffix(self) -> str:
        return f"{self.day:02d}{self.extra}"

    def __str__(self) -> str:
        return f"{self.prefix}{self.day:02d}{self.extra}"

    @classmethod
    def from_file(cls, base_dir: Path, source_file: Path) -> Target:
        m = re.match(
            r"(?P<prefix>day|test)(?P<day>\d+)(?P<extra>.*)$", source_file.stem
        )
        assert m is not None, source_file
        return cls(
            base_dir=base_dir, day=int(m["day"]), prefix=m["prefix"], extra=m["extra"]
        )

    def get_deps(self, mode: str) -> frozenset[Path]:
        deps = set()
        src = self.base_dir / "src"
        deps.add(src / f"{self}.cpp")
        for included_file in get_transitive_includes(src / f"{self}.cpp"):
            deps.add(included_file)
        deps.add(self.base_dir / "Makefile")
        if mode == "answer":
            for answer_test in self.base_dir.glob(f"answer_tests/{self}/*"):
                deps.add(answer_test)
                deps.add(self.base_dir / f"input/{self}/{answer_test.name}")
            run_answer_tests = self.base_dir / "run_answer_tests.sh"
            assert run_answer_tests.exists()
            deps.add(run_answer_tests.resolve())
        return frozenset(deps)


@dataclass(order=True, frozen=True, kw_only=True)
class Config:
    target: Target
    compiler: str
    stdlib: str = "libstdc++"

    def to_dict(self) -> dict[str, str]:
        return {
            "directory": str(self.target.base_dir),
            "target": str(self.target),
            "compiler": self.compiler,
            "stdlib": self.stdlib,
        }


def enumerate_targets(base_dir: Path) -> list[Target]:
    return sorted(
        Target.from_file(base_dir, file)
        for file in base_dir.glob("src/*.cpp")
        if file.stem != "template"
    )


def find_base_dirs() -> list[Path]:
    return sorted(makefile.parent for makefile in Path().glob("*/Makefile"))


class Matrix:
    def __init__(self, mode: str) -> None:
        if mode == "unit":
            target_pat = re.compile("test")
        elif mode == "build":
            target_pat = re.compile("day|test")
        elif mode == "answer":
            target_pat = re.compile("day")
        self.configs: set[Config] = set()

        self.targets: list[Target] = []
        for base_dir in find_base_dirs():
            if mode == "answer" and not (base_dir / "run_answer_tests.sh").exists():
                continue
            for target in enumerate_targets(base_dir):
                if target_pat.match(target.prefix):
                    self.targets.append(target)
        self.targets.sort()

        self.file_lookup: dict[Path, list[Target]] = defaultdict(list)
        for target in self.targets:
            for dep in target.get_deps(mode):
                self.file_lookup[dep].append(target)

        self.file_lookup[Path(".github/workflows/gen_matrix.py")] = self.targets
        for file in Path().glob(".github/workflows/*.yml"):
            with open(file, "r") as f:
                if mode in file.name and "gen_matrix.py" in f.read():
                    self.file_lookup[file] = self.targets

    def process_changed_file(self, file: Path) -> None:
        for target in self.file_lookup.get(file, ()):
            self.add_config(target)

    def add_config(self, target: Target) -> None:
        self.configs.add(Config(target=target, compiler="clang++"))
        self.configs.add(Config(target=target, compiler="clang++-17", stdlib="libc++"))
        self.configs.add(Config(target=target, compiler="g++"))

    def write_combinations(self, output_file: str) -> None:
        matrix = {}
        if self.configs:
            config_list = sorted(self.configs)
            includes = []
            print("\nconfigurations:")
            for config in config_list:
                includes.append(config.to_dict())
                print(f"  {config}")
            matrix["include"] = includes
        with open(output_file, "a") as f:
            f.write("matrix-combinations=")
            json.dump(matrix, f, indent=None, separators=(",", ":"))
            f.write("\n")


def main() -> None:
    changed_files_json = sys.argv[1]
    output_file = sys.argv[2]
    mode = sys.argv[3]

    matrix = Matrix(mode)

    for f in json.loads(changed_files_json):
        print(f"  {f}")
        matrix.process_changed_file(Path(f))

    matrix.write_combinations(output_file)


if __name__ == "__main__":
    main()
