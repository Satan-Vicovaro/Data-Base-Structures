import re


def main():
    with open("data_file.txt", "r") as f:
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
                print(
                    f"{i:<5} data chunk:"
                    f"Records:{record_num:<10},"
                    f"ROP:{read_operations:<10}"
                    f"WOP:{write_operations:<10}"
                    f"Time:{time:<3.8},"
                    f"Max buff: {max_buffers:<6},"
                    f"Records per page: {records_per_page:6}"
                )
                record_num = read_operations = write_operations = time = max_buffers = (
                    records_per_page
                ) = None
                i += 1


if __name__ == "__main__":
    main()
