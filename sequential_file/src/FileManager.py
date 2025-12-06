from src.Structs import Record
from src.IOManager import IOManager


class FileManager:
    def __init__(self, file_name) -> None:
        self.file_name = file_name
        self.io_manager = IOManager(file_name)
        self.page_cache = None

    def show_file(self):
        print(list(self.io_manager.read_whole_file()))

    def generate_random_records(self, num=0):
        records = [Record() for _ in range(0, num)]
        self.io_manager.block_write(records)
