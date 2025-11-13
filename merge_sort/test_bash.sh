#!/bin/bash

# FILE_SIZES=(100000 1000000 10000000)
# for file_size in "${FILE_SIZES[@]}"; do
#   echo "Record num: ${file_size}" >> "output/final_data_XD.txt"
#   echo -r "t g ${file_size} r q" \
#   | ./output/SortProgram --max_buffer_count 200 --records_per_page 200 \
#   | sed -n '/---Start---/,/---Stop---/p' \
#   >> "./output/final_data_XD.txt"
# done

J=0
for ((i = 100000; i < 101000000; i += 500000)); do
  echo "Record num: ${i}" >> "output/final_data_XD.txt"
  echo -r "t g ${i} r q" \
  | ./output/SortProgram --max_buffer_count 200 --records_per_page 200 \
  | sed -n '/---Start---/,/---Stop---/p' \
  >> "./output/final_data_XD.txt"
  ((J+=1))
  echo ${J}
done
echo ${J}

