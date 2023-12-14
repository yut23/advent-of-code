from __future__ import annotations

import json
import re
import sys
from collections import defaultdict
from dataclasses import dataclass
from pathlib import Path


@dataclass(order=True, frozen=True, kw_only=True)
class Target:
    base_dir: Path
    day: int
    prefix: str

    @property
    def suffix(self) -> str:
        return f"{self.day:02d}"

    def __str__(self) -> str:
        return f"{self.prefix}{self.day:02d}"

    @classmethod
    def from_file(cls, base_dir: Path, source_file: Path) -> Target:
        m = re.match(r"(?P<prefix>day|test)(?P<day>\d+)$", source_file.stem)
        assert m is not None, source_file
        return cls(base_dir=base_dir, day=int(m["day"]), prefix=m["prefix"])

    def get_deps(self) -> set[Path]:
        deps = set()
        src = self.base_dir / "src"
        deps.add(src / f"{self}.cpp")
        deps.add(src / f"day{self.suffix}.hpp")
        deps.add(src / "lib.hpp")
        deps.add(src / "lib.h")
        if self.prefix == "test" and (src / "unit_test").exists():
            for p in (src / "unit_test").iterdir():
                deps.add(p)
        deps.add(self.base_dir / "Makefile")
        return deps


@dataclass(order=True, frozen=True, kw_only=True)
class Config:
    target: Target
    compiler: str

    def to_dict(self) -> dict[str, str]:
        return {
            "directory": str(self.target.base_dir),
            "target": str(self.target),
            "compiler": self.compiler,
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
    def __init__(self, target_regex: str | None = None) -> None:
        if target_regex is not None:
            target_pat = re.compile(target_regex)
        else:
            target_pat = None
        self.configs: set[Config] = set()

        self.targets: list[Target] = []
        for base_dir in find_base_dirs():
            for target in enumerate_targets(base_dir):
                if target_pat is None or target_pat.match(target.prefix):
                    self.targets.append(target)
        self.targets.sort()

        self.file_lookup: dict[Path, list[Target]] = defaultdict(list)
        for target in self.targets:
            for dep in target.get_deps():
                self.file_lookup[dep].append(target)

        self.file_lookup[Path(".github/workflows/gen_matrix.py")] = self.targets
        for file in Path().glob(".github/workflows/*.yml"):
            with open(file, "r") as f:
                if "gen_matrix.py" in f.read():
                    self.file_lookup[file] = self.targets

    def process_changed_file(self, file: Path) -> None:
        for target in self.file_lookup.get(file, ()):
            self.add_config(target)

    def add_config(self, target: Target) -> None:
        self.configs.add(Config(target=target, compiler="clang++"))
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
    target_regex = sys.argv[3] if len(sys.argv) > 3 else None

    matrix = Matrix(target_regex)

    for f in json.loads(changed_files_json):
        print(f"  {f}")
        matrix.process_changed_file(Path(f))

    matrix.write_combinations(output_file)


if __name__ == "__main__":
    main()
