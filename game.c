#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

#define BOARD_SIZE 100
#define NUM_PLAYERS 4
#define SIZE 9

// =================== STRUCTURE ===================
struct Player {
    char name[30];
    int position;
    struct Player *next;
};

// =================== CREATE PLAYER ===================
struct Player* createPlayer(char *name) {
    struct Player* p = (struct Player*)malloc(sizeof(struct Player));
    strcpy(p->name, name);
    p->position = 0;
    p->next = NULL;
    return p;
}

// =================== LEADERBOARD ===================
void updateLeaderboard(char *game, char *winner) {
    FILE *f = fopen("leaderboard.txt", "a");
    if (f == NULL) return;
    time_t t; time(&t);
    fprintf(f, "%s | Winner: %s | %s", game, winner, ctime(&t));
    fclose(f);
}

void showLeaderboard() {
    FILE *f = fopen("leaderboard.txt", "r");
    if (f == NULL) {
        printf("\nNo leaderboard data yet!\n");
        return;
    }
    char line[200];
    printf("\n===== 🏆 LEADERBOARD =====\n");
    while (fgets(line, sizeof(line), f))
        printf("%s", line);
    fclose(f);
    printf("==========================\n");
}

// =================== RULES DISPLAY ===================
void showRules(char *game) {
    printf("\n=========== %s RULES ===========\n", game);
    if (strcmp(game, "Snake & Ladder") == 0) {
        printf("1. Each player rolls dice once per turn.\n");
        printf("2. Ladders move you UP; snakes move you DOWN.\n");
        printf("3. First to reach 100 wins.\n");
        printf("4. Exact roll needed to land on 100.\n");
        printf("5. Board uses linked list to rotate turns.\n");
        printf("6. Use ENTER to roll dice, 'L' to view leaderboard, 'E' to exit.\n\n");
    } else {
        printf("1. Sudoku is a 9x9 puzzle.\n");
        printf("2. Fill cells with digits 1–9, following Sudoku rules.\n");
        printf("3. '.' means empty cell.\n");
        printf("4. Enter numbers manually.\n");
        printf("5. Press '3' for leaderboard, '2' to exit.\n\n");
    }
}

// =================== SNAKE & LADDER ===================
void playSnakeLadder() {
    int board[BOARD_SIZE + 1] = {0};
    int ladders[][2] = {{3,22},{5,8},{11,26},{20,29},{36,44},{51,67}};
    int snakes[][2]  = {{27,1},{21,9},{17,4},{19,7},{95,24},{99,78}};

    for (int i=0;i<6;i++) board[ladders[i][0]] = ladders[i][1];
    for (int i=0;i<6;i++) board[snakes[i][0]] = snakes[i][1];

    printf("\nLadders:\n");
    for (int i=0;i<6;i++) printf("🪜 %d → %d\n", ladders[i][0], ladders[i][1]);
    printf("\nSnakes:\n");
    for (int i=0;i<6;i++) printf("🐍 %d → %d\n", snakes[i][0], snakes[i][1]);
    printf("\n");

    // Take player names
    struct Player *head=NULL, *tail=NULL;
    for (int i=1;i<=NUM_PLAYERS;i++) {
        char name[30];
        printf("Enter name for Player %d: ", i);
        scanf("%s", name);
        struct Player *p = createPlayer(name);
        if(!head) head=tail=p;
        else {tail->next=p; tail=p;}
    }
    tail->next=head; // make circular linked list

    srand(time(0));
    struct Player *curr=head;
    int gameOver=0;

    while(!gameOver) {
        printf("\n%s's turn. Press Enter to roll dice (L=leaderboard, E=exit): ", curr->name);
        char input = getchar();
        if (input == '\n') { }
        else if (input == 'L' || input == 'l') {
            showLeaderboard();
            continue;
        } else if (input == 'E' || input == 'e') {
            printf("\nExiting Snake & Ladder...\n");
            return;
        }

        int dice=(rand()%6)+1;
        printf("🎲 %s rolled %d\n", curr->name, dice);
        curr->position+=dice;

        if(curr->position>100){
            curr->position-=dice;
            printf("⚠ Need exact number to win!\n");
        }
        else if(board[curr->position]!=0){
            if(board[curr->position]>curr->position)
                printf("🪜 %s climbs ladder %d → %d\n", curr->name, curr->position, board[curr->position]);
            else
                printf("🐍 %s bitten by snake %d → %d\n", curr->name, curr->position, board[curr->position]);
            curr->position=board[curr->position];
        }

        printf("📍 %s now at %d\n", curr->name, curr->position);

        if(curr->position==100){
            printf("\n🎉 %s wins the game! 🎉\n", curr->name);
            updateLeaderboard("Snake & Ladder", curr->name);
            gameOver=1;
        }
        curr=curr->next;
    }
}

