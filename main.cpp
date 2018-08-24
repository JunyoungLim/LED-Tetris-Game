#include "mbed.h"
#include "RGBmatrixPanel.h" // Hardware-specific library
#include <stdint.h>
#include "MK64F12.h"
#include <stdlib.h>
#include <time.h>
#include "controller.h"
#include "display.h"
#include "input_handler.h"

PinName ub1=D13;
PinName ug1=D14;
PinName ur1=D15;
PinName lb2=D10;
PinName lg2=D11;
PinName lr2=D12;

RGBmatrixPanel matrix(ur1,ug1,ub1,lr2,lg2,lb2,A0,A1,A2,D9,A3,D8,false);

const uint8_t RIGHT_PRESSED    = 0b1;
const uint8_t LEFT_PRESSED     = 0b10;
const uint8_t DOWN_PRESSED     = 0b100;
const uint8_t UP_PRESSED       = 0b1000;
const uint8_t START_PRESSED    = 0b10000;
const uint8_t SELECT_PRESSED   = 0b100000;
const uint8_t B_PRESSED        = 0b1000000;
const uint8_t A_PRESSED        = 0b10000000;

/**
 * Delays for x * 10 microseconds.
 * @param x delay input
 */
void delay(int x){
    for(int i=0;i<x;i++){
        wait_us(10);
    }
}

// Color Initialization
uint16_t black = matrix.Color333(0,0,0);
uint16_t red = matrix.Color333(2,0,0);
uint16_t orange = matrix.Color333(3,1,0);
uint16_t yellow = matrix.Color333(1,1,0);
uint16_t green = matrix.Color333(0,2,0);
uint16_t blue = matrix.Color333(0,1,1);
uint16_t indigo = matrix.Color333(0,0,2);
uint16_t violet = matrix.Color333(1,0,1);
uint16_t white = matrix.Color333(1,1,1);

// Timer declaration
Ticker cntrl_timer;
Ticker moved_timer;

// Serial PC output initialization
Serial pc(USBTX, USBRX);

/**
 * Draw one pixel to the RGBMatrixPanel matrix.
 * @param x x coordinate
 * @param y y coordinate
 * @param r color value to put in
 */
void draw_board_pixel(int x, int y, int r) {
    switch (r) {
        case 0:
            matrix.drawPixel(x,y, black);
            break;
        case 1:
            matrix.drawPixel(x,y, red);
            break;
        case 2:
            matrix.drawPixel(x,y, orange);
            break;
        case 3:
            matrix.drawPixel(x,y, yellow);
            break;
        case 4:
            matrix.drawPixel(x,y, green);
            break;
        case 5:
            matrix.drawPixel(x,y, blue);
            break;
        case 6:
            matrix.drawPixel(x,y, indigo);
            break;
        case 7:
            matrix.drawPixel(x,y, violet);
            break;
        case 8:
            matrix.drawPixel(x,y, white);
            break;
    }
}

/**
 * Copy the display matrix from the display module to the RGBMatrixPanel pixel by pixel.
 */
void draw_board_matrix(){
    for(int x=0; x<32; x = x+1){
        for(int y=0; y<16; y = y+1){
            draw_board_pixel(x,y,display_matrix[x][y]);
        }
    }
}

/**
 * Update the next move to the controller whenever called.
 * To enable periodic piece dropping, attach this function to a Timer or Ticker.
 */
void periodic_move_down() {
    updateNextMove(DropD);
    update_display_package();
    draw_board_matrix();
}

/**
 * Read data from the InputHandler and performs action accordingly:
 * Keypress A is ignored.
 * Keypress B or UP perform rotation of the piece clockwise.
 * Keypress LEFT moves the piece to the left by one.
 * Keypress RIGHT moves the piece to the right by one.
 * Keypress DOWN moves the piece to the bottom by one.
 * Keypress START resumes the game if paused.
 * Keypress SELECT pauses the game if in progress.
 */
void move_based_on_control_input() {
    uint8_t cntrl = read_data_input_from_input_handler();
    if (cntrl == START_PRESSED) {
        resume_game();
        moved_timer.attach(&periodic_move_down, current_speed);
    } else if (cntrl == SELECT_PRESSED) {
        pause_game();
        moved_timer.detach();
        return;
    }else {
        switch (cntrl) {
            case 0:
                break;
            case A_PRESSED:
                break;
            case B_PRESSED:
                updateNextMove(RotateR);
                break;
            case LEFT_PRESSED:
                updateNextMove(MoveL);
                break;
            case RIGHT_PRESSED:
                updateNextMove(MoveR);
                break;
            case UP_PRESSED:
                updateNextMove(RotateR);
                break;
            case DOWN_PRESSED:
                updateNextMove(DropD);
                if (prev_level != get_game_level()) {
                    prev_level = get_game_level();
                    current_speed = SPEED[prev_level];
                    moved_timer.detach();
                    moved_timer.attach(&periodic_move_down, current_speed);
                }
                break;
            default:
                break;
        }
    }
    update_display_package();
    draw_board_matrix();
}

void game_start_based_on_control_input() {
    uint8_t cntrl = read_data_input_from_input_handler();
    switch (cntrl) {
        case B_PRESSED:
        case START_PRESSED:
            resume_game();
            moved_timer.attach(&periodic_move_down, current_speed);
            break;
        default:
            break;
    }
    draw_board_matrix();
}

void game_over_based_on_control_input() {
    uint8_t cntrl = read_data_input_from_input_handler();
    switch (cntrl) {
        case B_PRESSED:
        case START_PRESSED:
            restart_game();
            break;
        default:
            break;
    }
    draw_board_matrix();
}

/**
 * The main function that controls the entire tetris module
 * Any initializer gets initialized here, and any control flows
 * comes and goes out of the main function.
 *
 * @return 0 if successful, 1 otherwise (unreachable)
 */
int main() {
    
    // random seed for random block generation
    srand(time(NULL));
    
    // initialize game
    pc.printf("game started...\n");
    initialize_input_handler();
    
    // initialize timers
    // cntrl_timer.attach(&move_based_on_control_input, 0.1);
    matrix.begin();
    
    while (true) {
        startGame();
        
        displayGameStart();
        while (gameStart()) {
            delay(2000);
            game_start_based_on_control_input();
        }
        
        initialize_display_package();
        update_display_package();
        // update the matrix
        while(gameNotFinished()) {
            // for (int i = 0; i < DEFAULT_SYSTEM_CLOCK / 100; i++);
            // periodic_move_down();
            delay(2000);
            // matrix.updateDisplay();
            move_based_on_control_input();
        }
        
        moved_timer.detach();
        
        end_game();
        
        displayGameOver();
        while (gameOver()) {
            delay(2000);
            game_over_based_on_control_input();
        }
        
        reset_game();
        current_speed = SPEED[1];
    }
}
