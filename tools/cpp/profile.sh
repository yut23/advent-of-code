#!/bin/bash
set -euo pipefail

AOC_ROOT="$(git rev-parse --show-toplevel)"
# shellcheck source=../lib.sh
source "$AOC_ROOT/tools/lib.sh"
cd -- "$AOC_ROOT/$AOC_YEAR"

print_help() {
  cat <<EOF
Usage: $0 <day> [--num NUM [-f]] [hyperfine options...]

Run aoc under perf(1) and hyperfine generate flamegraphs.

Options:
 --num NUM    set the run number
 -f, --force  overwrite existing runs instead of incrementing the run number

 -h, --help   show this help message and exit
EOF
}

read -r -d ' ' day < <(parse_day_args "$1")
shift
force=n
num=auto
hyperfine_args=()
while [[ $# -gt 0 ]]; do
  arg=$1
  shift
  case "$arg" in
    -h|--help)
      print_help "$0"
      exit 0
      ;;
    --num)
      num=$1
      shift
      ;;
    -f|--force)
      force=y
      ;;
    --)
      hyperfine_args+=("$@")
      break
      ;;
    *)
      hyperfine_args+=("$arg")
      ;;
  esac
done

label="day${day}_${num}"
raw_log_file="profiling/logs/${label}.folded"
if [[ $num == auto ]] || [[ -f "$raw_log_file" ]]; then
  if [[ $force == n ]]; then
    if [[ $num == auto ]]; then
      num=1
    else
      echo "'$raw_log_file' already exists, trying a different name... (pass --force to overwrite instead)"
    fi
    while true; do
      label="day${day}_${num}"
      raw_log_file="profiling/logs/${label}.folded"
      [[ -f "$raw_log_file" ]] || break
      (( ++num ))
    done
    echo "new run number: $num"
  elif [[ $num == auto ]]; then
    echo "Error: --force requires an explicit --num"
    exit 1
  else
    echo "'$raw_log_file' already exists, overwriting due to --force"
  fi
fi
log_file="profiling/logs/${label}.filtered"
svg_file="profiling/${label}.svg"
rev_svg_file="profiling/${label}_reversed.svg"

BINARY=build/profile/day$day
make_quiet "$BINARY"
mkdir -p profiling/logs
perf record --debuginfod -g --call-graph dwarf,64000 -F 99 hyperfine --shell none "${hyperfine_args[@]}" "$BINARY input/day$day/input.txt"
#perf record -g --call-graph dwarf,64000 -F 99 "$BINARY" "input/day$day/input.txt"
if ! command -v stackcollapse-perf.pl >/dev/null 2>&1; then
  PATH=$PATH:$HOME/build/FlameGraph
fi
perf script --header -F comm,pid,tid,time,event,ip,sym,dso | stackcollapse-perf.pl > "$raw_log_file"
# skip extra frames between the executable name and main(), and exclude some stacks we don't care about
sed -E -e 's/^(.*;)?(main;.*)/\2/' "$raw_log_file" | grep -vE '^'"day$day"'(;_start|;__.*| [0-9]+)|^hyperfine' > "$log_file"
#if [[ $day == 12 ]]; then
#  sed -E -i 's/(;aoc::day12::ConditionRecord::count_arrangements)\1*/\1+/;' "$log_file"
#fi
# width is tuned for my laptop screen, with chrome at 125% zoom
args=(--minwidth 0.1 --deterministic --fonttype monospace --width 2000)
inferno-flamegraph --inverted "${args[@]}" "$log_file" > "$svg_file"
inferno-flamegraph --reverse "${args[@]}" "$log_file" > "$rev_svg_file"
