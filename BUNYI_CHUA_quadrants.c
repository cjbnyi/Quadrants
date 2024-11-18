/*
    DESCRIPTION:
        This program is titled "Quadrants". It is a two-player game where players must avoid picking
        predetermined sets of tiles to avoid securing opposite quadrants, causing them to lose.
        
        The game alternates between two players choosing any tile on the board and continues until
        a lose condition has been triggered, or until all tiles have been chosen, leading to a draw. 

        Put simply, it is a simple and interesting logic game that is difficult to win yet
        surprisingly easy to draw.

    AUTHORS:
        Christian Joseph C. Bunyi
        Erin Gabrielle T. Chua

    SECTION:
        S14

    LAST MODIFIED:
        March 25, 2024

    ACKNOWLEDGMENTS:
        1. Functions from Minesweeper, a CCPROG2 Machine Project by Christian Joseph Bunyi and Andre Gabriel Llanes
        2. Text to ASCII Art Generator by @patorjk
        3. ASCII codes from https://theasciicode.com.ar/
*/


// preprocessor directives
#include <conio.h>
#include <stdio.h>
#include <windows.h>

#define True 1
#define False 0
#define BOARD_ROWS 6
#define BOARD_COLUMNS 6

#define SHORT_SLEEP 500
#define LONG_SLEEP 1000

#define WON_A_OUTCOME "WonA"
#define WON_B_OUTCOME "WonB"
#define DRAW_OUTCOME "Draw"
#define QUIT_OUTCOME "Quit"

#define HISTORY_DIRECTORY "QuadHistory.txt"

typedef int bool;
typedef char String30[31];

struct C {
    int n;
    int arr[4][2];
};

struct F {
    int n;
    int arr[36][2];
};

struct Game {
    int gameboard[BOARD_ROWS][BOARD_COLUMNS];
    bool good;
    bool over;
    bool next;
    struct C C1;
    struct C C2;
    struct F F1;
    struct F F2;
    struct F F3;
    int result;
};

struct Names {
	String30 Name_A;
	String30 Name_B;
};

struct History {
    int totalGames;
    int wins;
    int draws;
    int quits;

    String30 outcomes[1001];
    struct Names names[1001];
};


void MainMenu();


/*
    @brief: prints all relevant game sets and variables for testing and debugging purposes

    @param: game - a struct Game instance representing the current game
*/
void Debugger(struct Game game) {
    int i;

    // game variables
    printf("\n---------------------------\n\n");
    printf("good: %d\nover: %d\nnext: %d\n\n", game.good, game.over, game.next);

    // C1
    printf("C1: {");
    for (i = 0; i < game.C1.n; i++) {
        printf("(%d, %d), ", game.C1.arr[i][0], game.C1.arr[i][1]);
    }
    printf("}\n\n");

    // C2
    printf("C2: {");
    for (i = 0; i < game.C2.n; i++) {
        printf("(%d, %d), ", game.C2.arr[i][0], game.C2.arr[i][1]);
    }
    printf("}\n\n");

    // F1
    printf("F1: {");
    for (i = 0; i < game.F1.n; i++) {
        printf("(%d, %d), ", game.F1.arr[i][0], game.F1.arr[i][1]);
    }
    printf("}\n\n");

    // F2
    printf("F2: {");
    for (i = 0; i < game.F2.n; i++) {
        printf("(%d, %d), ", game.F2.arr[i][0], game.F2.arr[i][1]);
    }
    printf("}\n\n");

    // F3
    printf("F3: {");
    for (i = 0; i < game.F3.n; i++) {
        printf("(%d, %d), ", game.F3.arr[i][0], game.F3.arr[i][1]);
    }
    printf("}\n");

    printf("\n---------------------------\n");
}


/*
    @brief: clears the input buffer after a user response
*/
void ClearInputBuffer() {
    while (getchar() != '\n'); // gets user input from the input stream until a newline is encountered
}


/*
    @brief: creates a struct C instance with all members initialized to defaults

    @return: a newly initialized struct C instance
*/
struct C CreateNewC() {
    int i;
    struct C NewC;
    
    NewC.n = 0;
    for (i = 0; i < 4; i++) {
        NewC.arr[i][0] = 0;
        NewC.arr[i][1] = 0;
    }

    return NewC;
}


/*
    @brief: creates a struct F instance with all members initialized to defaults

    @return: a newly initialized struct F instance
*/
struct F CreateNewF() {
    int i;
    struct F NewF;
    
    NewF.n = 0;
    for (i = 0; i < 36; i++) {
        NewF.arr[i][0] = 0;
        NewF.arr[i][1] = 0;
    }

    return NewF;
}


/*
    @brief: creates a struct Game instance with all members initialized to defaults

    @return: a newly initialized struct Game instance
*/
struct Game CreateNewGame() {
    int i, j;
    struct Game NewGame;

    for (i = 0; i < BOARD_ROWS; i++) {
        for (j = 0; j < BOARD_COLUMNS; j++) {
            NewGame.gameboard[i][j] = 0;
        }
    }
    NewGame.good = False;
    NewGame.over = False;
    NewGame.next = False;
    NewGame.C1 = CreateNewC();
    NewGame.C2 = CreateNewC();
    NewGame.F1 = CreateNewF();
    NewGame.F2 = CreateNewF();
    NewGame.F3 = CreateNewF();
    NewGame.result = 0;

