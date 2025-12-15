#include <stdio.h>

#define MAX_NAME 40
#define MAX_EXERCISES 20

typedef struct
{
    char name[MAX_NAME];
    int intensity;
    int minutes;
    float met;
} Exercise;

typedef struct
{
    int day;
    int month;
    int year;
    int calorieIntake;
    int count;
    Exercise exercises[MAX_EXERCISES];
} DayEntry;

int str_len(const char *s)
{
    int n = 0;
    while (s[n] != '\0' && s[n] != '\n')
        n++;
    return n;
}

char to_lower_char(char c)
{
    if (c >= 'A' && c <= 'Z')
        return (char)(c - 'A' + 'a');
    return c;
}

int str_equal_ignore_case(const char *a, const char *b)
{
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0')
    {
        if (to_lower_char(a[i]) != to_lower_char(b[i]))
            return 0;
        i++;
    }
    return a[i] == '\0' && b[i] == '\0';
}

void str_copy(char *dst, const char *src)
{
    int i = 0;
    while (src[i] != '\0' && src[i] != '\n' && i < MAX_NAME - 1)
    {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';
}

float lookup_met(const char *name, int intensity)
{
    FILE *f = fopen("met_values.txt", "r");
    if (!f)
        return 0.0f;

    char line[128];
    char key[MAX_NAME];
    float low, mid, high;
    float result = 0.0f;

    while (fgets(line, sizeof(line), f) != NULL)
    {
        if (sscanf(line, "%39s %f %f %f", key, &low, &mid, &high) == 4)
        {
            if (str_equal_ignore_case(key, name))
            {
                if (intensity == 1)
                    result = low;
                else if (intensity == 2)
                    result = mid;
                else
                    result = high;
                break;
            }
        }
    }

    fclose(f);
    return result;
}

void append_met_entry(const char *name, float low, float mid, float high)
{
    FILE *f = fopen("met_values.txt", "a");
    if (!f)
    {
        printf("Unable to open met_values.txt for writing.\n");
        return;
    }
    fprintf(f, "%s %.2f %.2f %.2f\n", name, low, mid, high);
    fclose(f);
}

void save_entry(const DayEntry *entry)
{
    FILE *f = fopen("exercise_records.txt", "a");
    if (!f)
    {
        printf("Unable to open exercise_records.txt.\n");
        return;
    }

    fprintf(f, "DATE %02d %02d %04d CAL %d COUNT %d\n",
            entry->day, entry->month, entry->year,
            entry->calorieIntake, entry->count);

    for (int i = 0; i < entry->count; i++)
    {
        const Exercise *ex = &entry->exercises[i];
        fprintf(f, "EX %s INT %d MIN %d MET %.2f\n",
                ex->name, ex->intensity, ex->minutes, ex->met);
    }

    fprintf(f, "END\n");
    fclose(f);
}

void show_all_records(void)
{
    FILE *f = fopen("exercise_records.txt", "r");
    if (!f)
    {
        printf("No record file yet.\n");
        return;
    }

    char line[160];
    while (fgets(line, sizeof(line), f) != NULL)
        fputs(line, stdout);

    fclose(f);
}

void show_specific_day(void)
{
    int targetDay, targetMonth, targetYear;
    printf("Enter day month year: ");
    if (scanf("%d %d %d", &targetDay, &targetMonth, &targetYear) != 3)
    {
        printf("Invalid date input.\n");
        return;
    }

    FILE *f = fopen("exercise_records.txt", "r");
    if (!f)
    {
        printf("No records found.\n");
        return;
    }

    char line[160];
    int match = 0;
    while (fgets(line, sizeof(line), f) != NULL)
    {
        int d, m, y, cal, count;
        if (sscanf(line, "DATE %d %d %d CAL %d COUNT %d", &d, &m, &y, &cal, &count) == 5)
        {
            if (d == targetDay && m == targetMonth && y == targetYear)
            {
                match = 1;
                printf("%s", line);
                for (int i = 0; i < count + 1; i++)
                {
                    if (fgets(line, sizeof(line), f) == NULL)
                        break;
                    printf("%s", line);
                }
            }
        }
    }

    if (!match)
        printf("No entry for that date.\n");

    fclose(f);
}

void read_word(char *buffer, int size)
{
    int ch;
    int i = 0;
    while ((ch = getchar()) == ' ' || ch == '\n')
        ;
    while (ch != EOF && ch != ' ' && ch != '\n' && i < size - 1)
    {
        buffer[i++] = (char)ch;
        ch = getchar();
    }
    buffer[i] = '\0';
}

void add_new_day(void)
{
    DayEntry entry;
    entry.count = 0;

    printf("Enter date (day month year): ");
    if (scanf("%d %d %d", &entry.day, &entry.month, &entry.year) != 3)
    {
        printf("Bad date input.\n");
        return;
    }

    printf("Enter calorie intake (kcal): ");
    if (scanf("%d", &entry.calorieIntake) != 1)
        entry.calorieIntake = 0;

    printf("How many exercises (max %d): ", MAX_EXERCISES);
    if (scanf("%d", &entry.count) != 1 || entry.count < 0)
        entry.count = 0;
    if (entry.count > MAX_EXERCISES)
        entry.count = MAX_EXERCISES;

    for (int i = 0; i < entry.count; i++)
    {
        Exercise *ex = &entry.exercises[i];
        printf("\nExercise %d name (no spaces): ", i + 1);
        read_word(ex->name, MAX_NAME);

        printf("Intensity 1/2/3: ");
        if (scanf("%d", &ex->intensity) != 1)
            ex->intensity = 1;
        if (ex->intensity < 1)
            ex->intensity = 1;
        if (ex->intensity > 3)
            ex->intensity = 3;

        printf("Duration in minutes: ");
        if (scanf("%d", &ex->minutes) != 1)
            ex->minutes = 0;

        ex->met = lookup_met(ex->name, ex->intensity);
        if (ex->met <= 0.0f)
        {
            printf("MET not found. Enter MET value: ");
            if (scanf("%f", &ex->met) != 1)
                ex->met = 1.0f;

            printf("Save this MET to met_values.txt? (y/n): ");
            char ans;
            scanf(" %c", &ans);
            if (ans == 'y' || ans == 'Y')
            {
                float low = ex->met;
                float mid = ex->met;
                float high = ex->met;
                if (ex->intensity == 1)
                    low = ex->met;
                else if (ex->intensity == 2)
                    mid = ex->met;
                else
                    high = ex->met;
                append_met_entry(ex->name, low, mid, high);
            }
        }
    }

    save_entry(&entry);
    printf("Entry saved to exercise_records.txt.\n");
}

void add_met_value(void)
{
    char name[MAX_NAME];
    float low, mid, high;

    printf("Exercise name (no spaces): ");
    read_word(name, MAX_NAME);

    printf("MET for low / moderate / high intensity: ");
    if (scanf("%f %f %f", &low, &mid, &high) != 3)
    {
        printf("Invalid input.\n");
        return;
    }

    append_met_entry(name, low, mid, high);
    printf("Saved to met_values.txt.\n");
}

void view_met_values(void)
{
    FILE *f = fopen("met_values.txt", "r");
    if (!f)
    {
        printf("met_values.txt not found.\n");
        return;
    }

    char line[128];
    printf("\nKnown MET entries:\n");
    while (fgets(line, sizeof(line), f) != NULL)
        fputs(line, stdout);

    fclose(f);
}

void quick_summary(void)
{
    FILE *f = fopen("exercise_records.txt", "r");
    if (!f)
    {
        printf("No records to summarize.\n");
        return;
    }

    char line[160];
    int days = 0;
    int totalCalories = 0;
    float totalMetMinutes = 0.0f;

    while (fgets(line, sizeof(line), f) != NULL)
    {
        int d, m, y, cal, count;
        if (sscanf(line, "DATE %d %d %d CAL %d COUNT %d", &d, &m, &y, &cal, &count) == 5)
        {
            days++;
            totalCalories += cal;
            for (int i = 0; i < count; i++)
            {
                if (fgets(line, sizeof(line), f) == NULL)
                    break;
                char name[MAX_NAME];
                int intensity, minutes;
                float met;
                if (sscanf(line, "EX %39s INT %d MIN %d MET %f", name, &intensity, &minutes, &met) == 4)
                {
                    totalMetMinutes += met * (float)minutes;
                }
            }
        }
    }

    fclose(f);

    if (days == 0)
    {
        printf("No completed day entries.\n");
        return;
    }

    printf("Days recorded: %d\n", days);
    printf("Average calories: %.1f kcal/day\n", (float)totalCalories / days);
    printf("Total MET*minutes: %.1f\n", totalMetMinutes);
}

int main()
{
    int choice;

    printf("Simple Fitness Log (stdio-only version)\n");
    printf("Make sure met_values.txt and exercise_records.txt exist in the same folder.\n");

    while (1)
    {
        printf("\n1. Add exercise day\n");
        printf("2. View all records\n");
        printf("3. Search record by date\n");
        printf("4. Quick summary\n");
        printf("5. View MET table\n");
        printf("6. Add MET entry\n");
        printf("7. Exit\n");
        printf("Choice: ");

        if (scanf("%d", &choice) != 1)
        {
            printf("Invalid input.\n");
            return 0;
        }

        if (choice == 1)
            add_new_day();
        else if (choice == 2)
            show_all_records();
        else if (choice == 3)
            show_specific_day();
        else if (choice == 4)
            quick_summary();
        else if (choice == 5)
            view_met_values();
        else if (choice == 6)
            add_met_value();
        else if (choice == 7)
            break;
        else
            printf("Please choose 1-7.\n");
    }

    printf("Goodbye.\n\n");
    printf("Project submitted by:-\n");
    printf("Chirag Kumar(2501030198)\n");
    printf("Diya Agarwal(2501030195)\n");
    printf("Tejas Jain(2501030209)\n");
    printf("Yana Bundel(2501030190)\n");
    return 0;
}