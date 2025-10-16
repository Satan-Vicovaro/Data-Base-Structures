#include "Belt.hpp"
#include "Record.hpp"
#include "object1.hpp"
#include <ctime>
#include <iostream>
#include <random>

int main(int argc, char *argv[]) {
  std::mt19937 mt(std::time(nullptr));

  std::cout << "Hello world" << std::endl;
  Object1 *meow = new Object1();
  std::cout << *meow << std::endl;

  Record record = Record("CCCCCC");
  record.print();

  std::vector<Record> records = Record::generate_random_records(mt, 40);
  for (auto record : records) {
    record.print();
  }

  Belt belt_1 = Belt();
  belt_1.init();
  belt_1.write_to_file(records);
}
