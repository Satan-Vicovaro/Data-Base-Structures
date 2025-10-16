#include "Record.hpp"
#include "object1.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
  std::cout << "Hello world" << std::endl;
  Object1 *meow = new Object1();
  std::cout << *meow << std::endl;

  Record record = Record("CCCCCC");
  record.print();
}
