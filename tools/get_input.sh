#!/bin/bash
set -euo pipefail
# run last command in a pipeline in the current shell environment, rather than a subshell
shopt -s lastpipe

AOC_ROOT="$(git rev-parse --show-toplevel)"
# shellcheck source=lib.sh
source "$AOC_ROOT/tools/lib.sh"

parse_day_args "$@" | read -r day_padded year
# remove leading zero
day=${day_padded#0}
dest_dir="input/day$day_padded"
mkdir -p "$dest_dir"
filename="$dest_dir/input.txt"
if ! [[ -e "$filename" ]]; then
  # download the input
  exit_code=0
  if command -v aocd &> /dev/null; then
    aocd "$day" "$year" > "$filename" || exit_code=$?
    url="input for day $day"
  else
    session=$(<~/.config/aocd/token)
    url="https://adventofcode.com/$year/day/$day/input"
    curl --fail "$url" --cookie "session=$session" -o "$filename" || exit_code=$?
  fi
  if [[ $exit_code -ne 0 ]]; then
    rm -f "$filename"
    >&2 echo "Failed to download $url"
    exit $exit_code
  fi
else
  >&2 echo "Input for day $day already downloaded."
  exit 1
fi
