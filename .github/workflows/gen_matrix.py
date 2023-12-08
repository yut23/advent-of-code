import json
import re
import sys
from collections import deque
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
    target_prefix = sys.argv[1]
    assert target_prefix in {"day", "test"}
    target_glob = target_prefix + "*.cpp"

    changed_files: deque[Path] = deque()
    print("got changed files:")
    for f in json.loads(sys.argv[2]):
        print(f"  {f}")
        changed_files.append(Path(f))

    while changed_files:
        path = changed_files.popleft()

        if path.parts[:2] == (".github", "workflows"):
            # changed a workflow file, so include everything in all base
            # directories
            for makefile in Path().glob("*/Makefile"):
                for path in (makefile.parent / "src").glob(target_glob):
                    changed_files.append(path)
            continue

        base_directory = path.parts[0]
        if path.name in {"Makefile", "lib.h", "lib.hpp"} or "unit_test" in path.parts:
            # changed a Makefile or some library code, so include everything in
            # that base directory
            for path in (Path(base_directory) / "src").glob(target_glob):
                changed_files.append(path)
            continue

        m = re.search(r"(day|test)(\d+.*)", path.stem)
        assert m is not None and path.suffix in {".cpp", ".hpp"}
        if Path(f"{base_directory}/src/{target_prefix}{m[2]}.cpp").exists():
            add_config(directory=base_directory, target=target_prefix + m[2])

    matrix = {}
    if configs:
        config_list = sorted(configs)
        includes = []
        print("\nconfigurations:")
        for config in config_list:
            includes.append(asdict(config))
            print(f"  {config}")
        matrix["include"] = includes
    with open(sys.argv[3], "a") as f:
        f.write("matrix-combinations=")
        json.dump(matrix, f, indent=None, separators=(",", ":"))
        f.write("\n")


if __name__ == "__main__":
    main()
