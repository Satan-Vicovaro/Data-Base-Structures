from config import INDEX_SIZE, SPARSE_INDEX_CHUNK_SIZE
from src.FileManager import FileManager, PageFindStatus
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
            "sparse_index_map.bin",
            chunk_size=SPARSE_INDEX_CHUNK_SIZE,
            record_size=INDEX_SIZE,
        )
        self.sparseIndexes: list[SparseIndex] = []
        self.isFull = True

        self.main_file = FileManager("main_file.bin")
        self.overflow_file = FileManager("overflow_file.bin")

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

    def show_file(self):
        print("Main:")
        self.main_file.show_file()

        print("Overflow:")
        self.overflow_file.show_file()

    def add_to_end(self, record: Record):
        pass

    def initialize(self, record: Record, main_file: FileManager):
        self.sparseIndexes.append(SparseIndex(record.key, 0))
        main_file.initialize(record)

    def add_key(self, record: Record):
        status, place = self.find_place(record.key)

        if status == FindPlaceStatus.FILE_IS_EMPTY:
            self.initialize(record, self.main_file)
            return
        if status == FindPlaceStatus.KEY_EXSITS:
            print("Key exsists, aborting")
            return
        if status == FindPlaceStatus.SMALLEST_IN_FILE:
            print("Handle smallest in the file")
            return

        if status == FindPlaceStatus.IN_MIDDLE:
            page_status, closest_record = self.main_file.find_on_page(
                record, place.page_index
            )

            if page_status == PageFindStatus.FILE_IS_FULL:
                print("Create new page and add key there")
                return

            if page_status == PageFindStatus.FREE_SPACE_TO_APPEND:
                self.main_file.append_to_current(record, place.page_index)
                return

            if page_status == PageFindStatus.IN_OVERFLOW:
                record_number = self.overflow_file.append_to_end(record)
                closest_record.overflow_ptr = record_number
                self.main_file.update_record(closest_record)
                return

            if page_status == PageFindStatus.VALUE_EXIST:
                print("Value exsits, aborting")
                return
            if status == PageFindStatus.EMPTY_FILE:
                print("LOL")
                return

    def iterate_overflow(self, current_page, next_page, record: Record):

        pass
