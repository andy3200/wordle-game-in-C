#include <stdio.h>
#include <stdlib.h>

#include "hw3.h"

int main(void) {
    static int num_tiles_placed;
    GameState* game = initialize_game_state("tests/boards/board01.txt");
    game = place_tiles(game, 2, 3, 'V', "T P", &num_tiles_placed);
    game = place_tiles(game, 2, 3, 'V', "T P", &num_tiles_placed);
    game = place_tiles(game, 2, 3, 'V', "T P", &num_tiles_placed);
    game = place_tiles(game, 2, 3, 'V', "T P", &num_tiles_placed);
    game = place_tiles(game, 2, 3, 'V', "T P", &num_tiles_placed);
    save_game_state(game, "tests/actual_outputs/output1");
    free_game_state(game);
    return 0;
}
