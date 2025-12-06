from sequential_file.config import INDEX_SIZE, SPARSE_INDEX_CHUNK_SIZE
from sequential_file.src.Structs import SparseIndex
from src.IOManager import IOManager


class SparseIndexMap:
    def __init__(self) -> None:
        self.io_manager = IOManager(
            SparseIndex,
            "sparse_index_map.txt",
            chunk_size=SPARSE_INDEX_CHUNK_SIZE,
            record_size=INDEX_SIZE,
        )