    return NewGame;
}


/*
    @brief: saves the lifetime game history from QuadHistory.txt into a struct History instance

    @return: a struct History instance containing updated history information
*/
struct History LoadHistory() {
    int i;
    struct History history;
    FILE *fp;

    fp = fopen(HISTORY_DIRECTORY, "r");

    if (fp == NULL) {
        history.totalGames = 0;
        history.wins = 0;
        history.draws = 0;
        history.quits = 0;

        return history;
    }

    fscanf(fp, "%d %d %d %d", &history.totalGames, &history.wins, &history.draws, &history.quits);

    for (i = 0; i < history.totalGames; i++) {
        fscanf(fp, "%s %s %s", history.outcomes[i], history.names[i].Name_A, history.names[i].Name_B);
    }

    fclose(fp);
    return history;
}


/*
    @brief: updates QuadHistory.txt based on game information

    @params: game - struct Game instance storing game information
    @params: names - struct Names instance storing both players' names
    @params: history - pointer to a struct History instance storing historical game information
*/
void UpdateHistory(struct Game game, struct Names names, struct History *history) {
    int i;
    FILE *fp;

    if (game.result == 1) { // player A won
        history->wins++;
        strcpy(history->outcomes[history->totalGames], WON_A_OUTCOME);
    }
    else if (game.result == 2) { // player B won
        history->wins++;
        strcpy(history->outcomes[history->totalGames], WON_B_OUTCOME);
    }
    else if (game.result == 3) { // draw
        history->draws++;
        strcpy(history->outcomes[history->totalGames], DRAW_OUTCOME);
    }
    else if (game.result == 4) { // quit
        history->quits++;
        strcpy(history->outcomes[history->totalGames], QUIT_OUTCOME);
    }

    history->names[history->totalGames++] = names;

    fp = fopen(HISTORY_DIRECTORY, "w");
    if (fp == NULL) return;

    fprintf(fp, "%d\n%d\n%d\n%d\n\n", history->totalGames, history->wins, history->draws, history->quits);

    for (i = 0; i < history->totalGames; i++) {
        fprintf(fp, "%s %s %s\n", history->outcomes[i], history->names[i].Name_A, history->names[i].Name_B);
    }

    fclose(fp);
}


/*
    @brief: resets historical game information in QuadHistory.txt
*/
void ResetHistory() {
    FILE *fp;
    char input;

    fp = fopen(HISTORY_DIRECTORY, "w");

    fprintf(fp, "%d\n%d\n%d\n%d\n\n", 0, 0, 0, 0);

    fclose(fp);

    system("cls");
    printf("\nHistory successfully resetted.\n\n");

    while (input != '1') {
        printf("\nEnter [1] to return to main menu: ");
        scanf(" %c", &input);
        ClearInputBuffer();
    }

    MainMenu();
}


/*
    @brief: prints the current game board

    @pre: assumes each integer in the 2d array is between 0 and 4
    @pre: assumes posRow and posColumn are between 0 and 5

    @param: gameboard - a 2D array of integers denoting each board tile's state
    @param: posRow - the row of the board indicator's current location
    @param: posColumn - the column of the board indicator's current location
*/
void PrintGameBoard(int gameboard[][BOARD_COLUMNS], int posRow, int posColumn) {
    int i, j, k;
    int state;
    char c;

    printf("\nBoard:\n\n");
    printf("%6d", 1);
    for (i = 2; i <= BOARD_COLUMNS; i++)
        printf("%4d", i);
    printf("\n");

    // print the upper border
    printf("%4c%c%c%c", 218, 196, 196, 196);
    for (i = 0; i < BOARD_COLUMNS - 1; i++)
        printf("%c%c%c%c", 194, 196, 196, 196);
    printf("%c\n", 191);

    for (i = 0; i < BOARD_ROWS; i++) {
        printf("%-3d", i + 1);

        for (j = 0; j < BOARD_COLUMNS; j++) {
            state = gameboard[i][j];

            printf("%c", 179);

            if (i == posRow && j == posColumn) {
                printf(">");
            }
            else {
                printf(" ");
            }
            
            if (state == 0) { // tile is unchosen
                c = 177;
            }
            else if (state == 1) { // tile is credited to Player A
                c = 'a';
            }
            else if (state == 2) { // tile is credited to Player B
                c = 'b';
            }
            else if (state == 3) { // quadrant is credited to Player A
                c = 'A';
            }
            else if (state == 4) { // quadrant is credited to Player B
                c = 'B';
            }

            printf("%c", c);

            if (i == posRow && j == posColumn) {
                printf("<");
            }
            else {
                printf(" ");
            }
        }

        printf("%c\n", 179);

        if (i < BOARD_ROWS - 1) { // print the middle border
            printf("%4c%c%c%c", 195, 196, 196, 196);
            for (k = 0; k < 5; k++)
                printf("%c%c%c%c", 197, 196, 196, 196);
            printf("%c\n", 180);
        }
    }

    // print the lower border
    printf("%4c%c%c%c", 192, 196, 196, 196);
    for (i = 0; i < BOARD_COLUMNS - 1; i++)
        printf("%c%c%c%c", 193, 196, 196, 196);
    printf("%c\n\n", 217);
}


