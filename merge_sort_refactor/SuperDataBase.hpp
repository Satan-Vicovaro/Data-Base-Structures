#pragma once
#include "Belt.hpp"
#include "BufferManager.hpp"
#include "Config.hpp"
#include "MinHeap.hpp"
#include "UserInput.hpp"
#include <algorithm>
#include <chrono>
#include <ctime>
#include <iostream>
#include <ostream>
#include <random>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

class SuperDataBase {
  std::mt19937 mt_;
  Belt main_belt_;
  Belt secondary_belt_;
  BufferManager buffer_manager_;
  std::chrono::duration<double> duration_;

public:
  SuperDataBase() {
    mt_ = std::mt19937(time(nullptr));
    main_belt_ = Belt("main");
    main_belt_.init(true);

    secondary_belt_ = Belt("secondary");
    secondary_belt_.init(false);

    buffer_manager_ = BufferManager(Config::vals().max_buffer_count);
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
        print_data_base();
        break;
      case UserInput::ADD_ROW:
        add_rows_user();
        break;
      case UserInput::NOTHING:
        std::cout << "not a valid command\n";
        break;
      case UserInput::DISPLAY_INFO:
        std::cout << "a - add row\nq - quit\nh - help\ns - show data base\n"
                     "g - generate random data\nf - load data from file\n"
                     "r - sort data\no - read One record\n";
        break;
      case UserInput::GENERATE_RANDOM_DATA:
        generate_random_data();
        break;
      case UserInput::LOAD_DATA_FROM_FILE:
        main_belt_.load_data_from_file();
        break;
      case UserInput::SORT_DATA:
        sort_data();
        print_sort_statistics();
        break;
      case UserInput::READ_RECORD:
        int index = 0;
        std::cout << "Number: \n";
        std::cin >> index;
        main_belt_.print_one_record(index);
        break;
      }
    }
    return 0;
  }

  void print_data_base() { main_belt_.print_whole_file_readable(); }
  void print_sort_statistics() {
    std::cout << "Disc read operations:\n";
    std::cout << main_belt_.read_operation_count() +
                     secondary_belt_.read_operation_count()
              << std::endl;
    std::cout << "Disc write operations:\n";
    std::cout << main_belt_.write_operation_count() +
                     secondary_belt_.write_operation_count()
              << std::endl;

    std::cout << "Time enlapsed: \n";
    std::cout << duration_.count() << std::endl;
  }
  void add_rows_user() {
    int record_num = 0;
    std::cout << "how many rows?\n";
    std::cin >> record_num;
    main_belt_.add_records_from_user(record_num);
  }

  void generate_random_data() {
    int record_num = 0;
    std::cout << "How many records?\n";
    std::cin >> record_num;
    main_belt_.generate_radom_data(mt_, record_num);
  }

  void sort_data() {
    auto start = std::chrono::high_resolution_clock::now();

    // phase one
    buffer_manager_.load_and_write_n_records(main_belt_, secondary_belt_);

    if (Config::vals().debug) {
      std::cout << "Phase one:\n";
      secondary_belt_.print_whole_file_readable();
    }

    std::swap(main_belt_, secondary_belt_);
    secondary_belt_.truncate_file();

    // phase two
    bool one_run_left = false;
    while (!one_run_left) {

      bool end_of_file = false;
      bool first_loop = true;
      while (!buffer_manager_.initialize_buffers(main_belt_)) {
        if (first_loop) {
          first_loop = false;
          if (buffer_manager_.get_buffers_count() == 1) {
            one_run_left = true;
            break;
          }
        }

        MinHeap min_heap = MinHeap();
        secondary_belt_.save_run();

        min_heap.init(buffer_manager_, main_belt_);

        std::vector<Buffer> &buffers = buffer_manager_.get_buffers_ref();

        while (!min_heap.empty()) {
          auto [record, buffer_index] = min_heap.pop();
          buffer_manager_.append_to_out_buffer(record, secondary_belt_);
          auto new_record = buffers[buffer_index].get_record(main_belt_);
          if (!new_record) {
            continue;
          }
          min_heap.append(std::make_tuple(*new_record, buffer_index));
        }

        buffer_manager_.write_remanings_of_out_buffer(secondary_belt_);

        if (Config::vals().debug) {
          secondary_belt_.print_whole_file_readable();
        }
      }
      if (!one_run_left) {
        std::swap(main_belt_, secondary_belt_);
        secondary_belt_.truncate_file();
      }
    }

    auto stop = std::chrono::high_resolution_clock::now();
    duration_ = stop - start;
    buffer_manager_.clear_buffers();
  }

  UserInput getUserInput() {
    std::string input_string;
    std::cout << "Waiting for input\n";
    std::cin >> input_string;
    char input_char = input_string[0];
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
    case ('o'):
      return UserInput::READ_RECORD;
    }
    return UserInput::NOTHING;
  }
};
