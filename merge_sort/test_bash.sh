#!/bin/bash

GRAIN_NUM=3

TEST_NUM=10

FILE_SIZES=(1000000 10000000 25000000 50000000 75000000 100000000)

G_MBC=10
G_RPP=10
LOOP_COUNT=0

for file_size in "${FILE_SIZES[@]}"; do
  for ((i = 0; i < GRAIN_NUM; i++)); do
    ((MBC=G_MBC + 200 * i))
    ((RPP=G_RPP + 400 * i))
    echo "--- MBC: ${MBC}"
    echo "--- RPP: ${RPP}"
    echo "--- + + "
    for ((j = 0; j < TEST_NUM; j++)); do
      ((L_MBC = MBC + 10*j))
      ((L_RPP = RPP + 50*j))
      if [[ $L_MBC -le 0 ]]; then
         continue
      fi
      if [[ $L_RPP -le 0 ]]; then
        continue
      fi 
      
      sleep .2
      echo "Record num: ${file_size}" >> "output/RPP_MBC_increase.txt"
      echo -r "t g ${file_size} r q" \
      | ./output/SortProgram --max_buffer_count ${L_MBC} --records_per_page ${L_RPP} \
      | sed -n '/---Start---/,/---Stop---/p' \
      >> "./output/RPP_MBC_increase.txt"

      ((LOOP_COUNT = LOOP_COUNT + 1))
    done

    echo "--- 0 + "
    for ((j = 0; j < TEST_NUM; j++)); do
      ((L_MBC = MBC))
      ((L_RPP = RPP + 50*j))
      if [[ $L_MBC -le 0 ]]; then
         continue
      fi
      if [[ $L_RPP -le 0 ]]; then
        continue
      fi
      
      sleep .2
      echo "Record num: ${file_size}" >> "output/RPP_increase.txt"
      echo -r "t g ${file_size} r q" \
      | ./output/SortProgram --max_buffer_count ${L_MBC} --records_per_page ${L_RPP} \
      | sed -n '/---Start---/,/---Stop---/p' \
      >> "./output/RPP_increase.txt"

      ((LOOP_COUNT = LOOP_COUNT + 1))
    done

    echo "--- + 0 "
    for ((j = 0; j < TEST_NUM; j++)); do
      ((L_MBC = MBC + 50*j))
      ((L_RPP = RPP))
      if [[ $L_MBC -le 0 ]]; then
         continue
      fi
      if [[ $L_RPP -le 0 ]]; then
        continue
      fi
      
      sleep .2
      echo "Record num: ${file_size}" >> "output/MBC_increase.txt"
      echo -r "t g ${file_size} r q" \
      | ./output/SortProgram --max_buffer_count ${L_MBC} --records_per_page ${L_RPP} \
      | sed -n '/---Start---/,/---Stop---/p' \
      >> "./output/MBC_increase.txt"
      
      ((LOOP_COUNT = LOOP_COUNT + 1))
    done
      ((LOOP_COUNT = LOOP_COUNT + 1))
  done
  ((LOOP_COUNT = LOOP_COUNT + 1))
done


G_MBC=1010
G_RPP=10

for file_size in "${FILE_SIZES[@]}"; do
  for ((j = 0; j < 21; j++)); do
    ((L_MBC = G_MBC - 50*j))
    ((L_RPP = G_RPP + 50*j))
      if [[ $L_MBC -le 0 ]]; then
         continue
      fi
      if [[ $L_RPP -le 0 ]]; then
        continue
      fi   

      sleep .2
      echo "Record num: ${file_size}" >> "output/RPP_MBC_change.txt"
      echo -r "t g ${file_size} r q" \
      | ./output/SortProgram --max_buffer_count ${L_MBC} --records_per_page ${L_RPP} \
      | sed -n '/---Start---/,/---Stop---/p' \
      >> "./output/RPP_MBC_change.txt"

      ((LOOP_COUNT = LOOP_COUNT + 1))
  done
done

echo "LOL"
for file_size in "${FILE_SIZES[@]}"; do
    ((HALF_SIZE = file_size/4))
    ((SUM = file_size + HALF_SIZE))
   
      sleep .2
      echo "Record num: ${SUM}" >> "output/mostly_sorted_.txt"
      echo -r "t g ${file_size} r g ${HALF_SIZE} r q" \
      | ./output/SortProgram --max_buffer_count ${MBC} --records_per_page ${RPP} \
      | sed -n '/---Start---/,/---Stop---/p' \
      >> "./output/mostly_sorted_.txt"
done


echo "LOOP_COUNT"
echo $LOOP_COUNT