/*
    @brief: checks if a tile has not yet been credited, i.e., if it is currently a member of F3

    @pre: assumes posRow and posColumn are between 1 and 6

    @param: posRow - the chosen tile's row
    @param: posColumn - the chosen tile's column
    @param: F3 - the set of uncredited board tiles, i.e., F - (F1 U F2)

    @return: True if the tile is currently a member of F3; otherwise, False
*/
bool PosInF3(int posRow, int posColumn, struct F F3) {
    int i;

    for (i = 0; i < F3.n; i++) {
        if (posRow == F3.arr[i][0] && posColumn == F3.arr[i][1]) {
            return True;
        }
    }

    return False;
}


/*
    @brief: removes a tile from F3 and updates F3

    @pre: assumes posRow and posColumn are between 1 and 6

    @param: posRow - the chosen tile's row
    @param: posColumn - the chosen tile's column
    @param: F3 - pointer to the set of uncredited board tiles, i.e., F - (F1 U F2)
*/
void RemoveFromF3(int posRow, int posColumn, struct F *F3) {
    int i, j;

    for (i = 0; i < F3->n; i++) {
        if (posRow == F3->arr[i][0] && posColumn == F3->arr[i][1]) {
            // shift all tiles to the left starting from the tile after the removed tile
            for (j = i + 1; j < F3->n; j++) {
                F3->arr[j - 1][0] = F3->arr[j][0];
                F3->arr[j - 1][1] = F3->arr[j][1];
            }
            F3->n--;
            return;
        }
    }
}


/*
    @brief: checks if any quadrant can be credited to the current player

    @param: game - pointer to the struct Game instance representing the current game
    @param: S - the set containing subsets comprising each quadrant's special tiles

    @return: True if any quadrant can be credited to the current player; otherwise, False
*/
bool HasNewQuadrant(struct Game *game, int S[][6][2]) {
    int i, j;
    struct C *C;
    struct F *F;
    int row, column;
    bool hasQ1, hasQ2, hasQ3, hasQ4;
    hasQ1 = hasQ2 = hasQ3 = hasQ4 = False;
    bool hasQuadrant, hasTile;

    if (game->next) { // player B
        C = &game->C1;
        F = &game->F1;
    }
    else if (!game->next) { // player A
        C = &game->C2;
        F = &game->F2;
    }

    for (i = 0; i < C->n; i++) {
        row = C->arr[i][0];
        column = C->arr[i][1];

        if (row == 1 && column == 1) {
            hasQ1 = True;
        }
        else if (row == 2 && column == 2) {
            hasQ2 = True;
        }
        else if (row == 1 && column == 2) {
            hasQ3 = True;
        }
        else if (row == 2 && column == 1) {
            hasQ4 = True;
        }
    }
    
    if (!hasQ1) { // check if quadrant 1 can be credited to the current player
        hasQuadrant = True;

        for (i = 0; i < 5; i++) { // iterate through each of the quadrant control tiles
            hasTile = False;

            for (j = 0; j < F->n; j++) { // iterate through each of the player's chosen tiles
                if (F->arr[j][0] == S[0][i][0] && F->arr[j][1] == S[0][i][1]) {
                    hasTile = True;
                }
            }

            if (!hasTile) {
                hasQuadrant = False;
            }
        }

        if (hasQuadrant) { // quadrant 1 can be credited to the current player
            game->gameboard[0][0] = 3 + game->next;
            game->gameboard[0][2] = 3 + game->next;
            game->gameboard[1][1] = 3 + game->next;
            game->gameboard[2][0] = 3 + game->next;
            game->gameboard[2][2] = 3 + game->next;

            return True;
        }
    }
    if (!hasQ2) { // check if quadrant 2 can be credited to the current player
        hasQuadrant = True;

        for (i = 0; i < 5; i++) { // iterate through each of the quadrant control tiles
            hasTile = False;

            for (j = 0; j < F->n; j++) { // iterate through each of the player's chosen tiles
                if (F->arr[j][0] == S[1][i][0] && F->arr[j][1] == S[1][i][1]) {
                    hasTile = True;
                }
            }

            if (!hasTile) {
                hasQuadrant = False;
            }
        }

        if (hasQuadrant) { // quadrant 2 can be credited to the current player
            game->gameboard[3][3] = 3 + game->next;
            game->gameboard[3][5] = 3 + game->next;
            game->gameboard[4][4] = 3 + game->next;
            game->gameboard[5][3] = 3 + game->next;
            game->gameboard[5][5] = 3 + game->next;
            
            return True;
        }
    }
    if (!hasQ3) { // check if quadrant 3 can be credited to the current player
        hasQuadrant = True;

        for (i = 0; i < 5; i++) { // iterate through each of the quadrant control tiles
            hasTile = False;

            for (j = 0; j < F->n; j++) { // iterate through each of the player's chosen tiles
                if (F->arr[j][0] == S[2][i][0] && F->arr[j][1] == S[2][i][1]) {
                    hasTile = True;
                }
            }

            if (!hasTile) {
                hasQuadrant = False;
            }
        }

        if (hasQuadrant) { // quadrant 3 can be credited to the current player
            game->gameboard[0][4] = 3 + game->next;
            game->gameboard[1][3] = 3 + game->next;
            game->gameboard[1][4] = 3 + game->next;
            game->gameboard[1][5] = 3 + game->next;
            game->gameboard[2][4] = 3 + game->next;
            
            return True;
        }
    }
    if (!hasQ4) { // check if quadrant 4 can be credited to the current player
        hasQuadrant = True;

        for (i = 0; i < 6; i++) { // iterate through each of the quadrant control tiles
            hasTile = False;

            for (j = 0; j < F->n; j++) { // iterate through each of the player's chosen tiles
                if (F->arr[j][0] == S[3][i][0] && F->arr[j][1] == S[3][i][1]) {
                    hasTile = True;
                }
            }

            if (!hasTile) {
                hasQuadrant = False;
            }
        }

        if (hasQuadrant) { // quadrant 4 can be credited to the current player
            game->gameboard[3][0] = 3 + game->next;
            game->gameboard[3][2] = 3 + game->next;
            game->gameboard[4][0] = 3 + game->next;
            game->gameboard[4][2] = 3 + game->next;
            game->gameboard[5][0] = 3 + game->next;
            game->gameboard[5][2] = 3 + game->next;
            
            return True;
        }
    }

    return False;
}


