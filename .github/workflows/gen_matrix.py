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
    changed_files = json.loads(sys.argv[1])
    for file in changed_files:
        path = Path(file)
        directory = path.parts[0]
        if path.name in {"Makefile", "lib.h", "lib.hpp"}:
            for path in (Path(directory) / "src").glob("day*.cpp"):
                add_config(directory=directory, target=path.stem)
        else:
            assert path.stem.startswith("day") and path.suffix == ".cpp"
            add_config(directory=directory, target=path.stem)

    if configs:
        matrix = {"include": list(map(asdict, sorted(configs)))}
    else:
        matrix = {}
    with open(sys.argv[2], "a") as f:
        f.write("matrix-combinations=")
        json.dump(matrix, f, indent=None, separators=(",", ":"))
        f.write("\n")


if __name__ == "__main__":
    main()
