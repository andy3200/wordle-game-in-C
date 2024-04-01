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
int is_tile_stack_empty(tile_stack *stack) {
    return (stack->top == -1);
}

// check if stack is full 
int is_tile_stack_full(tile_stack *stack) {
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
    if(stack->top == -1){
        return '.';
    }
    return stack->letters[(stack->top)];
}

//initializes the game state
GameState* initialize_game_state(const char *filename) {
    int rows = 0;
    int cols = 0;
    char reading_buffer[500];
    FILE *file;
    file = fopen(filename,"r");
    if(file == NULL){
        printf("error cant find file");
        exit(1);
    }
    //  get the number of rows and cols
    while (fgets(reading_buffer, sizeof(reading_buffer), file) != NULL) {
        rows++; // Increment line count for each line read
    }
    cols = strlen(reading_buffer) -1;
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
    // copy the file matrix into our code 
    while (fgets(reading_buffer, sizeof(reading_buffer), file) != NULL) {
        for(int x = 0 ; x < cols ; x++){
            if(reading_buffer[x] == '\n'){
                continue;
            }else{
                if(reading_buffer[x] == '.'){
                    continue;
                }else{
                    push_tile((game->gameboard[curr_row][x]),reading_buffer[x]);
                }
            }
        }
        curr_row++;
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
                *(new_game->gameboard[i][j]) = *(source->gameboard[i][j]);
            }
        }
    }
    return new_game;
}

