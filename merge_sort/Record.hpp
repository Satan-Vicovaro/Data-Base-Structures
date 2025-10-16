#pragma once
#include <cstdio>
#include <string>
#include <string_view>
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
};
