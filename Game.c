#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TOPICS 100
#define MAX_NAME_LENGTH 50
#define MAX_DOB_LENGTH 20
#define MAX_QUESTION_LENGTH 256
#define MAX_OPTIONS 5
#define MAX_HIGHSCORES 100

typedef struct {
    char name[MAX_NAME_LENGTH];
    char pass[MAX_DOB_LENGTH];
} Topic;

typedef struct {
    char username[MAX_NAME_LENGTH];
    int score;
} HighScore;

void printHeader(const char* title) {
    printf("\n\n");
    printf("=====================================================================\n");
    printf("||              %-50s ||\n", title);
    printf("=====================================================================\n\n");
}

void printDivider() {
    printf("\n--------------------------------------------------\n");
}

int readTopics(Topic topics[]) {
    FILE *fp;
    int count = 0;
    fp = fopen("credentials.txt", "r");
    if (fp == NULL) {
        printf("Error opening file.\n");
        exit(1);
    }
    while (fscanf(fp, "%s %s", topics[count].name, topics[count].pass) != EOF) {
        count++;
    }
    fclose(fp);
    return count;
}

void saveHighScore(const char *username, int score) {
    HighScore highScores[MAX_HIGHSCORES];
    int count = 0;
    int userFound = 0;
    int existingScore = 0; // Variable to hold the existing score for comparison
    int i;
    // Load existing high scores
    loadHighScores(highScores, &count);
    
    // Check if the user already has a high score
    for (int i = 0; i < count; i++) {
        if (strcmp(highScores[i].username, username) == 0) {
            userFound = 1;
            existingScore = highScores[i].score; // Get the existing high score
            break;
        }
    }
    
    // If user found, check the scores
    if (userFound) {
        if (score > existingScore) {
            // Update score only if new score is higher
            highScores[i].score = score;
            printf("High score updated successfully!\n");
        } else {
            // Warn the user if the current score is less than the existing high score
            printf("Warning: Your current score (%d) is less than your saved high score (%d). Not saving.\n", score, existingScore);
            return; // Exit the function without saving
        }
    } else if (count < MAX_HIGHSCORES) {
        // If user not found, add new entry
        strcpy(highScores[count].username, username);
        highScores[count].score = score;
        count++;
        printf("\n\nHigh score saved successfully!\n");
    }
    
    // Sort high scores
    sortHighScores(highScores, count);
    
    // Write sorted high scores back to file
    FILE *fp = fopen("highscores.txt", "w");
    if (fp == NULL) {
        printf("Error opening highscores file.\n");
        return;
    }
    
    fprintf(fp, "%-20s %-10s\n", "Username", "Score");
    fprintf(fp, "==============================\n");
    for (int i = 0; i < count; i++) {
        fprintf(fp, "%-20s %-10d\n", highScores[i].username, highScores[i].score);
    }
    
    fclose(fp);
}

void loadHighScores(HighScore highScores[], int *count) {
    FILE *fp = fopen("highscores.txt", "r");
    if (fp == NULL) {
        printf("Error opening highscores file.\n");
        return;
    }
    *count = 0;
    char line[100];
    
    // Skip the header lines
    fgets(line, sizeof(line), fp);
    fgets(line, sizeof(line), fp);
    
    while (fscanf(fp, "%s %d", highScores[*count].username, &highScores[*count].score) == 2) {
        (*count)++;
    }
    fclose(fp);
}

void sortHighScores(HighScore highScores[], int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (highScores[i].score < highScores[j].score) {
                HighScore temp = highScores[i];
                highScores[i] = highScores[j];
                highScores[j] = temp;
            }
        }
    }
}

