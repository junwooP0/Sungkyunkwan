import json
import random
import datetime


class User:
    def __init__(self, username, password, pin):
        self.username = username
        self.password = password
        self.pin = pin
        random_number = random.randint(0, 99999) 
        self.account_number = str(random_number)  
        while len(self.account_number) < 5: 
            self.account_number = "0" + self.account_number
        self.balance = 100
        self.transactions = []

    def to_dict(self):
        return {
            "Username": self.username,
            "Password": self.password,
            "PIN": self.pin,
            "Account number": self.account_number,
            "Current balance": self.balance,
            "Transactions": self.transactions,
        }

class BankingSystem:
    def __init__(self):
        self.file = "information.json" 
        self.users = {}
        self.load_data()

    def load_data(self):
        try:
            with open(self.file, "r") as file:
                data = json.load(file)
                self.users = data
        except FileNotFoundError:
            self.users = {}
            self.save_data()

    def save_data(self):
        with open(self.file, "w") as file:
            json.dump(self.users, file, indent=4)

    def register(self):
        print("\n---------- Register ----------")
        print("Please enter your information below.(Username, Password, PIN)")
        username = input("Enter username: ")
        if username in self.users:
            print("Error: Username already exists.")
            return

        while True:
            password = input("\nEnter password: ")
            is_valid = True

            if len(password) <= 7:
                print("Password needs to be over 7 letters.")
                is_valid = False

            if not any(char.isupper() for char in password):
                print("Password must contain at least one capital letter.")
                is_valid = False

            special_characters = "!@#$%^&*()"
            if not any(char in special_characters for char in password):
                print("Password must contain at least one special character.")
                is_valid = False

            if is_valid:
                break  
            
        while True: 
            pin = input("\nEnter a 4-digit PIN: ")
            is_valid = True
            if not (pin.isdigit() and len(pin) == 4):
                print("Error: PIN must be a 4-digit number.")
                is_valid = False
                
            if is_valid:
                break


        new_user = User(username, password, pin)
        self.users[username] = new_user.to_dict()
        self.save_data()
        print(f"Registered successfully! Your Account Number is {new_user.account_number}")

        
    def login(self):
        print("\n---------- Login ----------")
        username = input("Enter username: ")
        password = input("Enter login password: ")

        user = self.users.get(username)
        if user and user["Password"] == password:
            print(f"Logged in successfully")
            self.main_menu(user)
        else:
            print("Error: Incorrect username or password.")
            
    def main_menu(self, user):
        while True:
            print("\n---------- Main Menu ----------")
            print("---------- Account Information ----------")
            print(f"Username: {user['Username']}")
            print(f"Account Number: {user['Account number']}")
            print(f"Current balance: ${user['Current balance']}")
            print("\n <Options>")
            print("1. History")
            print("2. Withdraw")
            print("3. Deposit")
            print("4. Transfer")
            print("5. Logout")
            option = input("Choose a number of option to proceed: ")

            if option == "1":
                self.history(user)
            elif option == "2":
                self.withdraw(user)
            elif option == "3":
                self.deposit(user)
            elif option == "4":
                self.transfer(user)
            elif option == "5":
                print("Logging out...")
                return
            else:
                print("Error: Please enter a valid number.")
            
            
    def history(self, user):
        print("\n---------- Transaction History ----------")
        transactions = user["Transactions"]
        
        if not transactions:
            print("No history found.")
            return
        
        for transaction in reversed(transactions):
            print("Time: {}".format(transaction['time']))
            print("Transaction Type: {}".format(transaction['type']))
            print("{}".format(transaction['detail']))
            print()
    
    def withdraw(self, user):
        print("\n---------- Withdraw ----------")
        print(f"Username: {user['Username']}")
        print(f"Current Balance: ${user['Current balance']}")
        print(f"Account Number: {user['Account number']}")

        amount_input = input("Enter the amount of money to withdraw: ")
        if not amount_input.isdigit():
            print("Error: Invalid input. Please enter a valid number.")
            return

        amount = int(amount_input)
        if amount <= 0:
            print("Error: Amount must be greater than 0.")
            return

        pin = input("Enter your 4-digit PIN: ")
        if pin != user["PIN"]:
            print("Error: Incorrect PIN.")
            return

        if user["Current balance"] < amount:
            print("Error: Amount exceeds current balance.")
            return

        user["Current balance"] -= amount
        transaction = {
            "time": datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
            "type": "Withdraw",
            "detail": f"${amount} has been withdrawn"
        }
        user["Transactions"].append(transaction)
        self.save_data()
        print(f"Successfully withdrawn.")

            
    def deposit(self, user):
        print("\n---------- Deposit ----------")
        print(f"Username: {user['Username']}")
        print(f"Current Balance: ${user['Current balance']}")
        print(f"Account Number: {user['Account number']}")

        amount_input = input("Enter the amount of money to deposit: ")
        if not amount_input.isdigit():
            print("Error: Invalid input. Please enter a valid number.")
            return

        amount = int(amount_input)
        if amount <= 0:
            print("Error: Amount must be greater than 0.")
            return

        pin = input("Enter your 4-digit PIN: ")
        if pin != user["PIN"]:
            print("Error: Incorrect PIN.")
            return

        user["Current balance"] += amount
        transaction = {
            "time": datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
            "type": "Deposit",
            "detail": f"${amount} has been deposited"
        }
        user["Transactions"].append(transaction)
        self.save_data()
        print("Successfully deposited.")
            
                    
    def transfer(self, user):
        print("\n---------- Transfer ----------")
        print(f"Username: {user['Username']}")
        print(f"Current Balance: ${user['Current balance']}")
        print(f"Account Number: {user['Account number']}")

        account_number = input("Enter the recipient's account number: ")

        recipients = [
            other_user for other_user in self.users.values() if other_user.get("Account number") == account_number
        ]

        if not recipients:
            print("Error: Account number not found.")
            return

        recipient = recipients[0]

        amount_input = input("Enter the amount of money to transfer: ")
        if not amount_input.isdigit():
            print("Error: Invalid input. Please enter a valid number.")
            return

        amount = int(amount_input)
        if amount <= 0:
            print("Error: Amount must be greater than 0.")
            return

        pin = input("Enter your 4-digit PIN: ")
        if pin != user["PIN"]:
            print("Error: Incorrect PIN.")
            return

        if user["Current balance"] < amount:
            print("Error: Amount exceeds current balance.")
            return

        user["Current balance"] -= amount
        recipient["Current balance"] += amount

        money_sender = {
            "time": datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
            "type": "Transfer",
            "detail": f"${amount} has been transferred (From: {user['Username']} | To: {recipient['Username']})"
        }
        user["Transactions"].append(money_sender)

        money_recipient = {
            "time": datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
            "type": "Transfer",
            "detail": f"${amount} has been transferred (From: {user['Username']} | To: {recipient['Username']})"
        }
        recipient["Transactions"].append(money_recipient)

        self.save_data()
        print(f"Successfully transferred.")


if __name__ == "__main__":
    bank_system = BankingSystem()
    while True:
        print("\n---------- Welcome to the Banking System ----------")
        print("1. Register")
        print("2. Login")
        print("3. Exit")
        choice = input("Choose a number: ")
        if choice == "1":
            bank_system.register()
        elif choice == "2":
            bank_system.login()
        elif choice == "3":
            print("Goodbye!")
            break
        else:
            print("Error: Please enter a valid number.")
