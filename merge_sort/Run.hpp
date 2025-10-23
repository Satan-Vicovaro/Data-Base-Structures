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
    std::string line = std::string();
    if (std::getline(file_stream, line)) {
      current_record_ = Record(line);
    } else {
      std::cout << "Could not initialize the run properrly";
      current_record_ = Record();
    }
    current_record_pos_ = file_stream.tellg();
  }

  std::tuple<std::vector<Record>, bool>
  get_next_records(int record_num, std::fstream &file_stream) {
    bool end_of_run = false;
    int records_read = 0;
    std::vector<Record> result;
    result.emplace_back(current_record_);
    records_read++;

    std::string line;
    file_stream.clear();
    file_stream.seekg(current_record_pos_, std::ios::beg);
    if (!file_stream) {
      std::cerr << "seekg failed at position " << current_record_pos_ << '\n';
      return {result, true}; // Early return to avoid undefined behavior
    }

    while (records_read < record_num && std::getline(file_stream, line)) {
      Record next_record = Record(line);
      if (next_record < current_record_) {
        end_of_run = true;
        break;
      }
      result.emplace_back(Record(line));
      current_record_ = next_record;
      records_read++;
    }

    if (end_of_run) {
      return {result, end_of_run};
    }

    if (file_stream.eof()) {
      end_of_run = true;
      return {result, end_of_run};
    }

    std::getline(file_stream, line);

    Record next_record = Record(line);
    if (next_record < current_record_) {
      end_of_run = true;
      return {result, end_of_run};
    }

    current_record_ = next_record;
    current_record_pos_ = file_stream.tellg();

    return {result, end_of_run};
  }
};
