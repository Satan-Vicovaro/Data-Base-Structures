#include "Run.hpp"
#include "Belt.hpp"
#include <utility>

Run::Run() {
  current_record_pos_ = 0;
  current_record_ = Record();
  end_of_run = false;
}

Run::Run(std::streampos initial_record_pos_) {
  current_record_pos_ = initial_record_pos_;
  current_record_ = Record();
  end_of_run = false;
}

std::tuple<std::vector<Record>, bool> Run::get_next_records(Belt &belt) {
  bool end_of_file = false;
  std::vector<Record> records;

  std::tie(records, end_of_file) = belt.run_read(*this);

  Record last_record = Record();
  if (records.size() > 0) {
    last_record = std::move(records.back());
  }

  if (last_record < current_record_) {
    std::cerr << "Memory page is not sorted properly\n";
    end_of_run = true;
  }
  current_record_ = last_record;

  if (end_of_file)
    end_of_run = true;

  return {records, end_of_run};
}
