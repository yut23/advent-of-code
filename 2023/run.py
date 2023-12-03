#!/usr/bin/env python3

import argparse
import datetime
import importlib
import math
import statistics
import sys
import time
import timeit
from pathlib import Path
from typing import Any, Dict, Optional

ROOT = Path(__file__).parent.resolve()
TODAY = min(datetime.datetime.now(), datetime.datetime(2023, 12, 25)).day

# add the source directory to the beginning of Python's search path
sys.path.insert(0, str(ROOT / "src"))


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-a",
        "--all",
        action="store_true",
        help="run all days",
    )
    input_group = parser.add_mutually_exclusive_group()
    input_group.add_argument(
        "-e",
        "--example",
        action="store_const",
        dest="input_name",
        const="example1.txt",
        default="input.txt",
        help="use the example input from the instructions",
    )
    input_group.add_argument(
        "-i",
        "--input",
        dest="input_name",
        help="override the input file",
    )
    parser.add_argument("-t", "--timeit", action="store_true", help="time each part")
    parser.add_argument(
        "day",
        nargs=argparse.ZERO_OR_MORE,
        help="a day number to run (defaults to today)",
    )
    args = parser.parse_args()

    if args.all:
        days = list(map(str, range(1, TODAY + 1)))
    elif args.day:
        days = args.day
    else:
        days = [str(TODAY)]

    if len(days) > 1 and args.input is not None:
        parser.error("-i/--input cannot be used if multiple days are specified")

    for i, day_arg in enumerate(days):
        day_num = day_arg.partition("_")[0]
        if len(day_num) == 1:
            day_arg = f"0{day_arg}"
            day_num = f"0{day_num}"
        path = ROOT / f"input/day{day_num}" / args.input_name
        with open(path, "r") as file:
            lines = file.read().splitlines(keepends=False)

        day = importlib.import_module(f"day{day_arg}")

        print(f"Day {int(day_num)}")
        if hasattr(day, "setup"):
            start_time = time.perf_counter()
            input_data = day.setup(lines)
            elapsed = time.perf_counter() - start_time
            print(f"Setup: {_format_time(elapsed)}")
            if args.timeit:
                get_timing("day.setup(lines)", globals={**locals(), **globals()})
        else:
            input_data = lines

        if hasattr(day, "both_parts"):
            start_time = time.perf_counter()
            answers = day.both_parts(input_data)
            elapsed = time.perf_counter() - start_time
            print(
                f"Part 1: {answers[0]}\nPart 2: {answers[1]} ({_format_time(elapsed)})"
            )
            if args.timeit:
                get_timing(
                    "day.both_parts(input_data)", globals={**locals(), **globals()}
                )

        else:
            if hasattr(day, "part_1"):
                start_time = time.perf_counter()
                answer = day.part_1(input_data)
                elapsed = time.perf_counter() - start_time
                print(f"Part 1: {answer} ({_format_time(elapsed)})")
                if args.timeit:
                    get_timing(
                        "day.part_1(input_data)", globals={**locals(), **globals()}
                    )

            if hasattr(day, "part_2"):
                start_time = time.perf_counter()
                answer = day.part_2(input_data)
                elapsed = time.perf_counter() - start_time
                print(f"Part 2: {answer} ({_format_time(elapsed)})")
                if args.timeit:
                    get_timing(
                        "day.part_2(input_data)", globals={**locals(), **globals()}
                    )

        if i != len(days) - 1:
            print()


# from IPython/core/magics/execution.py
def _format_time(timespan: float, precision: int = 3) -> str:
    """Formats the timespan in a human readable form"""

    if timespan >= 60.0:
        # we have more than a minute, format that in a human readable form
        # Idea from http://snipplr.com/view/5713/
        parts = [("d", 60 * 60 * 24), ("h", 60 * 60), ("min", 60), ("s", 1)]
        time_str = []
        leftover = timespan
        for suffix, length in parts:
            value = int(leftover / length)
            if value > 0:
                leftover = leftover % length
                time_str.append(f"{value!s}{suffix}")
            if leftover < 1:
                break
        return " ".join(time_str)

    units = ["s", "ms", "μs", "ns"]
    scaling = [1, 1e3, 1e6, 1e9]

    if timespan > 0.0:
        order = min(-int(math.floor(math.log10(timespan)) // 3), 3)
    else:
        order = 3
    return f"{timespan * scaling[order]:.{precision}g} {units[order]}"


def get_timing(
    stmt: str = "pass",
    setup: str = "pass",
    repeat: int = 7,
    globals: Optional[Dict[str, Any]] = None,
) -> None:
    timer = timeit.Timer(stmt, setup, globals=globals)
    num_loops, total_time = timer.autorange()
    times = [
        t / num_loops
        for t in [total_time, *timer.repeat(repeat=repeat - 1, number=num_loops)]
    ]
    print(
        f"{_format_time(statistics.mean(times))} ± {_format_time(statistics.stdev(times))} per loop"
        f" (mean ± std. dev. of {repeat} loops, {num_loops} loops each)\n"
    )


if __name__ == "__main__":
    main()
