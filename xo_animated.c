#include <stdio.h>
#include <string.h>
#include <windows.h>

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

#define RESET  "\x1b[0m"
#define RED    "\x1b[1;31m"
#define CYAN   "\x1b[1;36m"
#define GREEN  "\x1b[1;32m"
#define YELLOW "\x1b[1;33m"
#define PURPLE "\x1b[1;35m"
#define BLUE   "\x1b[1;34m"
#define GRAY   "\x1b[90m"
#define INVERT "\x1b[7m"

char board[9];
const int lines[8][3] = {
    {0, 1, 2}, {3, 4, 5}, {6, 7, 8},
    {0, 3, 6}, {1, 4, 7}, {2, 5, 8},
    {0, 4, 8}, {2, 4, 6}
};
const char *rainbow[] = { RED, YELLOW, GREEN, CYAN, BLUE, PURPLE };

void enable_ansi(void) {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (GetConsoleMode(h, &mode))
        SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}

void clear_screen(void) { printf("\x1b[2J\x1b[H"); }
void hide_cursor(void)  { printf("\x1b[?25l"); fflush(stdout); }
void show_cursor(void)  { printf("\x1b[?25h"); fflush(stdout); }

void typewrite(const char *text, const char *color, int delay_ms) {
    printf("%s", color);
    for (size_t i = 0; i < strlen(text); i++) {
        putchar(text[i]);
        fflush(stdout);
        Sleep(delay_ms);
    }
    printf(RESET);
}

void print_cell(int i, int highlight) {
    const char *color = board[i] == 'X' ? RED : board[i] == 'O' ? CYAN : GRAY;
    printf("%s%s %c " RESET, highlight ? INVERT : "", color, board[i]);
}

/* mask: bit i set = cell i is highlighted when hl_on is true */
void draw_screen(int mask, int hl_on, const char *msg) {
    clear_screen();
    printf(YELLOW "\n   ===== X-O  GAME =====\n\n" RESET);
    for (int r = 0; r < 3; r++) {
        printf("       ");
        for (int c = 0; c < 3; c++) {
            int i = r * 3 + c;
            print_cell(i, hl_on && (mask & (1 << i)));
            if (c < 2) printf("|");
        }
        printf("\n");
        if (r < 2) printf("       ---+---+---\n");
    }
    printf("\n");
    if (msg && msg[0]) printf("   %s\n", msg);
    fflush(stdout);
}

void intro(void) {
    hide_cursor();
    clear_screen();
    printf("\n   ");
    typewrite("Welcome to  X - O  GAME", YELLOW, 60);
    printf("\n\n");

    /* X and O chase each other across the screen */
    for (int pos = 0; pos <= 30; pos++) {
        printf("\r%*s" RED "X" RESET "  " CYAN "O" RESET "   ", pos, "");
        fflush(stdout);
        Sleep(35);
    }
    for (int pos = 30; pos >= 0; pos--) {
        printf("\r%*s" CYAN "O" RESET "  " RED "X" RESET "        ", pos, "");
        fflush(stdout);
        Sleep(35);
    }

    /* spinner */
    const char spin[] = "|/-\\";
    printf("\n\n");
    for (int i = 0; i < 16; i++) {
        printf("\r   " GREEN "Loading %c" RESET, spin[i % 4]);
        fflush(stdout);
        Sleep(90);
    }
    show_cursor();
}

void flash_cell(int i) {
    hide_cursor();
    for (int k = 0; k < 6; k++) {
        draw_screen(1 << i, k % 2 == 0, "");
        Sleep(90);
    }
    show_cursor();
}

int find_win(char p) {
    for (int l = 0; l < 8; l++)
        if (board[lines[l][0]] == p && board[lines[l][1]] == p && board[lines[l][2]] == p)
            return l;
    return -1;
}

void win_animation(int line, char player) {
    int mask = (1 << lines[line][0]) | (1 << lines[line][1]) | (1 << lines[line][2]);
    char text[40];
    sprintf(text, "*** PLAYER %c WINS! ***", player);

    hide_cursor();
    for (int k = 0; k < 10; k++) {
        draw_screen(mask, k % 2 == 0, "");
        Sleep(150);
    }
    draw_screen(mask, 1, "");

    /* rainbow scrolling text */
    for (int frame = 0; frame < 30; frame++) {
        printf("\r   ");
        for (size_t c = 0; c < strlen(text); c++)
            printf("%s%c", rainbow[(c + frame) % 6], text[c]);
        printf(RESET);
        fflush(stdout);
        Sleep(80);
    }
    printf("\n");
    show_cursor();
}

void draw_animation(void) {
    hide_cursor();
    draw_screen(0, 0, "");
    /* shaking text */
    for (int f = 0; f < 14; f++) {
        printf("\r%*s" YELLOW "It's a DRAW!" RESET "   ", (f % 2) ? 5 : 3, "");
        fflush(stdout);
        Sleep(70);
    }
    printf("\n");
    show_cursor();
}

int main(void) {
    char again;

    enable_ansi();
    intro();

    do {
        char player = 'X';
        int moves = 0;
        int win_line = -1;
        char msg[80] = "";

        for (int i = 0; i < 9; i++) board[i] = '1' + i;

        while (moves < 9) {
            int pos;
            char prompt[120];

            sprintf(prompt, "%s%sPlayer %s%c" RESET ", choose a cell (1-9): ",
                    msg, msg[0] ? "\n   " : "",
                    player == 'X' ? RED : CYAN, player);
            draw_screen(0, 0, "");
            printf("   %s", prompt);
            fflush(stdout);
            msg[0] = '\0';

            if (scanf("%d", &pos) != 1) {
                int ch;
                while ((ch = getchar()) != '\n' && ch != EOF);
                strcpy(msg, RED "Invalid input. Enter a number 1-9." RESET);
                continue;
            }
            if (pos < 1 || pos > 9) {
                strcpy(msg, RED "Out of range. Try again." RESET);
                continue;
            }
            if (board[pos - 1] == 'X' || board[pos - 1] == 'O') {
                strcpy(msg, RED "Cell already taken. Try again." RESET);
                continue;
            }

            board[pos - 1] = player;
            moves++;
            flash_cell(pos - 1);

            win_line = find_win(player);
            if (win_line >= 0) {
                win_animation(win_line, player);
                break;
            }
            player = (player == 'X') ? 'O' : 'X';
        }

        if (win_line < 0)
            draw_animation();

        printf("\n   Play again? (y/n): ");
        scanf(" %c", &again);
    } while (again == 'y' || again == 'Y');

    clear_screen();
    printf("\n   ");
    typewrite("Thanks for playing! Bye!\n\n", GREEN, 50);
    show_cursor();
    return 0;
}
