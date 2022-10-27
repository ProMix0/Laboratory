#include <stdio.h>
#include <stdlib.h>

struct employee {
    char last_name[64];
    int birth_year;
    int dept_number;
    int salary;
};

void input_employee(struct employee *emp);

void sort(struct employee *emps);

void print_employee(struct employee *emp);

void main() {
    struct employee *employees = malloc(sizeof(struct employee) * 4);
    for (int i = 0; i < 4; i++)
        input_employee(employees + i);

    sort(employees);

    for (int i = 0; i < 4; i++)
        print_employee(employees + i);

    free(employees);
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

void sort(struct employee *emps) {
    char swap = 1;
    while (swap) {
        swap = 0;

        for (int i = 0; i < 4 - 1; i++)
            if (emps[i].birth_year > emps[i + 1].birth_year) {
                struct employee temp = emps[i];
                emps[i] = emps[i + 1];
                emps[i + 1] = temp;

                swap = 1;
            }
    }
}

void print_employee(struct employee *emp) {
    printf("\nLast name: %s\n", emp->last_name);
    printf("Birth year: %d\n", emp->birth_year);
    printf("Department number: %d\n", emp->dept_number);
    printf("Salary: %d\n", emp->salary);
}