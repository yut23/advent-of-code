import json
import sys
from dataclasses import asdict, dataclass
from pathlib import Path


@dataclass(order=True, frozen=True, kw_only=True)
class Config:
    directory: str
    target: str
    compiler: str


configs = set()


def add_config(directory: str, target: str) -> None:
    configs.add(Config(directory=directory, target=target, compiler="clang++"))
    configs.add(Config(directory=directory, target=target, compiler="g++"))


def main() -> None:
    target_glob = sys.argv[1]
    changed_files = json.loads(sys.argv[2])
    for file in changed_files:
        path = Path(file)
        if path.parts[:2] == (".github", "workflows"):
            for makefile in Path().glob("*/Makefile"):
                directory = makefile.parent.name
                for path in (Path(directory) / "src").glob(target_glob):
                    add_config(directory=directory, target=path.stem)
            continue
        directory = path.parts[0]
        if path.name in {"Makefile", "lib.h", "lib.hpp"} or "unit_test" in path.parts:
            for path in (Path(directory) / "src").glob(target_glob):
                add_config(directory=directory, target=path.stem)
        else:
            assert (
                path.stem.startswith("day") or path.stem.startswith("test")
            ) and path.suffix in {".cpp", ".hpp"}
            add_config(directory=directory, target=path.stem)

    if configs:
        matrix = {"include": list(map(asdict, sorted(configs)))}
    else:
        matrix = {}
    with open(sys.argv[3], "a") as f:
        f.write("matrix-combinations=")
        json.dump(matrix, f, indent=None, separators=(",", ":"))
        f.write("\n")


if __name__ == "__main__":
    main()
