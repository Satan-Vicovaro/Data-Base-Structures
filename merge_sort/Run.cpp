#include "Run.hpp"
#include "Belt.hpp"
#include <utility>

Run::Run() {
  current_record_pos = 0;
  current_record_last = Record();
  current_record_first = Record();
  end_of_run = false;
}

Run::Run(std::streampos initial_record_pos) {
  current_record_pos = initial_record_pos;
  current_record_last = Record();
  current_record_first = Record();
  end_of_run = false;
}

void Run::init(Belt &belt) {
  std::streampos stream_pos = current_record_pos;

  auto [records, end_of_file] = belt.run_read(*this);
  if (records.size() > 0) {
    current_record_first = std::move(records.front());
    current_record_last = std::move(records.back());
  }
  current_record_pos = stream_pos;
}

std::tuple<std::vector<Record>, bool> Run::get_next_records(Belt &belt) {
  bool end_of_file = false;
  std::vector<Record> records;

  std::tie(records, end_of_file) = belt.run_read(*this);

  Record next_last_record = Record();
  Record next_first_record = Record();
  if (records.size() > 0) {
    next_first_record = std::move(records.front());
    next_last_record = std::move(records.back());
  }

  if (current_record_last > next_first_record) {
    // std::cerr << "Memory page is not sorted properly\n";
    end_of_run = true;
  }
  current_record_last = std::move(next_last_record);
  current_record_first = std::move(next_first_record);

  if (end_of_file)
    end_of_run = true;

  return {records, end_of_run};
}
