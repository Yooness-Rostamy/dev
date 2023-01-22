#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXINPUT 20
// Entities:
// Admin
// Students
// Courses
// Teachers
// users

enum entities {AUTH, STUDENTS, TEACHERS, COURSES};
const char *files[4];
enum roles {admin, student, teacher};

struct _course {
    char courseId[MAXINPUT];
    char courseTitle[MAXINPUT];
    int units;
};

struct _student {
    char userName[MAXINPUT];
    char firstName[MAXINPUT];
    char lastName[MAXINPUT];
    struct _course courses[20];
};

struct _teacher {
    char userName[MAXINPUT];
    char firstName[MAXINPUT];
    char lastName[MAXINPUT];
    char title[MAXINPUT];
    struct _course courses[20];
};

// store user credentials in auth.txt
struct _user {
    char userName[MAXINPUT];
    char passWord[MAXINPUT];
    enum roles role;
};

int healthCheck(void);
int enterChoice(void);
int login(void);
int checkFileExists(char *filename);
int isStringEmpty(const char *s);
int searchForEntitie(enum entities entityFile, void *entity);
void authenticate(){

}

int main()
{
    files[AUTH] = "auth.txt";
    files[STUDENTS] = "students.txt";
    files[TEACHERS] = "teachers.txt";
    files[COURSES] = "courses.txt";

    if(healthCheck() == 1)
        return 1;

    struct _user testuser = {.userName = "admin", .role = admin};

    searchForEntitie(AUTH, &testuser);
    return 0;
    FILE *authFile;
    if((authFile = fopen("auth.txt", "rb"))==NULL)
    {
        puts("Couldnt open auth.txt file! exiting.\n");
        return 0;
    }
    struct _user user;
    fread(&user, sizeof(struct _user), 1, authFile);
    printf("username: %s, password: %s, role: %d\n", user.userName, user.passWord, user.role);
    fclose(authFile);
    //login();
    //enterChoice();
    //printf("Hello world!\n");
    return 0;
}

// check if auth.txt file exists, if not create the file with default credentials
int healthCheck(void){

    for(int i = AUTH; i <= COURSES; i++)
    {
        if(checkFileExists(files[i]) == -1)
        {
            FILE *filePtr;
            // create auth.txt file and populate it with default username and password
            if((filePtr = fopen(files[i], "wb"))!=NULL){
                    if(i == AUTH)
                    {
                        struct _user adminUser;
                        strcpy(adminUser.userName, "admin");
                        strcpy(adminUser.passWord, "1234");
                        adminUser.role = admin;
                        fwrite(&adminUser, sizeof(struct _user), 1, filePtr);
                        fclose(filePtr);
                        printf("Creating %s file, default credentians are:\n\tusername: admin\n\tpassword: 1234\n", files[i]);
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

int searchForEntitie(enum entities entityFile, void *entity)
{
    FILE *readPtr;
    if ((readPtr = fopen(files[entityFile], "rb")) == NULL)
    {
            puts("File could not be opened.");
            return -2; //program does not execute successfully
    }

    char *username;
    struct _user *user = (struct _user *) entity;
    struct _course *course = (struct _course *) entity;
    struct _student *student = (struct _student *) entity;
    struct _teacher *teacher = (struct _teacher *) entity;

    switch(entityFile)
    {
    case AUTH:
        username = user->userName;
        break;
    case COURSES:
        username = course->courseId;
        break;
    case STUDENTS:
        username = student->userName;
        break;
    case TEACHERS:
        username = teacher->userName;
        break;
    default:
        return -3;
    }


    if(isStringEmpty(username) == 1)
    {
        puts("Please input non empty string!\n");
        return -1;
    }


    while (!feof(readPtr)) {
        long int pos = ftell(readPtr);
        switch(entityFile)
            {
            case AUTH:
                {
                    struct _user temp;
                    fread(&temp, sizeof(struct _user), 1, readPtr);
                    if(strcmp(user->userName, temp.userName) == 0)
                    {
                        strcpy(user->passWord, temp.passWord);
                        user->role = temp.role;
                        return pos;
                    }
                }
                break;
            case STUDENTS:
                {
                    struct _student temp;
                    fread(&temp, sizeof(struct _student), 1, readPtr);
                    if(strcmp(student->userName, temp.userName) == 0)
                    {
                        strcpy(student->firstName, temp.firstName);
                        strcpy(student->lastName, temp.lastName);
                        return pos;
                    }
                }
                break;
            case TEACHERS:
                {
                    struct _teacher temp;
                    fread(&temp, sizeof(struct _teacher), 1, readPtr);
                    if(strcmp(teacher->userName, temp.userName) == 0)
                    {
                        strcpy(teacher->firstName, temp.firstName);
                        strcpy(teacher->lastName, temp.lastName);
                        strcpy(teacher->title, temp.title);
                        return pos;
                    }
                }
                break;
            case COURSES:
                {
                    struct _course temp;
                    fread(&temp, sizeof(struct _course), 1, readPtr);
                    if(strcmp(course->courseId, temp.courseId) == 0)
                    {
                        course->units = temp.units;
                        strcpy(course->courseTitle, temp.courseTitle);
                        return pos;
                    }
                }
                break;
            }
    }

    return -4;
}