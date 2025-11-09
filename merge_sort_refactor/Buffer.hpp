#pragma once
#include "Belt.hpp"
#include "Config.hpp"
#include "Record.hpp"
#include "Run.hpp"
#include <algorithm>
#include <iostream>
#include <optional>
#include <tuple>
#include <utility>
#include <vector>

class Buffer {
private:
  Run run_;
  std::vector<Record> records_;

public:
  Buffer() {
    run_ = Run();
    records_ = std::vector<Record>();
  }
  Buffer(Run starting_run) {
    run_ = starting_run;
    records_ = std::vector<Record>();
  }

  void init(Belt &belt) { load_next_records(belt); }

  std::optional<Record> get_record(Belt &belt) {

    if (!records_.empty()) {
      Record record = records_.back();
      records_.pop_back();
      return record;
    }

    if (!load_next_records(belt)) {
      Record record = records_.back();
      records_.pop_back();
      return record;
    }
    return std::nullopt;
  }

  bool load_next_records(Belt &belt) {
    bool end_of_run = false;
    std::tie(records_, end_of_run) = run_.get_next_records(belt);
    if (end_of_run) {
      records_.clear();
      return true;
    }
    std::reverse(records_.begin(), records_.end());
    return false;
  }

  // writes to file if full
  void append_out_buffer(Record &&record, Belt &belt) {
    records_.emplace_back(std::move(record));
    if (records_.size() >= Config::vals().records_per_page) {
      belt.append_to_file(records_);
      records_.clear();
    }
  }

  // empties the buffer
  void write_buffer(Belt &belt) {
    if (records_.size() > 0) {
      belt.append_to_file(records_);
      records_.clear();
      return;
    }
    std::cout << "Trying to write empty buffer to file\n";
  }
};
