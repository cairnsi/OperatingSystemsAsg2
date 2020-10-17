// If you are not compiling with the gcc option --std=gnu99, then
// uncomment the following line or you might get a compiler warning
//#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#define PREFIX "movies_"
#define EXTENSION ".csv"
/* struct for movie information */
struct movie
{
    char* title;
    int year;
    char** languages;
    double rating;
    struct movie* next;
};

/* struct for movie queue and array of highest rated movies per year */
struct dataContainer
{
    struct movie* head;
    struct movie** bestByYear;
    int movieCount;
};



/* Parse the current line which is comma delimited and create a
*  movie struct with the data in this line
*/
struct movie* createMovie(char* currLine)
{
    struct movie* currMovie = malloc(sizeof(struct movie));

    // For use with strtok_r
    char* saveptr;

    // The first token is the title
    char* token = strtok_r(currLine, ",", &saveptr);
    currMovie->title = calloc(strlen(token) + 1, sizeof(char));
    strcpy(currMovie->title, token);

    // The next token is the year
    token = strtok_r(NULL, ",", &saveptr);
    currMovie->year = atoi(token);

    // The next token is the languages
    token = strtok_r(NULL, ",", &saveptr);
    // create languages array
    char** languages = calloc( 5 , sizeof(char*));
    // new pointer to run strtok_r on substring with array elements
    char* saveptrLocation;
    // move pointer past the opening bracket '['
    token++;
    // make temp pointer for finding number of elements
    char* tempToken = token;
    int count = 0;
    // find number of elements by counting ';' in array
    while (tempToken < saveptr) {
        if (*tempToken == ';') {
            count++;
        }
        tempToken++;
    }
    // get each element before the closing bracket
    for (int i = 0; i < count;i++) {
        if (i == 0) {
            token = strtok_r(token, ";", &saveptrLocation);
        }
        else {
            token = strtok_r(NULL, ";", &saveptrLocation);
        }
        languages[i] = calloc(21, sizeof(char));
        strcpy(languages[i], token);
    }
    // get the last element of the array
    if (count == 0) {
        token = strtok_r(token, "]", &saveptrLocation);
    }
    else {
        token = strtok_r(NULL, "]", &saveptrLocation);
    }
    languages[count] = calloc(20, sizeof(char));
    strcpy(languages[count], token);
    currMovie->languages = languages;

    // The last token is the rating
    token = strtok_r(NULL, "\n", &saveptr);
    char* ptr;
    currMovie->rating = strtod(token, &ptr);

    // Set the next node to NULL in the newly created student entry
    currMovie->next = NULL;

    return currMovie;
}

/*
* Return a linked list of students by parsing data from
* each line of the specified file.
*/
struct dataContainer* processFile(char* filePath)
{
    // Open the specified file for reading only
    FILE* movieFile = fopen(filePath, "r");

    char* currLine = NULL;
    size_t len = 0;
    size_t nread;

    // The head of the linked list
    struct movie* head = NULL;
    // The tail of the linked list
    struct movie* tail = NULL;

    bool firstLine = true;
    int movieCount = 0;

    // Read the file line by line
    while ((nread = getline(&currLine, &len, movieFile)) != -1)
    {
        if (firstLine) {
            firstLine = false;
            continue;
        }

        movieCount++;
        // Get a new student node corresponding to the current line
        struct movie* newNode = createMovie(currLine);

        // Is this the first node in the linked list?
        if (head == NULL)
        {
            // This is the first node in the linked link
            // Set the head and the tail to this node
            head = newNode;
            tail = newNode;
        }
        else
        {
            // This is not the first node.
            // Add this node to the list and advance the tail
            tail->next = newNode;
            tail = newNode;
        }
    }
    free(currLine);
    fclose(movieFile);

    struct dataContainer *container = malloc(sizeof(struct dataContainer));
    container->head = head;
    container->bestByYear = calloc(movieCount, sizeof(struct movie*));
    container->movieCount = movieCount;
    
    struct movie* ptr = head;

    while (ptr != NULL) {
        for (int i = 0; i < movieCount; i++) {
            if (container->bestByYear[i] == 0) {
                container->bestByYear[i] = ptr;
                break;
            }
            else if (container->bestByYear[i]->year == ptr->year) {
                if (container->bestByYear[i]->rating < ptr->rating) {
                    container->bestByYear[i] = ptr;
                }
                break;
            }
        }

        ptr = ptr->next;
    }
    printf("Processed file %s and parsed data for %d movies\n\n", filePath , movieCount);

    return container;
}

/*
* Print data for the given movie
*/
void printMovie(struct movie* aMovie) {
    printf("%s, %i, %.2f\n", aMovie->title,
        aMovie->year,
        aMovie->rating);
    int index = 0;
    while (aMovie->languages[index] != 0 && index<5) {
        printf("%s\n", (aMovie->languages[index]));
        index++;
    }
}

/*
* Print the linked list of movies
*/
void printMovieList(struct dataContainer* container)
{
    struct movie* list = container->head;
    while (list != NULL)
    {
        printMovie(list);
        list = list->next;
    }
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
*   Process the file provided as an argument to the program to
*   create a linked list of movie structs and prompt the user for actions.
*   Compile the program as follows:
*       gcc --std=gnu99 -o movies main.c
*/

int main(int argc, char* argv[])
{
    bool askAgain = true;
    do {
        printf("1. Select file to process\n");
        printf("2. Exit the program\n\n");
        printf("Enter a choice 1 or 2: ");
        bool promptSecondChoice;
        int choice;
        scanf("%d", &choice);
        switch (choice) {
        case 1:
            do {
                printf("Which file do you want to process?\n");
                printf("Enter 1 to pick the largest file\n");
                printf("Enter 2 to pick the smallest file\n");
                printf("Enter 3 to specify the name of a file\n\n");
                printf("Enter a choice from 1 to 3 : ");
                int secondChoice;
                scanf("%d", &secondChoice);
                char* fileName;
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
                    char* inputFileName = calloc(256, sizeof(char));
                    scanf("%s", inputFileName);
                    bool found = findFile(inputFileName);
                    if (found) {
                        promptSecondChoice = false;
                    }
                    else {
                        printf("The file ");
                        printf(inputFileName);
                        printf(" was not found. Try again\n\n");
                        promptSecondChoice = true;
                    }

                    free(inputFileName);
                    break;
                default:
                    printf("You entered an incorrect choice. Try again.\n\n");
                    promptSecondChoice = true;
                    break;
                }
            } while (promptSecondChoice);

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
