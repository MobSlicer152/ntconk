import re
import sys

import pefile

def main(argv):
    if len(argv) < 2:
        print(f"usage: {argv[0]} <module> [regex]")
        exit(0)

    pe = pefile.PE(argv[1])
    r = None
    if len(argv) > 2:
        r = re.compile(argv[2])
    for e in pe.DIRECTORY_ENTRY_EXPORT.symbols:
        name = e.name.decode("utf-8") if e.name else None
        if name is not None and (r is None or r.search(name) is not None):
            print(name)

if __name__ == "__main__":
    main(sys.argv)

