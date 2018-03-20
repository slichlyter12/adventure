/*
 * Sam Lichlyter
 * Oregon State University : CS 344
 * 10/24/2015
 *
 * adventure: text based adventure with a Star Wars theme
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <dirent.h>

void generateFiles(int*);
void startGame();
void printData(FILE*);
void warpToPlanet(char*, int, char**);
int numberOfLinesIn(FILE*);
void shuffleFY(int, int*);
static int rand_int(int);

int main() {
    
    int *rooms = (int*)calloc(10, sizeof(int));
    
    // Setup room files
    shuffleFY(10, rooms);
    generateFiles(rooms);
    free(rooms);
    
    // Play game
    startGame();
    
    return 0;
}

void generateFiles(int *rooms) {
    
    //declare variables for later
    int i, j, t, roomIndex, numberOfConnections, alreadyConnected;
    char *roomNames[10], *currentRooms[10], roomDir[50], path[40];
    FILE *fp;
    
    // Create room list
    roomNames[0] = "Alderaan";
    roomNames[1] = "Bespin";
    roomNames[2] = "Courscant";
    roomNames[3] = "Dagobah";
    roomNames[4] = "Endor";
    roomNames[5] = "Hoth";
    roomNames[6] = "Jakku";
    roomNames[7] = "Sullust";
    roomNames[8] = "Tatooine";
    roomNames[9] = "Yavin";
    
    // Create room list for current round
    for (i = 0; i < 7; i++) {
        roomIndex = rooms[i];
        currentRooms[i] = roomNames[roomIndex];
    }
    
    // Create directory for room files
    sprintf(roomDir, "./lichlyts.rooms.%d", getpid());
    mkdir(roomDir, S_IRWXU);
    
    // Initialize connections
    int connections[7][7];
    int x, y;
    for (x = 0; x < 7; x++) {
        for (y = 0; y < 7; y++) {
            connections[x][y] = 0;
        }
    }
    
    // Create random connections
    for (i = 0; i < 7; i++) {
        alreadyConnected = 0;
        
        // generate number of connections for each room
        do {
            numberOfConnections = rand_int(7);
        } while (numberOfConnections < 3);
        
        // get already connected
        for (x = 0; x < 7; x++) {
            if (connections[i][x] == 1 && i != x) {
                alreadyConnected++;
            }
        }
        
        // check if more need to be connected
        if (numberOfConnections - alreadyConnected > 0) {
            for (j = 0; j < numberOfConnections - alreadyConnected; j++) {
                
                // generate random room to create connection
                do {
                    t = rand_int(7);
                } while (connections[i][t] == 1 || i == t);
                
                // create connections
                connections[i][t] = 1;
                connections[t][i] = 1;
            }
        }
    }
    
    // iterate through room list and create file; write room name, connections, and room type to file
    char roomName[15];
    int count;
    for (i = 0; i < 7; i++) {
        strcpy(roomName, currentRooms[i]);
        sprintf(path, "%s/%s.txt", roomDir, roomName);
        fp = fopen(path, "w");
        if (fp == NULL) {
            printf("Could not open %s! ** QUITTING **\n", path);
            exit(1);
        } else {
            fprintf(fp, "ROOM NAME: %s\n", roomName);
            count = 0;
            for (j = 0; j < 7; j++) {
                if (connections[i][j] == 1) {
                    count++;
                    fprintf(fp, "CONNECTION %i: %s\n", count, currentRooms[j]);
                }
            }
            if (i == 0) {
                fprintf(fp, "ROOM TYPE: START_ROOM\n");
            } else if (i == 6) {
                fprintf(fp, "ROOM TYPE: END_ROOM\n");
            } else {
                fprintf(fp, "ROOM TYPE: MID_ROOM\n");
            }
            fclose(fp);
        }
    }
}

void startGame() {
    
    // Declaring variables
    DIR *dp;
    struct dirent *ep;
    char roomDir[50], filename[15], copy[15];
    int numberOfSteps = 0, i;
    
    // Declaring a nasty variable
    char *path[25];
    for (i = 0; i < 25; i++) {
        path[i] = (char*)calloc(15, sizeof(char));
    }
    
    // Opens the directory, the first room is the start room, displays mission and warps to planet
    sprintf(roomDir, "./lichlyts.rooms.%d/", getpid());
    dp = opendir(roomDir);
    if (dp != NULL) {
        while (ep = readdir(dp)) {
            strcpy(filename, ep->d_name);
            if (strcmp(filename, ".") != 0 && strcmp(filename, "..") != 0) {
                
                // Display Mission
                system("clear");
                printf("WELCOME TO THE EMPIRE! YOUR MISSION IS TO HUNT DOWN THE LAST OF REBEL ALLIANCE.\nTHEY ARE HIDING ON A REMOTE PLANET.\nIF YOU FIND THEM, PERHAPS THE EMPEROR WILL REWARD YOU.\n\n");
                
                // Take ".txt" off filename for first round
                int length = strlen(filename);
                strncpy(copy, filename, length - 4);
                
                // Warp
                warpToPlanet(copy, numberOfSteps, path);
                
                break;
            }
            
        }
        (void)closedir(dp);
    } else {
        printf("Could not open directory; ** QUITTING **\n");
        exit(2);
    }
    
    for (i = 0; i < 25; i++) {
        free(path[i]);
    }
}

void warpToPlanet(char *planetName, int numberOfSteps, char **completedPath) {
    
    // Declare variables
    int i, lines = 0, length = 0, allow;
    char roomDir[50], path[50], filename[11], next[11], copy[15], fileBuffer[50], fileArray[10][40], line[30], t[30];
    FILE *fp;
    
    // Set filename and path
    strcpy(filename, planetName);
    strcat(filename, ".txt");
    sprintf(roomDir, "./lichlyts.rooms.%d/", getpid());
    if (strcmp(filename, ".") != 0 && strcmp(filename, "..") != 0) {
        //open and read planet name because the first one is always START_ROOM
        sprintf(path, "%s/%s", roomDir, filename);
        fp = fopen(path, "r");
        if (fp != NULL) {
            
            // Put file data into array (this came later, required for checking possible connections)
            while (fgets(fileBuffer, 255, fp) != NULL) {
                sscanf(fileBuffer, "%s %s %s", t, t, line);
                strcpy(fileArray[length], line);
                length++;
            }
            
            // step number of steps and add location to path
            strcpy(completedPath[numberOfSteps], planetName);
            numberOfSteps++;
            
            // Check victory conditions, display results if found END_ROOM
            if (strcmp("END_ROOM", fileArray[length - 1]) == 0) {
                printf("\nYOU HAVE FOUND AND CONQUERED THE REBEL BASE! CONGRATULATIONS!\nTHE EMPEROR WILL REWARD YOU FOR TAKING %i STEPS BY LETTING YOU FEEL THE TRUE POWER OF THE DARK SIDE.\nYOUR PATH TO VICTORY WAS:\n", numberOfSteps - 1);
                for (i = 0; i < numberOfSteps; i++) {
                    printf("%s\n", completedPath[i]);
                }
                return;
            }
            
            // get number of lines
            lines = numberOfLinesIn(fp);
            lines -= 2;
            
            // print current location (first method, didn't want to change)
            fseek(fp, 0, SEEK_SET);
            printf("\nCURRENT LOCATION: ");
            printData(fp);
            printf("\n");
            
            // print possible locations
            printf("POSSIBLE CONNECTIONS: ");
            for (i = 0; i < lines; i++) {
                if (i < lines - 1) {
                    printData(fp);
                    printf(", ");
                } else {
                    printData(fp);
                    printf(".\n");
                }
            }
            
            fclose(fp);
            
            // Warp / check if valid warp i.e. if connected to that planet
            printf("WHERE TO? >");
            scanf("%s", &next);
            allow = 0;
            while (allow == 0) {
                for (i = 1; i < length; i++) {
                    if (strcmp(next, fileArray[i]) == 0)
                        allow = 1;
                }
                if (allow != 1) {
                    printf("THERE IS AN ASTROID FIELD IN THE WAY, CHOOSE A NEW PLANET!\n>");
                    scanf("%s", &next);
                }
            }
            warpToPlanet(next, numberOfSteps, completedPath);
            
        } else {
            // this will probably never show up
            printf("THAT PLANET DOESN'T APPEAR IN OUR RECORDS. CHOOSE A NEW PLANET TO CONQUER.\n>");
            scanf("%s", &next);
            warpToPlanet(next, numberOfSteps, completedPath);
        }
    }
}

// Get number of lines in a file
int numberOfLinesIn(FILE *fp) {
    int lines = 0, t;
    
    // get number of lines
    fseek(fp, 0, SEEK_SET);
    do {
        t = fgetc(fp);
        if (t == '\n')
            lines++;
    } while (t != EOF);
    
    return lines;
}

// Print data one character at a time (first method)
void printData(FILE *fp) {
    int print = 0, t;
    do {
        if (print == 1 && t != ':') {
            printf("%c", t);
        }
        t = fgetc(fp);
        if (t == ':') {
            print = 1;
            fseek(fp, 1, SEEK_CUR);
        } else if (t == '\n') {
            return;
        }
    } while (t != EOF);
}

// Shuffle function based on Fisher-Yates algorithm
// set: the set you want to shuffle
// n: the number of items you want in the set
void shuffleFY(int n, int *set) {
    int i, j, t;
    
    // Initialize set
    for (i = 0; i < n; i++) {
        set[i] = i;
    }
    
    // Seed RNG
    srand(time(NULL));
    
    // Suffle set using Fisher-Yates Shuffle
    for (i = n - 1; i > 0; i--) {
        j = rand_int(i + 1);
        t = set[j];
        set[j] = set[i];
        set[i] = t;
    }
}

// Generates a random integer between 0 and n
static int rand_int(int n) {
    int limit = RAND_MAX - RAND_MAX % n;
    int rnd;
    
    do {
        rnd = rand();
    } while (rnd >= limit);
    
    return rnd % n;
}