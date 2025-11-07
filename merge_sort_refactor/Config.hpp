#pragma once

struct Config {
private:
  Config() {
    debug = false;
    record_char_size = 10;
    max_buffer_count = 3;
    records_per_page = 10;
    page_size = record_char_size * records_per_page;
  }

public:
  Config(const Config &) = delete;
  Config &operator=(const Config &) = delete;
  Config(Config &&) = delete;
  Config &operator=(Config &&) = delete;

  int debug;
  int record_char_size;
  int max_buffer_count;
  int records_per_page;
  int page_size;

  static Config &vals() {
    static Config instance;
    return instance;
  }
};
