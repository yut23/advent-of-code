#!/bin/bash
set -euo pipefail

AOC_ROOT="$(git rev-parse --show-toplevel)"
# shellcheck source=lib.sh
source "$AOC_ROOT/tools/lib.sh"
# cd to the base directory for this year
cd -- "$AOC_ROOT/$AOC_YEAR"

run_test() {
  local day input output args ret
  day=$1
  # just the basename
  input=$(basename "$2")
  output=$3
  args=()
  if [[ -n ${GITHUB_ACTIONS:+x} ]]; then
    # don't use systemd-run when running on GHA
    args+=(-m none)
  fi
  ./aoc "$day" -q "${args[@]}" -i "$input" | diff "$output" - && ret=$? || ret=$?
  if [[ $ret -eq 130 ]]; then
    # killed by SIGINT, exit immediately
    exit $ret
  fi
  if [[ $ret -eq 0 ]]; then
    printf 'Day %2d, %13s answer test \033[32mpassed\033[0m\n' "${day#0}" "$input:"
  else
    printf 'Day %2d, %13s answer test \033[31mfailed\033[0m\n' "${day#0}" "$input:"
  fi
  return $ret
}

days=("$@")
if [[ ${#days[@]} -eq 0 ]]; then
  # run all tests
  for src_file in src/day*.cpp; do
    day=${src_file#src/day}
    day=${day%.cpp}
    if [[ $day = *_* ]]; then
      continue;
    fi
    days+=("${day#0}")
  done
fi

padded_days=()
for day in "${days[@]}"; do
  padded_days+=("$(pad_day_number "${day#day}")")
done

# try building everything first
targets=()
for day in "${padded_days[@]}"; do
  target=build/release/day$day
  make -q "$target" 2>/dev/null || targets+=("$target")
done
if [[ ${#targets[@]} -gt 0 ]]; then
  make -j8 "${targets[@]}" || printf '\n'
fi

fail_count=0
for day in "${padded_days[@]}"; do
  outputs=("answer_tests/day${day}"/*)
  if [[ ${#outputs[@]} -eq 0 ]]; then
    continue
  fi
  if ! make -q "build/release/day$day"; then
    printf 'Day %2d: \033[31mcompilation error\033[m\n' "${day#0}"
    (( fail_count += ${#outputs[@]} ))
    continue
  fi
  for output in "${outputs[@]}"; do
    input="input/${output#answer_tests/}"
    if [[ -e "$input" ]]; then
      if ! run_test "$day" "$input" "$output"; then
        (( ++fail_count ))
      fi
    fi
  done
done
exit "$fail_count"
