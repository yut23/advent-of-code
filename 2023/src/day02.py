import functools
import operator
from collections import Counter


def both_parts(lines: list[str]) -> tuple[int, int]:
    part_1 = 0
    part_2 = 0
    for line in lines:
        label, _, entries = line.partition(": ")
        game_id = int(label.removeprefix("Game "))
        valid = True
        max_counts: Counter[str] = Counter()
        for entry in entries.split(";"):
            counts: Counter[str] = Counter()
            for cube in entry.split(","):
                count, color = cube.split()
                counts[color] = int(count)
                max_counts[color] = max(max_counts[color], counts[color])
            if counts["red"] > 12 or counts["green"] > 13 or counts["blue"] > 14:
                valid = False
        if valid:
            part_1 += game_id
        part_2 += functools.reduce(operator.mul, max_counts.values())

    return part_1, part_2
