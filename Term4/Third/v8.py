from datetime import *

class Employee:
    last_name = ""
    birth_date = date.today()
    salary = 0

    def __init__(self, last_name, birth_date, salary):
        self.last_name = last_name
        self.birth_date = birth_date
        self.salary = salary

    def get_age(self):
        return (date.today() - self.birth_date).days // 365

    def days_to_retire(self):
        return 50 * 365 - (date.today() - self.birth_date).days

    def __repr__(self): 
        return f"Employee '{self.last_name}' {self.get_age()} years old with salary {self.salary}. Will be retired after {self.days_to_retire()} days"

chel1 = Employee("1", date(1, 2, 3), 1000)
chel2 = Employee("2", date(1000, 4, 5), 2000)
chel3 = Employee(input("Enter name: "), date(int(input("Enter birth year: ")),int(input("Enter birth month: ")) ,int(input("Enter birth day: ")) ), int(input("Enter salary: ")))

print(chel1)
print(chel2)
print(chel3)

