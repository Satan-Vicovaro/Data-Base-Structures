#include "Belt.hpp"
#include "Buffer.hpp"
#include "Record.hpp"
#include <algorithm>
#include <vector>
class BufferManager {
  int max_buff_count;
  // first phase
  std::vector<std::vector<Record>> records_vec_;

  // second phase
  std::vector<Buffer> buffers_;
  Buffer out_buffer;

public:
  BufferManager() {
    max_buff_count = 0;
    buffers_ = std::vector<Buffer>();
    out_buffer = Buffer();
    records_vec_ = std::vector<std::vector<Record>>();
  }

  BufferManager(int buff_num) {
    max_buff_count = buff_num;
    buffers_ = std::vector<Buffer>(buff_num - 1);
    out_buffer = Buffer();
    records_vec_ = std::vector<std::vector<Record>>(buff_num);
  }

  void load_and_write_n_records(Belt &main_belt, Belt &secondary_belt) {

    main_belt.reset_cyclic_read();

    bool eof = false;
    while (!eof) {

      std::vector<std::vector<Record>> records_vec;
      // load n chunks
      for (int i = 0; i < max_buff_count; i++) {
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
};
