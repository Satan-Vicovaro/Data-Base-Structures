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
    main_belt_.init();
    secondary_belt_ = Belt("secondary_belt");
    secondary_belt_.init();
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
        main_belt_.print_whole_file();
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
    std::fstream &file_stream = main_belt_.get_opened_stream();

    bool end_of_file = false;
    std::vector<Run> runs;

    std::vector<Record> min_heap;
    while (!end_of_file) {
      std::tie(runs, end_of_file) =
          run_generator_.get_runs(config::max_buffer_count, file_stream);

      if (config::debug) {
        std::cout << "runs looking at\n";
        for (Run run : runs) {
          run.current_record_.print();
        }
      }

      std::vector<Buffer> buffers;
      for (Run run : runs) {
        buffers.emplace_back(Buffer(run, config::records_to_load, file_stream));
      }

      bool all_buffers_empty = false;
      std::vector<Record> output_buffer;
      while (!all_buffers_empty) {
        bool are_empty = true;

        for (int i = 0; i < buffers.capacity(); i++) {
          Buffer &buf = buffers[i];
          auto [record, is_empty] = buf.get_record(file_stream);
          if (!is_empty) {
            min_heap.emplace_back(record);
            std::push_heap(min_heap.begin(), min_heap.end());
            are_empty = false;
          }
        }
        std::pop_heap(min_heap.begin(), min_heap.end());
        output_buffer.emplace_back(min_heap.back());
        all_buffers_empty = are_empty;
      }

      if (config::debug) {
        for (Record &record : min_heap) {
          record.print();
        }
      }
    }
    if (config::debug) {
      std::cout << "final sort:\n";
      for (Record &record : min_heap) {
        record.print();
      }
    }

    main_belt_.close_opened_stream();
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