/*
    @brief: processes the current player's move and updates game circumstances correspondingly

    @pre: assumes posRow and posColumn are between 1 and 6

    @param: posRow - the chosen tile's row
    @param: posColumn - the chosen tile's column
    @param: game - pointer to the struct Game instance representing the current game
    @param: S - the set containing subsets comprising each quadrant's special tiles
*/
void NextPlayerMove(int posRow, int posColumn, struct Game *game, int S[][6][2]) {
    int c = (posRow - 1) / 3 + 1;
    int d = (posColumn - 1) / 3 + 1;

    if (!game->good) {
        if (PosInF3(posRow, posColumn, game->F3)) { // check if the tile has not been chosen yet
            game->good = !game->good;

            if (game->next) { // player B
                game->F1.arr[game->F1.n][0] = posRow;
                game->F1.arr[game->F1.n][1] = posColumn;
                game->F1.n++;
                game->gameboard[posRow - 1][posColumn - 1] = 2;
            }
            else if (!game->next) { // player A
                game->F2.arr[game->F2.n][0] = posRow;
                game->F2.arr[game->F2.n][1] = posColumn;
                game->F2.n++;
                game->gameboard[posRow - 1][posColumn - 1] = 1;
            }

            RemoveFromF3(posRow, posColumn, &game->F3); // remove the current tile from F3
        }
    }
    
    if (game->good) {
        if (HasNewQuadrant(game, S)) { // check if the current player secured a new quadrant
            if (game->next) { // player B
                game->C1.arr[game->C1.n][0] = c;
                game->C1.arr[game->C1.n][1] = d;
                game->C1.n++;
            }
            else if (!game->next) { // player A
                game->C2.arr[game->C2.n][0] = c;
                game->C2.arr[game->C2.n][1] = d;
                game->C2.n++;
            }
        }

        game->good = !game->good;
    }
}


/*
    @brief: checks if the game is over and updates game circumstances correspondingly

    @param: game - pointer to the struct Game instance representing the current game
*/
void GameOverCondition(struct Game *game) {
    int i;
    int row, column;
    bool hasQ1, hasQ2, hasQ3, hasQ4;
    hasQ1 = hasQ2 = hasQ3 = hasQ4 = False;

    if (game->F3.n == 0) { // check if the entire board has been occupied
        game->over = True;
        game->result = 3;
        return;
    }

    for (i = 0; i < game->C1.n; i++) { // check what quadrants player B has occupied
        row = game->C1.arr[i][0];
        column = game->C1.arr[i][1];

        if (row == 1 && column == 1) {
            hasQ1 = True;
        }
        else if (row == 2 && column == 2) {
            hasQ2 = True;
        }
        else if (row == 1 && column == 2) {
            hasQ3 = True;
        }
        else if (row == 2 && column == 1) {
            hasQ4 = True;
        }
    }

    if ((hasQ1 && hasQ2) || (hasQ3 && hasQ4)) { // check if player B has occupied two opposite quadrants
        game->over = True;
        game->result = 1;
        return;
    }

    hasQ1 = hasQ2 = hasQ3 = hasQ4 = False;

    for (i = 0; i < game->C2.n; i++) { // check what quadrants player A has occupied
        row = game->C2.arr[i][0];
        column = game->C2.arr[i][1];

        if (row == 1 && column == 1) {
            hasQ1 = True;
        }
        else if (row == 2 && column == 2) {
            hasQ2 = True;
        }
        else if (row == 1 && column == 2) {
            hasQ3 = True;
        }
        else if (row == 2 && column == 1) {
            hasQ4 = True;
        }
    }

    if ((hasQ1 && hasQ2) || (hasQ3 && hasQ4)) { // check if player A has occupied two opposite quadrants
        game->over = True;
        game->result = 2;
    }
}


