#pragma once
#include "Config.hpp"
#include "Record.hpp"
#include <cctype>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <iterator>
#include <random>
#include <shared_mutex>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

class Belt {
private:
  std::string belt_name_;
  std::string file_name_;

  int current_chunk_number_;
  std::fstream file_stream_;
  std::streampos last_stream_pos_;

public:
  Belt() {
    current_chunk_number_ = 0;
    last_stream_pos_ = 0;
    belt_name_ = "output/default_name";
    file_name_ = std::string(belt_name_ + ".txt");
  }

  Belt(std::string_view string_name) {
    current_chunk_number_ = 0;
    last_stream_pos_ = 0;
    belt_name_ = std::string(string_name);
    file_name_ = "output/" + std::string(string_name) + ".txt";
  }
  void init() {
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

  void write_to_file(const std::vector<Record> &records) {

    file_stream_.open(file_name_, std::ios::app);
    if (!file_stream_.is_open()) {
      std::cout << "Could not open file: " << file_name_ << std::endl;
      file_stream_.clear();
    }

    for (auto record : records) {
      std::string_view value = record.getRecord();
      file_stream_.write(value.data(), value.length());
      file_stream_.write("\n", 1);
    }
    file_stream_.close();
  }

  void print_whole_file() {
    file_stream_.open(file_name_);
    bool end_of_file = false;

    char file_values[config::print_file_stream_size];
    int chunk_num = 0;
    while (!end_of_file) {
      if (file_stream_.read(file_values, config::print_file_stream_size)) {
        std::cout << "Chunk: " << chunk_num << "\n" << file_values << "\n";
        chunk_num++;
        continue;
      }
      end_of_file = true;
    }
    file_stream_.close();
  }

  void generate_radom_data(std::mt19937 &generator, int record_num) {
    std::vector<Record> records =
        Record::generate_random_records(generator, record_num);

    write_to_file(records);
  }

  void add_records_from_user(int record_num) {
    std::vector<Record> records = std::vector(record_num, Record());
    std::string input_string = std::string(config::record_char_size, ' ');
    for (int i = 0; i < record_num; i++) {
      std::cin >> input_string;

      if (input_string.size() < config::record_char_size) {
        std::cout << "adding characters 'C' to end of string\n";
      } else if (input_string.size() > config::record_char_size) {
        std::cout << "truncating to " << config::record_char_size
                  << " characters\n";
      }

      input_string.resize(config::record_char_size, 'C');
      records[i] = Record(input_string);
    }
    write_to_file(records);
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

    char buffer[config::print_file_stream_size + 1];
    buffer[config::print_file_stream_size] = '\0';

    std::vector<Record> records = {};

    while (file_stream_.read(buffer, sizeof(buffer))) { // full chunk
      dispatch_into_records(records, buffer, sizeof(buffer), record_seperator);
    }
    // rest of chunk
    std::streamsize bytes_read = file_stream_.gcount();
    if (bytes_read > 0) {
      dispatch_into_records(records, buffer, bytes_read, record_seperator);
    }

    file_stream_.close();
    write_to_file(records);
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

  std::tuple<std::vector<Record>, bool> get_next_chunk() {
    int element_counter = 0;
    std::vector<Record> return_records = {};
    std::string line = "";
    bool end_of_file = false;

    file_stream_.open(file_name_, std::ios::out);

    file_stream_.seekg(last_stream_pos_);

    while (element_counter >= config::in_memory_chunk_element_count ||
           !end_of_file) {
      if (!std::getline(file_stream_, line)) {
        end_of_file = true;
      }
      return_records.emplace_back(Record(line));
      element_counter++;
    }
    if (end_of_file) {
      current_chunk_number_ = 0;
      last_stream_pos_ = 0;
    } else {
      current_chunk_number_++;
      last_stream_pos_ = file_stream_.tellg();
    }
    file_stream_.close();
    return std::make_tuple(return_records, end_of_file);
  }
};
