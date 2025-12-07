from functools import cached_property
import pathlib
import math
from src.Structs import Record


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

    def read_page(self, page_index=0):
        with open(self.filename, "rb", buffering=self.chunk_size) as f:
            f.seek(page_index * self.chunk_size)
            raw_data = f.read(self.chunk_size)
            raw_data = [
                raw_data[i : i + self.record_size]
                for i in range(0, len(raw_data), self.record_size)
            ]

        # return raw_data
        data_list = [self.target_class.from_bytes(b) for b in raw_data]
        for data in data_list:
            yield data

    def read_last_page(self):
        file_stats = pathlib.Path(self.filename).stat()
        page_index = math.ceil(file_stats.st_size / self.chunk_size)

        with open(self.filename, "rb", buffering=self.chunk_size) as f:
            f.seek(page_index * self.chunk_size)
            raw_data = f.read(self.chunk_size)
            raw_data = [
                raw_data[i : i + self.record_size]
                for i in range(0, len(raw_data), self.record_size)
            ]

        # return raw_data
        data_list = [self.target_class.from_bytes(b) for b in raw_data]
        for data in data_list:
            yield data

    def chunk_read(self, start: int = 0):
        with open(self.filename, "rb", buffering=self.chunk_size) as f:
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

    def write_block(self, block):
        byte_data = bytearray(data for record in block for data in bytes(record))
        with open(self.filename, "a+b", buffering=self.chunk_size) as f:
            f.write(byte_data)

    def write_page(self, block, page_index):
        byte_data = bytearray(data for record in block for data in bytes(record))
        with open(self.filename, "a+b", buffering=self.chunk_size) as f:
            f.seek(page_index * self.chunk_size)
            f.write(byte_data)

    def append_to_file(self, block) -> int:
        byte_data = bytearray(data for record in block for data in bytes(record))
        with open(self.filename, "a+b", buffering=self.chunk_size) as f:
            f.write(byte_data)

        file_stats = pathlib.Path(self.filename).stat()
        page_index = math.ceil(file_stats.st_size / self.chunk_size)
        return page_index
