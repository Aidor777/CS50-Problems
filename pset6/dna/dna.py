from sys import argv
import csv
import re


def main():
    if len(argv) != 3:
        print("Usage: python dna.py database.csv sequence.txt")
        exit(1)

    # Read the data from the csv file and store it in a list of dict
    data = []
    with open(argv[1], newline='') as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            data.append(row)

    # Store all STRs in a list
    strs = list(data[0].keys())
    strs.pop(0)

    # Read the sequence and store it
    with open(argv[2], "r") as file:
        sequence = file.read()

    matchings = {}
    for shortTR in strs:
        # Regex to find all occurences of the STR, repeated if they are but not overlapping
        occurences = re.findall("(?:" + shortTR + ")+", sequence)
        max = 0
        shortTRLength = len(shortTR)
        # Find the highest number of sequential repetitions
        for occurence in occurences:
            nMatches = (len(occurence) // shortTRLength)
            max = nMatches if nMatches > max else max
        matchings[shortTR] = max

    culprit = "No match"
    for person in data:
        allMatch = True
        # See for each person if the number of matchings from database are the same as the ones from the given sequence
        for matching in matchings:
            if matchings[matching] != int(person[matching]):
                allMatch = False
                break
        if allMatch == True:
            culprit = person["name"]

    print(culprit)


main()