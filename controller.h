#ifndef MBED_CONTROLLER_H
#define MBED_CONTROLLER_H

#include "model.h"

enum Move{RotateR = 0, MoveL = 1, MoveR = 2, DropD = 3};

void startGame();

void initialize_block_matrix(piece_t *piece);

void create_new_block(piece_t *piece);

void create_next_piece();

bool fixOutOfBound(piece_t *piece);

void flipMatrix(int mat[][4], int N);

void rotateMatrixClockwise(int mat[][4], int N);

void rotateMatrixCounterclockwise(int mat[][4], int N);

bool canRotateRight();

bool canDropDown();

bool canMoveLeft();

bool canMoveRight();

void rotateRight();

void dropDown();

void moveLeft();

void moveRight();

void mergeBoardandPiece();

void free_old_piece(piece_t *old_piece);

void deleteRowsIfDetected();

void pause_game();

void resume_game();

bool gameNotFinished();

bool gameFinished();

bool gamePaused();

bool gameStart();

bool gameOver();

void restart_game();

void end_game();

int get_game_level();

void updateNextMove(enum Move move);

#endif //MBED_CONTROLLER_H
