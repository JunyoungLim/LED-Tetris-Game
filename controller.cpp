#include <stdio.h>
#include <stdlib.h>
#include "controller.h"
#include "mbed.h"

// Communication block for row deletion
int output[2] = {0, 0};

// Serial output to PC
Serial cu(USBTX, USBRX);

int prev_score = 0;

/**
 * Start the game by initializing relevant information and data structures.
 */
void startGame() {
    prev_score = 0;
    initialize_game();
    create_next_piece();
    create_next_piece();
}

/**
 * Initializes the given piece block's matrix to 0.
 * @param piece the given piece
 */
void initialize_block_matrix(piece_t *piece) {
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            piece->block[r][c] = 0;
        }
    }
}

/**
 * Creat a new piece's block by initializing the block shape and rotating it accordingly.
 * @param piece the given piece
 */
void create_new_block(piece_t *piece) {
    for (int i = 0; i < 4; i++) {
        int x = BLOCK[(int)piece->shape][i];
        piece->block[x/4][x%4] = (int)piece->color;
    }
    
    if (piece->shape != Oshape) {
        int N = piece->shape == Ishape ? 4 : 3;
        switch(piece->rotation) {
            case 1:
                rotateMatrixClockwise(piece->block, N);
                break;
            case 2:
                flipMatrix(piece->block, N);
                break;
            case 3:
                rotateMatrixCounterclockwise(piece->block, N);
                break;
            default:
                break;
        }
    }
}

/**
 * Create a new piece with random block shape, rotation, and color initialization
 * and by enqueueing it to the piece queue of the current game.
 */
void create_next_piece() {
    piece_t *new_piece = new piece_t;
    new_piece->shape = (enum Shape)(rand() % 7);
    new_piece->color = (enum Color)(rand() % 7 + 1);
    new_piece->rotation = (rand() % 4);
    
    initialize_block_matrix(new_piece);
    create_new_block(new_piece);
    
    new_piece->row = 0;
    new_piece->col = 3;
    enqueue_piece(new_piece);
}

/**
 * For a given piece, if the piece's row and column are out of bound,
 * fix it automatically so that it fits in the screen.
 * @param piece the given piece
 * @return false if autocorrection fails (i.e. while fixing, it encounters other blocks at the location), true otherwise
 */
