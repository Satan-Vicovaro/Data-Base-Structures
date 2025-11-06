#pragma once
#include "Record.hpp"
#include "Run.hpp"
#include <vector>
class Buffer {
  Run run_;
  std::vector<Record> records_;
};
