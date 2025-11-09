#pragma once
#include "Belt.hpp"
#include "Buffer.hpp"
#include "Config.hpp"
#include "Record.hpp"
#include <algorithm>
#include <utility>
#include <vector>
class BufferManager {
  int max_buff_count_;
  // first phase
  std::vector<std::vector<Record>> records_vec_;

  // second phase
  std::vector<Buffer> buffers_;
  Buffer out_buffer_;

  int input_buff_count_;

public:
  BufferManager();
  BufferManager(int buff_num);
  std::vector<Buffer> &get_buffers_ref();

  void append_to_out_buffer(Record &record, Belt &belt);
  void write_remanings_of_out_buffer(Belt &belt);
  void load_and_write_n_records(Belt &main_belt, Belt &secondary_belt);
  bool initialize_buffers(Belt &belt);
  int get_buffers_count();
  void clear_buffers();
};
