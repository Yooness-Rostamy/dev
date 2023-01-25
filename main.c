#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//*******************
//fgets and scanf problem on not flushing input properly, read from this sources about solution and problem:
//https://stackoverflow.com/questions/34219549/how-to-properly-flush-stdin-in-fgets-loop
//https://www.daniweb.com/programming/software-development/code/217396/how-to-properly-flush-the-input-stream-stdin
#define FLUSH_STDIN(x) {if(x[strlen(x)-1]!='\n'){do fgets(Junk,16,stdin);while(Junk[strlen(Junk)-1]!='\n');}else x[strlen(x)-1]='\0';}

char Junk[16]; // buffer for discarding excessive user input,
               // used by "FLUSH_STDIN" macro
//*******************


#define MAXINPUT 20

// Entities:
// Auth
// Students
// Instructors
// Courses
enum entities {AUTH, STUDENTS, INSTRUCTORS, COURSES};

const char *files[4];

void initGlobalVariables(void)
{
    files[AUTH] = "auth.txt";
    files[STUDENTS] = "students.txt";
    files[INSTRUCTORS] = "instructors.txt";
    files[COURSES] = "courses.txt";
}

// store user credentials in auth.txt
struct _user {
    int  userId;
    char passWord[MAXINPUT];
};

struct _student {
    int  studentId;
    char firstName[MAXINPUT];
    char lastName[MAXINPUT];
};

struct _instructor {
    int  instructorId;
    char firstName[MAXINPUT];
    char lastName[MAXINPUT];
    char title[MAXINPUT];
};

struct _course {
    int  courseId;
    char Name[MAXINPUT];
    int  Unit;
};

int checkFileExists(char *filename);
int isStringEmpty(const char *s);
int startupCheck(void);
int loadAllEntityElementsFromFile(void *buffer, size_t elementSize, int entityType);
char *safe_fgets(char *buffer, int size);
int strToUnsignedInt(const char *strUserId);
int authenticate(struct _user allusers[], int totalUsers);
int getMenuChoice(int digits, int maxnum);
int getString(const char *prompt, char *buffer, int bufferSize, int repeatOnError, int clsOnRepeat);
int getInteger(const char *prompt, int *input, int inputDigits, int inputMinValue, int inputMaxValue, int repeatOnError, int clsOnRepeat);
int writeAllEntityElementsToFile(void *buffer, int size, int count, int entityType);
int editEntityElementInFile(void *buffer, int size, int position, int entityType);
int searchForStudendById(struct _student allstudents[], int totalStudents, int studentId);
void zeroMemory(void *buffer, int size);
void studentManagement(struct _student allstudents[], int *totalStudents);

int main()
{
    initGlobalVariables();

    if(startupCheck() == 1)
        return 1;

    // userId 0 for admin
    // userId 1 - 100 for students
    // userId 101 - 200 for instructors
    // using = {} for zero initializing arrays
    struct _user allusers[200] = {};

    struct _student allstudents[100] = {};
    struct _instructor allinstructors[100] = {};
    struct _course allcourses[100] = {};


    int totalUsers       = loadAllEntityElementsFromFile(allusers, sizeof(struct _user), AUTH);
    int totalStudents    = loadAllEntityElementsFromFile(allstudents, sizeof(struct _student), STUDENTS);
    int totalInstructors = loadAllEntityElementsFromFile(allinstructors, sizeof(struct _instructor), INSTRUCTORS);
    int totalCourses     = loadAllEntityElementsFromFile(allcourses, sizeof(struct _course), COURSES);

    int userId = authenticate(allusers, totalUsers);

    system("cls");
    printf("Welcome!\n");

    int menuChoice = -1 ;
    do{
        printf("Enter your choice:\n"
               "1 - Student management\n"
               "2 - Instructor management\n"
               "3 - Course management\n"
               "4 - change password\n"
               "0 - Exit!\n"
               );
        menuChoice = getMenuChoice(1, 4);
        //char tempBuf[10] = {};
        switch(menuChoice)
        {
        case 0:
            return 0;
        case 1:
            studentManagement(allstudents, &totalStudents);
            break;
        case 4:
            {
                //menuChoice = -1;
                char newPassword[MAXINPUT] = {}, repeatedNewPassword[MAXINPUT] = {};

                system("cls");
                getString("Enter new password:", newPassword, sizeof(newPassword)/sizeof(newPassword[0]), 1, 1);
                getString("Repeat new password:", repeatedNewPassword, sizeof(repeatedNewPassword)/sizeof(repeatedNewPassword[0]), 1, 1);
                if(strcmp(newPassword, repeatedNewPassword) != 0)
                {
                    system("cls");
                    puts("Passwords not match!");
                    continue;
                }
                for(int i = 0 ; i < totalUsers ; i++)
                {
                    if(allusers[i].userId == userId)
                    {
                        strcpy(allusers[i].passWord , newPassword);
                        break;
                    }
                }

                FILE *writePtr;
                if((writePtr = fopen(files[AUTH], "wb"))==NULL){
                    printf("Error, cannot access %s file! exiting...\n", files[AUTH]);
                    return 1;
                }

                for(int i = 0 ; i < totalUsers ; i++)
                {
                    fwrite(&allusers[i], sizeof(struct _user), 1, writePtr);
                }
                fclose(writePtr);

                system("cls");
                puts("Password changed successfully!");
            }
            break;
        default:
            system("cls");
            break;
        }

    }while(1);

    system("pause");
    return 0;
}

