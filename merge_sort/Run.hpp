#pragma once
#include "IOManager.hpp"
#include "Record.hpp"
#include <fstream>
#include <iosfwd>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

struct Run {
  std::streampos current_record_pos_;
  Record current_record_;
  std::shared_ptr<IOManager> io_manager_;

  Run() {
    current_record_pos_ = 0;
    current_record_ = Record();
    io_manager_ = nullptr;
  }

  Run(std::streampos initial_record_pos_, std::fstream &file_stream,
      std::shared_ptr<IOManager> manager) {
    io_manager_ = manager;
    current_record_pos_ = initial_record_pos_;
    current_record_ = Record();
  }

  std::tuple<std::vector<Record>, bool>
  get_next_records(int record_num, std::fstream &file_stream) {
    bool end_of_run = false;
    std::vector<Record> records;

    file_stream.clear();
    file_stream.seekg(current_record_pos_, std::ios::beg);
    if (!file_stream) {
      std::cerr << "seekg failed at position " << current_record_pos_ << '\n';
      return {records, true}; // Early return to avoid undefined behavior
    }

    auto opt_vector = io_manager_->get_memory_page(file_stream);
    if (!opt_vector) {
      std::cerr << "Error reading page?\n";
      return {records, true};
    }
    records = std::move(*opt_vector);

    Record last_record = records.back();

    if (last_record < current_record_) {
      std::cerr << "Memory page is not sorted properly\n";
      end_of_run = true;
    }

    current_record_ = last_record;
    if (file_stream.eof()) {
      end_of_run = true;
      return {records, end_of_run};
    }

    return {records, end_of_run};
  }
};
