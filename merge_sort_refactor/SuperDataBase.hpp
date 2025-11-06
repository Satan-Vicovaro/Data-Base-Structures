#include "Belt.hpp"
#include "UserInput.hpp"
#include <ctime>
#include <iostream>
#include <random>
#include <string>

class SuperDataBase {

  std::mt19937 mt_;
  Belt main_belt_;
  Belt secondary_belt_;

public:
  SuperDataBase() {
    mt_ = std::mt19937(time(nullptr));
    main_belt_ = Belt("main");
    main_belt_.init(true);

    secondary_belt_ = Belt("secondary");
    secondary_belt_.init(false);
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
        std::cout << "TODO: sort_data\n";
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
  void add_rows_user() {
    int record_num = 0;
    std::cout << "how many rows?\n";
    std::cin >> record_num;
    std::cout << "TODO: add_rows_user\n";
  }
  void generate_random_data() {
    int record_num = 0;
    std::cout << "How many records?\n";
    std::cin >> record_num;
    main_belt_.generate_radom_data(mt_, record_num);
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
