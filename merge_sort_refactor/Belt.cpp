#include "Belt.hpp"
#include "Config.hpp"
#include "Run.hpp"
#include <algorithm>
#include <iosfwd>
#include <iostream>
#include <utility>
#include <vector>

Belt::Belt() {
  last_stream_pos_ = 0;
  last_run_start_pos_ = 0;
  belt_name_ = "output/default_name";
  file_name_ = std::string(belt_name_ + ".txt");
  io_manager_ = IOManager();
  runs_in_file_ = std::vector<Run>();
}

Belt::Belt(std::string_view string_name) {
  last_stream_pos_ = 0;
  last_run_start_pos_ = 0;
  belt_name_ = std::string(string_name);
  file_name_ = "output/" + std::string(string_name) + ".txt";
  io_manager_ = IOManager();
  runs_in_file_ = std::vector<Run>();
}

void Belt::init(bool user_choice) {
  if (!user_choice) {
    file_stream_.open(file_name_, std::ios::out | std::ios::trunc);
    file_stream_.close();
    return;
  }

  file_stream_.open(file_name_);
  if (file_stream_.peek() != std::ifstream::traits_type::eof()) {
    std::cout << "Current file is not empty do you want to truncate (t) or "
                 "append (a) to file? \n";

    file_stream_.close();

    bool proper_answer = false;
    char input = 0;
    while (!proper_answer) {
      std::cin >> input;
      input = (char)tolower(input);
      if (input == 'a') {
        file_stream_.open(file_name_, std::ios::app);
        proper_answer = true;
      } else if (input == 't') {
        file_stream_.open(file_name_, std::ios::trunc | std::ios::out);
        proper_answer = true;
      } else {
        std::cout << "Wrong input\n";
      }
    }
  }
  file_stream_.close();
}

void Belt::find_optional_chunk_merges() {
  std::vector<Run> new_runs;

  int i = 0;
  while (true) {
    if (i + 1 > runs_in_file_.size()) {
      break;
    }
    Run good_run = std::move(runs_in_file_[i]);
    good_run.init(*this);
    new_runs.emplace_back(std::move(good_run));
    while (true) {
      i++;
      if (i + 1 > runs_in_file_.size()) {
        break;
      }

      Run potentially_mergable = std::move(runs_in_file_[i]);
      potentially_mergable.init(*this);

      std::streampos last_part_prev_run_pos =
          potentially_mergable.current_record_pos -
          (std::streampos)Config::vals().page_size;
      Run last_part_prev_run = Run(last_part_prev_run_pos);

      last_part_prev_run.init(*this);
      if (last_part_prev_run.current_record_last >
          potentially_mergable.current_record_first) {
        break;
      }
      std::cout << "Neighbour's runs got merged\n";
    }
  }

  runs_in_file_ = std::move(new_runs);
  // we have to remove curr records bsc it will break other functions
  for (Run &run : runs_in_file_) {
    run.current_record_last = Record();
  }
}

std::vector<Run> Belt::get_file_runs(int run_num) {
  find_optional_chunk_merges();
  int return_run_size = std::min(run_num, (int)runs_in_file_.size());

  // yes, im super lazy here
  std::reverse(runs_in_file_.begin(), runs_in_file_.end());
  std::vector<Run> return_vec;
  for (int i = 0; i < return_run_size; i++) {
    return_vec.emplace_back(runs_in_file_.back());
    runs_in_file_.pop_back();
  }
  std::reverse(runs_in_file_.begin(), runs_in_file_.end());
  return return_vec;
}

void Belt::save_run() {
  file_stream_.open(file_name_, std::ios::app);

  if (!file_stream_.is_open()) {
    std::cout << "Could not open file: " << file_name_ << std::endl;
    file_stream_.clear();
  }
  last_run_start_pos_ = file_stream_.tellp();

  runs_in_file_.emplace_back(Run(last_run_start_pos_));

  file_stream_.close();
}

void Belt::append_to_file(std::vector<Record> &records) {

  file_stream_.open(file_name_, std::ios::app);
  if (!file_stream_.is_open()) {
    std::cout << "Could not open file: " << file_name_ << std::endl;
    file_stream_.clear();
  }

  std::unique_ptr<char[]> buffer;
  int buffer_size;
  std::tie(buffer, buffer_size) = Record::into_char_buffer(records);

  io_manager_.write_memory_page(buffer.get(), buffer_size, file_stream_);
  file_stream_.close();
}

void Belt::append_flat_list_to_file(std::vector<Record *> &records) {

  file_stream_.open(file_name_, std::ios::app);
  if (!file_stream_.is_open()) {
    std::cout << "Could not open file: " << file_name_ << std::endl;
    file_stream_.clear();
  }

  runs_in_file_.emplace_back(Run(file_stream_.tellp()));

  int record_num = records.size();
  int per_page = Config::vals().records_per_page;

  for (int i = 0; i < record_num; i += per_page) {
    int end = std::min(i + per_page, record_num);
    std::vector<Record *> record_slice(records.begin() + i,
                                       records.begin() + end);

    std::unique_ptr<char[]> buffer;
    int buffer_size;
    std::tie(buffer, buffer_size) = Record::into_char_buffer(record_slice);

    io_manager_.write_memory_page(buffer.get(), buffer_size, file_stream_);
  }

  file_stream_.close();
}

void Belt::replace_from_beginning(std::vector<Record> &records) {
  file_stream_.open(file_name_, std::ios::out);

  if (!file_stream_.is_open()) {
    std::cout << "Could not open file: " << file_name_ << std::endl;
    file_stream_.clear();
  }

  std::unique_ptr<char[]> buffer;
  int buffer_size;
  std::tie(buffer, buffer_size) = Record::into_char_buffer(records);

  file_stream_.seekp(last_stream_pos_);
  io_manager_.write_memory_page(buffer.get(), buffer_size, file_stream_);
  if (file_stream_.eof()) {
    last_stream_pos_ = 0;
  } else {
    last_stream_pos_ = file_stream_.tellp();
  }
  file_stream_.close();
}

