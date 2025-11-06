#pragma once
#include "Config.hpp"
#include "IOManager.hpp"
#include "Record.hpp"
#include <filesystem>
#include <iostream>
#include <memory>
#include <ostream>
#include <tuple>
#include <vector>

class Belt {
private:
  std::string belt_name_;
  std::string file_name_;
  std::fstream file_stream_;
  std::streampos last_stream_pos_;
  IOManager io_manager_;

public:
  Belt() {
    last_stream_pos_ = 0;
    belt_name_ = "output/default_name";
    file_name_ = std::string(belt_name_ + ".txt");
    io_manager_ = IOManager();
  }

  Belt(std::string_view string_name) {
    last_stream_pos_ = 0;
    belt_name_ = std::string(string_name);
    file_name_ = "output/" + std::string(string_name) + ".txt";
    io_manager_ = IOManager();
  }

  void init(bool user_choice) {
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

  void append_to_file(std::vector<Record> &records) {

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

  void replace_from_beginning(std::vector<Record> &records) {
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

  std::tuple<std::vector<Record>, bool> cyclic_read_from_beginning() {

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

  void load_data_from_file() {
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

  void dispatch_into_records(std::vector<Record> &records, char buffer[],
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

  void print_whole_file_readable() {
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

  void print_one_record(int index) {
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

  void generate_radom_data(std::mt19937 &generator, int record_num) {
    std::vector<Record> records =
        Record::generate_random_records(generator, record_num);
    append_to_file(records);
  }

  void truncate_file() {
    file_stream_.open(file_name_, std::ios::trunc | std::ios::out);
    file_stream_.close();
  }
  void reset_cyclic_read() { last_stream_pos_ = 0; }
};
