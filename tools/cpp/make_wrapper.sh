#!/bin/bash
set -euo pipefail

AOC_ROOT="$(git rev-parse --show-toplevel)"
# shellcheck source=../lib.sh
source "$AOC_ROOT/tools/lib.sh"
# cd to the base directory for this year
cd -- "$AOC_ROOT/$AOC_YEAR"

# check if the makefile is newer than the compilation database
if [[ Makefile -nt compile_commands.json ]]; then
  echo "Makefile changed, rebuilding entire compilation database..."
  if ! [[ -e compile_commands.json ]]; then
    # the compilation database was removed, so touch the Makefile to make sure
    # everything gets rebuilt
    touch Makefile
  fi
  # clear the compilation database
  rm -f compile_commands.json
  # add "all" to the arguments if it's not already present
  if ! [[ " $* " =~ " all " ]]; then
    set -- all "$@"
  fi
  # add -j$(nproc) if no -j arguments are present
  if ! [[ " $*" =~ " -j" ]]; then
    set -- -j"$(nproc)" "$@"
  fi
fi

set +e
# this is a variable expansion for make, not bash
# shellcheck disable=SC2016
INTERCEPT_WRAPPER='intercept --output $(@:.o=_events.json) --' make "$@"
ret=$?
set -e

shopt -s nullglob globstar
events=(build/**/*_events.json)
shopt -u nullglob globstar
if [[ ${#events[@]} -gt 0 ]]; then
  cat "${events[@]}" > events.json
  rm "${events[@]}"
  citnames --append --config ../tools/cpp/bear_config.json
  rm events.json
fi

exit $ret
