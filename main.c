#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>


typedef enum player_role{
        FOX   = 1,
        HOUND = 2
}player_role;

void init_board(int board[8][8]){
        for(int i = 7; i >= 0; i--){
                for(int j = 0; j < 8; j++){
                        board[i][j] = 0;
                }
        }

        board[0][4] = FOX;

        board[7][1] = HOUND;
        board[7][3] = HOUND;
        board[7][5] = HOUND;
        board[7][7] = HOUND;
}

void print_board(const int board[8][8]){
        printf("   A B C D E F G H\n");
        for(int i = 7; i >= 0; i--){
                printf(" %d ", i+1);
                for(int j = 0; j < 8; j++){
                        if(board[i][j] == 1){
                                printf("🦊");
                                continue;
                        }
                        if(board[i][j] == 2){
                                printf("🐶");
                                continue;
                        }
                        if(((i+j) % 2) == 0){
                                printf("⬛");
                        }else{
                                printf("⬜");
                        }
                }
                printf(" %d ", i+1);
                puts("");
        }
        printf("   A B C D E F G H\n");
}

int test_move(const player_role role, int board[8][8], char move[10]){
        int curr_x = move[0] - 'a';
        int curr_y = move[1] - '0' - 1;
        int new_x = move[3] - 'a';
        int new_y = move[4] - '0' - 1;
        if(board[new_y][new_x] != 0){
                puts("Position already occupied!");
                return 0;
        }

        if(role == FOX){
                new_x = curr_x;
                new_y = curr_y;
                for(int i = 0; i < 8; i++){
                        for(int j = 0; j < 8; j++){
                                if(board[i][j] == 1){
                                        curr_x = j;
                                        curr_y = i;
                                        break;
                                }
                        }
                }
        }
        printf("(%d:%d) to (%d:%d)\n", curr_x, curr_y, new_x, new_y);

        if(role == HOUND){
                if(board[curr_y][curr_x] != HOUND){
                        printf("Must select a hound. Try again...\n");
                        return 0;
                }
        }

        if(new_y < 0 || new_y > 7 || new_x < 0 || new_x > 8){  // BOARD BOUNDS
                printf("Movement out of board. Try again...\n");
                return 0;
        }

        int delta_x = abs(new_x - curr_x);
        int delta_y = abs(new_y - curr_y);

        if((delta_x > 1) || (delta_y > 1)){                     // MOV DIST
                printf("Illegal move!\n");
                return 0;
        }
        if(delta_x + delta_y != 2){                             // DIAGONALS
                printf("Must move diagonally!\n");
                return 0;
        }
        if(role == HOUND){                                      // HOUND ONLY DOWN
                if(new_y > curr_y){
                        printf("Hound cant move up!\n");
                        return 0;
                }
        }

        return 1;
}

int move_fox(int board[8][8], char move[10]){
        int x = move[0] - 'a';
        int y = move[1] - '0' - 1;

        int curr_x, curr_y;
        for(int i = 0; i < 8; i++){
                for(int j = 0; j < 8; j++){
                        if(board[i][j] == 1){
                                curr_x = j;
                                curr_y = i;
                                break;
                        }
                }
        }
        
        board[curr_y][curr_x] = 0;
        board[y][x] = 1;
        return 1;
}
void move_hound(int board[8][8], char move[10]){
        int curr_x = move[0] - 'a';
        int curr_y = move[1] - '0' - 1;
        int new_x = move[3] - 'a';
        int new_y = move[4] - '0' - 1;


        board[curr_y][curr_x] = 0;
        board[new_y][new_x] = 2;
};

int board[8][8];
int end_condition;
int player_round;
void init_server(){
        init_board(board);
        player_round = FOX;
        end_condition = 0;
}

int test_end_condition(){
        int curr_x, curr_y;
        for(int i = 0; i < 8; i++){
                for(int j = 0; j < 8; j++){
                        if(board[i][j] == 1){
                                curr_x = j;
                                curr_y = i;
                                break;
                        }
                }
        }
        if(curr_y == 7){
                printf("FOX WON!");
                return FOX;
        }
        if(board[0][0] == FOX && board[1][1] == HOUND){
                printf("HOUNDS WON");
                return HOUND;
        }
        if(curr_y == 0 && board[1][curr_x+1] == HOUND && board[1][curr_x-1] == HOUND){
                printf("HOUNDS WON");
                return HOUND;
        }
        if(curr_x == 0 && board[curr_y-1][1] == HOUND && board[curr_y+1][1] == HOUND){
                printf("HOUNDS WON");
                return HOUND;
        }
        if(board[curr_y+1][curr_x+1] == HOUND && board[curr_y+1][curr_x-1] == HOUND && board[curr_y-1][curr_x+1] == HOUND && board[curr_y-1][curr_x-1] == HOUND){
                printf("HOUNDS WON");
                return HOUND;
        }
        return 0;
}

char buffer[2][127];
void server(){ // Responsável por receber os movimentos e processar a lógica do jogo.
        char move[10];
        switch(player_round){
                case FOX:
                        strcpy(move, buffer[0]);
                        if(test_move(FOX, board, move) == 0)
                                return;
                        move_fox(board, move);
                        player_round = HOUND;
                        break;
                case HOUND:
                        strcpy(move, buffer[1]);
                        if(test_move(HOUND, board, move) == 0)
                                return;
                        move_hound(board, move);
                        player_round = FOX;
                        break;
        }
        if(test_end_condition() != 0)
                end_condition = 1;
}

void send_message_to_server(char message[127], int channel){
        strcpy(buffer[channel], message);
}

void player1(){
        if(player_round == HOUND){
                return;
        }
        char move[10];
        print_board(board);
        printf("You: 🦊\n");
        printf("Next move: ");
        scanf("%s", move);
        send_message_to_server(move, 0);
}

void player2(){
        if(player_round == FOX){
                return;
        }
        char move[10];
        print_board(board);
        printf("You: 🐶\n");
        printf("Next move: ");
        fgets(move, sizeof(move), stdin);
        send_message_to_server(move, 1);
}


int main(int argc, char* argv[]){
        init_server();

        while(end_condition == 0){
                player1();
                player2();
                server();
        }

        return 1;

}
