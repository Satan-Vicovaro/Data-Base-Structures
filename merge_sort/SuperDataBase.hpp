#pragma once
#include "Belt.hpp"
#include "UserInput.hpp"
#include <cctype>
#include <iostream>
#include <random>
#include <tuple>
#include <vector>

class SuperDataBase {
private:
  Belt main_belt_;
  std::mt19937 mt_;

public:
  SuperDataBase() {
    main_belt_ = Belt("main_belt");
    main_belt_.init();
    std::mt19937 mt_(time(nullptr));
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
    while (!end_of_chunks) {
      std::tuple<std::vector<Record>, bool> result_val =
          main_belt_.get_next_chunk();
      end_of_chunks = std::get<1>(result_val);
      std::vector<Record> chunk = std::get<0>(result_val);
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
