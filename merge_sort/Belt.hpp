#pragma once
#include <charconv>
#include <cstdio>
#include <fstream>
#include <string>

class Belt {
private:
  int file_num_;
  std::string belt_name_;
  std::fstream file_stream_;

public:
  Belt() {
    file_num_ = 0;
    belt_name_ = "default_name";
  }
  void init() {
    file_num_++;
    char file_name[256];
    std::sprintf(file_name, "%s_%d.txt", belt_name_, file_num_);
    file_stream_.open(sprintf("%s_%d.txt", , ...));
  }

  void write_to_file() {}
};
