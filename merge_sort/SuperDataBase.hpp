#pragma once
#include "Belt.hpp"
#include "UserInput.hpp"
#include <cctype>
#include <iostream>

class SuperDataBase {
private:
  Belt main_belt_;

public:
  SuperDataBase() { main_belt_ = Belt("main_belt"); }
  SuperDataBase(SuperDataBase &&) = default;
  SuperDataBase &operator=(SuperDataBase &&) = default;
  ~SuperDataBase();

  int start() {
    bool quit = false;
    while (!quit) {
      UserInput input = getUserInput();
      switch (input) {
      case UserInput::QUIT:
        quit = true;
        break;

      case UserInput::SHOW_DATA_BASE:
        quit = true;
        break;

      case UserInput::ADD_ROW:
        quit = true;
        break;

      case UserInput::NOTHING:
        quit = true;
        break;
      }
    }
    return 0;
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
    }
    return UserInput::NOTHING;
  }
};
