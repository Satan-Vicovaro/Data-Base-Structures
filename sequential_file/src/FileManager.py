import bisect
from enum import Enum
from pathlib import PosixPath
from config import CHUNK_SIZE, RECORD_SIZE, RECORDS_PER_CHUNK
from src.Structs import Record
from src.IOManager import IOManager


class PageFindStatus(Enum):
    VALUE_EXIST = 0
    IN_OVERFLOW = 1
    FREE_SPACE_TO_APPEND = 2
    EMPTY_FILE = 3
    FILE_IS_FULL = 4


class FileManager:
    def __init__(self, file_name) -> None:
        self.file_name = file_name
        self.io_manager = IOManager(
            Record, filename=file_name, chunk_size=CHUNK_SIZE, record_size=RECORD_SIZE
        )
        self.page_cache = None
        self.cache_page_index: int = 0
        self.all_pages_full = True

    def show_file(self):
        print(list(self.io_manager.read_whole_file()))

    def generate_random_records(self, num=0):
        records = [Record() for _ in range(0, num)]
        records = bytearray(b for record in records for b in bytes(record))
        self.io_manager.write_block(records)

    def add_to_new_page(self, record: Record):
        page_num = self.io_manager.append_to_file(record)
        pass

    def initialize(self, record: Record):
        self.all_pages_full = True
        self.io_manager.append_to_file([record])

    def find_on_page(self, record: Record, page_index: int):
        self.cache_page = list(self.io_manager.read_page(page_index))
        self.cache_page_index = page_index

        # find place on page
        page = self.cache_page
        page_size = len(page)

        if page_size == 0:
            print("Page should not be empty")
            return PageFindStatus.EMPTY_FILE

        index = bisect.bisect_right(page, record.key, key=lambda l: l.key) - 1

        if index == -1:
            print("Why index is -1, Aborting")
            return PageFindStatus.EMPTY_FILE

        closest_record = page[index]

        if closest_record.key == record.key:
            return PageFindStatus.VALUE_EXIST

        if index == RECORDS_PER_CHUNK - 1:
            return PageFindStatus.FILE_IS_FULL

        if index == page_size - 1:
            return PageFindStatus.FREE_SPACE_TO_APPEND

        return PageFindStatus.IN_OVERFLOW

    def append_to_current(self, record: Record, page_index: int):

        self.all_pages_full = False
        if page_index != self.cache_page_index:
            print("Why are you inserting to the same page? Aborting")
            return

        self.cache_page.append(record)
        self.io_manager.write_page(self.cache_page, self.cache_page_index)
