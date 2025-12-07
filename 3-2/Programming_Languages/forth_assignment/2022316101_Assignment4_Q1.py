import random

first = 0
second = 0
third = 0
fourth = 0
fifth = 0

arr = []

for i in range (200):
    arr.append(random.randint(0,100))
    
arr.sort()

for i in range (200):
    print(arr[i], end = " ")
    
for i in range(200):
    if arr[i] < 21:
        first += 1
    elif arr[i] < 41:
        second += 1
    elif arr[i] < 61:
        third += 1
    elif arr[i] < 81:
        fourth += 1
    else:
        fifth += 1
        
print("\n")
print("\nResult")

print("\n1~20: ", end="")
for i in range(first):
    print("*", end=" ")
    
print(f'         {first}')
    
print("\n21~40: ", end="")
for i in range(second):
    print("*", end=" ")
    
print(f'         {second}')

print("\n41~60: ", end="")
for i in range(third):
    print("*", end=" ")

print(f'         {third}')
    
print("\n61~80: ", end="")
for i in range(fourth):
    print("*", end=" ")

print(f'         {fourth}')
    
print("\n81~100: ", end="")
for i in range(fifth):
    print("*", end=" ")

print(f'         {fifth}')    
    