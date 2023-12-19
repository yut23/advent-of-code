local wrapper_cmd=${0:h}/make_wrapper.sh
if [[ $autoenv_event == 'enter' ]]; then
  alias workon=${0:h}/workon
  alias aoc=${0:h}/aoc
  alias iwyu=${0:h}/iwyu
  alias profile=${0:h}/profile.sh
  alias make="$wrapper_cmd -f ${(q)0:h}/Makefile"
  compdef "$wrapper_cmd"=make
else
  unalias workon aoc iwyu profile make
  # remove completion definition
  unset "_comps[$wrapper_cmd]"
  unset "_services[$wrapper_cmd]"
fi
