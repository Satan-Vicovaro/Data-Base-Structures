import pathlib
import re
from src.IOManager import IOManager
from src.Structs import Record
from src.SparseIndexMap import FindPlaceStatus, SparseIndexMap
from src.FileManager import FileManager, PageFindStatus
import cmd
from itertools import pairwise


def count_io_operations(func):
    def wrapper(*args, **kwargs):
        read_count_before = IOManager.total_read_count
        write_count_before = IOManager.total_write_count
        result = func(*args, **kwargs)

        read_count_after = IOManager.total_read_count
        write_count_after = IOManager.total_write_count

        print(f"Reads: {read_count_after - read_count_before}")
        print(f"Writes: {write_count_after - write_count_before}")

        return result

    return wrapper


class SequentialDb(cmd.Cmd):
    intro = "welcome to this super cool SequentialDb"
    prompt = "(db) :3 >"

    def __init__(self) -> None:
        super().__init__()
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
            print(f"genenerating:{rand_record}")
            self.add_key(rand_record)

    @count_io_operations
    def do_add_key(self, arg: str):
        key, data = arg.split(" ", maxsplit=2)
        self.add_key(Record(int(key), str(data)))

    def do_help(self, arg: str) -> bool | None:
        return super().do_help(arg)

    def do_quit(self, arg: str):
        "Quits program"
        print("bye")
        return True

    @count_io_operations
    def do_delete(self, arg: str):
        result = self.sparse_index_map.delete_record(int(arg))
        if result:
            print("Succesfully deleted")
        else:
            print("Could not delete")

    @count_io_operations
    def do_update(self, arg: str):
        key, value = arg.split(" ", maxsplit=2)
        self.sparse_index_map.update_record(Record(int(key), value))
        # self.sparse_index_map

    @count_io_operations
    def do_find(self, arg: str):
        record = self.sparse_index_map.find_record(int(arg))
        if record is None:
            print("Value does not exist")
            return
        print(f"Found: {record}")

    def do_t1(self, arg: str):
        self.add_key(Record(5, "5"))
        self.add_key(Record(10, "10"))
        self.add_key(Record(15, "15"))
        self.add_key(Record(20, "20"))
        self.add_key(Record(30, "30"))
        self.add_key(Record(6, "6"))
        self.add_key(Record(7, "7"))
        self.add_key(Record(11, "11"))
        self.add_key(Record(12, "12"))
        self.add_key(Record(16, "13"))
        self.add_key(Record(21, "21"))
        self.check_proper_order()

    def do_t2(self, arg: str):
        self.add_key(Record(10, "10"))
        self.add_key(Record(20, "20"))
        for i in range(11, 20):
            self.add_key(Record(i, str(i)))
        self.check_proper_order()

    def do_t3(self, arg: str):
        self.add_key(Record(10, "10"))
        self.add_key(Record(20, "20"))
        self.add_key(Record(30, "30"))

        for i in range(1, 6):
            self.add_key(Record(10 + i, str(10 + i)))
            self.add_key(Record(20 + i, str(20 + i)))

        self.check_proper_order()

    def do_t4(self, arg: str):
        self.add_key(Record(10, "10"))
        self.add_key(Record(100, "20"))
        for i in range(11, 100):
            self.add_key(Record(i, str(i)))

        self.check_proper_order()

    def do_t5(self, arg: str):
        self.add_key(Record(99, "99"))
        self.add_key(Record(165, "165"))
        self.add_key(Record(133, "133"))
        self.add_key(Record(119, "119"))
        self.add_key(Record(42, "42"))
        self.add_key(Record(94, "94"))
        self.add_key(Record(140, "140"))

        self.check_proper_order()

    def do_t6(self, arg: str = "10"):
        self.add_key(Record(0, "0"))
        for _ in range(0, int(arg)):
            self.add_key(Record.random_record())

        self.check_proper_order()

    def do_t7(self, arg: str):
        self.add_key(Record(10, "10"))
        self.add_key(Record(20, "20"))
        self.add_key(Record(15, "15"))
        self.add_key(Record(12, "12"))
        self.check_proper_order()

    def do_t8(self, arg: str):
        self.add_key(Record(0, "0"))
        for _ in range(50):
            self.add_key(Record.random_record())
        self.do_reorganize("")
        for _ in range(50):
            self.add_key(Record.random_record())
        self.check_proper_order()
        self.do_reorganize("")
        self.check_proper_order()

    def do_t9(self, arg: str):
        for i in range(0, 30, 5):
            self.add_key(Record(i, str(i)))
        self.add_key(Record(6, "6"))
        self.add_key(Record(7, "6"))
        self.add_key(Record(8, "6"))

    def do_a(self, arg: str):
        self.add_key(Record.random_record())

    def do_reorganize(self, arg: str):
        self.sparse_index_map.reorganize()

    def do_q(self, arg: str):
        "Quits program"
        print("bye")
        return True

    def add_key(self, record: Record):
        self.sparse_index_map.add_key(record)

        if self.sparse_index_map.should_reorganize():
            print("Automatic reorganisation")
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
                print("Values are not sorted proprely!!")
                return
        print("With proper order pass")

    def start(self):
        try:
            self.cmdloop()
        except KeyboardInterrupt:
            print("\nExiting")
