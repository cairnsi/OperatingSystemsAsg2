// If you are not compiling with the gcc option --std=gnu99, then
// uncomment the following line or you might get a compiler warning
//#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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
* Print the linked list of movies by year
*/
void printByYear(struct movie* list, int year)
{
    bool found = false;
    while (list != NULL)
    {
        if (list->year == year) {
            printf("%s\n", list->title);
            found = true;
        }
        list = list->next;
    }
    
    if (!found) {
        printf("No data about movies released in the year %d\n", year);
    }
    printf("\n");
}

/*
* Print the linked list of movies by highest rated
*/
void printHighestRated(struct dataContainer* container)
{
    for (int i = 0; i < container->movieCount; i++) {
        if (container->bestByYear[i] == 0) {
            break;
        }
        struct movie* current = container->bestByYear[i];
        printf("%i %.2f %s\n", current->year, current->rating, current->title);
    }
    printf("\n");
}

/*
* Print the linked list of movies by language
*/
void printByLanguage(struct movie* list, char* language)
{
    bool found = false;
    while (list != NULL)
    {
        for (int i = 0; i < 5; i++) {
            if (list->languages[i] == 0) {
                break;
            }
            else if (strcmp(list->languages[i], language) == 0) {
                found = true;
                printf("%d %s\n", list->year, list->title);
            }
        }
        list = list->next;
    }

    if (!found) {
        printf("No data about movies released in %s\n", language);
    }
    printf("\n");
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
        int choice;
        scanf("%d", &choice);
        switch (choice) {
        case 1:
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
