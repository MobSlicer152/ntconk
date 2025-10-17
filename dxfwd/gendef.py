import sys

def main(argv):
    if len(argv) < 3:
        print(f"usage: {argv[0]} <function list> <forwarder dll name> [exclude list]")
        exit(0)

    print(f"generating D3DKMT->Dxgk forwarder def {argv[2]}.def from {argv[1]}")

    funcs = []
    with open(argv[1], "r") as f:
        funcs = list(map(lambda x: x.strip(), f.readlines()))

    if len(argv) > 3:
        with open(argv[3], "r") as f:
            excludes = map(lambda x: x.strip(), f.readlines())
        for e in excludes:
            if e in funcs:
                funcs.remove(e)

    with open(f"{argv[2]}.def", "w") as f:
        f.write(f"LIBRARY \"{argv[2]}\"\nEXPORTS\n")
        for func in funcs:
            print(f"{func}")
            f.write(f"\tD3DKMT{func}=DXGKERNEL.Dxgk{func}\n")

if __name__ == "__main__":
    main(sys.argv)

