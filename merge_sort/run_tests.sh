#!/bin/bash

GRAIN_NUM=20

TEST_NUM=12
# FILE_SIZES=(1000 50000 100000 1000000 10000000 25000000 50000000 75000000 100000000)

FILE_SIZES=(1000000 10000000 25000000 50000000 75000000 100000000)
G_MBC=500
G_RPP=500
for ((i = 0; i < GRAIN_NUM; i++)); do
  MBC=$G_MBC
  RRP=$G_RPP
  ((MBC += 200 * i))
  ((RPP += 500 * i))

echo "RPP increase"
for file_size in "${FILE_SIZES[@]}"; do
  MBC_DECREASE_FACTOR=0
  RPP_INCREASE_FACTOR=40
  for ((i = 0; i < TEST_NUM; i++)); do
    echo "Record num: ${file_size}" >> "output/RPP_increase.txt"
    echo -r "t g ${file_size} r q" \
    | ./output/SortProgram --max_buffer_count ${MBC} --records_per_page ${RPP} \
    | sed -n '/---Start---/,/---Stop---/p' \
    >> "./output/RPP_increase.txt"
    ((MBC -= MBC_DECREASE_FACTOR))
    ((RPP += RPP_INCREASE_FACTOR))
  done
done
  MBC=$G_MBC
  RRP=$G_RPP
  ((MBC += 200 * i))
  ((RPP += 500 * i))
echo "MBC decrease"

for file_size in "${FILE_SIZES[@]}"; do
  MBC_DECREASE_FACTOR=40
  RPP_INCREASE_FACTOR=0
  for ((i = 0; i < TEST_NUM; i++)); do
    echo "Record num: ${file_size}" >> "output/MBC_decrease.txt"
    echo -r "t g ${file_size} r q" \
    | ./output/SortProgram --max_buffer_count ${MBC} --records_per_page ${RPP} \
    | sed -n '/---Start---/,/---Stop---/p' \
    >> "output/MBC_decrease.txt"
    ((MBC -= MBC_DECREASE_FACTOR))
    ((RPP += RPP_INCREASE_FACTOR))
  done
done

  MBC=$G_MBC
  RRP=$G_RPP
  ((MBC += 200 * i))
  ((RPP += 500 * i))
echo "MBC decrease and  RPP increase"

for file_size in "${FILE_SIZES[@]}"; do
  MBC_DECREASE_FACTOR=40
  RPP_INCREASE_FACTOR=40
  for ((i = 0; i < TEST_NUM; i++)); do
    echo "Record num: ${file_size}" >> "output/MBC_and_RPP.txt"
    echo -r "t g ${file_size} r q" \
    | ./output/SortProgram --max_buffer_count ${MBC} --records_per_page ${RPP} \
    | sed -n '/---Start---/,/---Stop---/p' \
    >> "./output/MBC_and_RPP.txt"
    ((MBC -= MBC_DECREASE_FACTOR))
    ((RPP += RPP_INCREASE_FACTOR))
  done
done

done



# echo -r  "t g 12345678 r q" | ./output/SortProgram --max_buffer_count 50 --records_per_page 50 >> output/output.txt
