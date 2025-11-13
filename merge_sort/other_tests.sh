#!/bin/bash

FILE_SIZES=(167962)

for file_size in "${FILE_SIZES[@]}"
do
  echo "$file_size"
  echo "Record num: ${file_size}" >> "output/heat_map.txt"
  for ((MBC = 13; MBC < 1300; MBC+=13)); do
    for ((RPP = 13; RPP < 1300; RPP+=13)); do
      echo -r "t g ${file_size} r q" \
      | ./output/SortProgram --max_buffer_count ${MBC} --records_per_page ${RPP} \
      | sed -n '/---Start---/,/---Stop---/p' \
      >> "output/heat_map.txt"
      echo "MBC: ${MBC} RPP: ${RPP}"
    done
  done
  
  
done

