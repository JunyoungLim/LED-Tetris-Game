#ifndef MBED_TETRIS_H
#define MBED_TETRIS_H

#include "controller.h"
#include "display.h"

enum Input{Fall = 0, Left = 1, Right = 2, Rotate = 3, Pause = 4};

void update_based_on_input(enum Input input);

#endif //MBED_TETRIS_H
