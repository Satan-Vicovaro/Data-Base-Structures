#!/bin/bash

DATA_FILE="data_file.txt"
TEST_NUM=20
FILE_SIZES=(1000 50000 100000 1000000 10000000 50000000 100000000)

for file_size in "${FILE_SIZES[@]}"; do
  MBC=500
  MBC_DECREASE_FACTOR=30
  RPP=500
  RPP_INCREASE_FACTOR=0
  for ((i = 0; i < TEST_NUM; i++)); do
    echo -r "Record num:${file_size} " >> "output/${DATA_FILE}"
    echo -r "t g ${file_size} r q" \
    | ./output/SortProgram --max_buffer_count ${MBC} --records_per_page ${RPP} \
    | sed -n '/---Start---/,/---Stop---/p' \
    >> "output/${DATA_FILE}"
    ((MBC -= MBC_DECREASE_FACTOR))
    ((RPP += RPP_INCREASE_FACTOR))
  done
done


for file_size in "${FILE_SIZES[@]}"; do
  MBC=500
  MBC_DECREASE_FACTOR=0
  RPP=500
  RPP_INCREASE_FACTOR=100
  for ((i = 0; i < TEST_NUM; i++)); do
    echo -r "Record num:${file_size} " >> "output/${DATA_FILE}"
    echo -r "t g ${file_size} r q" \
    | ./output/SortProgram --max_buffer_count ${MBC} --records_per_page ${RPP} \
    | sed -n '/---Start---/,/---Stop---/p' \
    >> "output/${DATA_FILE}"
    ((MBC -= MBC_DECREASE_FACTOR))
    ((RPP += RPP_INCREASE_FACTOR))
  done
done

for file_size in "${FILE_SIZES[@]}"; do
  MBC=500
  MBC_DECREASE_FACTOR=30
  RPP=500
  RPP_INCREASE_FACTOR=100
  for ((i = 0; i < TEST_NUM; i++)); do
    echo -r "Record num:${file_size} " >> "output/${DATA_FILE}"
    echo -r "t g ${file_size} r q" \
    | ./output/SortProgram --max_buffer_count ${MBC} --records_per_page ${RPP} \
    | sed -n '/---Start---/,/---Stop---/p' \
    >> "output/${DATA_FILE}"
    ((MBC -= MBC_DECREASE_FACTOR))
    ((RPP += RPP_INCREASE_FACTOR))
  done
done
