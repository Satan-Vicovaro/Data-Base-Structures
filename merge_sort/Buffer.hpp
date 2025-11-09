#pragma once
#include "Belt.hpp"
#include "Config.hpp"
#include "Record.hpp"
#include "Run.hpp"
#include <algorithm>
#include <iostream>
#include <optional>
#include <tuple>
#include <utility>
#include <vector>

class Buffer {
private:
  Run run_;
  std::vector<Record> records_;

public:
  Buffer();
  Buffer(Run starting_run);
  void init(Belt &belt);
  std::optional<Record> get_record(Belt &belt);
  bool load_next_records(Belt &belt);

  // writes to file if full
  void append_out_buffer(Record &&record, Belt &belt);

  // empties the buffer
  void write_buffer(Belt &belt);
  void clear();
};