/*
    @brief: prints the result if the game is over and updates game circumstances correspondingly

    @param: game - pointer to the struct Game instance representing the current game
    @param: name - pointer to struct Names that store the players' names
*/
void GameOver(struct Game *game, struct Names *name) {
    if (game->over) {
        system("cls");
        PrintGameBoard(game->gameboard, -1, -1);

        Sleep(LONG_SLEEP);

        if (game->result == 1) { // player A won
            printf("%s wins!", name->Name_A);
        }
        else if (game->result == 2) { // player B won
            printf("%s wins!", name->Name_B);
        }
        else if (game->result == 3) { // draw
            printf("Draw!");
        }
        else if (game->result == 4) { // quit
            printf("Game successfully quitted.");
        }
        printf("\n\n");

        Sleep(LONG_SLEEP);
    }
    else if (!game->over) {
        game->next = !game->next; // switches the turn to the other player
    }
}


/*
    @brief: prints the results of previous games from QuadHistory.txt
*/
void ViewHistory() {
	FILE *fp;
    char input;

    int i;
    int totalGames;
    int wins, draws, quits;

    String30 outcome, playerA, playerB;

	fp = fopen(HISTORY_DIRECTORY, "r");

    if (fp == NULL) {
        fclose(fp);

        fp = fopen(HISTORY_DIRECTORY, "w");
        fprintf(fp, "%d\n%d\n%d\n%d\n\n", 0, 0, 0, 0);
        fclose(fp);

        fp = fopen(HISTORY_DIRECTORY, "r");
    }

    fscanf(fp, "%d %d %d %d", &totalGames, &wins, &draws, &quits);

    system("cls");

    printf("\n---------- LIFETIME STATISTICS ----------\n\n");

    if (totalGames == 0) {
        printf("No previous games.\n");
    }
	else {
        printf("Lifetime Games Played: %d\n", totalGames);
        printf("Win Rate: %.2f%% (%d wins)\n", wins * 1.0 / totalGames * 100, wins);
        printf("Draw Rate: %.2f%% (%d draws)\n", draws * 1.0 / totalGames * 100, draws);
        printf("Quit Rate: %.2f%% (%d quits)\n", quits * 1.0 / totalGames * 100, quits);

		printf("\n---------- PREVIOUS GAME RESULTS ----------\n\n");

        for (i = 0; i < totalGames; i++) {
            printf("Game %d: ", i + 1);

            fscanf(fp, "%s %s %s", outcome, playerA, playerB);

            if (strcmp(outcome, WON_A_OUTCOME) == 0) {
                printf("[WIN] %s won against %s.", playerA, playerB);
            }
            else if (strcmp(outcome, WON_B_OUTCOME) == 0) {
                printf("[WIN] %s won against %s.", playerB, playerA);
            }
            else if (strcmp(outcome, DRAW_OUTCOME) == 0) {
                printf("[DRAW] %s and %s drew the game.", playerA, playerB);
            }
            else if (strcmp(outcome, QUIT_OUTCOME) == 0) {
                printf("[QUIT] %s and %s quit the game.", playerA, playerB);
            }

            printf("\n");
        }
	}

    printf("\n-------------------------------------\n\n");

    fclose(fp);
    
    while (input != '1') {
        printf("\nEnter [1] to return to main menu: ");
        scanf(" %c", &input);
        ClearInputBuffer();
    }
	
    MainMenu();
}


/*
    @brief: waits for the user to press a key and modifies the board indicator's
        current row and column accordingly if an arrow key has been pressed

    @param: currRow - pointer to the board indicator's current row
    @param: currColumn - pointer to the board indicator's current column

    @return: 1 if the enter key has been pressed; otherwise, 0
*/
int DetectKeyPress(int *currRow, int *currColumn) {
    int key;

    key = getch();

    if (key == 13) { // enter key
        return 1;
    }
    else if (key == 27) { // escape key
        return -1;
    }
    else if (key == 0 || key == 224) { // arrow key press
        key = getch();

        if (key == 72) { // up arrow key
            if (*currRow != 0) {
                (*currRow)--;
            }
        }
        else if (key == 80) { // down arrow key
            if (*currRow != BOARD_ROWS - 1) {
                (*currRow)++;
            }
        }
        else if (key == 75) { // left arrow key
            if (*currColumn != 0) {
                (*currColumn)--;
            }
        }
        else if (key == 77) { // right arrow key
            if (*currColumn != BOARD_COLUMNS - 1) {
                (*currColumn)++;
            }
        }
    }

    return 0;
}


