if [[ $autoenv_event == 'enter' ]]; then
  alias workon=${0:h:h}/tools/cpp/workon
  alias aoc=${0:h:h}/tools/cpp/aoc
  alias iwyu=${0:h:h}/tools/cpp/iwyu
  alias profile=${0:h:h}/tools/cpp/profile.sh
else
  unalias workon aoc iwyu profile
fi
