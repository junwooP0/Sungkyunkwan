a = 12345

n = int(input("n: "))

if set(str(a)) == set(str(n)):
    print("n", n ,"은 12345와 쌍둥이입니다.")
if set(str(a)) != set(str(n)):
    not_same = set(str(n)) - set(str(a))
    print ("n", n ,"은  12345와 쌍둥이가 아니며, 포함되지 않는 숫자들은", not_same, "입니다.")