// =================== SUDOKU ===================
void displayBoard(int b[SIZE][SIZE]) {
    printf("\n+-------+-------+-------+\n");
    for(int r=0;r<SIZE;r++){
        printf("| ");
        for(int c=0;c<SIZE;c++){
            if(b[r][c]==0) printf(". ");
            else printf("%d ",b[r][c]);
            if((c+1)%3==0) printf("| ");
        }
        printf("\n");
        if((r+1)%3==0) printf("+-------+-------+-------+\n");
    }
}

bool canPlace(int b[SIZE][SIZE], int r,int c,int n){
    for(int x=0;x<SIZE;x++){
        if(b[r][x]==n||b[x][c]==n) return false;
    }
    int sr=r-r%3, sc=c-c%3;
    for(int i=0;i<3;i++)
        for(int j=0;j<3;j++)
            if(b[sr+i][sc+j]==n) return false;
    return true;
}

void playSudoku(){
    char name[30];
    printf("Enter your name: ");
    scanf("%s", name);

    int board[SIZE][SIZE]={
        {5,3,0,0,7,0,0,0,0},
        {6,0,0,1,9,5,0,0,0},
        {0,9,8,0,0,0,0,6,0},
        {8,0,0,0,6,0,0,0,3},
        {4,0,0,8,0,3,0,0,1},
        {7,0,0,0,2,0,0,0,6},
        {0,6,0,0,0,0,2,8,0},
        {0,0,0,4,1,9,0,0,5},
        {0,0,0,0,8,0,0,7,9}
    };

    int choice;
    while(1){
        displayBoard(board);
        printf("\n1. Enter number\n2. Exit\n3. Show Leaderboard\nChoose: ");
        scanf("%d",&choice);
        if(choice==1){
            int r,c,num;
            printf("Enter row(1-9), col(1-9), num(1-9): ");
            scanf("%d %d %d",&r,&c,&num);
            r--; c--;
            if(r<0||r>=SIZE||c<0||c>=SIZE||num<1||num>9){
                printf("Invalid input!\n"); continue;
            }
            if(board[r][c]!=0){
                printf("Cell already filled!\n"); continue;
            }
            if(canPlace(board,r,c,num)){
                board[r][c]=num;
                printf("✅ Placed %d at (%d,%d)\n",num,r+1,c+1);
            } else printf("❌ Invalid move!\n");
        }
        else if(choice==2){
            printf("Exiting Sudoku...\n");
            updateLeaderboard("Sudoku", name);
            break;
        }
        else if(choice==3){
            showLeaderboard();
        }
        else printf("Invalid choice!\n");
    }
}

// =================== MAIN HUB ===================
int main() {
    while(1){
        printf("\n========== GAME HUB ==========\n");
        printf("1. Play Snake & Ladder\n");
        printf("2. Play Sudoku\n");
        printf("3. Show Leaderboard\n");
        printf("4. Exit\n");
        printf("Choose: ");
        int ch;
        scanf("%d",&ch);
        getchar(); // consume newline
        if(ch==1){ showRules("Snake & Ladder"); playSnakeLadder(); }
        else if(ch==2){ showRules("Sudoku"); playSudoku(); }
        else if(ch==3){ showLeaderboard(); }
        else if(ch==4){ printf("Goodbye!\n"); break; }
        else printf("Invalid option!\n");
    }
    return 0;
}