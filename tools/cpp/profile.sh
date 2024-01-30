#!/bin/bash
set -euo pipefail

AOC_ROOT="$(git rev-parse --show-toplevel)"
# shellcheck source=../lib.sh
source "$AOC_ROOT/tools/lib.sh"
cd -- "$AOC_ROOT/$AOC_YEAR"

read -r -d ' ' day < <(parse_day_args "$1")
num=${2:-1}
log_file="profiling/logs/day${day}_${num}.folded"
svg_file="profiling/day${day}_${num}.svg"

BINARY=build/profile/day$day
make_quiet "$BINARY"
mkdir -p profiling/logs
perf record -g --call-graph dwarf,32768 -F 99 hyperfine --shell none "$BINARY input/day$day/input.txt"
perf script --header -F comm,pid,tid,time,event,ip,sym,dso | inferno-collapse-perf --skip-after main > "$log_file"
inferno-flamegraph --inverted --deterministic "$log_file" > "$svg_file"
