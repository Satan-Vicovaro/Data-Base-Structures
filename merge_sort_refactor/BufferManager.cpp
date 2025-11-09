#include "BufferManager.hpp"

BufferManager::BufferManager() {
  max_buff_count_ = 0;
  input_buff_count_ = 0;
  buffers_ = std::vector<Buffer>();
  out_buffer_ = Buffer();
  records_vec_ = std::vector<std::vector<Record>>();
}

BufferManager::BufferManager(int buff_num) {
  max_buff_count_ = buff_num;
  input_buff_count_ = buff_num - 1;
  buffers_ = std::vector<Buffer>();
  out_buffer_ = Buffer();
  records_vec_ = std::vector<std::vector<Record>>();
}

std::vector<Buffer> &BufferManager::get_buffers_ref() { return buffers_; }

void BufferManager::append_to_out_buffer(Record &record, Belt &belt) {
  out_buffer_.append_out_buffer(std::move(record), belt);
}
void BufferManager::write_remanings_of_out_buffer(Belt &belt) {
  out_buffer_.write_buffer(belt);
}

void BufferManager::load_and_write_n_records(Belt &main_belt,
                                             Belt &secondary_belt) {
  main_belt.reset_cyclic_read();

  bool eof = false;
  while (!eof) {

    std::vector<std::vector<Record>> records_vec;
    // load n chunks
    for (int i = 0; i < max_buff_count_; i++) {
      auto [records, end_of_file] = main_belt.cyclic_read_from_beginning();

      if (!records.empty())
        records_vec.emplace_back(std::move(records));

      if (end_of_file) {
        eof = end_of_file;
        break;
      }
    }

    // flatify
    std::vector<Record *> flatyfied_vec;
    for (const std::vector<Record> &records : records_vec) {
      for (const Record &record : records) {
        flatyfied_vec.emplace_back(const_cast<Record *>(&record));
      }
    }

    std::sort(flatyfied_vec.begin(), flatyfied_vec.end(),
              Record::compare_function);

    // save
    secondary_belt.append_flat_list_to_file(flatyfied_vec);
  }
}

bool BufferManager::initialize_buffers(Belt &belt) {
  buffers_.clear();
  std::vector<Run> runs = belt.get_file_runs(input_buff_count_);

  for (Run &run : runs) {
    Buffer buffer = Buffer(run);
    buffer.init(belt);
    buffers_.emplace_back(std::move(buffer));
  }
  if (buffers_.size() == 0) {
    return true;
  }
  return false;
}

int BufferManager::get_buffers_count() { return buffers_.size(); }
void BufferManager::clear_buffers() {
  buffers_.clear();
  out_buffer_.clear();
}
