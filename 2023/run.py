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
        nargs=argparse.OPTIONAL,
        default=argparse.SUPPRESS,
        help="use the example input from the instructions",
    )
    input_group.add_argument(
        "-i",
        "--input",
        dest="input_name",
        default="input.txt",
        help="override the input file",
    )
    parser.add_argument("-t", "--timeit", action="store_true", help="time each part")
    parser.add_argument(
        "day",
        nargs=argparse.ZERO_OR_MORE,
        help="a day number to run (defaults to today)",
    )
    args = parser.parse_args()

    if hasattr(args, "example"):
        # --example was passed
        if args.example is None:
            # no number, default to 1
            args.example = 1
        args.input_name = f"example{args.example}.txt"

    if args.all:
        days = list(map(str, range(1, TODAY + 1)))
    elif args.day:
        days = args.day
    else:
        days = [str(TODAY)]

    for i, day_arg in enumerate(days):
        run_day(day_arg, args.input_name, args.timeit)
        if i != len(days) - 1:
            print()


def run_day(day_arg: str, input_name: str, do_timing: bool) -> None:
    day_num = day_arg.partition("_")[0]
    if len(day_num) == 1:
        day_arg = f"0{day_arg}"
        day_num = f"0{day_num}"
    path = ROOT / f"input/day{day_num}" / input_name
    with open(path, "r") as file:
        lines = file.read().splitlines(keepends=False)

    day = importlib.import_module(f"day{day_arg}")

    print(f"Day {int(day_num)}")
    namespace = {**locals(), **globals()}
    if hasattr(day, "setup"):
        start_time = time.perf_counter()
        input_data = day.setup(lines)
        elapsed = time.perf_counter() - start_time
        print(f"Setup: {_format_time(elapsed)}")
        if do_timing:
            get_timing("day.setup(lines)", namespace=namespace)
    else:
        input_data = lines

    namespace["input_data"] = input_data
    if hasattr(day, "both_parts"):
        start_time = time.perf_counter()
        answers = day.both_parts(input_data)
        elapsed = time.perf_counter() - start_time
        print(f"Part 1: {answers[0]}\nPart 2: {answers[1]} ({_format_time(elapsed)})")
        if do_timing:
            get_timing("day.both_parts(input_data)", namespace=namespace)

    else:
        if hasattr(day, "part_1"):
            start_time = time.perf_counter()
            answer = day.part_1(input_data)
            elapsed = time.perf_counter() - start_time
            print(f"Part 1: {answer} ({_format_time(elapsed)})")
            if do_timing:
                get_timing("day.part_1(input_data)", namespace=namespace)

        if hasattr(day, "part_2"):
            start_time = time.perf_counter()
            answer = day.part_2(input_data)
            elapsed = time.perf_counter() - start_time
            print(f"Part 2: {answer} ({_format_time(elapsed)})")
            if do_timing:
                get_timing("day.part_2(input_data)", namespace=namespace)


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
    namespace: Optional[Dict[str, Any]] = None,
) -> None:
    timer = timeit.Timer(stmt, setup, globals=namespace)
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
