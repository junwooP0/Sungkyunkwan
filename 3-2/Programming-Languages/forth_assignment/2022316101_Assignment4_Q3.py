def isPrime(n):
    if n == 1:
        return False
    for i in range(2, n):
        if n % i == 0:
            return False
    return True

num = int(input("What is the prime number at rank: "))

answer = 0

while num > 0:
    answer += 1
    if isPrime(answer):
        num -= 1

print('The prime number is', answer)