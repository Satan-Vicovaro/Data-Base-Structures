from src.SequentialDb import SequentialDb
import config
import logging

if config.DEBUG_ON:
    current_level = logging.DEBUG
else:
    current_level = logging.INFO

logging.basicConfig(level=current_level, format="%(levelname)s: %(message)s")


def main():
    db = SequentialDb()
    db.start()


if __name__ == "__main__":
    main()
