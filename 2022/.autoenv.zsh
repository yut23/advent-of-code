if [[ $autoenv_event == 'enter' ]]; then
  alias workon=${0:h}/workon
  alias aoc=${0:h}/aoc
  alias iwyu=${0:h}/iwyu
  alias profile=${0:h}/profile.sh
else
  unalias workon aoc iwyu profile
fi
