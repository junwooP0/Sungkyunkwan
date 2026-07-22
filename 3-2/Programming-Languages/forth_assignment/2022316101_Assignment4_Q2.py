
def sum(n):
    if n == 1:
        return 1
    elif n == 0:
        return 0
    else:
        return n + sum(n-1)
    
while True:
    num = input('Enter a number or enter "Exit" to quit: ')

    if num.lower() == "exit":
        break
    elif num.isdigit():
        n = sum(int(num))
        print(f"Sum of all number before of the inserted number: {n}")
    else:
        print("Please enter an valid number or 'Exit' to quit")