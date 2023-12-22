#!/bin/bash
set -euo pipefail

AOC_ROOT="$(git rev-parse --show-toplevel)"
# shellcheck source=lib.sh
source "$AOC_ROOT/tools/lib.sh"
# cd to the base directory for this year
cd -- "$AOC_ROOT/$AOC_YEAR"

run_test() {
  day=$1
  # just the basename
  input=$(basename "$2")
  output=$3
  ./aoc "$day" -q -i "$input" | diff "$output" - && ret=$? || ret=$?
  if [[ $ret -eq 0 ]]; then
    printf 'Day %2d, %s: answer test \033[32mpassed\033[0m\n' "${day#0}" "$input"
  else
    printf 'Day %2d, %s: answer test \033[31mfailed\033[0m\n' "${day#0}" "$input"
  fi
  return $ret
}

days=("$@")
if [[ ${#days[@]} -eq 0 ]]; then
  # run all tests
  for src_file in src/day*.cpp; do
    day=${src_file#src/day}
    day=${day%.cpp}
    days+=("${day#0}")
  done
fi

fail_count=0
for day in "${days[@]}"; do
  day=$(pad_day_number "${day#day}")
  for output in "answer_tests/day${day}"/*; do
    input="input/${output#answer_tests/}"
    if [[ -e "$input" ]]; then
      if ! run_test "$day" "$input" "$output"; then
        (( ++fail_count ))
      fi
    fi
  done
done
exit "$fail_count"
