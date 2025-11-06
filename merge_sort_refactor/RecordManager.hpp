#include "Record.hpp"
#include <vector>
class RecordManager {
  std::vector<Record> records_;

public:
  RecordManager() { records_ = {}; }
};
