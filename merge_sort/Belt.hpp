#pragma once
#include "Record.hpp"
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

class Belt {
private:
  int file_num_;
  std::string belt_name_;
  std::fstream file_stream_;
  std::string file_name_;

public:
  Belt() {
    file_num_ = 0;
    belt_name_ = "default_name";
    file_name_ = std::string(belt_name_ + ".txt");
  }

  Belt(std::string_view string_name) {
    file_num_ = 0;
    belt_name_ = std::string(string_name);
    file_name_ = std::string(string_name) + ".txt";
  }
  void init() {
    file_stream_.open(file_name_, std::ios::app);
    file_stream_.close();
  }

  void write_to_file(const std::vector<Record> &records) {

    file_stream_.open(file_name_);
    if (!file_stream_.is_open()) {
      std::cout << "Could not open file: " << file_name_ << std::endl;
      file_stream_.clear();
    }

    for (auto record : records) {
      std::string_view value = record.getRecord();
      file_stream_.write(value.data(), value.length());
      file_stream_.write("\n", 1);
    }
    file_stream_.close();
  }
  void print_whole_file() { file_stream_.open(file_name_); }
};
