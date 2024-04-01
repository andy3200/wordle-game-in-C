#include <stdio.h>
#include <stdlib.h>

#include "hw3.h"

int main(void) {
    static int num_tiles_placed;
    GameState *game = initialize_game_state("./tests/boards/board05.txt"); 
    game = place_tiles(game, 4, 4, 'H', "T G", &num_tiles_placed);
    game = place_tiles(game, 5, 4, 'H', "H OK", &num_tiles_placed);
    save_game_state(game, "tests/actual_outputs/output");
    free_game_state(game);
    return 0;
}
