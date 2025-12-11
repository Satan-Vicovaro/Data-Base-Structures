import pathlib
from src.Structs import Record
from src.SparseIndexMap import FindPlaceStatus, SparseIndexMap
from src.FileManager import FileManager, PageFindStatus
import cmd


class SequentialDb(cmd.Cmd):
    intro = "welcome to this super cool SequentialDb"
    prompt = "(db) :3 >"

    def __init__(self) -> None:
        super().__init__()
        self.sparse_index_map = SparseIndexMap()

    def do_show(self, arg: str):
        self.sparse_index_map.show_file()

    def do_proper_show(self, arg: str):
        self.iterate_over_all()

    def do_gen(self, arg: str):
        # self.main_file.generate_random_records(20)
        pass

    def do_add_key(self, arg: str):
        key, data = arg.split(" ", maxsplit=2)
        self.add_key(Record(int(key), data))

    def do_help(self, arg: str) -> bool | None:
        return super().do_help(arg)

    def do_quit(self, arg: str):
        "Quits program"
        print("bye")
        return True

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

    def do_t2(self, arg: str):
        self.add_key(Record(10, "10"))
        self.add_key(Record(20, "20"))
        for i in range(11, 20):
            self.add_key(Record(i, str(i)))

    def do_t3(self, arg: str):
        self.add_key(Record(10, "10"))
        self.add_key(Record(20, "20"))
        self.add_key(Record(30, "30"))

        for i in range(1, 6):
            self.add_key(Record(10 + i, str(10 + i)))
            self.add_key(Record(20 + i, str(20 + i)))

    def do_t4(self, arg: str):
        self.add_key(Record(10, "10"))
        self.add_key(Record(100, "20"))
        for i in range(11, 100):
            self.add_key(Record(i, str(i)))

    def do_a(self, arg: str):
        self.add_key(Record())

    def do_q(self, arg: str):
        "Quits program"
        print("bye")
        return True

    def add_key(self, record: Record):
        self.sparse_index_map.add_key(record)

    def iterate_over_all(self):
        print("todo: implement inerate over all")

    def start(self):
        try:
            self.cmdloop()
        except KeyboardInterrupt:
            print("\nExiting")
