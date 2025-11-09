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
  SuperDataBase();
  int start();
  void print_data_base();
  void print_sort_statistics();
  void print_compact_sort_statistics();
  void add_rows_user();
  void generate_random_data();
  void sort_data();
  void reset_sort_vars();
  UserInput getUserInput();
};
