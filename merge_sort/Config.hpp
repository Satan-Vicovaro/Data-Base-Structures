#pragma once

namespace config {
const int print_file_stream_size = 100;
const int record_char_size = 10;

const int in_memory_chunk_element_count = 5;
const int records_to_load = 3;
const int max_buffer_count = 2;
const int debug = false;

const int records_per_page = 10;

const int page_size =
    (record_char_size + 1) * records_per_page; // + 1 for \n or \0 character
} // namespace config
