#pragma once
#include "Config.hpp"
#include <cstdio>
#include <iostream>
#include <random>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>
class Record {
private:
  std::string record_;
  int ones_num_;

public:
  Record() {
    record_ = std::string();
    ones_num_ = 0;
  }

  Record(const Record &) = default;

  Record(std::string record) {
    record_ = record;
    ones_num_ = count_zeros_in_binary(record);
  }
  Record(std::string_view record) {
    record_ = record;
    ones_num_ = count_zeros_in_binary(record);
  }

  ~Record() {}

  void print() {
    std::printf("record: %s, ones_num: %d\n", record_.c_str(), ones_num_);
  }

  std::string_view getRecord() { return record_; }

  static int count_zeros_in_binary(std::string_view string) {
    int ones_num = 0;
    for (int i = 0; i < string.length(); i++) {
      char character = string[i];

      char mask = 1;
      for (int j = 0; j < 8; j++) {
        if (mask & character) {
          ones_num++;
        }
        mask = mask << 1;
      }
    }
    return ones_num;
  }

  static std::vector<Record> generate_random_records(std::mt19937 &generator,
                                                     int record_num) {

    std::vector<Record> return_val;
    for (int j = 0; j < record_num; j++) {

      std::string record_data = std::string(config::record_char_size, 'C');
      for (int i = 0; i < config::record_char_size; i++) {
        record_data[i] = (char)('!' + (generator() % ('~' - '!')));
      }
      return_val.emplace_back(Record(record_data));
    }
    return return_val;
  }

  void into_file_string(char &buffer, int &buffsize) {
    if (record_.size() > buffsize) {
      std::cout << "record size if bigger than buffsize, skipping string\n";
      return;
    }
    record_.copy(&buffer, record_.size());
    buffsize = record_.size();
  }

  bool operator==(const Record &other) const {
    return (ones_num_ == other.ones_num_);
  }

  bool operator!=(const Record &other) const {
    return (ones_num_ != other.ones_num_);
  }

  bool operator>(const Record &other) const {
    return (ones_num_ > other.ones_num_);
  }

  bool operator<(const Record &other) const {
    return (ones_num_ < other.ones_num_);
  }

  bool operator>=(const Record &other) const {
    return (ones_num_ >= other.ones_num_);
  }

  bool operator<=(const Record &other) const {
    return (ones_num_ <= other.ones_num_);
  }

  static bool compare_function(Record *a, Record *b) {
    return a->ones_num_ < b->ones_num_;
  }
};