// check if auth.txt file exists, if not create the file with default credentials
int startupCheck(void){

    for(int i = AUTH; i <= COURSES; i++)
    {
        if(checkFileExists(files[i]) == -1)
        {
            FILE *filePtr;
            // create auth.txt file and populate it with default username and password
            if((filePtr = fopen(files[i], "wb"))!=NULL){
                    if(i == AUTH)
                    {
                        struct _user admin;
                        // 0 is reserved for admin user
                        admin.userId = 0;
                        strcpy(admin.passWord, "1234");
                        fwrite(&admin, sizeof(struct _user), 1, filePtr);
                        admin.userId = 20;
                        strcpy(admin.passWord, "144434");
                        fwrite(&admin, sizeof(struct _user), 1, filePtr);
                        fclose(filePtr);
                        printf("Creating %s file, default credentians are:\n\tusername: 0\n\tpassword: 1234\n", files[i]);
                    }
                    else
                        printf("Creating %s file.\n", files[i]);
            }
            else
            {
                printf("Error, cannot access %s file! exiting...\n", files[i]);
                return 1;
            }
        }
    }
}

int checkFileExists(char *filename)
{
    FILE *filePtr;
    if((filePtr = fopen(filename, "rb"))!=NULL)
    {
        fclose(filePtr);
        return 0;
    }
    return -1;
}

int isStringEmpty(const char *s)
{
    while(*s != '\0'){
        if(!isspace((unsigned char)*s))
            return 0;
        s++;
    }
    return 1;
}

char *safe_fgets(char *buffer, int size)
{
    fgets(buffer, size, stdin);
    FLUSH_STDIN(buffer);
}

int strToUnsignedInt(const char *strUserId)
{
    int len = strlen(strUserId);
    for(int i = 0; i < len ; i++)
        if(isdigit(strUserId[i]) == 0)
            return -1;

    int userId = atoi(strUserId);

    char temp[len];
    if(len != strlen(itoa(userId,temp,10)))
        return -1;
    return userId;
}

int loadAllEntityElementsFromFile(void *buffer, size_t elementSize, int entityType)
{
    // load all entity files into corresponding struct arrays
    FILE *readPtr;
    if ((readPtr = fopen(files[entityType], "rb")) == NULL)
    {
            puts("File could not be opened.");
            return -2; //program does not execute successfully
    }

    int counter = 0;
    while (!feof(readPtr)) {

        char readBuf[elementSize];
        // Zero init readBuf
        for(int i = 0; i < elementSize; i++)
            readBuf[i] = 0;

        int elementsRead = fread(&readBuf, elementSize, 1, readPtr);
        if(elementsRead > 0)
        {
            switch(entityType)
            {
            case AUTH:
                {
                    struct _user *tempUser = (struct _user *)readBuf;
                    struct _user *userBuffer = (struct _user *) buffer;
                    userBuffer[counter].userId = tempUser->userId;
                    strcpy(userBuffer[counter].passWord, tempUser->passWord);
                }
                break;
            case STUDENTS:
                {
                    struct _student *tempStudent = (struct _student *)readBuf;
                    struct _student *studentBuffer = (struct _student *) buffer;
                    studentBuffer[counter].studentId = tempStudent->studentId;
                    strcpy(studentBuffer[counter].firstName, tempStudent->firstName);
                    strcpy(studentBuffer[counter].lastName, tempStudent->lastName);
                }
                break;
            case INSTRUCTORS:
                {
                    struct _instructor *tempInstructor = (struct _instructor *)readBuf;
                    struct _instructor *instructorBuffer = (struct _instructor *) buffer;
                    instructorBuffer[counter].instructorId = tempInstructor->instructorId;
                    strcpy(instructorBuffer[counter].firstName, tempInstructor->firstName);
                    strcpy(instructorBuffer[counter].lastName, tempInstructor->lastName);
                    strcpy(instructorBuffer[counter].title, tempInstructor->title);
                }
                break;
            case COURSES:
                {
                    struct _course *tempCourse = (struct _course *)readBuf;
                    struct _course *courseBuffer = (struct _course *) buffer;
                    courseBuffer[counter].courseId = tempCourse->courseId;
                    courseBuffer[counter].Unit = tempCourse->Unit;
                    strcpy(courseBuffer[counter].Name, tempCourse->Name);
                }
                break;
            }
            counter++;
        }
    }
    fclose(readPtr);
    return counter;
}

