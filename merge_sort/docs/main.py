import re

import matplotlib.pyplot as plt

from itertools import groupby
from operator import itemgetter


class Data:
    def __init__(
        self,
        record_num=0,
        read_operations=0,
        write_operations=0,
        time_enlapsed=0.0,
        max_buffers=0,
        records_per_page=0,
    ):
        self.record_num = record_num
        self.read_operations = read_operations
        self.write_operations = write_operations
        self.time_enlapsed = time_enlapsed
        self.max_buffers = max_buffers
        self.records_per_page = records_per_page

    def __str__(self) -> str:
        return (
            f"Records:{self.record_num:<10},"
            f"ROP:{self.read_operations:<10}"
            f"WOP:{self.write_operations:<10}"
            f"Time:{self.time_enlapsed:<3.8},"
            f"Max buff: {self.max_buffers:<6},"
            f"Records per page: {self.records_per_page:6}"
        )

    def __repr__(self) -> str:
        return self.__str__()


def load_data() -> list[Data]:
    data = []
    with open("RPP_increase_1.txt", "r") as f:
        float_regex = f"[+-]?[0-9]+\\.[0-9]+"

        record_num = None
        read_operations = None
        write_operations = None
        time = None
        max_buffers = None
        records_per_page = None
        i = 0
        for line in f:
            # print(f"{i} : {line}")
            record_match = re.match(r"Record num: (\d+)", line)
            read_operations_match = re.match(r"ROP: (\d+)", line)
            write_operation_match = re.match(r"WOP: (\d+)", line)
            time_enlapsed_match = re.match(r"T:\s(" + float_regex + r")", line)
            max_buffers_match = re.match(r"MBC: (\d+)", line)
            records_per_page_match = re.match(r"RPP: (\d+)", line)

            if record_match:
                record_num = record_match.group(1)

            elif read_operations_match:
                read_operations = read_operations_match.group(1)

            elif write_operation_match:
                write_operations = write_operation_match.group(1)
            elif time_enlapsed_match:
                time = time_enlapsed_match.group(1)
            elif max_buffers_match:
                max_buffers = max_buffers_match.group(1)
            elif records_per_page_match:
                records_per_page = records_per_page_match.group(1)

            if (
                record_num
                and read_operations
                and write_operations
                and time
                and max_buffers
                and records_per_page
            ):
                data.append(
                    Data(
                        int(record_num),
                        int(read_operations),
                        int(write_operations),
                        float(time),
                        int(max_buffers),
                        int(records_per_page),
                    )
                )
                # print(
                #     f"{i:<5} data chunk:"
                #     f"Records:{record_num:<10},"
                #     f"ROP:{read_operations:<10}"
                #     f"WOP:{write_operations:<10}"
                #     f"Time:{time:<3.8},"
                #     f"Max buff: {max_buffers:<6},"
                #     f"Records per page: {records_per_page:6}"
                # )
                record_num = read_operations = write_operations = time = max_buffers = (
                    records_per_page
                ) = None
    print(data[0])
    return data


def main():
    data_list = load_data()

    for records, coresponding_data in groupby(
        sorted(data_list, key=lambda x: x.record_num), key=lambda x: x.record_num
    ):

        coresponding_data = list(coresponding_data)
        # plt.title(f"Records: {records}")
        # data = [(dat.time_enlapsed, dat.max_buffers) for dat in coresponding_data]
        # y, x = zip(*data)
        # plt.xlabel("buffer count [n]")
        # plt.ylabel("Time enlapsed [s]")
        # plt.plot(x, y)
        # plt.show()

        plt.title(f"Records: {records}")
        data = [(dat.time_enlapsed, dat.records_per_page) for dat in coresponding_data]
        y, x = zip(*data)
        plt.xlabel("rec_per_page [n]")
        plt.ylabel("Time enlapsed [s]")
        plt.plot(x, y)
        plt.show()

    x = [0, 1, 2, 3, 4]
    y = [0, 1, 4, 9, 16]

    plt.plot(x, y)
    plt.show()


if __name__ == "__main__":
    main()
