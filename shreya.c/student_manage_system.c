#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

#define MAX_SUBJECTS 5
#define USER_FILE     "users.dat"
#define STUDENT_FILE  "students.dat"

#ifdef _WIN32
    #define CLEAR "cls"
#else
    #define CLEAR "clear"
#endif

static const char *SUBJECTS[MAX_SUBJECTS] = {"Maths", "Science", "English", "History", "Computer"};

typedef struct {
    int  id;
    char name[50];
    int  classNo;
    int  marks[MAX_SUBJECTS];
    int  total;
    float average;
    int  rank;
} Student;

typedef struct {
    int  id;
    char username[30];
    char password[30];
    char role;
} User;

static void getMaskedPassword(char *pw) {
    int idx = 0;
    char ch;
    while (1) {
        ch = getch();
        if (ch == '\r') break;
        if (ch == '\b' && idx > 0) {
            printf("\b \b");
            idx--;
            continue;
        }
        if (idx < 29 && ch >= 32 && ch <= 126) {
            pw[idx++] = ch;
            printf("*");
        }
    }
    pw[idx] = '\0';
    puts("");
}

static void pause() {
    puts("\nPress any key to continue...");
    getch();
}

static void flushInput() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

static FILE *openFile(const char *path, const char *mode) {
    FILE *fp = fopen(path, mode);
    if (!fp) {
        perror(path);
        exit(EXIT_FAILURE);
    }
    return fp;
}

static int countStudents() {
    FILE *fp = openFile(STUDENT_FILE, "rb");
    fseek(fp, 0, SEEK_END);
    long bytes = ftell(fp);
    fclose(fp);
    return (int)(bytes / sizeof(Student));
}

static Student *loadStudents(int *n) {
    *n = countStudents();
    if (*n == 0) return NULL;
    Student *arr = malloc(sizeof(Student) * (*n));
    if (!arr) exit(EXIT_FAILURE);
    FILE *fp = openFile(STUDENT_FILE, "rb");
    fread(arr, sizeof(Student), *n, fp);
    fclose(fp);
    return arr;
}

static void saveStudents(Student *arr, int n) {
    FILE *fp = openFile(STUDENT_FILE, "wb");
    fwrite(arr, sizeof(Student), n, fp);
    fclose(fp);
}

static void updateRanks() {
    int n;
    Student *arr = loadStudents(&n);
    if (!arr) return;
    for (int i = 0; i < n-1; ++i)
        for (int j = i+1; j < n; ++j)
            if (arr[j].total > arr[i].total) {
                Student tmp = arr[i]; arr[i] = arr[j]; arr[j] = tmp;
            }
    for (int i = 0; i < n; ++i) arr[i].rank = i + 1;
    saveStudents(arr, n);
    free(arr);
}

static User *findUser(const char *uname, const char *pw) {
    FILE *fp = openFile(USER_FILE, "rb");
    User *u = malloc(sizeof(User));
    while (fread(u, sizeof(User), 1, fp)) {
        if (strcmp(u->username, uname) == 0 && strcmp(u->password, pw) == 0) {
            fclose(fp);
            return u;
        }
    }
    fclose(fp);
    free(u);
    return NULL;
}

static void changePassword(User *user) {
    char oldPw[30], newPw[30], confirmPw[30];
    puts("\n-- Change Password --");
    printf("Enter old password: ");
    getMaskedPassword(oldPw);
    if (strcmp(oldPw, user->password) != 0) {
        puts("Incorrect old password!");
        pause();
        return;
    }
    printf("Enter new password: ");
    getMaskedPassword(newPw);
    printf("Confirm new password: ");
    getMaskedPassword(confirmPw);
    if (strcmp(newPw, confirmPw) != 0) {
        puts("Passwords do not match!");
        pause();
        return;
    }
    strcpy(user->password, newPw);
    FILE *fp = openFile(USER_FILE, "rb+");
    User tmp;
    while (fread(&tmp, sizeof(User), 1, fp)) {
        if (tmp.id == user->id && tmp.role == user->role) {
            fseek(fp, -(long)sizeof(User), SEEK_CUR);
            fwrite(user, sizeof(User), 1, fp);
            break;
        }
    }
    fclose(fp);
    puts("Password changed successfully!");
    pause();
}

static void addStudent() {
    Student s = {0};
    int n = countStudents();
    s.id = n + 1;
    printf("\n-- Add Student --\n");
    printf("Name        : "); fgets(s.name, sizeof(s.name), stdin); s.name[strcspn(s.name, "\n")] = 0;
    printf("Class (int) : "); scanf("%d", &s.classNo); flushInput();
    for (int i = 0; i < MAX_SUBJECTS; ++i) {
        printf("%s marks : ", SUBJECTS[i]);
        scanf("%d", &s.marks[i]); flushInput();
        s.total += s.marks[i];
    }
    s.average = s.total / (float)MAX_SUBJECTS;
    FILE *fp = openFile(STUDENT_FILE, "ab");
    fwrite(&s, sizeof(Student), 1, fp);
    fclose(fp);

    User u = {0};
    u.id = s.id;
    sprintf(u.username, "S%03d", s.id);
    strcpy(u.password, "student");
    u.role = 'S';
    fp = openFile(USER_FILE, "ab");
    fwrite(&u, sizeof(User), 1, fp);
    fclose(fp);
    printf("Student added! Default login => username: S%03d, password: student\n", s.id);
    updateRanks();
    pause();
}

