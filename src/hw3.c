#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "hw3.h" 

#define DEBUG(...) fprintf(stderr, "[          ] [ DEBUG ] "); fprintf(stderr, __VA_ARGS__); fprintf(stderr, " -- %s()\n", __func__)
#define MAX_SIZE 5


//initialize the stack (might be useless)
void init_tile_stack(tile_stack *stack) {
    stack->top = -1;
}

// check if stack is empty 
int is_stack_empty(tile_stack *stack) {
    return (stack->top == -1);
}

// check if stack is full 
int is_stack_full(tile_stack *stack) {
    return (stack->top == (MAX_SIZE - 1));
}

// push letter into stack 
void push_tile(tile_stack *stack, char letter) {
    if (is_tile_stack_full(stack)) {
        printf("Stack is full\n");
        return;
    }
    stack->letters[++(stack->top)] = letter;
}

// pop from stack
char pop_tile(tile_stack *stack) {
    if (is_tile_stack_empty(stack)) {
        printf("error,stack is empty\n");
        exit(1);
    }
    return stack->letters[(stack->top)--];
}
//get the top letter of the stack 
char top_tile(tile_stack *stack){
    return stack->letters[(stack->top)];
}

//initializes the game state
GameState* initialize_game_state(const char *filename) {
    int rows = 0;
    int cols = 0;
    int on_first_line = 1;
    FILE *file;
    file = fopen(filename,"r");
    char ch;
    //  get the number of rows and cols
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') {
            rows++;
            on_first_line = 0;
        } else {
            if(on_first_line){
                cols++;
            }
        }
    }
    //allocate memory 
    GameState *game = malloc(sizeof(GameState));
    game-> game_rows = rows;
    game-> game_cols = cols;
    game->gameboard = (tile_stack ***)malloc((game-> game_rows) * sizeof(tile_stack **));
    for (int i = 0; i < rows; i++) {
        game->gameboard[i] = (tile_stack **)malloc((game->game_cols) * sizeof(tile_stack *));
        for (int j = 0; j < rows; j++) {
        game->gameboard[i][j] = (tile_stack *)malloc(sizeof(tile_stack));
        game-> gameboard[i][j] -> top = -1;
        }
    }
    fseek(file, 0, SEEK_SET); //reset the filepointer to iterate through it again.
    int curr_row = 0;
    int curr_col = 0;
    // copy the file matrix into our code 
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') {
            curr_row++;
            curr_col = 0;
        }else{
            push_tile(&(game->gameboard[curr_row][curr_col]), ch);
        curr_col++;
        }
    }
    game-> previous = NULL;
    return game; 
}

//checks if word is legal. 
int isLegalWord(const char *word) {
   return 0; 
}
void extract_word_H(GameState *game, int row, int col,int tiles_length){
    char word_extracted[50];
    int word_extracted_length; 
    int col_start_index = col;
    int col_end_index = col +tiles_length;
    //first condition
    //go left of the starting index
    
    while(((col_start_index -1) != -1) && (top_tile(game->gameboard[row][col_start_index-1]) != '.')){
        col_start_index--;
    }
    //go right of ending index
    while( ((col_end_index+1) < game->game_cols)&& (top_tile(game->gameboard[row][col_end_index+1]) != '.')){
        col_end_index++; 
    }
    //after this line we have the index of the word we want. 
    word_extracted_length = (col_start_index - col_end_index)+1;

}
void extract_word_V(GameState *game){

}
GameState* place_tiles(GameState *game, int row, int col, char direction, const char *tiles, int *num_tiles_placed) {
    int tiles_length = strlen(tiles);
    //error checking 
    if((row >= (game->game_rows)) || (col >= (game->game_cols))){
        return game;
    }
    if((direction != 'V') && (direction != 'H')){
        return game;
    }
    int curr_row = row;
    int curr_col = col; 
    //Horizontally 
    if (direction == 'H') {

    }



}

GameState* undo_place_tiles(GameState *game) {
    (void)game;
    return NULL;
}

void free_game_state(GameState *game) {
    (void)game;
}

void save_game_state(GameState *game, const char *filename) {
    (void)game;
    (void)filename;
}