/*
    @brief: handles the game logic and keeps the game running until the game results in a win,
        draw, or quit
*/
void PlayGame() {
	
	system("cls");
	
	// applicable sets
    int S[4][6][2] = {
        {{1, 1}, {1, 3}, {2, 2}, {3, 1}, {3, 3}},
        {{4, 4}, {4, 6}, {5, 5}, {6, 4}, {6, 6}},
        {{1, 5}, {2, 4}, {2, 5}, {2, 6}, {3, 5}},
        {{4, 1}, {4, 3}, {5, 1}, {5, 3}, {6, 1}, {6, 3}}
    };

    // prerequisites
    struct Game game = CreateNewGame();
    struct Names name;
    struct History history = LoadHistory();

    // local variables
    int i, j;
    int index;
    int posRow = 0;
    int posColumn = 0;
    char input;

    bool keyPressed;
    bool posInF3;
    bool escaped = 0;
    
    int a = 0, b = 0;
    
    while (a <= 0) {
    	printf("\nInput name for player A: ");
    	scanf("%s", name.Name_A);
        ClearInputBuffer();
    	
    	printf("Hello, %s! Your tiles will be the A tiles, goodluck and have fun!\n\n", name.Name_A);
    	a = strlen(name.Name_A);
	}
	
    Sleep(LONG_SLEEP);

	while (b <= 0) {
    	printf("Input name for player B: ");
    	scanf("%s", name.Name_B);
        ClearInputBuffer();
    	
    	printf("Hello, %s! Your tiles will be the B tiles, goodluck and have fun!", name.Name_B);
    	b = strlen(name.Name_B);
	}
	
	Sleep(LONG_SLEEP);
    
    system("cls");

    // initialize F3
    game.F3.n = BOARD_ROWS * BOARD_COLUMNS;
    for (i = 0; i < BOARD_ROWS; i++) {
        for (j = 0; j < BOARD_COLUMNS; j++) {
            index = i * BOARD_ROWS + j;
            game.F3.arr[index][0] = i + 1;
            game.F3.arr[index][1] = j + 1;
        }
    }

    // loop the game proper while it is not yet over
    while (!game.over) {
        // display the updated game board
        do {
            system("cls");
            PrintGameBoard(game.gameboard, posRow, posColumn);

            if (game.next) {
                printf("It's (Player B) %s's turn!", name.Name_B);
            }
            else if (!game.next) {
                printf("It's (Player A) %s's turn!", name.Name_A);
            }

            printf("\n\nNavigate the game board with your arrow keys. Press 'Enter' to select the current tile or 'Escape' to quit the game.");

            keyPressed = DetectKeyPress(&posRow, &posColumn);
            posInF3 = PosInF3(posRow + 1, posColumn + 1, game.F3);

            if (keyPressed == -1) {
                game.result = 4;
                game.over = True;
                escaped = 1;
                posInF3 = True;
            }

            if (keyPressed == 1 && !posInF3) {
                printf("\n\nTile already chosen! Please choose another tile.");
                Sleep(LONG_SLEEP);
            }

            printf("\n\n");
        } while (!((keyPressed == 1 && posInF3) || escaped));

        system("cls");
        PrintGameBoard(game.gameboard, posRow, posColumn);
        
        // process the current player's move
        if (!escaped) {
            NextPlayerMove(posRow + 1, posColumn + 1, &game, S);
            GameOverCondition(&game);
        }

        GameOver(&game, &name);
    }
    
    // updating the statistics file and prompt to return to menu
    if (game.over) {
    	UpdateHistory(game, name, &history);
    	
    	while (input != '1'){
            printf("Enter [1] to return to main menu: ");
            scanf(" %c", &input);
            ClearInputBuffer();
        }

		MainMenu();
    }
}


/*
    @brief: Prints the art for main menu
*/
void PrintMenu() {
	
	printf("\n\n\n");
	printf("\t\t\t\t\t\t .----------------. .----------------. .----------------. .----------------. \n");
	printf("\t\t\t\t\t\t| .--------------. | .--------------. | .--------------. | .--------------. |\n");
	printf("\t\t\t\t\t\t| |    ___       | | | _____  _____ | | |      __      | | |  ________    | |\n");
	printf("\t\t\t\t\t\t| |  .'   '.     | | ||_   _||_   _|| | |     /  \\     | | | |_   ___ `.  | |\n");
	printf("\t\t\t\t\t\t| | /  .-.  \\    | | |  | |    | |  | | |    / /\\ \\    | | |   | |   `. \\ | |\n");
	printf("\t\t\t\t\t\t| | | |   | |    | | |  | '    ' |  | | |   / ____ \\   | | |   | |    | | | |\n");
	printf("\t\t\t\t\t\t| | \\  `-'  \\_   | | |   \\ `--' /   | | | _/ /    \\ \\_ | | |  _| |___.' / | |\n");
	printf("\t\t\t\t\t\t| |  `.___.\\__|  | | |    `.__.'    | | ||____|  |____|| | | |________.'  | |\n");
	printf("\t\t\t\t\t\t| |              | | |              | | |              | | |              | |\n");
	printf("\t\t\t\t\t\t| '--------------' | '--------------' | '--------------' | '--------------' |\n");
	printf("\t\t\t\t\t\t '----------------' '----------------' '----------------' '----------------' \n");
	
	printf("\t\t\t\t\t .----------------. .----------------. .-----------------..----------------. .----------------. \n");
	printf("\t\t\t\t\t| .--------------. | .--------------. | .--------------. | .--------------. | .--------------. |\n");
	printf("\t\t\t\t\t| |  _______     | | |      __      | | | ____  _____  | | |  _________   | | |    _______   | |\n");
	printf("\t\t\t\t\t| | |_   __ \\    | | |     /  \\     | | ||_   \\|_   _| | | | |  _   _  |  | | |   /  ___  |  | |\n");
	printf("\t\t\t\t\t| |   | |__) |   | | |    / /\\ \\    | | |  |   \\ | |   | | | |_/ | | \\_|  | | |  |  (__ \\_|  | |\n");
	printf("\t\t\t\t\t| |   |  __ /    | | |   / ____ \\   | | |  | |\\ \\| |   | | |     | |      | | |   '.___`-.   | |\n");
	printf("\t\t\t\t\t| |  _| |  \\ \\_  | | | _/ /    \\ \\_ | | | _| |_\\   |_  | | |    _| |_     | | |  |`\\____) |  | |\n");
	printf("\t\t\t\t\t| | |____| |___| | | ||____|  |____|| | ||_____|\\____| | | |   |_____|    | | |  |_______.'  | |\n");
	printf("\t\t\t\t\t| |              | | |              | | |              | | |              | | |              | |\n");
	printf("\t\t\t\t\t| '--------------' | '--------------' | '--------------' | '--------------' | '--------------' |\n");
	printf("\t\t\t\t\t '----------------' '----------------' '----------------' '----------------' '----------------' \n\n");
	
	printf("\t\t\t\t\t\t\t\t\t\t[P]lay Game\n");
	printf("\t\t\t\t\t\t\t\t\t\t[G]ame Mechanics\n");
	printf("\t\t\t\t\t\t\t\t\t\t[V]iew History\n");
    printf("\t\t\t\t\t\t\t\t\t\t[R]eset History\n");
	printf("\t\t\t\t\t\t\t\t\t\t[E]xit Program\n\n");
	
	return;
	
}


