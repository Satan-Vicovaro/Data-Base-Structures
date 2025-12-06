import struct
from src.SparseIndexMap import SparseIndexMap
from src.FileManager import FileManager
import cmd


class SequentialDb(cmd.Cmd):
    intro = "welcome to this super cool SequentialDb"
    prompt = "(db) :3 >"

    def __init__(self) -> None:
        super().__init__()
        self.main_file = FileManager("main_file.txt")
        self.overflow_file = FileManager("overflow_file.txt")
        self.sparse_index_map = SparseIndexMap()

    def do_show(self, arg: str):
        self.main_file.show_file()

    def do_gen(self, arg: str):
        self.main_file.generate_random_records(20)

    def do_add_key(self, arg: str):
        self.add_key(arg)

    def do_help(self, arg: str) -> bool | None:
        return super().do_help(arg)

    def do_quit(self, arg: str):
        "Quits program"
        print("bye")
        return True

    def do_q(self, arg: str):
        "Quits program"
        print("bye")
        return True

    def add_key(self, arg: str):
        try:
            key, data = arg.split(" ", maxsplit=2)
        except:
            print("Wrong values")

    def start(self):
        try:
            self.cmdloop()
        except KeyboardInterrupt:
            print("\nExiting")
