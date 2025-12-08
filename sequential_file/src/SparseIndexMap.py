from config import INDEX_SIZE, SPARSE_INDEX_CHUNK_SIZE
from src.FileManager import FileManager
from src.Structs import Record, SparseIndex
from src.IOManager import IOManager
from enum import Enum
import bisect


class FindPlaceStatus(Enum):
    SMALLEST_IN_FILE = 0
    FILE_IS_EMPTY = 1
    KEY_EXSITS = 2
    IN_MIDDLE = 3


class SparseIndexMap:
    def __init__(self) -> None:
        self.io_manager = IOManager(
            SparseIndex,
            "sparse_index_map.txt",
            chunk_size=SPARSE_INDEX_CHUNK_SIZE,
            record_size=INDEX_SIZE,
        )
        self.sparseIndexes: list[SparseIndex] = []
        self.isFull = True

    def find_place(self, key: int) -> tuple[FindPlaceStatus, SparseIndex]:
        array_size = len(self.sparseIndexes)
        if array_size == 0:
            return (FindPlaceStatus.FILE_IS_EMPTY, SparseIndex(0, 0))

        index = bisect.bisect_right(self.sparseIndexes, key, key=lambda u: u.key) - 1

        place = self.sparseIndexes[index]

        if index == -1:
            return (FindPlaceStatus.SMALLEST_IN_FILE, place)

        if place.key == key:
            return (FindPlaceStatus.KEY_EXSITS, place)

        return (FindPlaceStatus.IN_MIDDLE, place)

    def add_to_end(self, record: Record):
        pass

    def initialize(self, record: Record, main_file: FileManager):
        self.sparseIndexes.append(SparseIndex(record.key, 0))
        main_file.initialize(record)
