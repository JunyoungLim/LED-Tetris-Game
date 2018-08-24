#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "display.h"
#include "controller.h"
#include "configuration.h"

// current display
int display_matrix[32][16];

/**
 * Initialize all display-related data structures by calling
 * all the relevant functions.
 */
void initialize_display_package() {
    initialize_display_matrix();
    display_next_block_outline();
    display_game_board_outline();
    display_level_outline();
}

/**
 * Updates all display-related data structures by calling
 * all the relevant functions.
 */
void update_display_package() {
    if (gameOver()) return;
    draw_game_board();
    draw_next_block();
    draw_level(game->level);
}

/**
 * Update the 20 * 10 tetris game board based on the current game information.
 */
void draw_game_board() {
    for (int r = 0; r < 20; r++) {
        for (int c = 0; c < 10; c++) {
            display_matrix[r+12][c+3] = game->board[r][c];
        }
    }
    
    for (int r = 0; r < 4; r++) {
        int row = game->current_piece->row + r;
        for (int c = 0; c < 4; c++) {
            int col = game->current_piece->col + c;
            if (game->current_piece->block[r][c]) {
                display_matrix[row+12][col+3] = game->current_piece->block[r][c];
            }
        }
    }
}

/**
 * Update the 4 * 4 next-piece-to-fall-down information based on the current game information.
 */
void draw_next_block() {
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            display_matrix[r + 4][c + 9] = game->current_piece->next->block[r][c];
        }
    }
}

/**
 * Helper function of draw_level().
 * It draws the number on the display given the defined matrix.
 */
void draw_number (const int s_number[4][7]){
    for(int a = 0; a<4; a = a+1) {
        for(int b = 0; b<7; b = b+1) {
            display_matrix[b + 2][a + 2] = s_number[a][b];
        }
    }
}

/**
 * Update level information based on the current game information.
 * @param n the level information to display
 */
void draw_level (int n){
    switch (n) {
        case 1:
            draw_number(LEVEL_1);
            break;
        case 2:
            draw_number(LEVEL_2);
            break;
        case 3:
            draw_number(LEVEL_3);
            break;
        case 4:
            draw_number(LEVEL_4);
            break;
        case 5:
            draw_number(LEVEL_5);
            break;
        case 6:
            draw_number(LEVEL_6);
            break;
        case 7:
            draw_number(LEVEL_7);
            break;
        case 8:
            draw_number(LEVEL_8);
            break;
        case 9:
            draw_number(LEVEL_9);
            break;
    }
}

/**
 * Initializes the 32 * 16 display matrix to 0.
 */
void initialize_display_matrix() {
    for (int r = 0; r < 32; r++) {
        for (int c = 0; c < 16; c++) {
            display_matrix[r][c] = 0;
        }
    }
}

/**
 * Initializes 5 * 5 outline for the next-piece-to-fall-down information.
 */
void display_next_block_outline() {
    for(int a = 0; a<6; a = a+1){
        for(int b = 0; b<6; b = b+1){
            if (NEXT_BLOCK_OUTLINE[a][b] == 1){
                display_matrix[a+3][b+8] = 8;
            }
        }
    }
    
}

/**
 * Initializes the game board outline.
 */
void display_game_board_outline() {
    for(int a = 0; a<23; a = a+1){
        for(int b = 0; b<12; b = b+1){
            if (GAME_BOARD_OUTLINE[a][b] == 1){
                display_matrix[a + 11][b + 2] = 8;
            }
        }
    }
}

/**
 * Initializes the level outline.
 */
void display_level_outline() {
    for(int a = 0; a<9; a = a+1){
        for(int b = 0; b<6; b = b+1){
            if (LEVEL_OUTLINE[a][b] == 1){
                display_matrix[a+1][b+1] = 8;
            }
        }
    }
}

void draw_font (const int font[5][3],int x, int y, int color){
    for(int a = 0; a<5; a = a+1){
        for(int b = 0; b<3; b = b+1){
            if (font[a][b]){
                display_matrix[a+x][b+y] = color;
            }
        }
    }
}


void display_font(char c,int h, int x, int pick_c){
    switch(c){
        case 'G':
            draw_font(G, h, x, pick_c );
            break;
        case 'A':
            draw_font(A, h, x, pick_c);
            break;
            
        case 'M':
            draw_font(M, h, x, pick_c);
            break;
            
        case 'O':
            draw_font(O, h, x, pick_c);
            break;
            
        case 'V':
            draw_font(V, h, x, pick_c);
            break;
            
        case 'T':
            draw_font(T, h, x, pick_c);
            break;
        case 'E':
            draw_font(E, h, x, pick_c);
            break;
            
        case 'R':
            draw_font(R, h, x, pick_c);
            break;
            
        case 'I':
            draw_font(I, h, x, pick_c);
            break;
            
        case 'S':
            draw_font(S, h, x, pick_c);
            break;
            
        case 'J':
            draw_font(J, h, x, pick_c);
            break;
    }
}


void displayGameStart() {
    initialize_display_matrix();
    
    //Test TETRIS + Start Screen
    display_font('T',6, 13, 5);
    display_font('E',6, 10, 4);
    display_font('T',6, 7, 2);
    display_font('R',6, 4, 6);
    display_font('I',6, 2, 8);
    display_font('S',6, 0, 5);
    
    display_font('S',14, 13 , 6);
    display_font('T',14, 10, 8);
    display_font('A',14, 7, 6);
    display_font('R',14, 4, 8);
    display_font('T',14, 1, 6);
    display_font('J',14, 0, 8);
}

void displayGameOver() {
    initialize_display_matrix();
    
    //Test Game Over
    display_font('G',6,11, 1);
    display_font('A',6,8, 5);
    display_font('M',6,5, 1);
    display_font('E',6, 2, 5);
    
    display_font('O',12,11, 1);
    display_font('V',12,8, 5);
    display_font('E',12,5, 1);
    display_font('R',12,2, 5);
}
