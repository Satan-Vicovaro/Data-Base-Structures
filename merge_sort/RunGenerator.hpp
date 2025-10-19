#pragma once
#include "Run.hpp"
#include <iosfwd>
#include <string>
#include <tuple>
#include <vector>
class RunGenerator {
private:
  std::streampos current_stream_pos_;
  bool reached_eof_;

public:
  RunGenerator() {
    current_stream_pos_ = 0;
    reached_eof_ = false;
  }

  std::tuple<std::vector<Run>, bool> get_runs(int runs_num,
                                              std::fstream &file_stream) {
    std::vector<Run> runs = {};
    Run first_run = Run(current_stream_pos_, file_stream);
    runs.emplace_back(first_run);

    Record cur_record = first_run.current_record_;

    std::string line = std::string();
    while (runs.size() < runs_num) {
      if (!std::getline(file_stream, line)) {
        reached_eof_ = true;
        break;
      }

      Record next_record = Record(line);
      if (next_record < cur_record) {
        runs.emplace_back(Run(current_stream_pos_, file_stream));
      }
      current_stream_pos_ = file_stream.tellg();
      cur_record = next_record;
    }

    bool found_next_starting_point = false;
    while (!found_next_starting_point) {
      if (!std::getline(file_stream, line)) {
        reached_eof_ = true;
        break;
      }

      Record next_record = Record(line);
      if (next_record < cur_record) {
        found_next_starting_point = true;
      }

      current_stream_pos_ = file_stream.tellg();
    }

    return std::make_tuple(runs, reached_eof_);
  }
};
