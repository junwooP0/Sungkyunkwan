import random

score_category = {
    "Aces": None, "Twos": None, "Threes": None, "Fours": None, "Fives": None, "Sixes": None, "Chance": None,
    "Three Of A Kind": None, "Four Of A Kind": None, "Full House": None,
    "Small Straight": None, "Large Straight": None, "Yahtzee": None
}


def Aces(dice):
    score = 0
    for i in dice:
        if i == 1:
            score += 1
    return score


def Twos(dice):
    score = 0
    for i in dice:
        if i == 2:
            score += 2
    return score


def Threes(dice):
    score = 0
    for i in dice:
        if i == 3:
            score += 3
    return score


def Fours(dice):
    score = 0
    for i in dice:
        if i == 4:
            score += 4
    return score


def Fives(dice):
    score = 0
    for i in dice:
        if i == 5:
            score += 5
    return score


def Sixes(dice):
    score = 0
    for i in dice:
        if i == 6:
            score += 6
    return score


def Chance(dice):
    return sum(dice)


def Three_Of_A_Kind(dice):
    counts = {}
    for x in dice:
        if x in counts:
            counts[x] += 1
        else:
            counts[x] = 1

    for counts in counts.values():
        if counts >= 3:
            return sum(dice)

    return 0


def Four_Of_A_Kind(dice):
    counts = {}
    for x in dice:
        if x in counts:
            counts[x] += 1
        else:
            counts[x] = 1

    for counts in counts.values():
        if counts >= 4:
            return sum(dice)

    return 0


def Full_House(dice):
    counts = {}

    for x in dice:
        if x in counts:
            counts[x] += 1
        else:
            counts[x] = 1

    count_value = list(counts.values())

    if 3 in count_value and 2 in count_value:
        return 25
    else:
        return 0


def Small_Straight(dice):
    dice = sorted(set(dice))
    if (1 in dice and 2 in dice and 3 in dice and 4 in dice) or (2 in dice and 3 in dice and 4 in dice and 5 in dice) or (3 in dice and 4 in dice and 5 in dice and 6 in dice):
        return 30
    else:
        return 0


def Large_Straight(dice):
    dice = sorted(set(dice))
    if (1 in dice and 2 in dice and 3 in dice and 4 in dice and 5 in dice) or (2 in dice and 3 in dice and 4 in dice and 5 in dice and 6 in dice):
        return 40
    else:
        return 0


def Yahtzee(dice):
    if len(set(dice)) == 1:
        return 50
    else:
        return 0


def expected_score(dice):
    print("\n <Possible Scores for Current Roll>")
    print(f"1. Aces                            {Aces(dice) if score_category['Aces'] is None else '✔'}")
    print(f"2. Twos                            {Twos(dice) if score_category['Twos'] is None else '✔'}")
    print(f"3. Threes                          {Threes(dice) if score_category['Threes'] is None else '✔'}")
    print(f"4. Fours                           {Fours(dice) if score_category['Fours'] is None else '✔'}")
    print(f"5. Fives                           {Fives(dice) if score_category['Fives'] is None else '✔'}")
    print(f"6. Sixes                           {Sixes(dice) if score_category['Sixes'] is None else '✔'}")
    print(f"7. Chance                          {Chance(dice) if score_category['Chance'] is None else '✔'}")
    print(f"8. Three Of A Kind                 {Three_Of_A_Kind(dice) if score_category['Three Of A Kind'] is None else '✔'}")
    print(f"9. Four Of A Kind                  {Four_Of_A_Kind(dice) if score_category['Four Of A Kind'] is None else '✔'}")
    print(f"10. Full House                     {Full_House(dice) if score_category['Full House'] is None else '✔'}")
    print(f"11. Small Straight                 {Small_Straight(dice) if score_category['Small Straight'] is None else '✔'}")
    print(f"12. Large Straight                 {Large_Straight(dice) if score_category['Large Straight'] is None else '✔'}")
    print(f"13. Yahtzee                        {Yahtzee(dice) if score_category['Yahtzee'] is None else '✔'}")

