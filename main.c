
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>


#define PREFIX "movies_"
#define EXTENSION ".csv"

/* Parse the current line which is comma delimited and create a
*  file with the name of each movie by year
*/
void writeMovie(char* currLine, char* dirName)
{
    // For use with strtok_r
    char* saveptr;

    // The first token is the title
    char* token = strtok_r(currLine, ",", &saveptr);
    char* title = calloc(strlen(token) + 2, sizeof(char));
    strcpy(title, token);
    strcat(title, "\n");

    // The next token is the year
    token = strtok_r(NULL, ",", &saveptr);
    int year = atoi(token);

    // a large amount of this code comes from the files module of this class
    int fd;
    char* newFilePath = calloc(100, sizeof(char));
    strcpy(newFilePath, dirName);
    strcat(newFilePath, "/");
    char buffer[50];
    sprintf(buffer, "%d", year);
    strcat(newFilePath, buffer);
    strcat(newFilePath, ".txt");

    // We first open a file for read write, creating it if necessary and truncating it if it already exists
    fd = open(newFilePath, O_RDWR | O_CREAT | O_APPEND, 0640);
    if (fd == -1) {
        printf("open() failed on \"%s\"\n", newFilePath);
        perror("Error");
        exit(1);
    }

    // We write a string to the file
    write(fd, title, strlen(title));
    // Close the file descriptor
    close(fd);
    free(title);
    free(newFilePath);
}

/*
* Return a linked list of students by parsing data from
* each line of the specified file.
*/
void processFile(char* filePath)
{
    printf("Now processing the chosen file named ");
    printf(filePath);
    printf("\n");
    //create directory
    int randVal = random() % 100000;
    char buffer[50];
    sprintf(buffer, "%d", randVal);
    char* dirName = calloc(50, sizeof(char));
    strcpy(dirName, "cairnsi.movies.");
    strcat(dirName, buffer);
    mkdir(dirName, 0750);
    printf("Created directory with name ");
    printf(dirName);
    printf("\n\n");

    // Open the specified file for reading only
    FILE* movieFile = fopen(filePath, "r");

    char* currLine = NULL;
    size_t len = 0;
    size_t nread;


    bool firstLine = true;

    // Read the file line by line
    while ((nread = getline(&currLine, &len, movieFile)) != -1)
    {
        if (firstLine) {
            firstLine = false;
            continue;
        }

        // Write the movie to the file
        writeMovie(currLine, dirName);
    }
    free(currLine);
    fclose(movieFile);
}

/*
* Find largest file
*/
char* findLargestFile()
{
    // Open the current directory
    // A majortiy of the code in this function is found in the Directories module of this class
    DIR* currDir = opendir(".");
    struct dirent* aDir;
    struct stat dirStat;
    char* entryName = calloc(256, sizeof(char));
    off_t size = 0;
    // Go through all the entries
    while ((aDir = readdir(currDir)) != NULL) {
        //check prefix
        if (strncmp(PREFIX, aDir->d_name, strlen(PREFIX)) == 0) {
            int fileNameLength = strlen(aDir->d_name);
            char* ptr = aDir->d_name;
            //move pointer to end of name to check 3 letter extension
            ptr += (fileNameLength - 4);
            //check extension
            if (strncmp(EXTENSION, ptr, strlen(EXTENSION)) == 0) {
                stat(aDir->d_name, &dirStat);
                //check size
                if (dirStat.st_size > size) {
                    size = dirStat.st_size;
                    free(entryName);
                    entryName = calloc(256, sizeof(char));
                    strcpy(entryName, aDir->d_name);
                }
            }
        }
    }
    return entryName;
}


/*
* Find smallest file
*/
char* findSmallestFile()
{
    // Open the current directory
    // A majortiy of the code in this function is found in the Directories module of this class
    DIR* currDir = opendir(".");
    struct dirent* aDir;
    struct stat dirStat;
    char* entryName = calloc(256, sizeof(char));
    off_t size = 0;
    // Go through all the entries
    while ((aDir = readdir(currDir)) != NULL) {
        //check prefix
        if (strncmp(PREFIX, aDir->d_name, strlen(PREFIX)) == 0) {
            int fileNameLength = strlen(aDir->d_name);
            char* ptr = aDir->d_name;
            //move pointer to end of name to check 3 letter extension
            ptr += (fileNameLength - 4);
            //check extension
            if (strncmp(EXTENSION, ptr, strlen(EXTENSION)) == 0) {
                stat(aDir->d_name, &dirStat);
                //check size
                if (dirStat.st_size < size || size == 0) {
                    size = dirStat.st_size;
                    free(entryName);
                    entryName = calloc(256, sizeof(char));
                    strcpy(entryName, aDir->d_name);
                }
            }
        }
    }
    return entryName;
}

/*
* Find file
*/
bool findFile(char* fileName)
{
    // Open the current directory
    // A majortiy of the code in this function is found in the Directories module of this class
    DIR* currDir = opendir(".");
    struct dirent* aDir;
    bool found = false;
    // Go through all the entries
    while ((aDir = readdir(currDir)) != NULL) {
        //check file name
        if (strncmp(fileName, aDir->d_name, strlen(fileName)) == 0) {
            found = true;
        }
    }
    return found;
}

/*
*   Process the file provided specified either by size or file name
*   add movies to files based on years
*   Compile the program as follows:
*       gcc --std=gnu99 -o movies main.c
*/
int main(int argc, char* argv[])
{
    time_t t;
    int seed = time(&t);
    srandom((unsigned)seed);
    bool askAgain = true;
    //run outer loop asking if user would like to specify a file
    do {
        printf("1. Select file to process\n");
        printf("2. Exit the program\n\n");
        printf("Enter a choice 1 or 2: ");
        bool promptSecondChoice;
        int choice;
        char* fileName;
        scanf("%d", &choice);
        switch (choice) {
        case 1:
            do {
                //run inner loop checking if user would like a file based on size or name
                printf("Which file do you want to process?\n");
                printf("Enter 1 to pick the largest file\n");
                printf("Enter 2 to pick the smallest file\n");
                printf("Enter 3 to specify the name of a file\n\n");
                printf("Enter a choice from 1 to 3 : ");
                int secondChoice;
                scanf("%d", &secondChoice);
                switch (secondChoice) {
                case 1:
                    fileName = findLargestFile();
                    promptSecondChoice = false;
                    break;
                case 2:
                    fileName = findSmallestFile();
                    promptSecondChoice = false;
                    break;
                case 3:
                    printf("Enter the complete file name: ");
                    fileName = calloc(256, sizeof(char));
                    scanf("%s", fileName);
                    bool found = findFile(fileName);
                    if (found) {
                        promptSecondChoice = false;
                    }
                    else {
                        printf("The file ");
                        printf(fileName);
                        printf(" was not found. Try again\n\n");
                        promptSecondChoice = true;
                        free(fileName);
                    }

                    break;
                default:
                    printf("You entered an incorrect choice. Try again.\n\n");
                    promptSecondChoice = true;
                    break;
                }
            } while (promptSecondChoice);
            processFile(fileName);
            free(fileName);
            break;
        case 2:
            askAgain = false;
            break;
        default:
            printf("You entered an incorrect choice. Try again.\n\n");
            break;
        }
    } while (askAgain);
    
    return EXIT_SUCCESS;
}