/*
    @brief: Function for game mechanics option in main menu
*/
void GameMechanics() {
	
	system("cls");
	
	int i, j, k;
	char input;
	char c;
	
	printf("\n ____  _  _  ___  ____  ____  __  __  ___  ____  ____  _____  _  _  ___ \n");
	printf("(_  _)( \\( )/ __)(_  _)(  _ \\(  )(  )/ __)(_  _)(_  _)(  _  )( \\( )/ __)\n");
	printf(" _)(_  )  ( \\__ \\  )(   )   / )(__)(( (__   )(   _)(_  )(_)(  )  ( \\__ \\\n");
	printf("(____)(_)\\_)(___/ (__) (_)\\_)(______)\\___) (__) (____)(_____)(_)\\_)(___/\n\n");
	
	printf("\n\"Quadrants\" is a 2-player game where players are given a [6 by 6 board], \n");
    printf("taking turns choosing an unoccupied tile on the board to claim a tile. The\n");
    printf("[6 by 6 board] is further divided into 4 quadrants, creating 4 [3 by 3 grids]\n");
    printf("that consist of their own individual patterns.\n\n");
	
    printf("QUADRANTS AND THEIR PATTERNS:\n\n");

	// printing the pattern to get credited a quadrant
	printf("%6d", 1);
    for (i = 2; i <= BOARD_COLUMNS; i++)
        printf("%4d", i);
    printf("\n");
    
	// print the upper border
    printf("%4c%c%c%c", 218, 196, 196, 196);
    for (i = 0; i < BOARD_COLUMNS - 1; i++)
        printf("%c%c%c%c", 194, 196, 196, 196);
    printf("%c\n", 191);
	
	for (i = 0; i < BOARD_ROWS; i++) {
        printf("%-3d", i + 1);

        for (j = 0; j < BOARD_COLUMNS; j++) {

            printf("%c ", 179);
            
            if ((i == 0 && (j == 0 || j == 2)) || (i == 1 && j == 1) || (i == 2 && (j == 0 || j == 2))) { 
                c = 49;
            }
            else if ((i == 3 && (j == 3 || j == 5)) || (i == 4 && j == 4) || (i == 5 && (j == 3 || j == 5))) { 
                c = 50;
            }
            else if ((i == 0 && j == 4) || (i == 1 && (j == 3 || j == 4 || j == 5)) || (i == 2 && j == 4)) { 
                c = 51;
            }
            else if ((i == 3 && (j == 0 || j == 2)) || (i == 4 && (j == 0 || j == 2)) || (i == 5 && (j == 0 || j == 2))) { 
                c = 52;
            }
            else 
            	c = 178;

            printf("%c ", c);
        }

        printf("%c\n", 179);

        if (i < BOARD_ROWS - 1) { // print the middle border
            printf("%4c%c%c%c", 195, 196, 196, 196);
            for (k = 0; k < 5; k++)
                printf("%c%c%c%c", 197, 196, 196, 196);
            printf("%c\n", 180);
        }
    }
        
    // print the lower border 
    printf("%4c%c%c%c", 192, 196, 196, 196);
    for (i = 0; i < BOARD_COLUMNS - 1; i++)
        printf("%c%c%c%c", 193, 196, 196, 196);
    printf("%c\n\n", 217);
	
    printf("\nOnce a player completes a quadrant's pattern, they get credited that\n");
    printf("quadrant. A player loses when 2 quadrants diagonal to each other are\n");
    printf("credited to them. Thus, players must avoid occupying special tiles from\n");
    printf("diagonally adjacent quadrants. If all the tiles have been occupied and\n");
    printf("neither player is credited with any 2 diagonally adjacent quadrants, the\n");
    printf("game results in a draw.\n\n");
	
	//printing the patterns of the diagonal quadrants 
	printf("LOSE CONDITION 1:\n\n");
	printf("%6d", 1);
    for (i = 2; i <= BOARD_COLUMNS; i++)
        printf("%4d", i);
    printf("\n");
    
	// print the upper border
    printf("%4c%c%c%c", 218, 196, 196, 196);
    for (i = 0; i < BOARD_COLUMNS - 1; i++)
        printf("%c%c%c%c", 194, 196, 196, 196);
    printf("%c\n", 191);
    
    for (i = 0; i < BOARD_ROWS; i++) {
        printf("%-3d", i + 1);

        for (j = 0; j < BOARD_COLUMNS; j++) {

            printf("%c ", 179);
            
            if ((i == 0 && (j == 0 || j == 1 || j == 2)) || (i == 1 && (j == 0 || j == 1 || j == 2)) || (i == 2 && (j == 0 || j  == 1 || j == 2))) { 
                c = 178;
            }
            else if ((i == 3 && (j == 3 || j == 4 || j == 5)) || (i == 4 && (j == 3 || j == 4 || j == 5)) || (i == 5 && (j == 3 || j  == 4 || j == 5))) { 
                c = 178;
            }
            else 
            	c = 32;

            printf("%c ", c);
        }

        printf("%c\n", 179);

        if (i < BOARD_ROWS - 1) { // print the middle border
            printf("%4c%c%c%c", 195, 196, 196, 196);
            for (k = 0; k < 5; k++)
                printf("%c%c%c%c", 197, 196, 196, 196);
            printf("%c\n", 180);
        }
    }
        
    // print the lower border 
    printf("%4c%c%c%c", 192, 196, 196, 196);
    for (i = 0; i < BOARD_COLUMNS - 1; i++)
        printf("%c%c%c%c", 193, 196, 196, 196);
    printf("%c\n\n", 217);
	
	printf("LOSE CONDITION 2:\n\n");
	printf("%6d", 1);
    for (i = 2; i <= BOARD_COLUMNS; i++)
        printf("%4d", i);
    printf("\n");
    
	// print the upper border
    printf("%4c%c%c%c", 218, 196, 196, 196);
    for (i = 0; i < BOARD_COLUMNS - 1; i++)
        printf("%c%c%c%c", 194, 196, 196, 196);
    printf("%c\n", 191);
    
    for (i = 0; i < BOARD_ROWS; i++) {
        printf("%-3d", i + 1);

        for (j = 0; j < BOARD_COLUMNS; j++) {

            printf("%c ", 179);
                        
            if ((i == 0 && (j == 3 || j == 4 || j == 5)) || (i == 1 && (j == 3 || j == 4 || j == 5)) || (i == 2 && (j == 3 || j == 4 || j == 5))) { 
                c = 178;
            }
            else if ((i == 3 && (j == 0 || j == 1 || j == 2)) || (i == 4 && (j == 0 || j == 1 || j == 2)) || (i == 5 && (j == 0 || j == 1 || j == 2))) { 
                c = 178;
            }
            else 
            	c = 32;

            printf("%c ", c);
        }

        printf("%c\n", 179);

        if (i < BOARD_ROWS - 1) { // print the middle border
            printf("%4c%c%c%c", 195, 196, 196, 196);
            for (k = 0; k < 5; k++)
                printf("%c%c%c%c", 197, 196, 196, 196);
            printf("%c\n", 180);
        }
    }
        
    // print the lower border 
    printf("%4c%c%c%c", 192, 196, 196, 196);
    for (i = 0; i < BOARD_COLUMNS - 1; i++)
        printf("%c%c%c%c", 193, 196, 196, 196);
    printf("%c\n\n", 217);
    
    //returning to menu
	while (input != '1'){
		printf("\nEnter [1] to return to menu: ");
		scanf(" %c", &input);
		ClearInputBuffer();
	}
	
	MainMenu();
}


