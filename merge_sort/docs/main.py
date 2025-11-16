from math import log10
from os import major
import re

import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d.art3d import math
import numpy as np

from itertools import groupby
from operator import itemgetter
from mpl_toolkits.mplot3d import Axes3D
from matplotlib.ticker import ScalarFormatter


class Data:
    def __init__(
        self,
        record_num=0,
        read_operations=0,
        write_operations=0,
        time_enlapsed=0.0,
        max_buffers=0,
        records_per_page=0,
        phase_count=0,
    ):
        self.record_num = record_num
        self.read_operations = read_operations
        self.write_operations = write_operations
        self.time_enlapsed = time_enlapsed
        self.max_buffers = max_buffers
        self.records_per_page = records_per_page
        self.phase_count = phase_count

    def __str__(self) -> str:
        return (
            f"Records:{self.record_num:<10},"
            f"ROP:{self.read_operations:<10}"
            f"WOP:{self.write_operations:<10}"
            f"Time:{self.time_enlapsed:<3.8},"
            f"Max buff: {self.max_buffers:<6},"
            f"Records per page: {self.records_per_page:6}"
            f"Phase count: {self.phase_count:4}"
        )

    def __repr__(self) -> str:
        return self.__str__()

    def file_write(self) -> str:
        return (
            f"{self.record_num}\t{self.read_operations}\t"
            f"{self.write_operations}\t{self.time_enlapsed}\t"
            f"{self.phase_count}\t{self.records_per_page}\t"
            f"{self.max_buffers}\n"
        )


def load_data() -> list[Data]:
    data = []

    files = ["final_data_XD_next.txt"]

    for file in files:
        with open(file, "r") as f:
            float_regex = f"[+-]?[0-9]+\\.[0-9]+"

            record_num = None
            read_operations = None
            write_operations = None
            time = None
            max_buffers = None
            records_per_page = None
            phase_cout = None
            i = 0
            for line in f:
                # print(f"{i} : {line}")
                record_match = re.match(r"Record num: (\d+)", line)
                read_operations_match = re.match(r"ROP: (\d+)", line)
                write_operation_match = re.match(r"WOP: (\d+)", line)
                time_enlapsed_match = re.match(r"T:\s(" + float_regex + r")", line)
                max_buffers_match = re.match(r"MBC: (\d+)", line)
                records_per_page_match = re.match(r"RPP: (\d+)", line)
                phase_count_match = re.match(r" PHS: (\d+)", line)

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
                elif phase_count_match:
                    phase_cout = phase_count_match.group(1)

                if (
                    record_num
                    and read_operations
                    and write_operations
                    and time
                    and max_buffers
                    and records_per_page
                    and phase_cout
                ):
                    data.append(
                        Data(
                            int(record_num),
                            int(read_operations),
                            int(write_operations),
                            float(time),
                            int(max_buffers),
                            int(records_per_page),
                            int(phase_cout) - 1,
                        )
                    )
                    record_num = read_operations = write_operations = time = (
                        max_buffers
                    ) = records_per_page = phase_cout = None
    return data


def cool_data_viz(data_list):
    # data_list = [
    #     data
    #     for data in data_list
    #     if data.read_operations != 3 and data.time_enlapsed < 1000.0
    # ]

    for records, coresponding_data in groupby(
        sorted(data_list, key=lambda x: (x.record_num, x.time_enlapsed)),
        key=lambda x: x.record_num,
    ):

        data = list(coresponding_data)

        data = [
            (dat.max_buffers, dat.records_per_page, dat.phase_count) for dat in data
        ]

        y, x, z = zip(*data)
        # z = [val for val in z]

        x_bins = np.linspace(min(x), max(x), 100)
        y_bins = np.linspace(min(y), max(y), 100)
        # x_bins = sorted(x)
        # y_bins = sorted(y)

        heatmap, xedges, yedges = np.histogram2d(
            x, y, bins=[x_bins, y_bins], weights=z, density=False
        )
        xpos, ypos = np.meshgrid(
            xedges[:-1] + 0.5 * (xedges[1] - xedges[0]),
            yedges[:-1] + 0.5 * (yedges[1] - yedges[0]),
        )

        xpos = xpos.flatten()
        ypos = ypos.flatten()
        zpos = np.zeros_like(xpos)

        z = heatmap.flatten()

        fig, ax = plt.subplots(figsize=(8, 6))
        c = ax.pcolormesh(xedges, yedges, heatmap.T, shading="auto", cmap="viridis")
        colors = fig.colorbar(c, ax=ax)
        colors.set_label("Time")
        ax.set_xlabel("Records per page")
        ax.set_ylabel("Max buffer count")
        ax.set_title("2D Heatmap")
        plt.show()
        plt.show(block=True)


def log_n(k, n):
    return np.log(k) / np.log(n)


def proper_viz(data_list: list[Data]):

    expected_operation_x = np.linspace(
        min(data_list, key=lambda x: x.record_num).record_num,
        max(data_list, key=lambda x: x.record_num).record_num,
        len(data_list),
    )
    print(np.min(expected_operation_x))
    print(np.max(expected_operation_x))
    print(min(data_list, key=lambda x: x.record_num).record_num)
    print(max(data_list, key=lambda x: x.record_num).record_num)

    expected_operation_y = (
       2* (expected_operation_x / 200) * (log_n(expected_operation_x/200, 200))
    )
    # expected_operation_y = np.ones(len(data_list))

    viz_data = [
        (dat.read_operations + dat.write_operations, dat.record_num)
        for dat in data_list
    ]

    y, x = zip(*viz_data)
    plt.gca().yaxis.set_major_formatter(ScalarFormatter(useMathText=True))
    plt.gca().xaxis.set_major_formatter(ScalarFormatter(useMathText=True))
    plt.title("Read + write")
    plt.grid(visible=True, which="both")
    plt.scatter(x, y)
    plt.plot(
        expected_operation_x, expected_operation_y, "r", label="expected reads + writes"
    )
    plt.xlabel("Record num [n]")
    plt.ylabel("Read + write operations [n]")
    plt.legend()
    plt.show()

    viz_data = [(dat.time_enlapsed, dat.record_num) for dat in data_list]

    y, x = zip(*viz_data)
    plt.title("Time")
    plt.grid(visible=True, which="both")
    plt.scatter(x, y)
    plt.xlabel("Record num [n]")
    plt.ylabel("Time [s]")
    plt.show()

    viz_data = [
        (dat.read_operations, dat.write_operations, dat.record_num) for dat in data_list
    ]

    y, z, x = zip(*viz_data)
    plt.title("Read and Write")
    plt.grid(visible=True, which="both")
    plt.scatter(x, y, label="Read operations")
    plt.scatter(x, z, label="Write operations")
    plt.plot(
        expected_operation_x, expected_operation_y, "r", label="expected reads + writes"
    )
    plt.legend()
    plt.xlabel("Record num [n]")
    plt.ylabel("Operation count [n]")
    plt.show()
    viz_data = [(dat.phase_count, dat.record_num) for dat in data_list]

    y, x = zip(*viz_data)
    plt.title("Phase count")
    plt.grid(visible=True)
    plt.scatter(x, y)
    plt.xlabel("Record num [n]")
    plt.ylabel("Phase count [n]")
    plt.show()


def into_file(data_list: list[Data]):
    with open("Formated data", "w+") as f:
        for record in data_list:
            f.write(record.file_write())


def main():
    data_list = load_data()
    # cool_data_viz(data_list)
    proper_viz(data_list)
    into_file(data_list)


if __name__ == "__main__":
    main()