int authenticate(struct _user allusers[], int totalUsers){
    while(1){
        char username[MAXINPUT+1] = {0};
        char password[MAXINPUT+1] = {0};

        printf("Please enter your credentials(max %d chars):\n\tUsername:", MAXINPUT-1);
        safe_fgets(username, MAXINPUT+1);
        printf("\tPassword:");
        safe_fgets(password, MAXINPUT+1);

        int usernameLen = strlen(username);
        int passwordLen = strlen(password);
        if(usernameLen == MAXINPUT || passwordLen == MAXINPUT)
        {
            system("cls");
            printf("You exceeded max %d chars!\n", MAXINPUT - 1);
            continue;
        }

        if(usernameLen > 3)
        {
            system("cls");
            printf("Id must be between 1-200!\n");
            continue;
        }

        int userId = strToUnsignedInt(username);
        if(userId == -1 || userId > 200)
        {
            system("cls");
            printf("Id must be between 1-200!\n");
            continue;
        }

        for(int i = 0 ; i < totalUsers ; i++)
        {
            if(allusers[i].userId == userId && strcmp(allusers[i].passWord, password) == 0)
            {
                //printf("Welcome!\n");
                return userId;
            }
        }
        system("cls");
        printf("Your id or password is incorrect, try again!\n");
    }
}

int getMenuChoice(int digits, int maxnum){
    int menuChoice;

    char strMenuChoice[digits+2];
    safe_fgets(strMenuChoice, digits+2);

    int strMenuChoiceLen = strlen(strMenuChoice);
    if(strMenuChoiceLen == digits+1 )
    {
        system("cls");
        printf("You exceeded max %d chars!\n", digits);
        return -1;
    }

    if(strMenuChoiceLen > digits)
    {
        system("cls");
        printf("Select valid number!\n");
        return -1;
    }

    menuChoice = strToUnsignedInt(strMenuChoice);
    if(menuChoice == -1 || menuChoice > maxnum)
    {
        system("cls");
        printf("Select valid number!\n");
        return -1;
    }
    return menuChoice;
}

int getString(const char *prompt, char *buffer, int bufferSize, int repeatOnError, int clsOnRepeat)
{
    do{
        char tempBuffer[bufferSize+2];

        puts(prompt);
        safe_fgets(tempBuffer, bufferSize+2);

        int tempBufferLen = strlen(tempBuffer);
        if(tempBufferLen == bufferSize+1 )
        {
            if(clsOnRepeat)
                system("cls");
            printf("You exceeded max %d chars!\n", bufferSize);
        }
        else if(isStringEmpty(tempBuffer))
        {
            if(clsOnRepeat)
                system("cls");
            printf("Please enter valid string!\n", bufferSize);
        }
        else{
            strcpy(buffer , tempBuffer);
            return tempBufferLen;
        }
    }while(repeatOnError);
    return -1;
}

int getInteger(const char *prompt, int *input, int inputDigits, int inputMinValue, int inputMaxValue, int repeatOnError, int clsOnRepeat)
{
    do{
        char tempBuffer[inputDigits+2];

        puts(prompt);
        safe_fgets(tempBuffer, inputDigits+2);

        int tempBufferLen = strlen(tempBuffer);
        if(tempBufferLen == inputDigits+1 )
        {
            if(clsOnRepeat)
                system("cls");
            printf("You exceeded max %d chars!\n", inputDigits);
        }
        else {
            int n = strToUnsignedInt(tempBuffer);
            if(n == -1 || n > inputMaxValue || n < inputMinValue)
            {
                if(clsOnRepeat)
                    system("cls");
                printf("Input must be between %d-%d!\n", inputMinValue, inputMaxValue);
            }
            else{
                *input = n;
                return 1;
            }
        }
    }while(repeatOnError);
    return -1;
}

int writeAllEntityElementsToFile(void *buffer, int size, int count, int entityType)
{
    FILE *writePtr;
    if((writePtr = fopen(files[entityType], "wb"))==NULL){
        printf("Error, cannot access %s file! exiting...\n", files[entityType]);
        return 1;
    }

    for(int i = 0 ; i < count ; i++, buffer+=size)
        fwrite(buffer, size, 1, writePtr);

    fclose(writePtr);
    return 0;
}