void loadQuestions(const char *baseFilename, int difficulty, int *score) {
    char filename[100];

    if (difficulty == 1) {
        sprintf(filename, "questions/%s_easy.txt", baseFilename);
    } else {
        sprintf(filename, "questions/%s_difficult.txt", baseFilename);
    }

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening questions file: %s\n", filename);
        return;
    }

    char question[MAX_QUESTION_LENGTH];
    char options[MAX_OPTIONS][MAX_QUESTION_LENGTH];
    int correctAnswer = 0;
    int questionCount = 1;

    while (fgets(question, sizeof(question), file)) {
        if (strlen(question) <= 1) continue;

        printf("%d) %s", questionCount++, question);
        
        for (int i = 0; i < 4; i++) {
            fgets(options[i], sizeof(options[i]), file);
            printf("%s", options[i]);
        }

        fgets(question, sizeof(question), file);
        sscanf(question, "Correct: %d", &correctAnswer);

        int userAnswer;
        printf("Enter your answer (1-4): ");
        scanf("%d", &userAnswer);

        if (userAnswer == correctAnswer) {
            printf("CORRECT ANSWER\n");
            (*score)++;
        } else {
            printf("WRONG ANSWER\n");
        }
        printf("\n");
        
        fgets(question, sizeof(question), file);
    }
    
    fclose(file);
}

void displayHighScores() {
    HighScore highScores[MAX_HIGHSCORES];
    int count;
    loadHighScores(highScores, &count);
    sortHighScores(highScores, count);

    printHeader("HIGH SCORES");
    printf("%-5s %-20s %-10s\n", "Rank", "Username", "Score");
    printf("=====================================\n");
    for (int i = 0; i < count; i++) {
        printf("%-5d %-20s %-10d\n", i + 1, highScores[i].username, highScores[i].score);
    }
    printf("=====================================\n");
}

void game(char *username) {
    int topic, difficulty, replay, score = 0;
    
    printHeader("WELCOME TO QUIZ GAME");
    
    do {
        printf("Please select a topic for your quiz:\n\n");
        printf("  1. HISTORY\n");
        printf("  2. POLITICS\n");
        printf("  3. GEOGRAPHY\n");
        printf("  4. BOLLYWOOD\n");
        printf("  5. CURRENT AFFAIRS\n");
        printf("  6. SPORTS\n\n");
        
        printf("Enter the number of your chosen topic: ");
        scanf("%d", &topic);

        if (topic < 1 || topic > 6) {
            printf("\nInvalid topic selection. Please try again.\n\n");
        }
    } while (topic < 1 || topic > 6);

    do {
        printDivider();
        printf("\nSelect difficulty level:\n\n");
        printf("  1. Easy\n");
        printf("  2. Difficult\n\n");
        printf("Enter your choice (1 or 2): ");
        scanf("%d", &difficulty);

        if (difficulty != 1 && difficulty != 2) {
            printf("\nInvalid difficulty selection. Please try again.\n");
        }
    } while (difficulty != 1 && difficulty != 2);

    printf("\nYou've selected: %s difficulty\n", (difficulty == 1) ? "Easy" : "Difficult");
    
    printDivider();
    printf("\nPrepare yourself! The quiz is about to begin...\n");
    printDivider();

    char *files[] = {"history", "politics", "geography", "bollywood", "current_affairs", "sports"};
    loadQuestions(files[topic - 1], difficulty, &score);

    printDivider();
    printf("\nYour final score: %d\n", score);
    printDivider();

    HighScore highScores[MAX_HIGHSCORES];
    int count;
    loadHighScores(highScores, &count);
    int userHighScore = 0;
    for (int i = 0; i < count; i++) {
        if (strcmp(highScores[i].username, username) == 0) {
            userHighScore = highScores[i].score;
            break;
        }
    }
    printf("Your current high score: %d\n", userHighScore);

    do {
        printf("\nWhat would you like to do next?\n\n");
        printf("  1. Save High Score\n");
        printf("  2. Play Again\n");
        printf("  3. Return to Main Menu\n\n");
        printf("Enter your choice: ");
        scanf("%d", &replay);

        switch (replay) {
            case 1:
                saveHighScore(username, score);
                do {
                    printf("\nWhat would you like to do next?\n\n");
                    printf("  1. Play Again\n");
                    printf("  2. View High Scores\n");
                    printf("  3. Return to Main Menu\n\n");
                    printf("Enter your choice: ");
                    scanf("%d", &replay);

                    if (replay == 1) {
                        game(username);
                        return;
                    } else if (replay == 2) {
                        displayHighScores();
                        do {
                            printf("\nWhat would you like to do next?\n\n");
                            printf("  1. Play Again\n");
                            printf("  2. Return to Main Menu\n\n");
                            printf("Enter your choice: ");
                            scanf("%d", &replay);

                            if (replay == 1) {
                                game(username);
                                return;
                            } else if (replay == 2) {
                                user();
                                return;
                            } else {
                                printf("\nInvalid input. Please try again.\n");
                            }
                        } while (replay != 1 && replay != 2);
                    } else if (replay == 3) {
                        user();
                        return;
                    } else {
                        printf("\nInvalid input. Please try again.\n");
                    }
                } while (replay != 1 && replay != 2 && replay != 3);
                break;
            case 2:
                game(username);
                return;
            case 3:
                user();
                return;
            default:
                printf("\nInvalid input. Please try again.\n");
        }
    } while (replay != 1 && replay != 2 && replay != 3);
}

