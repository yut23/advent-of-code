#!/bin/bash
set -euo pipefail

AOC_ROOT="$(git rev-parse --show-toplevel)"
# shellcheck source=../lib.sh
source "$AOC_ROOT/tools/lib.sh"
# cd to the base directory for this year
cd -- "$AOC_ROOT/$AOC_YEAR"

if ! command -v bear &> /dev/null || [[ " $* " =~ " clean " ]]; then
  # just run make if bear isn't present, or if cleaning
  make "$@"
  exit
fi

# store the compilation database in the repo root, so ccls+vim-lsp works
# properly with `-I../aoc_lib`
comp_db="../compile_commands_$AOC_YEAR.json"

# check if the makefile is newer than the compilation database
if [[ Makefile -nt "$comp_db" ]]; then
  echo "Makefile changed, rebuilding entire compilation database..."
  if ! [[ -e $comp_db ]]; then
    # the compilation database was removed, so touch the Makefile to make sure
    # everything gets rebuilt
    touch Makefile
  fi
  # clear the compilation database
  rm -f "$comp_db"
  # add "all" to the arguments if it's not already present
  if ! [[ " $* " =~ " all " ]]; then
    set -- all "$@"
  fi
  # add -j$(nproc) if no -j arguments are present
  if ! [[ " $*" =~ " -j" ]]; then
    set -- -j"$(nproc)" "$@"
  fi
fi

if printf '3.1.0\n%s\n' "$(bear --version | cut -d' ' -f2)" | sort -V -C; then
  BEAR_PREFIX='bear '
else
  BEAR_PREFIX=
fi
# this is a variable expansion for make, not bash
# shellcheck disable=SC2016
INTERCEPT_WRAPPER="$BEAR_PREFIX"'intercept --output $(@:.o=_events.json) --' make "$@" && ret=$? || ret=$?

shopt -s nullglob globstar
events=(build/**/*_events.json)
shopt -u nullglob globstar
if [[ ${#events[@]} -gt 0 ]]; then
  cat "${events[@]}" > events.json
  rm "${events[@]}"
  $BEAR_PREFIX citnames --append --config ../tools/cpp/bear_config.json --output "$comp_db"
  if command -v jq &> /dev/null && command -v sponge &> /dev/null; then
    # sort with jq
    jq 'sort_by(.file, .output)' "$comp_db" | sponge "$comp_db"
  fi
  # merge the per-year compilation databases into one
  cat ../compile_commands_*.json | jq '.[]' | jq -s '.' > ../compile_commands.json
  rm events.json
fi

exit "$ret"
