#pragma once
#include "Belt.hpp"
#include "Buffer.hpp"
#include "Config.hpp"
#include "IOManager.hpp"
#include "Record.hpp"
#include "RunGenerator.hpp"
#include "UserInput.hpp"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <random>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

class SuperDataBase {
private:
  Belt main_belt_;
  Belt secondary_belt_;
  std::mt19937 mt_;
  RunGenerator run_generator_;
  IOManager io_manager_;

public:
  SuperDataBase() {
    io_manager_ = IOManager();
    main_belt_ = Belt(std::make_shared<IOManager>(io_manager_), "main_belt");
    main_belt_.init(true);
    secondary_belt_ =
        Belt(std::make_shared<IOManager>(io_manager_), "secondary_belt");
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

  void phase_1_sort() {
    bool end_of_chunks = false;

    std::vector<std::vector<Record>> chunk_vec;

    while (!end_of_chunks) {
      for (int i = 0; i < config::max_buffer_count && !end_of_chunks; i++) {
        std::vector<Record> chunk;
        std::tie(chunk, end_of_chunks) = main_belt_.get_next_chunk();
        chunk_vec.emplace_back(chunk);
      }

      if (end_of_chunks) {
        break;
      }

      std::vector<Record *> flat_record_list;
      for (std::vector<Record> &chunk : chunk_vec) {
        for (Record &record : chunk) {
          flat_record_list.emplace_back(&record);
        }
      }

      std::sort(flat_record_list.begin(), flat_record_list.end(),
                Record::compare_function);

      secondary_belt_.save_next_chunk(&flat_record_list);

      // if (config::debug) {
      //   for (Record &record : chunk) {
      //     record.print();
      //   }
      // }
    }
    std::swap(main_belt_, secondary_belt_);
    main_belt_.print_whole_file_readable();
  }

  void phase_2_sort() {
    bool one_run_left = false;
    // std::fstream *file_stream;
    while (!one_run_left) {

      int currently_used_belt = 0;

      std::fstream &file_stream = main_belt_.get_opened_stream();

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
            run_generator_.get_runs(config::max_buffer_count, file_stream);

        if (config::debug) {
          print_runs(runs, "Runs looking at:");
        }

        // load buffers
        std::vector<Buffer> buffers;
        for (Run run : runs) {
          buffers.emplace_back(
              Buffer(run, config::records_to_load, file_stream));
        }

        // initialize min heap
        for (int i = 0; i < buffers.size(); i++) {
          Buffer &buf = buffers[i];
          auto record = buf.get_record(file_stream);
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

          auto new_record = buff.get_record(file_stream);
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

        secondary_belt_.append_to_file(output_buffer);
        if (config::debug) {
          print_records(output_buffer, "Saved chunk: ");
        }
        output_buffer.clear();
      }

      main_belt_.close_opened_stream();
      if (config::debug)
        main_belt_.print_whole_file_readable();
      one_run_left = run_generator_.is_one_run_left();
      run_generator_.reset();

      if (!one_run_left) {
        main_belt_.truncate_file();
        std::swap(main_belt_, secondary_belt_);
      }
    }
    std::swap(main_belt_, secondary_belt_);
  }

  void sort_data_base() {
    phase_1_sort();
    phase_2_sort();
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

  static void print_runs(std::vector<Run> &runs,
                         std::string_view additional_text) {

    std::cout << additional_text << std::endl;
    for (Run run : runs) {
      run.current_record_.print();
    }
  }
  static void print_records(std::vector<Record> records,
                            std::string_view additional_text) {

    std::cout << additional_text << std::endl;
    for (Record record : records) {
      record.print();
    }
  }
};
