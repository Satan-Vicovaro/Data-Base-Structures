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
        self.main_file = FileManager("main_file.txt")
        self.overflow_file = FileManager("overflow_file.txt")
        self.sparse_index_map = SparseIndexMap()

    def do_show(self, arg: str):
        self.main_file.show_file()

    def do_gen(self, arg: str):
        self.main_file.generate_random_records(20)

    def do_add_key(self, arg: str):
        key, data = arg.split(" ", maxsplit=2)
        self.add_key(Record(int(key), data))

    def do_help(self, arg: str) -> bool | None:
        return super().do_help(arg)

    def do_quit(self, arg: str):
        "Quits program"
        print("bye")
        return True

    def do_a(self, arg: str):
        self.add_key(Record())

    def do_q(self, arg: str):
        "Quits program"
        print("bye")
        return True

    def add_key(self, record: Record):
        status, place = self.sparse_index_map.find_place(record.key)

        if status == FindPlaceStatus.FILE_IS_EMPTY:
            self.sparse_index_map.initialize(record, self.main_file)
            return
        if status == FindPlaceStatus.KEY_EXSITS:
            print("Key exsists, aborting")
            return
        if status == FindPlaceStatus.SMALLEST_IN_FILE:
            print("Handle smallest in the file")
            return

        if status == FindPlaceStatus.IN_MIDDLE:
            page_status = self.main_file.find_on_page(record, place.page_index)

            if page_status == PageFindStatus.FILE_IS_FULL:
                print("Create new page and add key there")
                return

            if page_status == PageFindStatus.FREE_SPACE_TO_APPEND:
                self.main_file.append_to_current(record, place.page_index)

    def start(self):
        try:
            self.cmdloop()
        except KeyboardInterrupt:
            print("\nExiting")
