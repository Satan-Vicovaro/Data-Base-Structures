#pragma once
#include "Record.hpp"
#include <fstream>
#include <iosfwd>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>

struct Run {
  std::streampos current_record_pos_;
  Record current_record_;

  Run() {
    current_record_pos_ = 0;
    current_record_ = Record();
  }

  Run(std::streampos initial_record_pos_, std::fstream &file_stream) {

    file_stream.seekg(initial_record_pos_, std::ios::beg);
    current_record_pos_ = file_stream.tellg();

    std::string line = std::string();
    if (std::getline(file_stream, line)) {
      current_record_ = Record(line);
    } else {
      std::cout << "Could not initialize the run properrly";
      current_record_ = Record();
    }
  }

  std::tuple<std::vector<Record>, bool>
  get_next_records(int record_num, std::fstream &file_stream) {
    bool end_of_run = false;
    int records_read = 0;
    std::vector<Record> result = {};
    std::string line;

    file_stream.seekg(current_record_pos_, std::ios::beg);

    while (std::getline(file_stream, line) && records_read < record_num) {
      Record next_record = Record(line);
      if (next_record < current_record_) {
        end_of_run = true;
        break;
      }
      result.emplace_back(Record(line));
      current_record_ = next_record;
      records_read++;
    }

    if (file_stream.eof()) {
      end_of_run = true;
      return std::make_tuple(result, end_of_run);
    }

    std::getline(file_stream, line);

    Record next_record = Record(line);
    if (next_record < current_record_) {
      end_of_run = true;
      return std::make_tuple(result, end_of_run);
    }

    current_record_ = next_record;
    current_record_pos_ = file_stream.tellg();

    return std::make_tuple(result, end_of_run);
  }

  std::vector<Run> generate_runs(int runs_num, std::fstream &file_stream) {
    std::streampos streampos = 0;
    std::vector<Run> runs = {};
    runs.emplace_back(Run(streampos, file_stream));
    while (runs.size() < runs_num) {
    }
  }
};
