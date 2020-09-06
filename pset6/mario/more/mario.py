from cs50 import get_int


def main():
    height = get_height()
    for i in range(height):
        print(" " * (height - i - 1), end="")
        print("#" * (i + 1), end="")
        print(" " * 2, end="")
        print("#" * (i + 1))


def get_height():
    while True:
        h = get_int("Height: ")
        if h > 0 and h < 9:
            break
    return h


main()