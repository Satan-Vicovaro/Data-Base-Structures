import secrets
import random
import struct

from config import DATA_SIZE, OVERFLOW_PTR_SIZE, PAGE_KEY_SIZE, RAND_KEY_SIZE


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
        else:
            self.data = data

        if overflow_ptr == None:
            self.overflow_ptr: int = 0
        else:
            self.overflow_ptr = overflow_ptr

    def __bytes__(self) -> bytes:
        fmt = f"{RAND_KEY_SIZE}s{DATA_SIZE}s{OVERFLOW_PTR_SIZE}s"
        return struct.pack(
            fmt,
            self.key.to_bytes(RAND_KEY_SIZE, "little"),
            bytes(self.data),
            self.overflow_ptr.to_bytes(OVERFLOW_PTR_SIZE, byteorder="little"),
        )

    @classmethod
    def from_bytes(cls, record_list: bytes):
        fmt = f"{RAND_KEY_SIZE}s{DATA_SIZE}s{OVERFLOW_PTR_SIZE}s"
        key, data_bytes, overflow_ptr = struct.unpack(fmt, record_list)
        return Record(
            int.from_bytes(key, "little"),
            Data(str(data_bytes)),
            int.from_bytes(overflow_ptr, "little"),
        )

    def __str__(self) -> str:
        return f"key: {self.key:12d} data: {self.data} overflow_ptr: {self.overflow_ptr:10d}\n"

    def __repr__(self) -> str:
        return f"Record(key: {self.key:12d} data: {self.data} overflow_ptr: {self.overflow_ptr:10d})\n"


class SparseIndex:
    def __init__(self, key: int, page_num: int) -> None:
        self.key: int = key
        self.page_num: int = page_num

    @classmethod
    def from_bytes(cls, sparse_index_bytes: bytes):
        fmt = f"{RAND_KEY_SIZE}s{PAGE_KEY_SIZE}s"
        key, page_num = struct.unpack(fmt, sparse_index_bytes)
        return SparseIndex(
            int.from_bytes(key, "little"), int.from_bytes(page_num, "little")
        )

    def __bytes__(self) -> bytes:
        fmt = f"{RAND_KEY_SIZE}s{PAGE_KEY_SIZE}s"
        return struct.pack(fmt, int.to_bytes(self.key), int.to_bytes(self.page_num))

    def __str__(self) -> str:
        return f"SparseIndex(key: {self.key:10}, page_num: {self.page_num:10})"

    def __repr__(self) -> str:
        return f"SparseIndex(key: {self.key}, page_num: {self.page_num})"
