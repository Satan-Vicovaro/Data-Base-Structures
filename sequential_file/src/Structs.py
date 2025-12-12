import bisect
from itertools import count
import secrets
import random
import struct
from typing import dataclass_transform

from config import (
    DATA_SIZE,
    OVERFLOW_PTR_SIZE,
    PAGE_KEY_SIZE,
    RAND_KEY_SIZE,
    RECORD_SIZE,
    RECORDS_PER_CHUNK,
)


def random_key_gen() -> int:
    return secrets.randbits(RAND_KEY_SIZE * 8)


class Data:
    record_size = DATA_SIZE

    def __init__(self, value: str = "CCCCCCCCCC") -> None:
        self.value = value[: self.record_size].ljust(self.record_size, "C")

    @classmethod
    def random_record(cls):
        raw_record = [
            random.randrange(ord(" "), ord("~"), 1) for _ in range(0, cls.record_size)
        ]

        value = "".join(chr(x) for x in raw_record)
        return Data(value)

    def __bytes__(self) -> bytes:
        return bytes(str(self.value), "ASCII")

    def __str__(self) -> str:
        return f"{self.value:12}"


class Record:
    def __init__(self, key=None, data=None, overflow_ptr=None) -> None:
        if key == None:
            self.key: int = random_key_gen()
        else:
            self.key: int = key

        if data == None:
            self.data = Data.random_record()
        elif isinstance(data, Data):
            self.data = data
        else:
            self.data = Data(data)

        if overflow_ptr == None:
            self.overflow_ptr: int = 0
        else:
            self.overflow_ptr = overflow_ptr

    def __bytes__(self) -> bytes:
        fmt = f"{RAND_KEY_SIZE}s{DATA_SIZE}s{OVERFLOW_PTR_SIZE}s"
        return struct.pack(
            fmt,
            self.key.to_bytes(RAND_KEY_SIZE, byteorder="little"),
            bytes(self.data),
            self.overflow_ptr.to_bytes(OVERFLOW_PTR_SIZE, byteorder="little"),
        )

    @classmethod
    def from_bytes(cls, record_list: bytes):
        fmt = f"{RAND_KEY_SIZE}s{DATA_SIZE}s{OVERFLOW_PTR_SIZE}s"
        key, data_bytes, overflow_ptr = struct.unpack(fmt, record_list)
        return Record(
            int.from_bytes(key, "little"),
            Data(data_bytes.decode("ASCII")),
            int.from_bytes(overflow_ptr, "little"),
        )

    @classmethod
    def random_record(cls):
        key = random_key_gen()
        data = Data.random_record()
        overflow_ptr = 0
        return Record(key, data, overflow_ptr)

    @classmethod
    def empty_record(cls):
        key = 0
        data = Data(str("CCCCCCCCCC"))
        overflow_ptr = 0
        return Record(key, data, overflow_ptr)

    def is_empty(self) -> bool:
        if (
            self.key == 0
            and self.data.value == str("CCCCCCCCCC")
            and self.overflow_ptr == 0
        ):
            return True
        return False

    def is_deleted(self) -> bool:
        if self.key != 0 and self.data.value == str("C" * RECORD_SIZE):
            return True
        return False

    def __str__(self) -> str:
        return f"key: {self.key:12d} data: {self.data} overflow_ptr: {self.overflow_ptr:10d}"

    def __repr__(self) -> str:
        return f"Record(key: {self.key:} data: {self.data} overflow_ptr: {self.overflow_ptr:})"


class SparseIndex:
    def __init__(self, key: int, page_index: int) -> None:
        self.key: int = key
        self.page_index: int = page_index

    @classmethod
    def from_bytes(cls, sparse_index_bytes: bytes):
        fmt = f"{RAND_KEY_SIZE}s{PAGE_KEY_SIZE}s"
        key, page_index = struct.unpack(fmt, sparse_index_bytes)
        return SparseIndex(
            int.from_bytes(key, "little"), int.from_bytes(page_index, "little")
        )

    def __bytes__(self) -> bytes:
        fmt = f"{RAND_KEY_SIZE}s{PAGE_KEY_SIZE}s"
        return struct.pack(
            fmt,
            int.to_bytes(self.key, byteorder="little"),
            int.to_bytes(self.page_index, byteorder="little"),
        )

    def __str__(self) -> str:
        return f"SparseIndex(key: {self.key:10}, page_index: {self.page_index:10})"

    def __repr__(self) -> str:
        return f"SparseIndex(key: {self.key}, page_index: {self.page_index})"


class Page:

    def __init__(self, records: list[Record], page_index: int, location: str) -> None:
        self.records = records
        self.page_index = page_index
        self.location = location

    def update_record(self, record: Record):
        updated = False
        for record_to_update in self.records:
            if record_to_update.key == record.key:
                record_to_update = record
                updated = True
                break

        if not updated:
            print("Value does not exist!!!")

    def exist(self, record):
        return any(rec.key == record.key for rec in self.records)

    def size(self):
        elem_num = 0
        for record in self.records:
            if not record.is_empty():
                elem_num += 1
                continue
            break
        return elem_num

    def insert(self, record: Record):
        index = bisect.bisect_right(
            self.records, record.key, hi=self.size(), key=lambda l: l.key
        )
        self.records.insert(index, record)

        if len(self.records) == RECORDS_PER_CHUNK + 1:
            self.records.pop()

    def can_insert(self) -> bool:
        for record in self.records:
            if record.is_empty():
                return True

        if len(self.records) < RECORDS_PER_CHUNK:
            return True

        return False