bool fixOutOfBound(piece_t *piece) {
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            if (piece->block[r][c]) {
                if (game->board[piece->row + r][piece->col + c]) {
                    return false;
                }
                
                while (piece->row + r >= 20) {
                    piece->row -= 1;
                    if (game->board[piece->row + r][piece->col + c]) {
                        return false;
                    }
                }
                
                while (piece->col + c >= 10) {
                    piece->col -= 1;
                    if (game->board[piece->row + r][piece->col + c]) {
                        return false;
                    }
                }
                
                while (piece->col + c < 0) {
                    piece->col += 1;
                    if (game->board[piece->row + r][piece->col + c]) {
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

/**
 * Flip the first N * N quadrant of a given 4x4 matrix by 180 degree
 * @param mat the given matrix
 * @param N the number of first set of blocks to flip
 */
void flipMatrix(int mat[][4], int N) {
    int lim = (N * N / 2);
    for (int i = 0; i < lim; i++) {
        int r = i / N;
        int c = i % N;
        int temp = mat[r][c];
        mat[r][c] = mat[N-r-1][N-c-1];
        mat[N-r-1][N-c-1] = temp;
    }
}

/**
 * Rotate clockwise the first N * N quadrant of a given 4x4 matrix by 90 degree
 * @param mat the given matrix
 * @param N the number of first set of blocks to rotate
 */
void rotateMatrixClockwise(int mat[][4], int N) {
    // Inspired by: https://www.geeksforgeeks.org/
    // How to rotate a matrix by 90 degrees
    for (int r = 0; r < N; r++) {
        for (int c = r; c < N - r - 1; c++) {
            int temp = mat[r][c];
            mat[r][c] = mat[N-c-1][r];
            mat[N-1-c][r] = mat[N-1-r][N-1-c];
            mat[N-1-r][N-1-c] = mat[c][N-1-r];
            mat[c][N-1-r] = temp;
        }
    }
}

/**
 * Rotate counter-clockwise the first N * N quadrant of a given 4x4 matrix by 90 degree
 * @param mat the given matrix
 * @param N the number of first set of blocks to rotate
 */
void rotateMatrixCounterclockwise(int mat[][4], int N) {
    // Inspired by: https://www.geeksforgeeks.org/
    // How to rotate a matrix by 90 degrees
    for (int r = 0; r < N; r++) {
        for (int c = r; c < N - r - 1; c++) {
            int temp = mat[r][c];
            mat[r][c] = mat[c][N-1-r];
            mat[c][N-1-r] = mat[N-1-r][N-1-c];
            mat[N-1-r][N-1-c] = mat[N-1-c][r];
            mat[N-1-c][r] = temp;
        }
    }
}

/**
 * Checks whether the current piece of the game can rotate clockwise
 * @return true if possible, false otherwise
 */
bool canRotateRight() {
    if (game->current_piece->shape == Oshape) {
        return false;
    }
    
    piece_t *demo = new piece_t;
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            int copy = game->current_piece->block[r][c];
            demo->block[r][c] = copy;
        }
    }
    int row_copy = game->current_piece->row;
    demo->row = row_copy;
    int col_copy = game->current_piece->col;
    demo->col = col_copy;
    
    int N = game->current_piece->shape == Ishape ? 4 : 3;
    
    rotateMatrixClockwise(demo->block, N);
    
    bool result = fixOutOfBound(demo);
    
    free_old_piece(demo);
    
    return result;
}

/**
 * Checks whether the current piece of the game can fall down by one pixel
 * @return true if possible, false otherwise
 */
bool canDropDown() {
    for (int r = 3; r >= 0; r--) {
        for (int c = 0; c < 4; c++) {
            if (game->current_piece->block[r][c]) {
                int row = game->current_piece->row + r;
                int col = game->current_piece->col + c;
                
                if (row == 20 - 1) {
                    return false;
                }
                
                if (game->board[row + 1][col] > 0) {
                    return false;
                }
            }
        }
    }
    return true;
}

/**
 * Checks whether the current piece of the game can move right by one pixel
 * @return true if possible, false otherwise
 */
bool canMoveRight() {
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            if (game->current_piece->block[r][c]) {
                int row = game->current_piece->row + r;
                int col = game->current_piece->col + c;
                
                if (col == 0) {
                    return false;
                }
                
                if (game->board[row][col - 1] > 0) {
                    return false;
                }
            }
        }
    }
    return true;
}

/**
 * Checks whether the current piece of the game can move left by one pixel
 * @return true if possible, false otherwise
 */
bool canMoveLeft() {
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            if (game->current_piece->block[r][c]) {
                int row = game->current_piece->row + r;
                int col = game->current_piece->col + c;
                
                if (col == 10 - 1) {
                    return false;
                }
                
                if (game->board[row][col + 1] > 0) {
                    return false;
                }
            }
        }
    }
    return true;
}

/**
 * Merge the current board and the current piece once the piece reaches the bottom-most layer
 * so that the next piece can pop in and the current piece can stay still.
 * It also handles freeing the memory space for the dequeued piece by calling free_old_piece().
 */
void mergeBoardandPiece() {
    
    for (int i = 0; i < 4; i++) {
        int row = game->current_piece->row + i;
        for (int j = 0; j < 4; j++) {
            int col = game->current_piece->col + j;
            if (game->current_piece->block[i][j]) {
                game->board[row][col] = game->current_piece->block[i][j];
            }
        }
    }
    piece_t *old_piece = dequeue_piece();
    free_old_piece(old_piece);
}

/**
 * Free the piece memory.
 * @param old_piece the piece to be freed
 */
void free_old_piece(piece_t *old_piece) {
    old_piece->next = NULL;
    delete old_piece;
}

/**
 * Detect the first row to be deleted, and also record how many lines after that row
 * must be deleted sequentially.
 * Use output[2] to transfer information.
 */
