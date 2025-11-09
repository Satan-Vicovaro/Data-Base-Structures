#pragma once
#include "Config.hpp"
#include "IOManager.hpp"
#include "Record.hpp"
#include "Run.hpp"
#include <algorithm>
#include <filesystem>
#include <iosfwd>
#include <iostream>
#include <iterator>
#include <memory>
#include <ostream>
#include <tuple>
#include <vector>

class Run;

class Belt {
private:
  std::string belt_name_;
  std::string file_name_;
  std::fstream file_stream_;
  std::streampos last_stream_pos_;
  std::streampos last_run_start_pos_;
  IOManager io_manager_;
  std::vector<Run> runs_in_file_;

public:
  Belt();
  Belt(std::string_view string_name);

  void init(bool user_choice);
  std::vector<Run> get_file_runs(int run_num);
  void save_run();
  void append_flat_list_to_file(std::vector<Record *> &records);
  void append_to_file(std::vector<Record> &records);
  void replace_from_beginning(std::vector<Record> &records);
  std::tuple<std::vector<Record>, bool> run_read(Run &run);
  std::tuple<std::vector<Record>, bool> cyclic_read_from_beginning();
  void load_data_from_file();
  void dispatch_into_records(std::vector<Record> &records, char buffer[],
                             int buff_size, char record_seperator);
  void print_whole_file_readable();
  void print_one_record(int index);
  void generate_radom_data(std::mt19937 &generator, int record_num);
  void add_records_from_user(int record_num);
  void truncate_file();
  void reset_cyclic_read();
  int read_operation_count();
  int write_operation_count();
};
