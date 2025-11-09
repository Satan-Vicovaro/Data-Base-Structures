#include "Buffer.hpp"
#include "BufferManager.hpp"
#include "Record.hpp"
#include <algorithm>
#include <tuple>
#include <vector>
class MinHeap {
  std::vector<std::tuple<Record, int>> min_heap_;

public:
  MinHeap() { min_heap_ = std::vector<std::tuple<Record, int>>(); }

  void init(BufferManager &buffer_manager, Belt &belt) {
    std::vector<Buffer> &buffers = buffer_manager.get_buffers_ref();

    int i = 0;
    for (Buffer &buffer : buffers) {
      auto record = buffer.get_record(belt);
      if (!record) {
        continue;
      }
      min_heap_.emplace_back(std::move(*record), i);
      i++;
    }
    std::make_heap(min_heap_.begin(), min_heap_.end(), compare_function);
  }

  bool empty() { return min_heap_.empty(); }

  void clear() { min_heap_.clear(); }

  void append(std::tuple<Record, int> &&record) {
    min_heap_.emplace_back(std::move(record));
    std::push_heap(min_heap_.begin(), min_heap_.end(), compare_function);
  }

  std::tuple<Record, int> pop() {
    if (min_heap_.empty()) {
      std::cerr << "Poping form empty min_heap!\n";
      return std::make_tuple(Record(), 0);
    }

    std::pop_heap(min_heap_.begin(), min_heap_.end(), compare_function);
    auto return_val = std::move(min_heap_.back());
    min_heap_.pop_back();
    return return_val;
  }

  static bool compare_function(const std::tuple<Record, int> &a,
                               const std::tuple<Record, int> &b) {

    Record r_a = std::get<0>(a);
    Record r_b = std::get<0>(b);
    return a > b;
  }
};