/*
    @brief: Menu as to avoid calling on main
*/
void MainMenu() {
	
	system("cls");

    char choice;
    int valid = False;
    
    PrintMenu();
    
    // determining user input
    while (!valid) {
    	
    	printf("\t\t\t\t\t\t\t\t\tEnter chosen program option: ");
    	scanf(" %c", &choice);
        ClearInputBuffer();
    	
    	if(choice != 'P' && choice != 'G' && choice != 'V' && choice != 'R' && choice != 'E')
    		printf("\t\t\t\t\t\t\t\t\tChoice invalid, try again.\n\n");
    	else
    		valid = True;
	}
	
	// redirection
	switch(choice){
		case 'P': PlayGame();
			break;
		case 'G': GameMechanics();
			break;
		case 'V': ViewHistory();
			break;
        case 'R': ResetHistory();
            break;
		case 'E': exit(1);
			break;
	}
}


/*
    @brief: Main function of the program.

    @return: 0 for successful execution; otherwise, a non-zero value corresponding to the status.
*/
int main() {
    
    MainMenu();

    return 0;
}


/*
    This is to certify that this project is our own work, based on our personal
    efforts in studying and applying the concepts learned. We have constructed
    the functions and their respective algorithms and corresponding code by
    ourselves. The program was run, tested, and debugged by our own efforts.
   
    We further certify that we have not copied in part or whole or otherwise
    plagiarized the work of other students and/or persons.

    Christian Joseph C. Bunyi, DLSU ID# 12306452
    Erin Gabrielle T. Chua, DLSU ID# 12306428
*/