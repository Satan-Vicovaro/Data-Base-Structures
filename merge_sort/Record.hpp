#pragma once
#include "Config.hpp"
#include <cstdio>
#include <random>
#include <string>
#include <string_view>
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

  Record(std::string record) {
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
};
