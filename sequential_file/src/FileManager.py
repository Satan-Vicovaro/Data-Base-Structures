import bisect
from enum import Enum
import math
from pathlib import PosixPath
from re import finditer
import re
from typing import IO, Self
from config import ALPHA, CHUNK_SIZE, RECORD_SIZE, RECORDS_PER_CHUNK
from src.Structs import Page, Record
from src.IOManager import IOManager


class PageFindStatus(Enum):
    VALUE_EXIST = 0
    IN_OVERFLOW = 1
    FREE_SPACE_TO_APPEND = 2
    EMPTY_FILE = 3
    FILE_IS_FULL = 4
    IS_DELETED = 5


class FileManager:
    def __init__(self, file_name) -> None:
        self.file_name = file_name
        self.io_manager = IOManager(
            Record, filename=file_name, chunk_size=CHUNK_SIZE, record_size=RECORD_SIZE
        )
        self.cache_page: Page = Page([], 0, file_name)
        self.cache_page_known_values: dict[int, int] = {}  # [ Record.key, index ]
        self.all_pages_full = True
        assert 0 < ALPHA <= 1, "alpha should be in range (0,1]"
        self.alpha = ALPHA

    def show_file(self):
        for i, record in enumerate(self.io_manager.read_whole_file()):
            print(f"{i+1} : {record}")

    def initialize(self, record: Record):
        self.all_pages_full = True
        self.io_manager.append_to_file(Page([record], 0, self.file_name))
        self.cache_page = self.io_manager.read_page(0)

    def find_on_page(self, record: Record, page_index: int):

        if self.cache_page.page_index != page_index:
            # cache miss
            self.cache_page = self.io_manager.read_page(page_index)

        # find place on page
        page = self.cache_page
        if page.size() == 0:
            print("Page should not be empty")
            return PageFindStatus.EMPTY_FILE, Record(0, 0, 0)

        index = (
            bisect.bisect_right(
                page.records, record.key, hi=page.size(), key=lambda l: l.key
            )
            - 1
        )

        if index == -1:
            print("Why index is -1, Aborting")
            return PageFindStatus.EMPTY_FILE, Record(0, 0, 0)

        closest_record = page.records[index]

        if page.exist_and_not_valid(record):
            return PageFindStatus.IS_DELETED, closest_record

        if page.exist(record):
            return PageFindStatus.VALUE_EXIST, closest_record

        if page.can_insert():
            return PageFindStatus.FREE_SPACE_TO_APPEND, closest_record

        return PageFindStatus.IN_OVERFLOW, closest_record

    def append_to_current(self, record: Record, page_index: int):
        if page_index != self.cache_page.page_index:
            print("Why are you NOT inserting to the same page? Cache miss")
            self.cache_page = self.io_manager.read_page(page_index)

        self.cache_page.insert(record)
        self.io_manager.write_page(self.cache_page)

    def append_to_end(self, record: Record):
        record_number = self.io_manager.get_record_num()

        page = self.io_manager.read_last_page()
        page.records.append(record)

        self.cache_page = page
        self.io_manager.write_last_page(page)
        return record_number + 1

    def update_record(self, record: Record):
        index = (
            bisect.bisect_right(
                self.cache_page.records, record.key, key=lambda l: l.key
            )
            - 1
        )
        if record.key != self.cache_page.records[index].key:
            print("Upadte record could not locate record")
            return
        self.cache_page.records[index] = record
        self.io_manager.write_page(self.cache_page)

    def update_record_overflow(self, record: Record):
        if not self.cache_page.exist(record):
            print("could not locate record")
            return

        self.cache_page.update_record(record)
        self.io_manager.write_page(self.cache_page)

    def page_iter(self):
        yield self.io_manager.read_whole_file()

    def get_page_and_record_from_ptr(self, overflow_ptr: int):
        if overflow_ptr == 0:
            print("Wrong overflow ptr")
            return self.cache_page, Record.empty_record()

        overflow_ptr -= 1
        page_index = overflow_ptr // RECORDS_PER_CHUNK

        if page_index != self.cache_page.page_index:
            self.cache_page = self.io_manager.read_page(page_index)

        if self.cache_page.size() == 0:
            print("Error: cache is empty")
            return self.cache_page, Record.empty_record()

        record_index = overflow_ptr % RECORDS_PER_CHUNK
        record = self.cache_page.records[record_index]

        return self.cache_page, record

    def write_updated_page(self, page: Page):
        self.cache_page = page
        self.io_manager.write_page(page)

    def get_next_overflow_ptr(self):
        next_overflow_ptr = self.io_manager.get_record_num()
        return next_overflow_ptr + 1

    def get_page(self, page_index) -> Page:
        if self.cache_page.page_index != page_index:
            self.cache_page = self.io_manager.read_page(page_index)
        return self.cache_page

    def init_new_file(self):
        self.new_file = IOManager(
            Record,
            "new_" + self.file_name,
            chunk_size=CHUNK_SIZE,
            record_size=RECORD_SIZE,
        )
        self.output_buffer: list[Record] = []

    def aggreate_append_to_new_file(self, record: Record):
        record.overflow_ptr = 0
        self.output_buffer.append(record)
        max_record_num = math.ceil(RECORDS_PER_CHUNK * self.alpha)
        if len(self.output_buffer) >= max_record_num:

            for _ in range(RECORDS_PER_CHUNK - max_record_num):
                self.output_buffer.append(Record.empty_record())

            page_index = self.new_file.append_to_file(Page(self.output_buffer, 0, ""))
            smallest_key_on_page = self.output_buffer[0].key
            self.output_buffer.clear()
            return smallest_key_on_page, page_index

        return None, None

    def append_leftovers(self):
        if len(self.output_buffer) > 0:
            page_index = self.new_file.append_to_file(Page(self.output_buffer, 0, ""))

            smallest_key_on_page = self.output_buffer[0].key
            return smallest_key_on_page, page_index
        return None, None

    def switch_to_new_file(self):

        proper_name = self.io_manager.filename
        self.io_manager.delete()
        self.io_manager = self.new_file
        self.io_manager.rename(proper_name)
        self.cache_page = self.io_manager.read_page(0)
        del self.new_file
        del self.output_buffer

    def reset_cache(self):
        self.cache_page = Page([], 0, self.file_name)

    def truncate_file(self):
        self.io_manager.truncate()