//checks if word is legal. return 1 if it is legal. 
int isLegalWord(const char *word) {
    char buffer[50];
    size_t length = strlen(word);
    if(length < 2){
        return 0;
    }
    FILE* word_file = fopen("tests/words.txt", "r");
    while(fgets(buffer,50,word_file)!= NULL){
        size_t word_length = strlen(buffer);
        //remove the new line character and replace it with null terminator
        if (word_length > 0 && buffer[word_length - 1] == '\n') {
            buffer[word_length - 1] = '\0';
        }
        for (size_t i = 0; buffer[i]; i++) {
            buffer[i] = toupper(buffer[i]);
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
    int empty = 1;
    char word_extracted[50]; 
    char old_word[50];
    int col_start_index = col;
    int col_end_index = col +tiles_length -1;
    if(tiles_length== 0){
        col_end_index++;
    }
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
                memset(word_extracted, '\0', sizeof(word_extracted));
                return 1;
            }else{
                memset(word_extracted, '\0', sizeof(word_extracted));
                return 0;
            }
        }
    }else{
        if(is_extending){//handle extending 
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
            //check if you want to overlap same word
            int old_word_index = 0;
            for(int index = col_start_index; index <= col_end_index; index++){
                if((game->game_cols <= index)){
                    old_word[old_word_index] = '.';
                }else{
                    old_word[old_word_index] = top_tile(game->gameboard[row][index]);
                }
                old_word_index++;
            }
            for(int index_old = 0; index_old < old_word_index; index_old++){
                if(old_word[index_old] != '.'){
                    empty = 0;
                }
            }
            if(empty){
                memset(old_word, '\0', sizeof(old_word));
                memset(word_extracted, '\0', sizeof(word_extracted));
                return 0; //cant place on non-touching
            }
            word_extracted[word_extracted_index] = '\0';
            if(isLegalWord(word_extracted)){
                memset(old_word, '\0', sizeof(old_word));
                memset(word_extracted, '\0', sizeof(word_extracted));
                return 2; // return 2 for valid word and board needs to be extended 
            }else{
                memset(old_word, '\0', sizeof(old_word));
                memset(word_extracted, '\0', sizeof(word_extracted));
                return 0;
            }
        }else{
            //after this line we have the index of the word we want. 
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
            col--; //-1 because after the last loop col will be incremented
            if(col_end_index > col){ // extract word after "tiles"
                for(int y = col+1; y <= col_end_index; y++){
                    word_extracted[word_extracted_index] = (top_tile(game->gameboard[row][y]));
                    word_extracted_index++; 
                }
            }
            //check if you want to overlap same word
            int old_word_index = 0;
            for(int index = col_start_index; index <= col_end_index; index++){
                if((game->game_cols <= index)){
                    old_word[old_word_index] = '.';
                }else{
                    old_word[old_word_index] = top_tile(game->gameboard[row][index]);
                }
                old_word_index++;
            }
            for(int index_old = 0; index_old < old_word_index; index_old++){
                if(old_word[index_old] != '.'){
                    empty = 0;
                }
            }
            if(empty){
                memset(old_word, '\0', sizeof(old_word));
                memset(word_extracted, '\0', sizeof(word_extracted));
                return 0; //cant place on non-touching
            }
            if(strcmp(old_word,word_extracted)==0){//overlapping same word
                memset(old_word, '\0', sizeof(old_word));
                memset(word_extracted, '\0', sizeof(word_extracted));
                return 0;
            }
            word_extracted[word_extracted_index] = '\0';
            if(isLegalWord(word_extracted)){
                memset(old_word, '\0', sizeof(old_word));
                memset(word_extracted, '\0', sizeof(word_extracted));
                return 1;
            }else{
                memset(old_word, '\0', sizeof(old_word));
                memset(word_extracted, '\0', sizeof(word_extracted));
                return 0;
            }
        }
    }
}
int check_vertical(GameState *game, int row, int col,int tiles_length,const char *tiles, int simple_check){
    int empty = 1; 
    int is_extending = 0; 
    char word_extracted[50];
    char old_word[50];
    int row_start_index = row;
    int row_end_index = row +tiles_length -1;
    if(tiles_length== 0){
        row_end_index++;
    }
    int word_extracted_index = 0; 
    //first condition
    //go up of the starting index
    
    if(row_end_index > ((game->game_rows)-1)){ // extending
        is_extending = 1;
        while(((row_start_index -1) != -1) && (top_tile(game->gameboard[row_start_index-1][col]) != '.')){
        row_start_index--;
        }
        row_end_index = row + tiles_length -1; //basically no change 
    }else{//not extending
        while(((row_start_index -1) != -1) && ((top_tile(game->gameboard[row_start_index-1][col])) != '.')){
            row_start_index--;
        }
        //go down of ending index
        while( ((row_end_index+1) < game->game_rows)&& (top_tile(game->gameboard[row_end_index+1][col]) != '.')){
            row_end_index++; 
        }
    }
    if(simple_check){
        if((row_start_index == row) && (row_end_index == row)){ //meaning that up and down doesn't have anything. it works
            return 1; 
        }else{
            for(int x = row_start_index; x <= row_end_index; x++){
                if(x == row){
                    if(tiles[0] == ' '){
                        word_extracted[word_extracted_index] = (top_tile(game->gameboard[row][col]));
                    }else{
                        word_extracted[word_extracted_index] = tiles[0];//get the letter that you want to check. make sure pass it in.
                    }
                    word_extracted_index++;
                }else{
                    word_extracted[word_extracted_index] = (top_tile(game->gameboard[x][col]));
                    word_extracted_index++; 
                }
            }
            if(isLegalWord(word_extracted)){
                memset(word_extracted, '\0', sizeof(word_extracted));
                return 1;
            }else{
                memset(word_extracted, '\0', sizeof(word_extracted));
                return 0;
            }
        }
    }else{
        if(is_extending){//handle extending 
            if(row_start_index < row){//extract stuff before tiles you want to place
                for(int x = row_start_index; x < row; x++){
                    word_extracted[word_extracted_index] = (top_tile(game->gameboard[x][col]));
                    word_extracted_index++; 
                }
            }
            for( int tiles_index = 0; tiles_index < tiles_length;tiles_index++){ //extract word from "tiles"
                if(tiles[tiles_index] == ' '){//get it from the gameboard
                    word_extracted[word_extracted_index] = (top_tile(game->gameboard[row][col]));
                    word_extracted_index++;
                    row++;
                }else{ //get it frm tiles
                    word_extracted[word_extracted_index] = tiles[tiles_index];
                    word_extracted_index++;
                    row++;
                }
            }
             //check if you want to overlap same word
            int old_word_index = 0;
            for(int index = row_start_index; index <= row_end_index; index++){
                if((game->game_rows <= index)){
                    old_word[old_word_index] = '.';
                }else{
                    old_word[old_word_index] = top_tile(game->gameboard[index][col]);
                }
                old_word_index++;
            }
            for(int index_old = 0; index_old < old_word_index; index_old++){
                if(old_word[index_old] != '.'){
                    empty = 0;
                }
            }
            if(empty){
                memset(old_word, '\0', sizeof(old_word));
                memset(word_extracted, '\0', sizeof(word_extracted));
                return 0; //cant place on non-touching
            }
            if(strcmp(old_word,word_extracted)==0){//overlapping same word
                memset(old_word, '\0', sizeof(old_word));
                memset(word_extracted, '\0', sizeof(word_extracted));
                return 0;
            }
            word_extracted[word_extracted_index] = '\0';
            if(isLegalWord(word_extracted)){
                memset(old_word, '\0', sizeof(old_word));
                memset(word_extracted, '\0', sizeof(word_extracted));
                return 2; // return 2 for valid word and board needs to be extended 
            }else{
                memset(old_word, '\0', sizeof(old_word));
                memset(word_extracted, '\0', sizeof(word_extracted));
                return 0;
            }
        }else{
            //after this line we have the index of the word we want. 
            if(row_start_index < row){//extract stuff before tiles you want to place
                for(int x = row_start_index; x < row; x++){
                    word_extracted[word_extracted_index] = (top_tile(game->gameboard[x][col]));
                    word_extracted_index++; 
                }
            }
            for( int tiles_index = 0; tiles_index < tiles_length;tiles_index++){ //extract word from "tiles"
                if(tiles[tiles_index] == ' '){//get it from the gameboard
                    word_extracted[word_extracted_index] = (top_tile(game->gameboard[row][col]));
                    word_extracted_index++;
                    row++;
                }else{ //get it frm tiles
                    word_extracted[word_extracted_index] = tiles[tiles_index];
                    word_extracted_index++;
                    row++;
                }
            }
            row--; //-1 because after the last loop row will be incremented
            if(row_end_index > row){ // extract word after "tiles"
                for(int y = row+1; y <= row_end_index; y++){
                    word_extracted[word_extracted_index] = (top_tile(game->gameboard[y][col]));
                    word_extracted_index++; 
                }
            }
            //check if you want to overlap same word
            int old_word_index = 0;
            for(int index = row_start_index; index <= row_end_index; index++){
                if((game->game_rows <= index)){
                    old_word[old_word_index] = '.';
                }else{
                    old_word[old_word_index] = top_tile(game->gameboard[index][col]);
                }
                old_word_index++;
            }
            for(int index_old = 0; index_old < old_word_index; index_old++){
                if(old_word[index_old] != '.'){
                    empty = 0;
                }
            }
            if(empty){
                memset(old_word, '\0', sizeof(old_word));
                memset(word_extracted, '\0', sizeof(word_extracted));
                return 0; //cant place on non-touching
            }
            if(strcmp(old_word,word_extracted)==0){//overlapping same word
                memset(old_word, '\0', sizeof(old_word));
                memset(word_extracted, '\0', sizeof(word_extracted));
                return 0;
            }
            word_extracted[word_extracted_index] = '\0';
            if(isLegalWord(word_extracted)){
                memset(old_word, '\0', sizeof(old_word));
                memset(word_extracted, '\0', sizeof(word_extracted));
                return 1;
            }else{
                memset(old_word, '\0', sizeof(old_word));
                memset(word_extracted, '\0', sizeof(word_extracted));
                return 0;
            }
        }
    }
}
GameState* place_tiles(GameState *game, int row, int col, char direction, const char *tiles, int *num_tiles_placed) {
    int extending = 0;
    int all_valid = 0; 
    int tiles_placed = 0;
    int tiles_length = strlen(tiles);
    int col_end_index = col +tiles_length -1;
    int row_end_index = row +tiles_length -1;
    //error checking 
    if((row >= (game->game_rows)) || (col >= (game->game_cols))){
        return game;
    }
    if((direction != 'V') && (direction != 'H')){
        return game;
    } 
    //Horizontally 
    if (direction == 'H') {
        int H_col  = col; 
        if(check_horizontal(game,row,col,tiles_length,tiles,0) ==1 ){ //check the new word constructed (not extending)
            for(int x = 0; x < tiles_length; x++){
                if((check_vertical(game,row,H_col,0,&tiles[x],1))!= 1){//remember to change this to vertical 
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
                    if((check_vertical(game,row,H_col,0,&tiles[x],1))!= 1){//remember to change this to vertical 
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
    //Vertically 
    if (direction == 'V') {
        int V_row = row; //index to start placing 
        if(check_vertical(game,row,col,tiles_length,tiles,0) ==1 ){ //check the new word constructed (not extending)
            for(int x = 0; x < tiles_length; x++){
                if((check_horizontal(game,V_row,col,0,&tiles[x],1))!= 1){
                    all_valid = 0;
                    return game;
                }
                V_row++;
            }
            all_valid = 1; 
        }else if(check_vertical(game,row,col,tiles_length,tiles,0) ==2){//check the word constructed (extending)
            extending = 1; 
            for(int x = 0; x < tiles_length; x++){
                if(V_row < game->game_rows){
                    if((check_horizontal(game,V_row,col,0,&tiles[x],1))!= 1){//remember to change this to vertical 
                        all_valid = 0;
                        return game;
                    }
                }
                V_row++;
            }
            all_valid = 1;
        }else{//not valid
            return game; 
        }
        if(all_valid){ //start placing (safe now)
            int new_board_row;
            int curr_col = col;
            int curr_row = row; 
            if(extending){
                new_board_row = row_end_index+1;
            }else{
                new_board_row = game->game_rows;
            }
            GameState* new_game = gamestate_copy(game,new_board_row,game->game_cols); //create the new game 
            for( int tiles_index = 0; tiles_index < tiles_length;tiles_index++){ //start placing tiles
                if(tiles[tiles_index] == ' '){//dont put anything
                    curr_row++;
                }else{ //place tile
                    push_tile(new_game->gameboard[curr_row][curr_col],tiles[tiles_index]);
                    curr_row++;
                    tiles_placed++;
                }
            }
            *num_tiles_placed = tiles_placed;
            new_game->previous = game;
            return new_game;
        }
    }
    return game; //this never happens 
}

void free_each_game_state(GameState *game){
    if (game == NULL) {
        return;
    }
    for (int x = 0; x < game->game_rows; x++) {
        for (int y = 0; y < game->game_cols; y++) {
            free(game->gameboard[x][y]); //free each entry (the stack)
        }
        free(game->gameboard[x]);
    }
    free(game->gameboard);
    free(game);
}

GameState* undo_place_tiles(GameState *game) {
    if(game->previous == NULL){
        return game;
    }
    GameState* returned_game = game->previous;
    free_each_game_state(game);
    return returned_game; 
}

void free_game_state(GameState *game) {
    GameState* current_game = game;
    while(current_game != NULL){
        GameState* next_to_free = current_game->previous;
        free_each_game_state(current_game);
        current_game = next_to_free;
    }
}

void save_game_state(GameState *game, const char *filename) {
    FILE* dest_file = fopen(filename,"w");
    tile_stack* current_stack;
    char letter;
    for (int x = 0; x < game->game_rows; x++) {
        for (int y = 0; y < game->game_cols; y++) {
            current_stack = game->gameboard[x][y];
            if(current_stack->top == -1){
                letter = '.';
            }else{
                letter = top_tile(current_stack);
            }
            fprintf(dest_file, "%c", letter);
        }
        fprintf(dest_file, "\n");
    }
    for (int x = 0; x < game->game_rows; x++) {
        for (int y = 0; y < game->game_cols; y++) {
            current_stack = game->gameboard[x][y];
            int top_index = (current_stack ->top) +1;
            fprintf(dest_file, "%d", top_index);
        }
        fprintf(dest_file, "\n");
    }
    fclose(dest_file);
    
}