void detect_row_completion() {
    bool detected = false;
    int len_counter = 0;
    for (int r = 0; r < 20; r++) {
        int count = 0;
        for (int c = 0; c < 10; c++) {
            if (game->board[r][c] > 0) {
                count++;
            }
        }
        if (!detected && count == 10) {
            detected = true;
            output[0] = r;
            len_counter++;
        } else if (detected && count == 10) {
            len_counter++;
        } else if (detected && count != 10) {
            break;
        }
    }
    output[1] = len_counter;
}

/**
 * Based on the records from detect_row_completion(), perform deletion.
 * Use output[2] to transfer information.
 */
void delete_rows() {
    for (int r = output[0] + output[1] - 1; r >= output[1]; r--) {
        for (int c = 0; c < 10; c++) {
            game->board[r][c] = game->board[r - output[1]][c];
        }
    }
    
    for (int r = output[1] - 1; r >= 0; r--) {
        for (int c = 0; c < 10; c++) {
            game->board[r][c] = 0;
        }
    }
}

/**
 * Detect rows to be deleted, and delete those if detected any.
 * It also udpates game level and score based on any additional rows deleted.
 */
void deleteRowsIfDetected() {
    output[0] = 0;
    output[1] = 0;
    detect_row_completion();
    delete_rows();
    game->score += output[1];
    if (game->score < 9) {
        game->level = game->score + 1;
    }
}

/**
 * Pause the game by changing the current game's state.
 */
void pause_game() {
    game->state = GamePaused;
}

/**
 * Resume the game by changing the current game's state.
 */
void resume_game() {
    game->state = InProgress;
}

bool gameStart() {
    return game->state == GameStart;
}

bool gameOver() {
    return game->state == GameOver;
}

void restart_game() {
    game->state = GameStart;
}

/**
 * Checks whether the game is finished or not.
 * @return true if not finished, false otherwise
 */
bool gameNotFinished() {
    return game->state == InProgress || game->state == GamePaused;
}

bool gameFinished() {
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            if (game->current_piece->block[r][c]) {
                if (game->board[game->current_piece->row + r][game->current_piece->col + c]) {
                    return true;
                }
            }
        }
    }
    return false;
}

/**
 * Perform clockwise rotation of the current piece of the current game.
 */
void rotateRight() {
    int N = game->current_piece->shape == Ishape ? 4 : 3;
    rotateMatrixClockwise(game->current_piece->block, N);
}

/**
 * Perform drop-down of the current piece of the current game.
 */
void dropDown() {
    game->current_piece->row += 1;
}

/**
 * Perform leftward movement of the current piece of the current game.
 */
void moveLeft() {
    game->current_piece->col += 1;
}

/**
 * Perform rightward movement of the current piece of the current game.
 */
void moveRight() {
    game->current_piece->col -= 1;
}

bool gamePaused() {
    return game->state == GamePaused;
}

void end_game() {
    while(game->current_piece != NULL) {
        piece_t *old_piece = dequeue_piece();
        free_old_piece(old_piece);
    }
}

int get_game_level() {
    return game->level;
}

/**
 * Based on the given input move, performs an action and updates any relevant data
 * stored in the game module.
 * @param move the move given from the main function
 */
void updateNextMove(enum Move move) {
    switch (move) {
        case RotateR:
            if (canRotateRight()) {
                rotateRight();
                fixOutOfBound(game->current_piece);
            }
            break;
        case MoveL:
            if (canMoveLeft()) {
                moveLeft();
            }
            break;
        case MoveR:
            if (canMoveRight()) {
                moveRight();
            }
            break;
        case DropD:
            //cu.printf("we going down");
            if (canDropDown()) {
                //cu.printf("about to dropDown()");
                dropDown();
                //cu.printf("finished dropDown()");
                //cu.printf("%d", game->current_piece->row);
            }
            else {
                //cu.printf("about to create_next_piece()");
                //cu.printf("about to delete rows if detected.");
                mergeBoardandPiece();
                deleteRowsIfDetected();
                //cu.printf("about to merge board");
                if (prev_score != game->score) {
                    cu.printf("current score: %d\n", game->score);
                    prev_score = game->score;
                }
                
                if (gameFinished()) {
                    game->state = GameOver;
                    piece_t *old_piece = dequeue_piece();
                    free_old_piece(old_piece);
                } else {
                    create_next_piece();
                }
            }
            break;
    }
}
