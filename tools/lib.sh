# common bash variables and functions used by multiple utilities

TOOLS_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)"
if ! [[ -v AOC_ROOT ]]; then
  AOC_ROOT="$(git -C "$TOOLS_DIR" rev-parse --show-toplevel)"
fi
if ! [[ -v AOC_PWD ]] && [[ $PWD != "${PWD#"$AOC_ROOT/"}" ]]; then
  # PWD is under AOC_ROOT
  AOC_PWD=${PWD#"$AOC_ROOT/"}
  # try getting the year based on the current directory
  if [[ $AOC_PWD =~ ^(20[1-9][0-9])(/|$) ]]; then
    export AOC_YEAR=${BASH_REMATCH[1]}
  fi
fi

# usage: parse_day_args [day [year]] | read -r day year
parse_day_args() {
  local day year
  if [[ -v AOC_YEAR ]]; then
    year=$AOC_YEAR
    if [[ $# -lt 1 ]]; then
      day=$(date +'%-d')
    fi
  else
    if [[ $# -lt 2 ]]; then
      # get the current year if AOC_YEAR is not set
      read -r day year < <(date +'%-d %Y')
    fi
  fi
  # use the arguments if present, and fall back to the current day/year otherwise
  day=${1:-$day}
  year=${2:-$year}
  # pad day number with zeros to two digits
  printf '%02d %d' "${day#0}" "$year"
}

make_quiet() {
  # make a target only if make thinks it's necessary (outputs nothing if not)
  targets=()
  args=()
  while [[ $# -gt 0 ]]; do
    args+=("$1")
    case $1 in
      -*)
        # allow any number of options
        ;;
      *)
        targets+=("$1")
        ;;
    esac
    shift
  done
  set -- "${args[@]}"
  if [[ ${#targets[@]} -gt 1 ]]; then
    >&2 echo "Internal error: make_quiet should only be passed one target"
    return 1
  fi
  make -q "${targets[@]}" 2>/dev/null || make "$@"
}
