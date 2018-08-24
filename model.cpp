#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "model.h"

// the current game module
game_t *game;

/**
 * Expose the current game explicitly.
 * @return the current game module
 */
game_t *get_current_game() {
    return game;
}

/**
 * Initializes the 20 * 10 game board matrix to 0.
 */
void initialize_board_matrix() {
    for (int r = 0; r < 20; r++) {
        for (int c = 0; c < 10; c++) {
            game->board[r][c] = 0;
        }
    }
}

/**
 * Initialize the game with the initial values.
 */
void initialize_game() {
    game = new game_t;
    initialize_board_matrix();
    game->state = GameStart;
    game->current_piece = NULL;
    game->score = 0;
    game->level = 1;
}

void reset_game() {
    initialize_board_matrix();
    game->state = GameStart;
    game->current_piece = NULL;
    game->score = 0;
    game->level = 1;
}

/**
 * Expose the current game's state.
 * @return the current game state
 */
State get_current_state() {
    return game->state;
}

/**
 * Expose the current game's level.
 * @return the current game level
 */
int get_current_level() {
    return game->level;
}

/**
 * Expose the current game's score.
 * @return the current game score
 */
int get_current_score() {
    return game->score;
}

/**
 * Checks whether the current game is over
 * @param game the current game
 * @return true if ended, false otherwise
 */
bool is_game_over(game_t *game) {
    if (game->state == GameOver) {
        return true;
    }
    return false;
}

/**
 * Copy the game board matrix and returns its copied data.
 * @return copied matrix data
 */
int **return_board_matrix() {
    int **board_copy = new int*[BOARD_HEIGHT];
    
    for (int r = 0; r < BOARD_HEIGHT; r++) {
        board_copy[r] = new int[BOARD_WIDTH];
        for (int c= 0; c < BOARD_WIDTH; c++) {
            int x = game->board[r][c];
            board_copy[r][c] = x;
        }
    }
    return board_copy;
}

/**
 * Get the current piece.
 * @return the current piece
 */
piece_t * get_current_piece() {
    return game->current_piece;
}

/**
 * Dequeue a piece from the current game's piece queue.
 * @return the dequeued piece
 */
piece_t * dequeue_piece() {
    return dequeue(&(game->current_piece));
}

/**
 * Enqueue a piece to the current game's piece queue.
 * @param piece a piece to be enqueued
 */
void enqueue_piece(piece_t *piece) {
    enqueue(&(game->current_piece), piece);
}

/**
 * Dequeue a piece from the any piece queue.
 * @param queue the piece queue
 * @return the dequeued piece
 */
piece_t * dequeue(piece_t **queue) {
    if (!(*queue)) return NULL;
    else{
        piece_t *piece = *queue;
        *queue = piece->next;
        piece->next = NULL;
        return piece;
    }
}

/**
 * Enqueue a piece to the any piece queue.
 * @param queue the piece queue
 * @return the enqueued piece
 */
void enqueue(piece_t **queue, piece_t *piece) {
    piece_t *tmp;
    if (*queue == NULL) {
        *queue = piece;
        (*queue)->next = NULL;
    }
    
    else {
        tmp = *queue;
        while (tmp->next != NULL) {
            tmp = tmp->next;
        }
        tmp->next = piece;
        piece->next = NULL;
    }
}
