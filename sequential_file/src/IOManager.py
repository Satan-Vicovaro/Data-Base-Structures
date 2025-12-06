import config as c
import pathlib

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

        if not pathlib.Path(filename).exists():
            with open(filename, "w") as f:
                f.write("")

    def block_read(self, start: int = 0):
        with open(self.filename, "rb", buffering=self.chunk_size) as f:
            f.seek(start)
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

    def block_write(self, block):

        byte_data = bytearray(data for record in block for data in bytes(record))
        with open(self.filename, "a+b", buffering=c.CHUNK_SIZE) as f:
            f.write(block)
