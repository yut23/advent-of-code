import json
import subprocess
import sys


def test_on_input(day: int, input_name: str, expected_answers: tuple[int, int]) -> bool:
    result = subprocess.run(
        [
            f"build/fast/day{day:02d}",
            f"unofficial-aoc2023-inputs/day_{day:03d}/{input_name}",
        ],
        check=False,
        text=True,
        stdout=subprocess.PIPE,
    )
    if result.returncode != 0:
        print(f"Day {day}, {input_name}: exited with status {result.returncode}")
        return False
    answers = tuple(map(int, result.stdout.splitlines(keepends=False)))
    correct = True
    for i in range(2):
        if not isinstance(expected_answers[i], int):
            continue
        if answers[i] != expected_answers[i]:
            print(
                f"Day {day}, {input_name}: part {i+1} incorrect; expected {expected_answers[i]}, got {answers[i]}"
            )
            correct = False
    return correct


def main() -> int:
    subprocess.run(["make", "-j", "fast"], check=True)
    failure_count = 0
    for day in range(1, 26):
        with open(f"unofficial-aoc2023-inputs/day_{day:03d}/solutions.json", "r") as f:
            solutions = json.load(f)
        for input_name, answers in solutions.items():
            if not test_on_input(
                day, input_name, (answers["part_one"], answers["part_two"])
            ):
                failure_count += 1
    return failure_count


if __name__ == "__main__":
    sys.exit(main())
