#include <stdio.h>
#include <stdlib.h>

struct employee {
    char last_name[64];
    int birth_year;
    int dept_number;
    int salary;
};

void input_employee(struct employee *emp);

void print_employee(struct employee *emp);

void main() {
    struct employee employees[4];
    for (int i = 0; i < 4; i++)
        input_employee(employees + i);

    for (int i = 0; i < 4; i++)
        print_employee(employees + i);
}

void input_employee(struct employee *emp) {
    printf("Enter last name\n");
    scanf("%64s", emp->last_name);
    printf("Enter birth year\n");
    scanf("%d", &emp->birth_year);
    printf("Enter department number\n");
    scanf("%d", &emp->dept_number);
    printf("Enter salary\n");
    scanf("%d", &emp->salary);
}

void print_employee(struct employee *emp) {
    printf("\nLast name: %s\n", emp->last_name);
    printf("Birth year: %d\n", emp->birth_year);
    printf("Department number: %d\n", emp->dept_number);
    printf("Salary: %d\n", emp->salary);
}
