#pragma once

#include "Config.hpp"
#include "Record.hpp"
#include <array>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iosfwd>
#include <iostream>
#include <optional>
#include <string_view>
#include <vector>
class IOManager {
private:
  int disc_operation_counter_;
  int reads_counter_;
  int writes_counter_;

public:
  IOManager() {
    disc_operation_counter_ = 0;
    reads_counter_ = 0;
    writes_counter_ = 0;
  }

  std::optional<std::vector<Record>> get_memory_page(std::fstream &f_stream) {

    disc_operation_counter_++;
    reads_counter_++;
    if (!f_stream.is_open()) {
      std::cerr << "File is not opened \n";
      return std::nullopt;
    }

    std::streampos curr_steam_pos = f_stream.tellg();
    std::array<char, config::page_size> buffer;

    f_stream.read(buffer.data(), config::page_size);

    if (f_stream.bad()) {
      std::cerr << "Failing to read form file\n";
      return std::nullopt;
    }
    if (f_stream.eof()) {
      return std::nullopt;
    }

    if (f_stream.gcount() != config::page_size) {
      std::cerr << "could not read the whole page";
      return std::nullopt;
    }

    std::vector<Record> return_vec;
    for (int i = 0; i < config::page_size; i += config::record_char_size + 1) {
      std::string_view run_data = std::string_view(
          &buffer.data()[i], config::record_char_size); // ignoring \n character
      return_vec.emplace_back(Record(run_data));
    }
    return return_vec;
  }

  void write_memory_page(char *buffer, int buffer_size,
                         std::fstream &f_stream) {

    disc_operation_counter_++;
    writes_counter_++;
    if (!f_stream.is_open()) {
      std::cerr << "File is not opened \n";
    }

    if (buffer_size % config::page_size != 0) {
      std::cout << "Data does not match the page, adding padding\n";
      int whole_pages_count = buffer_size / config::page_size;
      int whole_page_buffer_size = whole_pages_count * config::page_size;
      if (whole_pages_count != 0) {
        f_stream.write(buffer, whole_page_buffer_size);
      }

      char *padded_buffer = new char[config::page_size];
      std::memset(padded_buffer, 0, config::page_size);

      if (!std::strncpy(padded_buffer, &buffer[whole_page_buffer_size],
                        buffer_size - whole_page_buffer_size)) {
        std::cerr << "Error coppying into padded buffer\n";
      }

      std::cout << padded_buffer;
      f_stream.write(padded_buffer, config::page_size);

      free(padded_buffer);

      disc_operation_counter_++;
      writes_counter_++;
      return;
    }

    f_stream.write(buffer, buffer_size);

    if (f_stream.fail()) {
      std::cerr << "Failing to write to file\n";
    }
  }
};
