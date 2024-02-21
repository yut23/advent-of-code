#!/bin/bash
set -euo pipefail

AOC_ROOT="$(git rev-parse --show-toplevel)"
# shellcheck source=../lib.sh
source "$AOC_ROOT/tools/lib.sh"
cd -- "$AOC_ROOT/$AOC_YEAR"

read -r -d ' ' day < <(parse_day_args "$1")
num=${2:-1}
raw_log_file="profiling/logs/day${day}_${num}.folded"
log_file="profiling/logs/day${day}_${num}.filtered"
svg_file="profiling/day${day}_${num}.svg"
rev_svg_file="profiling/day${day}_${num}_reversed.svg"

BINARY=build/profile/day$day
make_quiet "$BINARY"
mkdir -p profiling/logs
perf record -g --call-graph dwarf,64000 -F 99 hyperfine --shell none "$BINARY input/day$day/input.txt"
#perf record -g --call-graph dwarf,64000 -F 99 "$BINARY" "input/day$day/input.txt"
perf script --header -F comm,pid,tid,time,event,ip,sym,dso | stackcollapse-perf.pl > "$raw_log_file"
# skip extra frames between the executable name and main(), and exclude some stacks we don't care about
sed -E -e 's/^(.*;)?(main;.*)/\2/' "$raw_log_file" | grep -vE '^'"day$day"'(;_start|;__.*| [0-9]+)|^hyperfine' > "$log_file"
#if [[ $day == 12 ]]; then
#  sed -E -i 's/(;aoc::day12::ConditionRecord::count_arrangements)\1*/\1+/;' "$log_file"
#fi
inferno-flamegraph --inverted --deterministic "$log_file" > "$svg_file"
inferno-flamegraph --reverse --deterministic "$log_file" > "$rev_svg_file"
