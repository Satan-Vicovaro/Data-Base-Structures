import pathlib
import random
import re
import config
from src.IOManager import IOManager
from src.Structs import Record
from src.SparseIndexMap import FindPlaceStatus, SparseIndexMap
from src.FileManager import FileManager, PageFindStatus
import cmd
from itertools import pairwise
import time

import logging


def count_io_operations(func):
    def wrapper(*args, **kwargs):
        time_before = time.time()
        read_count_before = IOManager.total_read_count
        write_count_before = IOManager.total_write_count

        result = func(*args, **kwargs)

        time_after = time.time()
        read_count_after = IOManager.total_read_count
        write_count_after = IOManager.total_write_count

        logging.info(f"Reads: {read_count_after - read_count_before}")
        logging.info(f"Writes: {write_count_after - write_count_before}")
        logging.info(f"Time: {time_after - time_before}")

        return result

    return wrapper


class SequentialDb(cmd.Cmd):
    intro = "welcome to this super cool SequentialDb"
    prompt = "(db) :3 >"

    def __init__(self) -> None:
        super().__init__()
        self.sparse_index_map = SparseIndexMap()

    def reinit(self) -> None:
        self.sparse_index_map = SparseIndexMap()

    def do_show(self, arg: str):
        self.sparse_index_map.show_file()

    def do_proper_show(self, arg: str):
        self.sparse_index_map.proper_order_show()

    @count_io_operations
    def do_gen(self, arg: str):
        if arg == "":
            return
        key_num = int(arg)
        for _ in range(0, key_num):
            rand_record = Record.random_record()
            logging.debug(f"genenerating:{rand_record}")
            self.add_key(rand_record)

    @count_io_operations
    def do_add_key(self, arg: str):
        key, data = arg.split(" ", maxsplit=2)
        self.add_key(Record(int(key), str(data)))

    def do_help(self, arg: str) -> bool | None:
        return super().do_help(arg)

    def do_quit(self, arg: str):
        "Quits program"
        logging.debug("bye")
        return True

    @count_io_operations
    def do_delete(self, arg: str):
        result = self.sparse_index_map.delete_record(int(arg))
        if result:
            logging.debug("Succesfully deleted")
        else:
            logging.debug("Could not delete")

    @count_io_operations
    def do_update(self, arg: str):
        key, value = arg.split(" ", maxsplit=2)
        self.sparse_index_map.update_record(Record(int(key), value))
        # self.sparse_index_map

    @count_io_operations
    def do_find(self, arg: str):
        record = self.sparse_index_map.find_record(int(arg))
        if record is None:
            logging.debug("Value does not exist")
            return
        logging.debug(f"Found: {record}")

    @count_io_operations
    def do_test_add_records(self, arg: str):
        if arg == "":
            record_num = 10
        else:
            record_num = int(arg)

        for i in range(0, record_num):
            self.add_key(Record(i, str(i)))

        self.do_show("")
        self.check_proper_order()

    @count_io_operations
    def do_test_random_adds_and_deletions(self, arg: str):

        if arg == "":
            record_num = 10
        else:
            record_num = int(arg)

        for _ in range(record_num):
            mode = random.randint(0, 2)
            added_keys = {}
            if mode == 0:
                if len(added_keys) == 0:
                    continue
                delete_record_key = random.choice(list(added_keys.keys()))
                self.sparse_index_map.delete_record(delete_record_key)
            else:
                rand_record = Record.random_record()
                added_keys[rand_record.key] = rand_record
                self.add_key(rand_record)

    @count_io_operations
    def do_test_add_and_delete_all_record(self, arg: str):

        if arg == "":
            record_num = 30
        else:
            record_num = int(arg)

        key_added = {}

        for _ in range(record_num):
            rand_record = Record.random_record()
            key_added[rand_record.key] = rand_record
            self.add_key(rand_record)
        for key in key_added.keys():
            self.sparse_index_map.delete_record(key)

        self.do_show("")
        self.do_proper_show("")

    def do_ar(self, arg: str):
        self.add_key(Record.random_record())

    def do_reorganize(self, arg: str):
        self.sparse_index_map.reorganize()

    def do_q(self, arg: str):
        "Quits program"
        logging.debug("bye")
        return True

    def add_key(self, record: Record):
        self.sparse_index_map.add_key(record)

        if self.sparse_index_map.should_reorganize():
            logging.debug("Automatic reorganisation")
            self.do_reorganize("")

    def check_proper_order(self):
        for (cur_record, _), (next_record, _) in pairwise(
            self.sparse_index_map.iter_all()
        ):
            if cur_record.is_empty():
                continue
            if next_record.is_empty():
                continue
            if cur_record.key >= next_record.key:
                logging.debug("Values are not sorted proprely!!")
                return
        logging.debug("With proper order pass")

    def do_tests(self, arg: str):
        for record_num in range(10, 100001, 50000):
            for alpha_val in range(1, 11, 5):
                for reorog_val in range(1, 11, 5):
                    config.ALPHA = alpha_val / 10
                    config.REORGANIZATION_TRESHOLD = reorog_val / 10
                    self.__init__()
                    self.do_gen(str(record_num))

    def start(self):
        try:
            self.cmdloop()
        except KeyboardInterrupt:
            logging.info("\nExiting")
