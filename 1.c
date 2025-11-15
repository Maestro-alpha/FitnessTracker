#include <stdio.h>
#include <string.h>

typedef struct date
{
    int date;
    char month[10];
    int year;
} date;

typedef struct exercise
{
    char exercise[20];
    int duration;
    int set;
} exc;

void readingDate(int n, date today[n])
{
    char month[10];
    printf("Enter the date: ");
    scanf("%d", &today[n].date);
    printf("Enter the month: ");
    scanf(" %[^\n]", &month);
    strcpy(today[n].month, month);
    printf("Enter the year: ");
    scanf("%d", &today[n].year);
}

void exercise(int n, exc exer[n])
{
    char exeName[20];
    for (int i = 0; i < n; i++)
    {
        printf("Enter the name of the %d execise: ", i);
        scanf(" %[^\n]", exeName);
        strcpy(exer[i].exercise, exeName);
        printf("Enter the duration: ");
        scanf("%d", &exer[i].duration);
        printf("Enter the no. of sets performed: ");
        scanf("%d\n", &exer[i].set);
    }
}

int main()
{
    int option, noOfExe, input;

    printf("Enter the no. of inputs you wish to have: ");
    scanf("%d", &input);
    date *today[input];
    exc *exer[1];

    printf("\nWelcome to the Fitness tracker.\nThis is the welcome interface of the system,\n(PLEASE NOTE, the system works for %d repetitions on one run.)\n\n\n", input);

    for (int i = 0; i < input; i++)
    {
        printf("Enter today's date: \n\n");
        readingDate(input, today[input]);
        printf("The main menu is as follows:\n1. Start the entry of a new exercise.\n2. Review a previous entry.\n3. Enter today's calorie intake.\n4. Exit the system.\n\nEnter your choice: ");
        scanf("%d", &option);
        switch (option)
        {

        case 1:
            printf("How many exercises did you do today: ");
            scanf("%d", &noOfExe);
            exercise(noOfExe, exer[noOfExe]);
            printf("Your input has been recored...Thank you!!!");
            break;

        case 2:
            printf("Enter the date to review: \n\n");
            break;

        case 3:
            printf("Enter today's intake: \n\n");
            break;

        case 4:
            printf("Exiting the system: \n\n");
            break;

        default:
            printf("Enter a valid number...\n\n");
            break;
        }
    }
    return 0;
}