from __future__ import annotations

from dataclasses import dataclass


@dataclass(order=True)
class MapEntry:
    src_start: int
    dest_start: int
    length: int

    @classmethod
    def parse(cls, text: str) -> MapEntry:
        values = list(map(int, text.split()))
        return cls(dest_start=values[0], src_start=values[1], length=values[2])


@dataclass
class ConversionMap:
    label: str
    entries: list[MapEntry]

    def __post_init__(self) -> None:
        self.entries.sort()

    def apply(self, value: int) -> int:
        for entry in self.entries:
            if entry.src_start <= value < entry.src_start + entry.length:
                return value - entry.src_start + entry.dest_start
        return value


def parse(lines: list[str]) -> tuple[list[int], list[ConversionMap]]:
    text = "\n".join(lines)
    sections = text.split("\n\n")
    seeds = [int(x) for x in sections[0].split()[1:]]
    maps: list[ConversionMap] = []
    for section in sections[1:]:
        lines = section.splitlines(keepends=False)
        label = lines[0].partition(" ")[0]
        entries = list(map(MapEntry.parse, lines[1:]))
        maps.append(ConversionMap(label, entries))
    return seeds, maps


def part_1(lines: list[str]) -> int:
    seeds, maps = parse(lines)
    for m in maps:
        seeds = [m.apply(seed) for seed in seeds]
    return min(seeds)