std::tuple<std::vector<Record>, bool> Belt::run_read(Run &run) {
  file_stream_.open(file_name_, std::ios::in);

  if (!file_stream_.is_open()) {
    std::cout << "Could not open file: " << file_name_ << std::endl;
    file_stream_.clear();
  }

  file_stream_.seekg(run.current_record_pos);
  IOManagerResult result = io_manager_.get_memory_page(file_stream_);
  run.current_record_pos = file_stream_.tellg();
  file_stream_.close();

  std::vector<Record> records = std::move(result.records);
  bool eof = false;
  if (result.info == END_OF_FILE || result.info == NOT_WHOLE_PAGE_ERROR) {
    last_stream_pos_ = 0;
    eof = true;
  }
  return {records, eof};
}

std::tuple<std::vector<Record>, bool> Belt::cyclic_read_from_beginning() {

  file_stream_.open(file_name_, std::ios::in);

  if (!file_stream_.is_open()) {
    std::cout << "Could not open file: " << file_name_ << std::endl;
    file_stream_.clear();
  }

  file_stream_.seekg(last_stream_pos_);
  IOManagerResult result = io_manager_.get_memory_page(file_stream_);
  last_stream_pos_ = file_stream_.tellg();
  file_stream_.close();

  std::vector<Record> records = std::move(result.records);
  bool eof = false;
  if (result.info == END_OF_FILE || result.info == NOT_WHOLE_PAGE_ERROR) {
    last_stream_pos_ = 0;
    eof = true;
  }
  return {records, eof};
}

void Belt::load_data_from_file() {
  std::string load_file_name = std::string();
  std::cout << "File name:\n";
  std::cin >> load_file_name;

  if (!std::filesystem::exists(load_file_name)) {
    std::cout << "File does not exist " << load_file_name;
    return;
  }

  char record_seperator = 0;
  std::cout << "Values seperated by:\n";
  std::cin >> record_seperator;

  file_stream_.open(load_file_name);

  char *buffer = new char[Config::vals().page_size + 1];
  buffer[Config::vals().page_size] = '\0';
  int buffer_size = Config::vals().page_size;

  std::vector<Record> records = {};
  while (file_stream_.read(buffer, buffer_size)) { // full chunk
    dispatch_into_records(records, buffer, buffer_size, record_seperator);
  }
  // rest of chunk
  std::streamsize bytes_read = file_stream_.gcount();
  if (bytes_read > 0) {
    dispatch_into_records(records, buffer, bytes_read, record_seperator);
  }
  file_stream_.close();

  append_to_file(records);
}

void Belt::dispatch_into_records(std::vector<Record> &records, char buffer[],
                                 int buff_size, char record_seperator) {
  int base_index = 0;
  int cur_index = 0;
  while (cur_index <= buff_size) {
    if (buffer[cur_index] == record_seperator) {
      records.emplace_back(
          Record(std::string(&buffer[base_index], cur_index - base_index)));

      base_index = cur_index + 1;
    }
    cur_index++;
  }
}

void Belt::print_whole_file_readable() {
  reset_cyclic_read();
  bool end_of_file = false;
  std::vector<Record> records;
  int i = 0;
  while (!end_of_file) {
    std::tie(records, end_of_file) = cyclic_read_from_beginning();
    for (Record &record : records) {
      std::cout << i << ": ";
      record.print();
      i++;
    }
  }
}

void Belt::print_one_record(int index) {
  if (index < 0) {
    return;
  }
  reset_cyclic_read();
  bool end_of_file = false;

  std::vector<Record> records;
  int cur_index_record = 0;
  while (!end_of_file) {
    std::tie(records, end_of_file) = cyclic_read_from_beginning();
    // std::cout << "index: " << index << " Records: " << records.size()
    // << std::endl;

    // std::cout << "diff " << index - (int)records.size() << std::endl;
    if ((index - (int)records.size()) >= 0) {
      index -= records.size();
      continue;
    }
    records[index].print();
    break;
  }
}

void Belt::generate_radom_data(std::mt19937 &generator, int record_num) {
  std::vector<Record> records =
      Record::generate_random_records(generator, record_num);
  append_to_file(records);
}

void Belt::add_records_from_user(int record_num) {
  std::vector<Record> records = std::vector(record_num, Record());
  std::string input_string = std::string(Config::vals().record_char_size, ' ');
  for (int i = 0; i < record_num; i++) {
    std::cin >> input_string;

    if (input_string.size() < Config::vals().record_char_size) {
      std::cout << "adding characters 'C' to end of string\n";
    } else if (input_string.size() > Config::vals().record_char_size) {
      std::cout << "truncating to " << Config::vals().record_char_size
                << " characters\n";
    }

    input_string.resize(Config::vals().record_char_size, 'C');
    records[i] = std::move(Record(input_string));
  }
  append_to_file(records);
}

void Belt::truncate_file() {
  file_stream_.open(file_name_, std::ios::trunc | std::ios::out);
  file_stream_.close();
}
void Belt::reset_cyclic_read() { last_stream_pos_ = 0; }
int Belt::read_operation_count() { return io_manager_.reads_counter_; }
int Belt::write_operation_count() { return io_manager_.writes_counter_; }

void Belt::reset() {
  runs_in_file_.clear();
  last_stream_pos_ = 0;
  last_run_start_pos_ = 0;
}
