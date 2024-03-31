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
    for (int i = 0; i < game->game_rows; i++) {
        game->gameboard[i] = (tile_stack **)malloc((game->game_cols) * sizeof(tile_stack *));
        for (int j = 0; j < game-> game_cols; j++) {
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
    fclose(file);
    return game; 
}
//makes a copy of the gamestate
GameState* gamestate_copy(GameState *source, int new_rows, int new_cols){
    GameState *new_game = malloc(sizeof(GameState));
    new_game-> game_rows = new_rows;
    new_game-> game_cols = new_cols;
    new_game -> previous = source; 
    new_game->gameboard = (tile_stack ***)malloc((new_game-> game_rows) * sizeof(tile_stack **));
    for (int i = 0; i < new_game->game_rows; i++) {
        new_game->gameboard[i] = (tile_stack **)malloc((new_game->game_cols) * sizeof(tile_stack *));
        for (int j = 0; j < new_game-> game_cols; j++) {
            new_game->gameboard[i][j] = (tile_stack *)malloc(sizeof(tile_stack));
            if((source->game_cols <= j) || (source->game_rows <= i)){//meaning source doesn't have that index
                new_game-> gameboard[i][j] -> top = -1;
            }else{
                new_game-> gameboard[i][j] -> top = source->gameboard[i][j] ->top;
                strcpy(new_game-> gameboard[i][j] -> letters,source->gameboard[i][j]->letters);
            }
        }
    }
    return new_game;
}

//checks if word is legal. return 1 if it is legal. 
int isLegalWord(const char *word) {
    char buffer[50];
    FILE* word_file = fopen("words.txt", "r");
    while(fgets(buffer,50,word_file)!= NULL){
        size_t word_length = strlen(buffer);
        //remove the new line character and replace it with null terminator
        if (word_length > 0 && buffer[word_length - 1] == '\n') {
            buffer[word_length - 1] = '\0';
        }
        if(strcmp(buffer, word) == 0){
            fclose(word_file);
            return 1; 
        }
    }
    fclose(word_file);
    return 0;
}
int check_horizontal(GameState *game, int row, int col,int tiles_length,const char *tiles, int simple_check){
    int is_extending = 0; 
    char word_extracted[50];
    int word_extracted_length; 
    int col_start_index = col;
    int col_end_index = col +tiles_length -1;
    int word_extracted_index = 0; 
    //first condition
    //go left of the starting index
    
    if(col_end_index > ((game->game_cols)-1)){ // extending
        is_extending = 1;
        while(((col_start_index -1) != -1) && (top_tile(game->gameboard[row][col_start_index-1]) != '.')){
        col_start_index--;
        }
        col_end_index = col + tiles_length -1; //basically no change 
    }else{//not extending
        while(((col_start_index -1) != -1) && (top_tile(game->gameboard[row][col_start_index-1]) != '.')){
            col_start_index--;
        }
        //go right of ending index
        while( ((col_end_index+1) < game->game_cols)&& (top_tile(game->gameboard[row][col_end_index+1]) != '.')){
            col_end_index++; 
        }
    }
    if(simple_check){
        if((col_start_index == col) && (col_end_index == col)){ //meaning that left and right doesn't have anything. it works
            return 1; 
        }else{
            for(int x = col_start_index; x <= col_end_index; x++){
                if(x == col){
                    if(tiles[0] == ' '){
                        word_extracted[word_extracted_index] = (top_tile(game->gameboard[row][col]));
                    }else{
                        word_extracted[word_extracted_index] = tiles[0];//get the letter that you want to check. make sure pass it in.
                    }
                    word_extracted_index++;
                }else{
                    word_extracted[word_extracted_index] = (top_tile(game->gameboard[row][x]));
                    word_extracted_index++; 
                }
            }
            if(isLegalWord(word_extracted)){
                return 1;
            }else{
                return 0;
            }
        }
    }else{
        if(is_extending){//handle extending 
            GameState* new_game = gamestate_copy(game, game->game_rows, col_end_index+1);
            if(col_start_index < col){//extract stuff before tiles you want to place
                for(int x = col_start_index; x < col; x++){
                    word_extracted[word_extracted_index] = (top_tile(game->gameboard[row][x]));
                    word_extracted_index++; 
                }
            }
            for( int tiles_index = 0; tiles_index < tiles_length;tiles_index++){ //extract word from "tiles"
                if(tiles[tiles_index] == ' '){//get it from the gameboard
                    word_extracted[word_extracted_index] = (top_tile(game->gameboard[row][col]));
                    word_extracted_index++;
                    col++;
                }else{ //get it frm tiles
                    word_extracted[word_extracted_index] = tiles[tiles_index];
                    word_extracted_index++;
                    col++;
                }
            }
            if(isLegalWord(word_extracted)){
                return 2; // return 2 for valid word and board needs to be extended 
            }else{
                return 0;
            }
        }else{
            //after this line we have the index of the word we want. 
            word_extracted_length = (col_start_index - col_end_index)+1; //might be useless
            if(col_start_index < col){//extract stuff before tiles you want to place
                for(int x = col_start_index; x < col; x++){
                    word_extracted[word_extracted_index] = (top_tile(game->gameboard[row][x]));
                    word_extracted_index++; 
                }
            }
            for( int tiles_index = 0; tiles_index < tiles_length;tiles_index++){ //extract word from "tiles"
                if(tiles[tiles_index] == ' '){//get it from the gameboard
                    word_extracted[word_extracted_index] = (top_tile(game->gameboard[row][col]));
                    word_extracted_index++;
                    col++;
                }else{ //get it frm tiles
                    word_extracted[word_extracted_index] = tiles[tiles_index];
                    word_extracted_index++;
                    col++;
                }
            }
            if(col_end_index > col){ // extract word after "tiles"
                for(int y = col+1; y <= col_end_index; y++){
                    word_extracted[word_extracted_index] = (top_tile(game->gameboard[row][y]));
                    word_extracted_index++; 
                }
            }
            if(isLegalWord(word_extracted)){
                return 1;
            }else{
                return 0;
            }
        }
    }
}
void extract_word_V(GameState *game){

}
GameState* place_tiles(GameState *game, int row, int col, char direction, const char *tiles, int *num_tiles_placed) {
    int extending = 0;
    int all_valid = 0; 
    int tiles_placed = 0;
    int tiles_length = strlen(tiles);
    int col_end_index = col +tiles_length -1;
    //error checking 
    if((row >= (game->game_rows)) || (col >= (game->game_cols))){
        return game;
    }
    if((direction != 'V') && (direction != 'H')){
        return game;
    } 
    //Horizontally 
    if (direction == 'H') {
        int H_row = row; //index to start placing 
        int H_col  = col; 
        if(check_horizontal(game,row,col,tiles_length,tiles,0) ==1 ){ //check the new word constructed (not extending)
            for(int x = 0; x < tiles_length; x++){
                if((check_horizontal(game,row,H_col,0,tiles[x],1))!= 1){//remember to change this to vertical 
                    all_valid = 0;
                    return game;
                }
                H_col++;
            }
            all_valid = 1; 
        }else if(check_horizontal(game,row,col,tiles_length,tiles,0) ==2){//check the word constructed (extending)
            extending = 1; 
            for(int x = 0; x < tiles_length; x++){
                if(H_col < game->game_cols){
                    if((check_horizontal(game,row,H_col,0,tiles[x],1))!= 1){//remember to change this to vertical 
                        all_valid = 0;
                        return game;
                    }
                }
                H_col++;
            }
            all_valid = 1;
        }else{//not valid
            return game; 
        }
        if(all_valid){ //start placing (safe now)
            int new_board_col;
            int curr_col = col;
            int curr_row = row; 
            if(extending){
                new_board_col = col_end_index+1;
            }else{
                new_board_col = game->game_cols;
            }
            GameState* new_game = gamestate_copy(game,game->game_rows,new_board_col); //create the new game 
            for( int tiles_index = 0; tiles_index < tiles_length;tiles_index++){ //start placing tiles
                if(tiles[tiles_index] == ' '){//dont put anything
                    curr_col++;
                }else{ //place tile
                    push_tile(new_game->gameboard[curr_row][curr_col],tiles[tiles_index]);
                    curr_col++;
                    tiles_placed++;
                }
            }
            *num_tiles_placed = tiles_placed;
            new_game->previous = game;
            return new_game;
        }
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
