#include "Record.hpp"
Record::Record() {
  record_ = std::string();
  ones_num_ = 0;
}

Record::Record(std::string record) {
  record_ = record;
  ones_num_ = count_zeros_in_binary(record);
}
Record::Record(std::string_view record) {
  record_ = record;
  ones_num_ = count_zeros_in_binary(record);
}

void Record::print() {
  std::cout << "record: " << record_ << ", ones_num_: " << ones_num_
            << std::endl;
}

std::string_view Record::get_record_view() { return record_; }
const char *Record::get_record_c_str() { return record_.c_str(); }
int Record::get_record_size() { return record_.size(); }

void Record::into_file_string(char &buffer, int &buffsize) {
  if (record_.size() > buffsize) {
    std::cout << "record size if bigger than buffsize, skipping string\n";
    return;
  }
  record_.copy(&buffer, record_.size());
  buffsize = record_.size();
}

bool Record::operator==(const Record &other) const {
  return (ones_num_ == other.ones_num_);
}

bool Record::operator!=(const Record &other) const {
  return (ones_num_ != other.ones_num_);
}

bool Record::operator>(const Record &other) const {
  return (ones_num_ > other.ones_num_);
}

bool Record::operator<(const Record &other) const {
  return (ones_num_ < other.ones_num_);
}

bool Record::operator>=(const Record &other) const {
  return (ones_num_ >= other.ones_num_);
}

bool Record::operator<=(const Record &other) const {
  return (ones_num_ <= other.ones_num_);
}
