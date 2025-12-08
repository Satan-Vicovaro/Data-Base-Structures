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
        self.main_file = FileManager("main_file.bin")
        self.overflow_file = FileManager("overflow_file.bin")
        self.sparse_index_map = SparseIndexMap()

    def do_show(self, arg: str):
        print("Main File")
        self.main_file.show_file()
        print("\n Overflow File")
        self.overflow_file.show_file()

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

    def start(self):
        try:
            self.cmdloop()
        except KeyboardInterrupt:
            print("\nExiting")
