#include <stdio.h>
#include <stdlib.h>

#include "hw3.h"

int main(void) {
    GameState* game = initialize_game_state("tests/boards/board01.txt");
    save_game_state(game, "tests/actual_outputs/output1");
    free_game_state(game);
    return 0;
}
