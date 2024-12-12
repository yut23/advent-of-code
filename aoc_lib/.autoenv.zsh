local wrapper_cmd=${0:h}/make_wrapper.sh
if [[ $autoenv_event == 'enter' ]]; then
  alias iwyu=${0:h}/iwyu
  alias make="$wrapper_cmd -f ${(q)0:h}/Makefile"
  compdef "$wrapper_cmd"=make
  export AOC_YEAR=aoc_lib
else
  unalias iwyu make
  # remove completion definition
  unset "_comps[$wrapper_cmd]"
  unset "_services[$wrapper_cmd]"
  unset AOC_YEAR
fi
