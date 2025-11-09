#pragma once
#include "Belt.hpp"
#include "IOManager.hpp"
#include "Record.hpp"
#include <iosfwd>
#include <memory>
#include <tuple>

class Belt;

struct Run {
  std::streampos current_record_pos_;
  Record current_record_;
  bool end_of_run;

  Run();

  Run(std::streampos initial_record_pos_);

  std::tuple<std::vector<Record>, bool> get_next_records(Belt &belt);
};
