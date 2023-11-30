from typing import List

import numpy as np
import numpy.typing as npt


def parse(lines: List[str]) -> npt.NDArray[np.int8]:
    data = np.asarray([list(line.encode()) for line in lines], dtype=np.int8) - np.int8(
        ord("0")
    )
    return data


def get_visible(heights: npt.NDArray[np.int8]) -> npt.NDArray[np.bool_]:
    return np.diff(np.maximum.accumulate(heights, axis=0), axis=0, prepend=-1) > 0


def part_1(lines: List[str]) -> int:
    heights = parse(lines)
    mask = get_visible(heights)
    mask |= get_visible(heights[::-1])[::-1]
    mask |= get_visible(heights.T).T
    mask |= get_visible(heights.T[::-1])[::-1].T
    return mask.sum()