static void listStudents() {
    int n;
    Student *arr = loadStudents(&n);
    if (!arr) {
        puts("\nNo student records found.");
        pause();
        return;
    }
    puts("\nID  Name                      Class  Total  Avg   Rank");
    puts("-------------------------------------------------------");
    for (int i = 0; i < n; ++i) {
        printf("%-3d %-25s %-6d %-6d %-5.1f %-4d\n",
               arr[i].id, arr[i].name, arr[i].classNo, arr[i].total, arr[i].average, arr[i].rank);
    }
    free(arr);
    pause();
}

static void adminPanel(User *admin) {
    int choice;
    while (1) {
        system(CLEAR);
        printf("\n==== ADMIN PANEL (Welcome, %s) ====\n", admin->username);
        printf("1. Add Student\n2. View All Students\n3. Change Password\n4. Logout\nEnter choice: ");
        if (scanf("%d", &choice) != 1) { flushInput(); continue; }
        flushInput();
        switch (choice) {
            case 1: addStudent(); break;
            case 2: listStudents(); break;
            case 3: changePassword(admin); break;
            case 4: return;
            default: break;
        }
    }
}

static Student *findStudentByID(int id) {
    FILE *fp = openFile(STUDENT_FILE, "rb");
    Student *s = malloc(sizeof(Student));
    while (fread(s, sizeof(Student), 1, fp)) {
        if (s->id == id) { fclose(fp); return s; }
    }
    fclose(fp);
    free(s);
    return NULL;
}

static void showBarChart(const Student *s) {
    puts("\n--- Performance Bar Chart ---");
    for (int i = 0; i < MAX_SUBJECTS; ++i) {
        printf("%-10s | ", SUBJECTS[i]);
        int bars = s->marks[i] / 2;
        for (int b = 0; b < bars; ++b) putchar('#');
        printf(" (%d)\n", s->marks[i]);
    }
}

static void studentPanel(User *stuUser) {
    Student *me = findStudentByID(stuUser->id);
    if (!me) { puts("Student record not found!"); pause(); return; }
    int choice;
    while (1) {
        system(CLEAR);
        printf("\n==== STUDENT PANEL (ID %d) ====\n", me->id);
        printf("1. View Report Card\n2. View Bar Chart\n3. Change Password\n4. Logout\nEnter choice: ");
        if (scanf("%d", &choice) != 1) { flushInput(); continue; }
        flushInput();
        switch (choice) {
            case 1:
                puts("\nSubject      Marks\n----------------------");
                for (int i = 0; i < MAX_SUBJECTS; ++i)
                    printf("%-12s %d\n", SUBJECTS[i], me->marks[i]);
                printf("----------------------\nTotal: %d\nAverage: %.1f\nRank: %d\n", me->total, me->average, me->rank);
                pause();
                break;
            case 2: showBarChart(me); pause(); break;
            case 3: changePassword(stuUser); break;
            case 4: free(me); return;
            default: break;
        }
    }
}

static void ensureMasterAdmin() {
    FILE *fp = fopen(USER_FILE, "rb");
    if (!fp) {
        User admin = {.id = 0, .role = 'A'};
        strcpy(admin.username, "admin");
        strcpy(admin.password, "admin123");
        fp = openFile(USER_FILE, "wb");
        fwrite(&admin, sizeof(User), 1, fp);
        fclose(fp);
    } else {
        fclose(fp);
    }
    fp = fopen(STUDENT_FILE, "ab");
    if (fp) fclose(fp);
}

int main() {
    ensureMasterAdmin();
    while (1) {
        system(CLEAR);
        puts("===== STUDENT RECORD MANAGEMENT SYSTEM =====\n");
        puts("1. Admin Login\n2. Student Login\n3. Exit");
        printf("Enter choice: ");
        int option;
        if (scanf("%d", &option) != 1) { flushInput(); continue; }
        flushInput();
        if (option == 3) {
            puts("Goodbye!");
            break;
        }
        char uname[30], pw[30];
        printf("Username: "); fgets(uname, sizeof(uname), stdin); uname[strcspn(uname, "\n")] = 0;
        printf("Password: "); getMaskedPassword(pw);
        User *u = findUser(uname, pw);
        if (!u) { puts("Invalid credentials!"); pause(); continue; }
        if (option == 1 && u->role != 'A') { puts("Not an admin account!"); pause(); free(u); continue; }
        if (option == 2 && u->role != 'S') { puts("Not a student account!"); pause(); free(u); continue; }
        if (u->role == 'A') adminPanel(u);
        else studentPanel(u);
        free(u);
    }
    return 0;
}