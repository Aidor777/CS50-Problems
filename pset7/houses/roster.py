from cs50 import SQL
from sys import argv


def main():
    if len(argv) != 2:
        print("Usage: python roster.py houseName")
        exit(1)

    # Open connection to the database
    db = SQL("sqlite:///students.db")

    houseName = argv[1]

    # Query the database for all students from given house
    students = db.execute("SELECT first, middle, last, birth FROM students WHERE house = ? ORDER BY last ASC, first ASC", houseName)

    for student in students:
        # Re-build the full name
        fullName = student["first"] + ((" " + student["middle"] + " ") if student["middle"] is not None else " ") + student["last"]
        birth = str(student["birth"])
        print(f"{fullName}, born {birth}", end='\n')


main()