def record_score(dice):
    category = input("\nWhich category would you like to score? (Enter number 1-13) : ")
    
    if category == '1':
        if score_category["Aces"] is None:
            score_category["Aces"] = Aces(dice)
        else:
            print("This category already has a score. Please choose another category.")
            record_score(dice)
    elif category == '2':
        if score_category["Twos"] is None:
            score_category["Twos"] = Twos(dice)
        else:
            print("This category already has a score. Please choose another category.")
            record_score(dice)
    elif category == '3':
        if score_category["Threes"] is None:
            score_category["Threes"] = Threes(dice)
        else:
            print("This category already has a score. Please choose another category.")
            record_score(dice)
    elif category == '4':
        if score_category["Fours"] is None:
            score_category["Fours"] = Fours(dice)
        else:
            print("This category already has a score. Please choose another category.")
            record_score(dice)
    elif category == '5':
        if score_category["Fives"] is None:
            score_category["Fives"] = Fives(dice)
        else:
            print("This category already has a score. Please choose another category.")
            record_score(dice)
    elif category == '6':
        if score_category["Sixes"] is None:
            score_category["Sixes"] = Sixes(dice)
        else:
            print("This category already has a score. Please choose another category.")
            record_score(dice)
    elif category == '7':
        if score_category["Chance"] is None:
            score_category["Chance"] = Chance(dice)
        else:
            print("This category already has a score. Please choose another category.")
            record_score(dice)
    elif category == '8':
        if score_category["Three Of A Kind"] is None:
            score_category["Three Of A Kind"] = Three_Of_A_Kind(dice)
        else:
            print("This category already has a score. Please choose another category.")
            record_score(dice)    
    elif category == '9':
        if score_category["Four Of A Kind"] is None:
            score_category["Four Of A Kind"] = Four_Of_A_Kind(dice)
        else:
            print("This category already has a score. Please choose another category.")
            record_score(dice)
    elif category == '10':
        if score_category["Full House"] is None:
            score_category["Full House"] = Full_House(dice)
        else:
            print("This category already has a score. Please choose another category.")
            record_score(dice)
    elif category == '11':
        if score_category["Small Straight"] is None:
            score_category["Small Straight"] = Small_Straight(dice)
        else:
            print("This category already has a score. Please choose another category.")
            record_score(dice)
    elif category == '12':
        if score_category["Large Straight"] is None:
            score_category["Large Straight"] = Large_Straight(dice)
        else:
            print("This category already has a score. Please choose another category.")
            record_score(dice)
    elif category == '13':
        if score_category["Yahtzee"] is None:
            score_category["Yahtzee"] = Yahtzee(dice)
        else:
            print("This category already has a score. Please choose another category.")
            record_score(dice)
    else:
        print("Sorry, You have to write the valid input. Please select the number between 1-13")
        record_score(dice)
        
        
def current_score():
    print("Category                   Score")
    print('-' * 33)
    for category, score in score_category.items():
        print(f"{category:<20} {score if score is not None else '-':>10}")

def roll():
    dice = []
    for x in range(5):
        dice.append(random.randint(1, 6))
    return dice


def reroll(dice, index):
    for x in range(5):
        if x not in index:
            dice[x] = random.randint(1, 6)
    return dice



def main():
    round = 1
    appear = False
    
    print("Welcome! Let's play Yahtzee!")
    
    while round <= 13:
        if not appear:
            print(f"\n[Round {round}]")
            appear = True
        print("\nChoose an option")
        print("1. Roll")
        print("2. See Score Sheet")
        choice = input("\nEnter 1 or 2: ")

        if choice == '1':
            dice = roll()
            print(f"\nFirst Roll - Dice: {dice}")
            expected_score(dice)
            recoll_count = 0
            while recoll_count < 2:
                print(f"\nDo you want to reroll any dice? {
                    2 - recoll_count} rolls left")
                print("1. Yes")
                print("2. No (Record Score)")
                reroll_choice = input("\nEnter 1 or 2: ")
                if reroll_choice == '1':
                    while True:
                        reroll_index = input("\nWhich index of dice do you want to keep when you reroll? (Enter number 1-5) (ex. 1 4 5) : ")
                        reroll_index = reroll_index.split()


                        if all(i.isdigit() and 1 <= int(i) <= 5 for i in reroll_index) and len(reroll_index) <= 5:
                            reroll_index = list(set(int(i) - 1 for i in reroll_index))
                            break           
                        else:
                            print("Sorry, You have to write the valid input. Please try again")
                    dice = reroll(dice, reroll_index)
                    recoll_count += 1
                    print(f"\nReroll - Dice: {dice}")
                    expected_score(dice)
                elif reroll_choice == '2':
                    record_score(dice)
                    break
                else:
                    print("Sorry, You have to write the valid input. Please select the number between 1-2")
                    continue
                
            if recoll_count == 2:
                print("\nYou have used all rerolls. Let's record your score.")
                record_score(dice)
            
                
            round += 1
            if round == 14:
                break
            
            print("\nNice! Your score has been updated. Let's move on to the next round.")
            appear = False
            

        elif choice == '2':
            print("\n          <Score Sheet>")
            current_score()

        else:
            print("Sorry, You have to write the valid input. Please select the number between 1-2")
            
    print("\nGame Over! Here is your final score sheet.")
    print("\n          <Final Score Sheet>")
    current_score()
        
    print(f"\nYour final score: {sum(score for score in score_category.values() if score is not None)}")
    print("Thank you for playing! Press any key to exit the game.")
    input()
    exit()
    
main()