void registration() {
    int choice;
    char name[20], pass[20];
    FILE *fptr;

    printHeader("USER REGISTRATION");

    fptr = fopen("credentials.txt", "a");
    if(fptr == NULL) {
        printf("Error 404 !! FILE NOT FOUND !!\n");   
        exit(1);             
    }
    
    printf("Please enter the following details:\n\n");
    printf("  Name     : ");
    scanf("%s", name);
    printf("  Password : ");
    scanf("%s", pass);


    // Writing to file in a formatted way
    fprintf(fptr, "%-20s %-15s\n", 
            name, pass );
    
    fclose(fptr);
    
    printDivider();
    printf("\nRegistration successful!\n\n");
    printf("What would you like to do next?\n\n");
    printf("  1. Login\n");
    printf("  2. Return to main menu\n\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);
    
    if(choice == 1) {
        customer_login();
    } else {
        main();
    }
}

void customer_login() {
    Topic topics[MAX_TOPICS];
    int count, i;
    char name[MAX_NAME_LENGTH], pass[MAX_DOB_LENGTH];
    
    printHeader("USER LOGIN");

    count = readTopics(topics);
    
    printf("Please enter your credentials:\n\n");
    printf("  Username: ");
    scanf("%s", name);
    printf("  Password: ");
    scanf("%s", pass);
    
    for (i = 0; i < count; i++) {
        if (strcmp(topics[i].name, name) == 0 && strcmp(topics[i].pass, pass) == 0) {
            printf("\nLogin successful! Welcome, %s.\n", topics[i].name);
            printDivider();
            game(name);
            return;
        }
    }
    
    printf("\nInvalid username or password.\n");
    printf("Please try again.\n");
    customer_login();
}

void user() {
    int choice;
    printHeader("QUIZ GAME - MAIN MENU");
    
    printf("Please select an option:\n\n");
    printf("  1. Register\n");
    printf("  2. Login\n");
    printf("  3. View HighScores\n");
    printf("  4. Quit\n\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);

    switch(choice) {
        case 1:
            registration();
            break;
        case 2:
            customer_login();
            break;
        case 3:
            displayHighScores();
            printf("Please select an option:\n\n");
            printf("  1. Return to MAIN MENU\n");
            printf("  2. Exit\n");
            int m;
            printf("Enter your choice: ");
            scanf("%d", &m);
            if (m == 1){
                user();
            }
            else if (m == 2)
            {
                printf("Goodbye!\n");
                break;
            }
            else{
                printf("\nInvalid choice. Redirecting to main  menu.\n");
                user();
                break;
            }
            
        case 4:
            printf("\nGoodbye!\n");
            break;
        default:
            printf("\nInvalid choice. Please try again.\n");
            user();
    }
}

int main() {
    printHeader("WELCOME TO THE ULTIMATE QUIZ GAME");
    user();
    return 0;
}