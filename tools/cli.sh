AOC_ROOT="$(git rev-parse --show-toplevel)"
# shellcheck source=lib.sh
source "$AOC_ROOT/tools/lib.sh"

setup_help() {
  true
  #help_sections=()
}

add_help() {
  true
  # TODO: implement this
  #local section=$1
  #local desc=$2
  #local -A helps
  #local -a names params
  #local num_names num_params
  #while [[ $# -gt 0 ]]; do
  #  local num_names=$1
  #  shift
  #  names=("${@:1:$num_names}")
  #  shift "$num_names"
  #  local num_params=$1
  #  shift
  #  params=("${@:1:$num_params}")
  #  shift "$num_params"
  #  local help=$1
  #  helps["${names[*]}"]="$help"
  #done
}

print_help() {
  true;
}

init_day_args() {
  day=$(TZ='America/New_York' date +%-d)
  first_iter=y
}
handle_day_arg() {
  if [[ $first_iter == y ]]; then
    day=$1
  else
    echo "Error: day can only be in the first argument"
    usage
    exit 1
  fi
}
finalize_day_args() {
  day=$(pad_day_number "$day")
}

# usage: see tools/cpp/aoc
# parses out options for target selection
init_target_args() {
  bin_dir=build/release
  suffix=
  binary=

  # add help section
  add_help "Target selection" "" \
    2 "-d" "--debug"  0     "run the debug build" \
    2 "-f" "--fast"   0     "run the fast build" \
    2 "-s" "--suffix" 1 "S" "select a different source file with the given suffix"
}
handle_target_arg() {
  local init_nargs=$#
  case $1 in
    -d|--debug)
      bin_dir=build/debug
      shift
      ;;
    -f|--fast)
      bin_dir=build/fast
      shift
      ;;
    -s|--suffix)
      if [[ $2 == _* ]]; then
        suffix=$2
      else
        suffix=_$2
      fi
      shift
      shift
      ;;
    *)
      return 1
      ;;
  esac
  shift_count=$(( init_nargs - $# ))
  return 0
}
finalize_target_args() {
  # if --gdb was passed, use debug build, or profile build if --fast was also passed
  if [[ ${use_gdb:-n} == y ]]; then
    if [[ $bin_dir == build/fast ]]; then
      bin_dir=build/profile
    else
      bin_dir=build/debug
    fi
  fi

  binary=$bin_dir/day$day$suffix
}

# parses out options for input file selection
init_input_args() {
  input_name=input.txt

  # add help section
  add_help "Input selection" "" \
    2 "-e" "--example" 1 "N"    "use a numbered example (example<N>.txt)" \
    2 "-i" "--input"   1 "FILE" "use a specific input file (e.g. input.txt)"
}
handle_input_arg() {
  local init_nargs=$#
  case $1 in
    -e|--example)
      input_name=example1.txt
      if [[ $# -gt 1 ]]; then
        # example number given
        input_name=example$2.txt
        shift
      fi
      shift
      ;;
    -i|--input)
      input_name=$2
      shift
      shift
      ;;
    *)
      return 1
      ;;
  esac
  shift_count=$(( init_nargs - $# ))
  return 0
}
finalize_input_args() {
  input_path="input/day$day/$input_name"
  if ! [[ -e "$input_path" ]]; then
    if [[ $input_name == input.txt ]]; then
      bash "$TOOLS_DIR/get_input.sh" "$day" "$AOC_YEAR"
    else
      >&2 echo "ERROR: could not find input file at $input_path"
      exit 2
    fi
  fi
}

# parses out options that control the make invocation
init_make_args() {
  make_args=()
  skip_make=n

  add_help "Make invocation" "" \
    3 "-q" "--quiet" "--silent" 0 "don't echo make recipes" \
    1 "--no-make"               0 "don't run make at all" \
    1 "--no-make-wrapper"       0 "run make directly, not through make_wrapper.sh"
}
handle_make_arg() {
  local init_nargs=$#
  case $1 in
    -q|--quiet|--silent)
      make_args+=(--silent)
      shift
      ;;
    --no-make)
      skip_make=y
      shift
      ;;
    --no-make-wrapper)
      make_args+=("$1")
      shift
      ;;
    *)
      return 1
      ;;
  esac
  shift_count=$(( init_nargs - $# ))
  return 0
}
finalize_make_args() {
  if [[ $skip_make != y ]]; then
    # rebuild if make says it's necessary
    make_quiet "${make_args[@]}" "$binary"
  fi
}
