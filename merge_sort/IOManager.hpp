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
  IOManager();
  IOManagerResult get_memory_page(std::fstream &f_stream);
  void write_memory_page(char *buffer, int buffer_size, std::fstream &f_stream);
};
