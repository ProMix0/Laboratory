import sys

if len(sys.argv) < 2:
    print("No char to replace specified in command line")
    sys.exit(1)

char = sys.argv[1]
if len(char) < 1:
    print("First argument is empty")
    sys.exit(1)
char = char[0]

try:
    with open("in.txt", 'r') as f_in, open("out.txt", 'w') as f_out:
        for line in f_in:
            print(line.replace(char, " "), file=f_out, end="")
except:
    print("Error happen while work with files; does input file 'in.txt' exists?")
    sys.exit(1)

