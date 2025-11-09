#pragma once
#include "Config.hpp"
#include "Record.hpp"
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iosfwd>
#include <iostream>
#include <optional>
#include <string_view>
#include <vector>

enum IOManagerInfo {
  SUCCES,
  END_OF_FILE,
  FILE_NOT_OPENED,
  READ_ERROR,
  NOT_WHOLE_PAGE_ERROR
};

struct IOManagerResult {
  std::vector<Record> records;
  IOManagerInfo info;
};

struct IOManager {
  int disc_operation_counter_;
  int reads_counter_;
  int writes_counter_;

public:
  IOManager() {
    disc_operation_counter_ = 0;
    reads_counter_ = 0;
    writes_counter_ = 0;
  };

  IOManagerResult get_memory_page(std::fstream &f_stream) {

    IOManagerResult return_val;
    disc_operation_counter_++;
    reads_counter_++;

    std::vector<Record> return_vec;

    if (!f_stream.is_open()) {
      std::cerr << "File is not opened \n";
      return_val.records = return_vec;
      return_val.info = IOManagerInfo::FILE_NOT_OPENED;
      return return_val;
    }
    f_stream.clear();

    char *buffer = new char[Config::vals().page_size];

    f_stream.read(buffer, Config::vals().page_size);

    if (f_stream.bad()) {
      std::cerr << "Failing to read form file\n";
      free(buffer);
      return_val.records = return_vec;
      return_val.info = IOManagerInfo::READ_ERROR;
      return return_val;
    }

    if (f_stream.gcount() == 0) {
      free(buffer);
      return_val.records = return_vec;
      return_val.info = IOManagerInfo::END_OF_FILE;
      return return_val;
    }

    for (int i = 0; i < Config::vals().page_size;
         i += Config::vals().record_char_size) {
      std::string_view run_data =
          std::string_view(&buffer[i], Config::vals().record_char_size);
      if (run_data.front() == 0 && run_data.back() == 0) {
        continue;
      }
      return_vec.emplace_back(Record(run_data));
    }
    free(buffer);

    return_val.records = std::move(return_vec);
    if (f_stream.gcount() != Config::vals().page_size) {
      std::cerr << "could not read the whole page";
      return_val.info = IOManagerInfo::NOT_WHOLE_PAGE_ERROR;
      return return_val;
    }

    if (f_stream.eof()) {
      return_val.info = IOManagerInfo::END_OF_FILE;
      return return_val;
    }
    return_val.info = IOManagerInfo::SUCCES;
    return return_val;
  }

  void write_memory_page(char *buffer, int buffer_size,
                         std::fstream &f_stream) {

    disc_operation_counter_++;
    writes_counter_++;
    if (!f_stream.is_open()) {
      std::cerr << "File is not opened \n";
    }

    f_stream.clear();

    if (buffer_size % Config::vals().page_size != 0) {
      // std::cout << "Data does not match the page, adding padding\n";
      int whole_pages_count = buffer_size / Config::vals().page_size;
      int whole_page_buffer_size = whole_pages_count * Config::vals().page_size;
      if (whole_pages_count != 0) {
        f_stream.write(buffer, whole_page_buffer_size);
      }

      char *padded_buffer = new char[Config::vals().page_size];
      std::memset(padded_buffer, 0, Config::vals().page_size);

      if (!std::strncpy(padded_buffer, &buffer[whole_page_buffer_size],
                        buffer_size - whole_page_buffer_size)) {
        std::cerr << "Error coppying into padded buffer\n";
      }

      // std::cout << padded_buffer;
      f_stream.write(padded_buffer, Config::vals().page_size);

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
