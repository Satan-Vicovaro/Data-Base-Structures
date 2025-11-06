#pragma once
#include "IOManager.hpp"
#include "Record.hpp"
#include <memory>

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
};
