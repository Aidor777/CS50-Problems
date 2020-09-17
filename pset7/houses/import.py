from cs50 import SQL
from sys import argv
import csv


def main():
    if len(argv) != 2:
        print("Usage: python import.py file.csv")
        exit(1)

    # Open connection to the database
    db = SQL("sqlite:///students.db")

    # Import data from the CSV file
    with open(argv[1], "r") as csvFile:
        reader = csv.DictReader(csvFile)
        # Read data row by row
        for row in reader:
            middleName = None
            fullName = row["name"].split(" ")
            # This person has a middle name
            if len(fullName) == 3:
                firstName = fullName[0]
                middleName = fullName[1]
                lastName = fullName[2]
            # This person does not have a middle name
            else:
                firstName = fullName[0]
                lastName = fullName[1]

            # Insert data into the database
            db.execute("INSERT INTO students (first, middle, last, house, birth) VALUES(?, ?, ?, ?, ?)",
                        firstName, middleName, lastName, row["house"], int(row["birth"]))


main()