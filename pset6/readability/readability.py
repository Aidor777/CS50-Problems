from cs50 import get_string


def main():
    inputText = get_string("Text: ")
    grade = runColemanLiau(inputText)

    if grade < 1:
        print("Before Grade 1")
    elif grade >= 16:
        print("Grade 16+")
    else:
        print("Grade " + str(round(grade)))


def runColemanLiau(text):
    nLetters = 0
    nWords = 1
    nSentences = 0
    # Iterate over each character in the text
    for char in text:
        # The char is a letter
        if char.isalpha():
            nLetters += 1
        # The char is any kind of space
        elif char.isspace():
            nWords += 1
        # The char denotes end of sentence
        elif char == "." or char == "!" or char == "?":
            nSentences += 1

    # Calculate params for the formula
    L = nLetters / nWords * 100
    S = nSentences / nWords * 100
    # Coleman-Liau formula
    index = 0.0588 * L - 0.296 * S - 15.8
    return index


main()