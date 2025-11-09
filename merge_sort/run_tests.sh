#!/bin/bash

DATA_FILE="data_file.txt"
TEST_NUM=20
# FILE_SIZES=(1000 50000 100000 1000000 10000000 25000000 50000000 75000000 100000000)

FILE_SIZES=(25000000 50000000 75000000 100000000)

echo "RPP increase"
for file_size in "${FILE_SIZES[@]}"; do
  MBC=500
  MBC_DECREASE_FACTOR=0
  RPP=500
  RPP_INCREASE_FACTOR=100
  for ((i = 0; i < TEST_NUM; i++)); do
    echo "Record num: ${file_size}" >> "output/${DATA_FILE}"
    echo -r "t g ${file_size} r q" \
    | ./output/SortProgram --max_buffer_count ${MBC} --records_per_page ${RPP} \
    | sed -n '/---Start---/,/---Stop---/p' \
    >> "output/${DATA_FILE}"
    ((MBC -= MBC_DECREASE_FACTOR))
    ((RPP += RPP_INCREASE_FACTOR))
  done
done

echo "MBC decrease"

for file_size in "${FILE_SIZES[@]}"; do
  MBC=500
  MBC_DECREASE_FACTOR=30
  RPP=500
  RPP_INCREASE_FACTOR=0
  for ((i = 0; i < TEST_NUM; i++)); do
    echo "Record num: ${file_size}" >> "output/${DATA_FILE}"
    echo -r "t g ${file_size} r q" \
    | ./output/SortProgram --max_buffer_count ${MBC} --records_per_page ${RPP} \
    | sed -n '/---Start---/,/---Stop---/p' \
    >> "output/${DATA_FILE}"
    ((MBC -= MBC_DECREASE_FACTOR))
    ((RPP += RPP_INCREASE_FACTOR))
  done
done

echo "MBC decrease and  RPP increase"

for file_size in "${FILE_SIZES[@]}"; do
  MBC=1000
  MBC_DECREASE_FACTOR=40
  RPP=700
  RPP_INCREASE_FACTOR=40
  for ((i = 0; i < TEST_NUM; i++)); do
    echo "Record num: ${file_size}" >> "output/${DATA_FILE}"
    echo -r "t g ${file_size} r q" \
    | ./output/SortProgram --max_buffer_count ${MBC} --records_per_page ${RPP} \
    | sed -n '/---Start---/,/---Stop---/p' \
    >> "output/${DATA_FILE}"
    ((MBC -= MBC_DECREASE_FACTOR))
    ((RPP += RPP_INCREASE_FACTOR))
  done
done

# echo -r  "t g 12345678 r q" | ./output/SortProgram --max_buffer_count 50 --records_per_page 50 >> output/output.txt
