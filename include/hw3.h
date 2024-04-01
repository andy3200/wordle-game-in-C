#include <stdlib.h>

typedef struct tile_stack{
    int top; 
    char letters[5];
} tile_stack;

typedef struct GameState
{   
    struct GameState* previous; 
    struct tile_stack *** gameboard; 
    int game_rows;
    int game_cols;
} GameState;

void init_tile_stack(tile_stack *stack);
int is_tile_stack_empty(tile_stack *stack);
int is_tile_stack_full(tile_stack *stack);
void push_tile(tile_stack *stack, char letter);
char pop_tile(tile_stack *stack);
char top_tile(tile_stack *stack);
GameState* initialize_game_state(const char *filename);
GameState* gamestate_copy(GameState *source, int new_rows, int new_cols);
int isLegalWord(const char *word);
int check_horizontal(GameState *game, int row, int col,int tiles_length,const char *tiles, int simple_check);
int check_vertical(GameState *game, int row, int col,int tiles_length,const char *tiles, int simple_check);
GameState* place_tiles(GameState *game, int row, int col, char direction, const char *tiles, int *num_tiles_placed);
void free_each_game_state(GameState *game);
GameState* undo_place_tiles(GameState *game);
void free_game_state(GameState *game);
void save_game_state(GameState *game, const char *filename);
