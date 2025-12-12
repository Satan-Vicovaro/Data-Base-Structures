from functools import cached_property
from os import set_blocking
import pathlib
import math
from typing import List
from config import CHUNK_SIZE, RECORD_SIZE
from src.Structs import Page, Record


class IOManager:
    total_read_count = 0
    total_write_count = 0

    def __init__(
        self,
        target_class,
        filename: str = "file.txt",
        chunk_size: int = 0,
        record_size: int = 0,
    ) -> None:
        self.filename = filename
        self.chunk_size = chunk_size
        self.record_size = record_size
        self.target_class = target_class

        # if not pathlib.Path(filename).exists():
        with open(filename, "w") as f:
            f.write("")

    def delete(self):
        file = pathlib.Path(self.filename)
        file.unlink()

    def rename(self, new_filename):
        file = pathlib.Path(self.filename)
        file.rename(new_filename)
        self.filename = new_filename

    def truncate(self):
        with open(self.filename, "w") as f:
            f.write("")

    def read_page(self, page_index=0):
        with open(self.filename, "r+b", buffering=self.chunk_size) as f:
            f.seek(page_index * self.chunk_size)
            raw_data = f.read(self.chunk_size)
            raw_data = [
                raw_data[i : i + self.record_size]
                for i in range(0, len(raw_data), self.record_size)
            ]

        # return raw_data
        data_list = [self.target_class.from_bytes(b) for b in raw_data]
        return Page(data_list, page_index, self.filename)

    def read_last_page(self):
        file_stats = pathlib.Path(self.filename).stat()
        page_index = file_stats.st_size // self.chunk_size

        with open(self.filename, "r+b", buffering=self.chunk_size) as f:
            f.seek(page_index * self.chunk_size)
            raw_data = f.read(self.chunk_size)
            raw_data = [
                raw_data[i : i + self.record_size]
                for i in range(0, len(raw_data), self.record_size)
            ]

        # return raw_data
        data_list = [self.target_class.from_bytes(b) for b in raw_data]
        return Page(data_list, page_index, self.filename)

    def get_last_page_index(self):
        file_stats = pathlib.Path(self.filename).stat()
        return math.ceil(file_stats.st_size / self.chunk_size)

    def chunk_read(self, start: int = 0):
        with open(self.filename, "r+b", buffering=self.chunk_size) as f:
            f.seek(start)
            raw_data = f.read(self.chunk_size)
            file_pos = f.tell()

            eof = len(raw_data) < self.chunk_size

            raw_data = [
                raw_data[i : i + self.record_size]
                for i in range(0, len(raw_data), self.record_size)
            ]

        data = [self.target_class.from_bytes(b) for b in raw_data]
        return (data, file_pos, eof)

    def read_whole_file(self):
        current_file_pos = 0
        eof = False
        while not eof:
            records, file_pos, eof = self.chunk_read(current_file_pos)
            current_file_pos = file_pos
            for record in records:
                yield record

    def write_page(self, page: Page):
        byte_data = bytearray(data for record in page.records for data in bytes(record))
        with open(self.filename, "r+b", buffering=self.chunk_size) as f:
            f.seek(page.page_index * self.chunk_size)
            f.write(byte_data)

    def write_last_page(self, page: Page):
        file_stats = pathlib.Path(self.filename).stat()
        page_index = file_stats.st_size // self.chunk_size
        byte_data = bytearray(data for record in page.records for data in bytes(record))
        with open(self.filename, "r+b") as f:
            f.seek(page_index * self.chunk_size)
            f.write(byte_data)

    def append_to_file(self, page: Page) -> int:

        byte_data = bytearray(data for record in page.records for data in bytes(record))

        file_stats = pathlib.Path(self.filename).stat()
        page_index = file_stats.st_size // self.chunk_size

        with open(self.filename, "a+b", buffering=self.chunk_size) as f:
            f.write(byte_data)

        return page_index

    def get_record_num(self) -> int:
        path = pathlib.Path(self.filename)
        file_size = path.stat().st_size
        if file_size % self.record_size != 0:
            print("There is error on page layout")
            return 1
        return file_size // self.record_size
