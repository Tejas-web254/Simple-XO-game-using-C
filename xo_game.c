#include <stdio.h>

char board[3][3];

void init_board(void) {
    char c = '1';
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            board[i][j] = c++;
}

void print_board(void) {
    printf("\n");
    for (int i = 0; i < 3; i++) {
        printf("  %c | %c | %c\n", board[i][0], board[i][1], board[i][2]);
        if (i < 2)
            printf(" ---+---+---\n");
    }
    printf("\n");
}

int check_win(char p) {
    for (int i = 0; i < 3; i++) {
        if (board[i][0] == p && board[i][1] == p && board[i][2] == p) return 1;
        if (board[0][i] == p && board[1][i] == p && board[2][i] == p) return 1;
    }
    if (board[0][0] == p && board[1][1] == p && board[2][2] == p) return 1;
    if (board[0][2] == p && board[1][1] == p && board[2][0] == p) return 1;
    return 0;
}

int main(void) {
    char again;

    do {
        char player = 'X';
        int moves = 0;
        int winner = 0;

        init_board();
        printf("===== X-O (Tic-Tac-Toe) =====\n");

        while (moves < 9) {
            int pos;
            print_board();
            printf("Player %c, choose a cell (1-9): ", player);

            if (scanf("%d", &pos) != 1) {
                int ch;
                while ((ch = getchar()) != '\n' && ch != EOF);
                printf("Invalid input. Enter a number from 1 to 9.\n");
                continue;
            }

            if (pos < 1 || pos > 9) {
                printf("Out of range. Try again.\n");
                continue;
            }

            int r = (pos - 1) / 3;
            int c = (pos - 1) % 3;

            if (board[r][c] == 'X' || board[r][c] == 'O') {
                printf("Cell already taken. Try again.\n");
                continue;
            }

            board[r][c] = player;
            moves++;

            if (check_win(player)) {
                print_board();
                printf("*** Player %c wins! ***\n", player);
                winner = 1;
                break;
            }

            player = (player == 'X') ? 'O' : 'X';
        }

        if (!winner) {
            print_board();
            printf("It's a draw!\n");
        }

        printf("\nPlay again? (y/n): ");
        scanf(" %c", &again);
    } while (again == 'y' || again == 'Y');

    printf("Thanks for playing!\n");
    return 0;
}