int searchForStudendById(struct _student allstudents[], int totalStudents, int studentId)
{
    for(int i = 0 ; i < totalStudents ; i++)
        if(allstudents[i].studentId == studentId)
            return i;
    return -1;
}

void zeroMemory(void *buffer, int size)
{
    char *p = buffer;
    while(size--)
        *p++ = 0;
}

int editEntityElementInFile(void *buffer, int size, int position, int entityType)
{
    FILE *writePtr;
    if((writePtr = fopen(files[entityType], "rb+"))==NULL){
        printf("Error, cannot access %s file! exiting...\n", files[entityType]);
        return 1;
    }

    fseek(writePtr, size*position, SEEK_SET);
    fwrite(buffer, size, 1, writePtr);

    fclose(writePtr);
    return 0;
}


void studentManagement(struct _student allstudents[], int *totalStudents)
{
    while(1){
        system("cls");
        printf("Enter your choice:\n"
               "1 - List students\n"
               "2 - Add student\n"
               "3 - Edit student\n"
               "4 - Delete student\n"
               "0 - Back!\n"
               );
        int menuChoice = getMenuChoice(1, 4);
        if(menuChoice == 0)
        {
            system("cls");
            return;
        }
        if(menuChoice == 1)
        {
            printf("                     Students List\n"
                   "-------------------------------------------------------\n"
                       );
            for(int i = 0 ; i < (*totalStudents) ; i++)
            {
                printf("#%-3d - %-20s%-20s  , Id:%-3d\n", i, allstudents[i].firstName, allstudents[i].lastName, allstudents[i].studentId);
            }
            system("pause");
        }
        if(menuChoice == 2)
        {
            int studentId = -1;
            char firstName[MAXINPUT] = {}, lastName[MAXINPUT] = {};

            system("cls");
            int result = getInteger("Enter student Id , must be unique and between 1-101:", &studentId, 3, 1, 101, 1, 1);
            if(searchForStudendById(allstudents, *totalStudents, studentId) == -1)
            {
                getString("Enter your first name:", firstName, sizeof(firstName)/sizeof(firstName[0]), 1, 1);
                getString("Enter your last name:", lastName, sizeof(lastName)/sizeof(lastName[0]), 1, 1);
                allstudents[*totalStudents].studentId = studentId;
                strcpy(allstudents[*totalStudents].firstName, firstName);
                strcpy(allstudents[*totalStudents].lastName , lastName);
                (*totalStudents)++;
                writeAllEntityElementsToFile(allstudents, sizeof(struct _student), *totalStudents, STUDENTS);
                puts("Entity created successfully!");
                system("pause");
            }
            else
            {
                puts("Error, This id is already used! try again.");
                system("pause");
            }
        }
        if(menuChoice == 3)
        {
            int studentId = -1;
            char firstName[MAXINPUT] = {}, lastName[MAXINPUT] = {};

            system("cls");
            int result = getInteger("Enter student Id:", &studentId, 3, 1, 101, 1, 1);
            int found = searchForStudendById(allstudents, *totalStudents, studentId);
            if(found == -1)
            {
                puts("Error, id not found!");
                system("pause");
            }
            else{
                getString("Enter your first name:", firstName, sizeof(firstName)/sizeof(firstName[0]), 1, 1);
                getString("Enter your last name:", lastName, sizeof(lastName)/sizeof(lastName[0]), 1, 1);
                allstudents[found].studentId = studentId;
                strcpy(allstudents[found].firstName , firstName);
                strcpy(allstudents[found].lastName , lastName);
                if(editEntityElementInFile(&allstudents[found], sizeof(struct _student), found, STUDENTS) == 1)
                    return 1;
                puts("Entity edited successfully!");
                system("pause");
            }
        }
        if(menuChoice == 4)
        {
            int studentId = -1;

            system("cls");
            int result = getInteger("Enter student Id:", &studentId, 3, 1, 101, 1, 1);
            int found = searchForStudendById(allstudents, *totalStudents, studentId);
            if(found == -1)
            {
                puts("Error, id not found!");
                system("pause");
            }
            else{
                FILE *writePtr;
                if((writePtr = fopen(files[STUDENTS], "wb"))==NULL){
                    printf("Error, cannot access %s file! exiting...\n", files[STUDENTS]);
                    return 1;
                }

                for(int i = 0 ; i < (*totalStudents) ; i++)
                    if(allstudents[i].studentId != studentId)
                        fwrite(&allstudents[i], sizeof(struct _student), 1, writePtr);
//                    else
//                        zeroMemory(&allstudents[i], sizeof(struct _student));

                fclose(writePtr);

                *totalStudents = loadAllEntityElementsFromFile(allstudents, sizeof(struct _student), STUDENTS);
                puts("Entity deleted successfully!");
                system("pause");
            }
        }
    }
}
