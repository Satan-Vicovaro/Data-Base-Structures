#pragma once

#include "Config.hpp"
#include "Record.hpp"
#include "Run.hpp"
#include <algorithm>
#include <atomic>
#include <cstddef>
#include <fstream>
#include <iosfwd>
#include <tuple>
#include <vector>
class Buffer {
private:
  Run run_;
  std::vector<Record> records_;
  bool end_of_run_;

public:
  Buffer() {
    run_ = Run();
    records_ = {};
    end_of_run_ = false;
  }
  Buffer(std::streampos initial_stream_pos, std::fstream &file_stream) {
    run_ = Run(initial_stream_pos, file_stream);
    records_ = {};
    end_of_run_ = false;
  }
  Buffer(Run &run, int initial_load, std::fstream &file_stream) {
    run_ = run;
    std::tie(records_, end_of_run_) =
        run_.get_next_records(initial_load, file_stream);
  }

  void init(std::fstream &file_stream) {
    std::tie(records_, end_of_run_) =
        run_.get_next_records(config::records_to_load, file_stream);
    std::reverse(records_.begin(), records_.end());
  }

  std::tuple<Record, bool> get_record(std::fstream &file_stream) {

    if (!records_.empty()) {
      Record record = records_.back();
      records_.pop_back();
      return {record, false};
    }

    if (end_of_run_) {
      return {Record(), true};
    }

    std::tie(records_, end_of_run_) =
        run_.get_next_records(config::records_to_load, file_stream);
    std::reverse(records_.begin(), records_.end());

    Record record = records_.back();
    records_.pop_back();
    return {record, false};
  }

  void print_records() {
    for (Record record : records_) {
      record.print();
    }
  }
};
