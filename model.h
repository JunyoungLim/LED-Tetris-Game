#ifndef MBED_MODEL_H
#define MBED_MODEL_H

enum Shape{Oshape = 0, Ishape = 1, Tshape = 2, Sshape = 3, Zshape = 4, Jshape = 5, Lshape = 6};
enum Color{NoColor = 0, Red = 1, Orange = 2, Yellow = 3, Green = 4, Blue = 5, Navy = 6, Purple = 7};
enum State{GameStart = 0, InProgress = 1, GamePaused = 2, GameOver = 3};

const int BOARD_HEIGHT = 20;
const int BOARD_WIDTH = 10;

const int BLOCK[7][4] = {
        {1, 2, 5, 6},
        {4, 5, 6, 7},
        {1, 4, 5, 6},
        {0, 1, 5, 6},
        {1, 2, 4, 5},
        {0, 4, 5, 6},
        {2, 4, 5, 6}
};

typedef struct piece_state {
    enum Shape shape;
    enum Color color;
    int block[4][4];
    int rotation;
    int row;
    int col;
    struct piece_state *next;
} piece_t;

typedef struct game_state {
    State state;
    int board[20][10];
    int score;
    int level;
    piece_t *current_piece;
} game_t;

extern game_t *game;

game_t * get_current_game();

void initialize_board_matrix();

void initialize_game();

void reset_game();

State get_current_state();

int get_current_level();

int get_current_score();

bool is_game_over(game_t *game);

int **return_board_matrix();

piece_t * get_current_piece();

piece_t * dequeue(piece_t **queue);

void enqueue(piece_t **queue, piece_t *piece);

piece_t * dequeue_piece();

void enqueue_piece(piece_t *piece);

#endif //MBED_MODEL_H
