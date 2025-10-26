#pragma once
#include "Belt.hpp"
#include "Buffer.hpp"
#include "Config.hpp"
#include "Record.hpp"
#include "RunGenerator.hpp"
#include "UserInput.hpp"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <random>
#include <tuple>
#include <utility>
#include <vector>

class SuperDataBase {
private:
  Belt main_belt_;
  Belt secondary_belt_;
  std::mt19937 mt_;
  RunGenerator run_generator_;

public:
  SuperDataBase() {
    main_belt_ = Belt("main_belt");
    main_belt_.init(true);
    secondary_belt_ = Belt("secondary_belt");
    secondary_belt_.init(false);
    std::mt19937 mt_(time(nullptr));
    run_generator_ = RunGenerator();
  }
  int start() {

    std::cout << "type 'h' for help\n";
    bool quit = false;
    int record_num = 0;
    while (!quit) {
      UserInput input = getUserInput();
      switch (input) {
      case UserInput::QUIT:
        quit = true;
        break;
      case UserInput::SHOW_DATA_BASE:
        main_belt_.print_whole_file_readable();
        break;
      case UserInput::ADD_ROW:
        std::cout << "how many rows?\n";
        std::cin >> record_num;
        main_belt_.add_records_from_user(record_num);
        break;
      case UserInput::NOTHING:
        std::cout << "not a valid command\n";
        break;
      case UserInput::DISPLAY_INFO:
        std::cout << "a - add row\nq - quit\nh - help\ns - show data base\n"
                     "g - generate random data\nf - load data from file\n"
                     "r - sort data\n";
        break;
      case UserInput::GENERATE_RANDOM_DATA:
        std::cout << "How many records?\n";
        std::cin >> record_num;
        main_belt_.generate_radom_data(mt_, record_num);
        break;
      case UserInput::LOAD_DATA_FROM_FILE:
        main_belt_.load_data_from_file();
        break;
      case UserInput::SORT_DATA:
        sort_data_base();
        break;
      }
    }
    return 0;
  }

  void sort_data_base() {
    // phase 1, sort chunks in memory
    bool end_of_chunks = false;
    std::vector<Record> chunk;
    while (!end_of_chunks) {
      std::tie(chunk, end_of_chunks) = main_belt_.get_next_chunk();

      std::sort(chunk.begin(), chunk.end());

      main_belt_.save_next_chunk(chunk);

      if (config::debug) {
        for (Record &record : chunk) {
          record.print();
        }
      }
    }

    // phase 2, start merging parts
    bool one_run_left = false;
    std::fstream *file_stream;
    while (!one_run_left) {

      int currently_used_belt = 0;

      if (currently_used_belt == 0) {
        file_stream = &main_belt_.get_opened_stream();
      } else {
        file_stream = &secondary_belt_.get_opened_stream();
      }

      bool end_of_file = false;
      std::vector<Run> runs;

      std::vector<std::tuple<Record, int>> min_heap;
      auto min_comp = [](const std::tuple<Record, int> &a,
                         const std::tuple<Record, int> &b) {
        Record r_a = std::get<0>(a);
        Record r_b = std::get<0>(b);
        return a > b;
      };

      std::vector<Record> output_buffer;

      while (!end_of_file) {
        // generating runs
        std::tie(runs, end_of_file) =
            run_generator_.get_runs(config::max_buffer_count, *file_stream);

        if (config::debug) {
          std::cout << "runs looking at\n";
          for (Run run : runs) {
            run.current_record_.print();
          }
        }

        // load buffers
        std::vector<Buffer> buffers;
        for (Run run : runs) {
          buffers.emplace_back(
              Buffer(run, config::records_to_load, *file_stream));
        }

        // initialize min heap
        for (int i = 0; i < buffers.size(); i++) {
          Buffer &buf = buffers[i];
          auto record = buf.get_record(*file_stream);
          if (!record) {
            continue;
          }
          min_heap.emplace_back(std::move(*record), i);
        }
        std::make_heap(min_heap.begin(), min_heap.end(), min_comp);

        // min_heap -> output_buffer
        int last_buffer_index = 0;
        while (!min_heap.empty()) {
          std::pop_heap(min_heap.begin(), min_heap.end(), min_comp);
          auto [record, buff_index] = min_heap.back();
          min_heap.pop_back();
          output_buffer.emplace_back(std::move(record));

          last_buffer_index = buff_index;
          // get next element form buffer
          Buffer &buff = buffers[buff_index];
          if (buff.is_buffer_finished()) {
            continue;
          }

          auto new_record = buff.get_record(*file_stream);
          if (!new_record) {
            continue;
          }

          min_heap.emplace_back(std::move(*new_record), buff_index);
          std::push_heap(min_heap.begin(), min_heap.end(), min_comp);
        }

        // if heap is empty there is nothing left in Buffers, or only one lasts
        Buffer &last_buffer = buffers[last_buffer_index];
        if (!last_buffer.is_buffer_finished()) {
          std::vector<Record> records = last_buffer.get_records();
          output_buffer.insert(output_buffer.end(), records.begin(),
                               records.end());
        }

        if (currently_used_belt == 0) {
          secondary_belt_.append_to_file(output_buffer);
        } else {
          main_belt_.append_to_file(output_buffer);
        }

        if (config::debug) {
          std::cout << "Saved chunk: \n";
          for (Record record : output_buffer) {
            record.print();
          }
        }
        output_buffer.clear();
      }

      if (currently_used_belt == 0) {
        main_belt_.close_opened_stream();
        one_run_left = secondary_belt_.is_file_sorted();
      } else {
        secondary_belt_.close_opened_stream();
        one_run_left = main_belt_.is_file_sorted();
      }

      currently_used_belt =
          (currently_used_belt + 1) % 2; // we are using only 2 belts
    }
  }

  UserInput getUserInput() {
    char input_char = 0;
    std::cout << "Waiting for input\n";
    std::cin >> input_char;
    input_char = std::tolower(input_char);
    switch (input_char) {
    case ('a'):
      return UserInput::ADD_ROW;
    case ('q'):
      return UserInput::QUIT;
    case ('h'):
      return UserInput::DISPLAY_INFO;
    case ('s'):
      return UserInput::SHOW_DATA_BASE;
    case ('g'):
      return UserInput::GENERATE_RANDOM_DATA;
    case ('f'):
      return UserInput::LOAD_DATA_FROM_FILE;
    case ('r'):
      return UserInput::SORT_DATA;
    }
    return UserInput::NOTHING;
  }
};
