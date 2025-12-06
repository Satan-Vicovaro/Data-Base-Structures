import config as c
import pathlib

from src.Structs import Record


class IOManager:
    total_read_count = 0
    total_write_count = 0

    def __init__(self, filename: str = "file.txt") -> None:
        self.filename = filename
        self.record_num = 0
        if not pathlib.Path(filename).exists():
            with open(filename, "w") as f:
                f.write("")

    def block_read(self, start: int = 0):
        with open(self.filename, "rb", buffering=c.CHUNK_SIZE) as f:
            f.seek(start)
            raw_data = f.read(c.CHUNK_SIZE)
            raw_data = [
                raw_data[i : i + c.RECORD_SIZE]
                for i in range(0, len(raw_data), c.RECORD_SIZE)
            ]

        record_list = [Record.from_bytes(b) for b in raw_data]
        for record in record_list:
            yield record

    def chunk_read(self, start: int = 0):
        with open(self.filename, "rb", buffering=c.CHUNK_SIZE) as f:
            f.seek(start)
            raw_data = f.read(c.CHUNK_SIZE)
            file_pos = f.tell()
            eof = len(raw_data) < c.CHUNK_SIZE
            raw_data = [
                raw_data[i : i + c.RECORD_SIZE]
                for i in range(0, len(raw_data), c.RECORD_SIZE)
            ]

        record_list = [Record.from_bytes(b) for b in raw_data]
        return (record_list, file_pos, eof)

    def read_whole_file(self):

        current_file_pos = 0
        eof = False
        while not eof:
            records, file_pos, eof = self.chunk_read(current_file_pos)
            current_file_pos = file_pos
            for record in records:
                yield record

    def block_write(self, block: list[Record]):
        self.record_num += len(block)
        with open(self.filename, "a+b", buffering=c.CHUNK_SIZE) as f:
            f.write(b"".join(map(bytes, block)))
