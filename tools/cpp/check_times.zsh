#!/usr/bin/env zsh
# benchmark all days and output a summary

make -q fast || make -j24 fast

days=(build/fast/day<->(:t))

hyperfine_cmdline=(
  hyperfine
  --min-runs 5
  --max-runs 100
  --shell none
  --export-json -
  --style none
  'build/fast/{day} input/{day}/input.txt'
  --parameter-list day
)

echo '            mean ± stdev     min [ms]'
for day ($days) {
  values=("${(@z)$($hyperfine_cmdline "$day" 2>/dev/null | jq -r '.results[] | [.mean * 1000, .stddev * 1000, .min * 1000] | @sh')}")
  day_num=${day##day(0|)}
  print -r -v mean -f '%.2f ± %5.2f' $values[1] $values[2]
  print -r -f 'Day %2d: %16s %7.1f\n' $day_num $mean $values[3]
}
