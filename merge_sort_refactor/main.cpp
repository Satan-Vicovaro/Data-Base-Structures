#include "Config.hpp"
#include "SuperDataBase.hpp"
#include <cstring>
#include <iostream>
#include <ostream>

void handle_argc(int argc, char *argv[]) {
  for (int i = 1; i < argc; i++) {
    std::string rcs = "--record_char_size";
    if (std::strncmp(argv[i], rcs.c_str(), rcs.length()) == 0) {
      std::cout << "record_char_size:\n" << argv[i + 1] << std::endl;
      int rcs_val = std::atoi(argv[i + 1]);
      Config::vals().record_char_size = rcs_val;
      Config::vals().page_size = rcs_val * Config::vals().records_per_page;
      continue;
    }

    std::string mbc = "--max_buffer_count";
    if (std::strncmp(argv[i], mbc.c_str(), mbc.length()) == 0) {
      std::cout << "max_buffer_count:\n" << argv[i + 1] << std::endl;
      int mbc_val = std::atoi(argv[i + 1]);
      Config::vals().max_buffer_count = mbc_val;
      continue;
    }

    std::string rpp = "--records_per_page";
    if (std::strncmp(argv[i], rpp.c_str(), rpp.length()) == 0) {
      std::cout << "records_per_page:\n" << argv[i + 1] << std::endl;
      int rpp_val = std::atoi(argv[i + 1]);
      Config::vals().records_per_page = rpp_val;

      Config::vals().page_size = rpp_val * Config::vals().record_char_size;
      continue;
    }
  }
}

int main(int argc, char *argv[]) {
  handle_argc(argc, argv);
  SuperDataBase sdb = SuperDataBase();
  sdb.start();
  return 0;
}
