from config import (
    INDEX_SIZE,
    INDEXES_PER_CHUNK,
    REORGANIZATION_TRESHOLD,
    SPARSE_INDEX_CHUNK_SIZE,
)
from src.FileManager import FileManager, PageFindStatus
from src.Structs import Page, Record, SparseIndex
from src.IOManager import IOManager
from enum import Enum
from copy import copy
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
            return (FindPlaceStatus.SMALLEST_IN_FILE, self.sparseIndexes[0])

        if place.key == key:
            return (FindPlaceStatus.KEY_EXSITS, place)

        return (FindPlaceStatus.IN_MIDDLE, place)

    def show_file(self):
        print("Main:")
        self.main_file.show_file()

        print("Overflow:")
        self.overflow_file.show_file()

    def proper_order_show(self):
        for record, depth in self.iter_all():
            if record.is_empty() or record.is_deleted():
                continue
            indent = " " * depth
            print(f"{indent} {record}")

    def initialize(self, record: Record, main_file: FileManager):
        self.sparseIndexes.append(SparseIndex(record.key, 0))
        main_file.initialize(record)

    def update_record(self, record: Record):
        status, place = self.find_place(record.key)

        if status != FindPlaceStatus.IN_MIDDLE and status != FindPlaceStatus.KEY_EXSITS:
            print("Can not update value")
            return False

        page_status, closest_record = self.main_file.find_on_page(
            record, place.page_index
        )

        if page_status == PageFindStatus.VALUE_EXIST:
            self.main_file.update_record(record)
            return True
        if page_status == PageFindStatus.IN_OVERFLOW:
            for overflow_val, _ in self.iter_overflow(closest_record):
                if overflow_val.key != record.key:
                    continue
                if closest_record.is_empty():
                    return False

                overflow_val.data = record.data
                self.overflow_file.update_record_overflow(overflow_val)
                return True
        return False

    def __handle_smaller_insert(self, record: Record, place: SparseIndex):
        page = self.main_file.get_page(place.page_index)

        if page.can_insert():
            self.sparseIndexes[0].key = record.key
            page.insert(record)
            self.main_file.write_updated_page(page)
            return

        self.sparseIndexes[0].key = record.key
        record.overflow_ptr = self.overflow_file.get_next_overflow_ptr()

        page = self.main_file.get_page(place.page_index)
        old_page_smallest = page.records[0]
        page.records[0] = record

        self.main_file.write_updated_page(page)
        self.overflow_file.append_to_end(old_page_smallest)

    def add_key(self, record: Record):
        status, place = self.find_place(record.key)

        if status == FindPlaceStatus.FILE_IS_EMPTY:
            self.initialize(record, self.main_file)
            return
        if status == FindPlaceStatus.KEY_EXSITS:
            print("Key exsists, aborting")
            return
        if status == FindPlaceStatus.SMALLEST_IN_FILE:
            self.__handle_smaller_insert(record, place)

        if status == FindPlaceStatus.IN_MIDDLE:
            page_status, closest_record = self.main_file.find_on_page(
                record, place.page_index
            )

            if page_status == PageFindStatus.FREE_SPACE_TO_APPEND:
                self.main_file.append_to_current(record, place.page_index)
                return

            if page_status == PageFindStatus.IN_OVERFLOW:
                self.add_overflow(self.main_file.cache_page, closest_record, record)
                return

            if page_status == PageFindStatus.IS_DELETED:
                closest_record.data = record.data
                self.main_file.write_updated_page(self.main_file.cache_page)

            if page_status == PageFindStatus.VALUE_EXIST:
                print("Value exsits, aborting")
                return

            if status == PageFindStatus.EMPTY_FILE:
                print("Error: I should not be here!")
                return

    def find_record(self, key: int) -> Record | None:
        map_status, sparse_index = self.find_place(key)

        if map_status == FindPlaceStatus.FILE_IS_EMPTY:
            return None
        if map_status == FindPlaceStatus.SMALLEST_IN_FILE:
            return None

        page_status, closest_record = self.main_file.find_on_page(
            Record(key, "", 0), sparse_index.page_index
        )

        if page_status == PageFindStatus.VALUE_EXIST:
            if closest_record.is_empty():
                return None
            if closest_record.is_deleted():
                return None
            return closest_record

        if page_status == PageFindStatus.IN_OVERFLOW:
            for overflow_val, _ in self.iter_overflow(closest_record):
                if overflow_val.key != key:
                    continue

                if closest_record.is_empty():
                    return None
                if closest_record.is_deleted():
                    return None
                return overflow_val
        return None

    def delete_record(self, key: int) -> bool:
        map_status, sparse_index = self.find_place(key)

        if map_status == FindPlaceStatus.FILE_IS_EMPTY:
            return False
        if map_status == FindPlaceStatus.SMALLEST_IN_FILE:
            return False

        page_status, closest_record = self.main_file.find_on_page(
            Record(key, "", 0), sparse_index.page_index
        )

        if page_status == PageFindStatus.VALUE_EXIST:
            if closest_record.is_deleted():
                print("Value already deleted")
                return False

            if closest_record.is_empty():
                return False

            closest_record.mark_as_deleted()
            self.main_file.update_record(closest_record)
            return True

        if page_status == PageFindStatus.IN_OVERFLOW:
            for overflow_val, _ in self.iter_overflow(closest_record):
                if overflow_val.key != key:
                    continue

                if closest_record.is_deleted():
                    print("Value already deleted")
                    return False

                if closest_record.is_empty():
                    return False

                overflow_val.mark_as_deleted()
                self.overflow_file.update_record_overflow(overflow_val)
                return True

        return False

    def add_overflow(
        self, current_page: Page, current_record: Record, record_to_add: Record
    ):
        while True:
            if current_record.overflow_ptr == 0:

                overflow_ptr_to_record_to_add = (
                    self.overflow_file.get_next_overflow_ptr()
                )
                current_record.overflow_ptr = overflow_ptr_to_record_to_add
                current_page.update_record(current_record)
                self.main_file.write_updated_page(current_page)
                self.overflow_file.append_to_end(record_to_add)
                break

            next_page, next_record = self.overflow_file.get_page_and_record_from_ptr(
                current_record.overflow_ptr
            )

            if next_record.key == record_to_add.key:
                print("Value exsits aborting")
                break

            if next_record.key > record_to_add.key:
                record_to_add.overflow_ptr = current_record.overflow_ptr

                overflow_ptr_to_record_to_add = (
                    self.overflow_file.get_next_overflow_ptr()
                )
                current_record.overflow_ptr = overflow_ptr_to_record_to_add

                current_page.update_record(current_record)
                if current_page.location == self.overflow_file.file_name:
                    self.overflow_file.write_updated_page(current_page)
                else:
                    self.main_file.write_updated_page(current_page)

                self.overflow_file.append_to_end(record_to_add)
                break

            if next_record.overflow_ptr == 0:
                overflow_ptr_to_record_to_add = (
                    self.overflow_file.get_next_overflow_ptr()
                )
                next_record.overflow_ptr = overflow_ptr_to_record_to_add
                next_page.update_record(next_record)

                self.overflow_file.write_updated_page(next_page)
                self.overflow_file.append_to_end(record_to_add)
                break

            current_page = next_page
            current_record = next_record

    def iter_overflow(self, record: Record):
        cur_record = record
        depth = 1
        while cur_record.overflow_ptr != 0:
            _, next_record = self.overflow_file.get_page_and_record_from_ptr(
                cur_record.overflow_ptr
            )

            yield next_record, depth
            depth += 1
            cur_record = next_record

    def iter_page(self, page_index: int):
        page = self.main_file.get_page(page_index)

        for record in page.records:
            yield record, 0
            yield from self.iter_overflow(record)

    def iter_all(self):
        for sprase_index in self.sparseIndexes:
            yield from self.iter_page(sprase_index.page_index)

    def get_record_num(self) -> int:
        record_num_main = self.main_file.get_next_overflow_ptr() - 1
        record_num_overflow = self.overflow_file.get_next_overflow_ptr() - 1
        return record_num_main + record_num_overflow

    def should_reorganize(self) -> bool:
        main_file_size = self.main_file.get_next_overflow_ptr() - 1
        of_file_size = self.overflow_file.get_next_overflow_ptr() - 1

        return of_file_size / main_file_size > REORGANIZATION_TRESHOLD

    def reorganize(self):
        self.main_file.init_new_file()
        new_spare_indexes: list[SparseIndex] = []
        self.io_manager.truncate()

        for record, _ in self.iter_all():
            if record.is_empty() or record.is_deleted():
                continue

            smallest_key_on_page, page_index = (
                self.main_file.aggreate_append_to_new_file(copy(record))
            )

            if smallest_key_on_page is not None and page_index is not None:
                new_spare_indexes.append(SparseIndex(smallest_key_on_page, page_index))

                if len(new_spare_indexes) > INDEXES_PER_CHUNK:
                    self.io_manager.append_chunk_to_file(new_spare_indexes)
                    new_spare_indexes.clear()

        smallest_key_on_page, page_index = self.main_file.append_leftovers()

        if smallest_key_on_page is not None and page_index is not None:
            new_spare_indexes.append(SparseIndex(smallest_key_on_page, page_index))

        if len(new_spare_indexes) > 0:
            self.io_manager.append_chunk_to_file(new_spare_indexes)
            new_spare_indexes.clear()

        self.main_file.switch_to_new_file()
        self.overflow_file.truncate_file()
        self.sparseIndexes = list(self.io_manager.read_whole_file())
