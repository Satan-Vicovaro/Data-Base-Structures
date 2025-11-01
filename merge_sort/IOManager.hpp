#pragma once
#include "Config.hpp"
#include "Record.hpp"
#include <array>
#include <fstream>
#include <iosfwd>
#include <iostream>
#include <optional>
#include <string_view>
#include <vector>
class IOManager {
private:
  int disc_operation_counter_;

public:
  IOManager() { disc_operation_counter_ = 0; }

  std::optional<std::vector<Record>>
  get_memory_page(std::fstream &f_stream, std::streampos stream_pos) {

    disc_operation_counter_++;
    if (!f_stream.is_open()) {
      std::cerr << "File is not opened \n";
      return std::nullopt;
    }

    std::streampos curr_steam_pos = f_stream.tellg();
    std::array<char, config::page_size> buffer;

    f_stream.read(buffer.data(), config::page_size);
    stream_pos = curr_steam_pos;

    if (f_stream.fail()) {
      std::cerr << "Failing to read form file\n";
      return std::nullopt;
    }

    if (f_stream.gcount() != config::page_size) {
      std::cerr << "could not read the whole page";
      return std::nullopt;
    }

    std::vector<Record> return_vec;
    for (int i = 0; i < config::page_size; i += config::record_char_size + 1) {
      std::string_view run_data =
          std::string_view(&buffer.data()[i], config::record_char_size + 1);
      return_vec.emplace_back(Record(run_data));
    }
    return return_vec;
  }

  void write_memory_page(std::vector<Record> records) {
    int bytes_num = records.size() * config::record_char_size;
  }
};
