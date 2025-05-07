#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include "game.h"
#include "game_aux.h"

bool test_game_new_empty() {
	game g = game_default();
	if (g == NULL) {
		fprintf(stderr, "Error: invalid game new empty!\n");
		return false;
	} else {
		game_delete(g);
		return true;
	}
}

bool test_game_new() {
	game g = game_default();
	square square_tents[8][8] = {
		{EMPTY, EMPTY, EMPTY, EMPTY, TREE, TREE, EMPTY, EMPTY},
		{TREE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, TREE},
		{EMPTY, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY},
		{TREE, EMPTY, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY},
		{EMPTY, TREE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
		{TREE, EMPTY, EMPTY, EMPTY, TREE, EMPTY, TREE, EMPTY},
		{EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
		{TREE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY}};

	if (g == NULL) {
		fprintf(stderr, "Error: invalid game new!\n");
		return false;
	}
	for (uint i = 0; i <= 7; i++) {
		for (uint j = 0; j <= 7; j++) {
			if (game_get_square(g, i, j) != square_tents[i][j]) {
				fprintf(stderr,
						"Error: error of square copy in function game_copy!\n");
				return false;
			}
		}
	}
	game_delete(g);
	return true;
}

bool test_game_delete() {
	game g = game_default_solution();
	if (g == NULL) {
		fprintf(stderr, "Error: invalid game delete!\n");
		exit(EXIT_FAILURE);
	}
	game_delete(g);
	return true;
}

bool test_game_is_over() {
	game g = game_default_solution();

	if (g == NULL) {
		fprintf(stderr, "Error: invalid game new!\n");
		return false;
	}
	if (game_is_over(g)) {
		game_delete(g);
		return true;
	} else {
		fprintf(stderr, "Error: invalid game is over!\n");
		game_delete(g);
		return false;
	}
}

bool test_game_copy() {
	// create a game
	game test_game = game_default();

	// copy the game
	cgame ctest = test_game;
	game copy_game = game_copy(ctest);
	cgame c_copy = copy_game;

	// test if games are valid
	if ((test_game == NULL) || (copy_game == NULL)) {
		fprintf(stderr, "Error: invalid game in function test_game_copy!\n");
		game_delete(test_game);
		game_delete(copy_game);
		return false;
	}
	for (uint y = 0; y <= 7; y++) {
		for (uint x = 0; x <= 7; x++) {
			if (game_get_square(ctest, x, y) != game_get_square(c_copy, x, y)) {
				fprintf(stderr,
						"Error: error of square copy in function game_copy!\n");
				game_delete(test_game);
				game_delete(copy_game);
				return false;
			}
		}
	}
	game_delete(test_game);
	game_delete(copy_game);
	return true;
}

bool test_game_get_current_nb_tents_all() {
	uint nb_tents_current_all = 0;
	game g = game_default();
	game_play_move(g, 0, 0, TENT);
	game_play_move(g, 6, 1, TENT);
	game_play_move(g, 0, 1, TENT);
	for (uint i = 0; i <= 7; i++) {
		nb_tents_current_all += game_get_current_nb_tents_row(g, i);
	}
	if (nb_tents_current_all != 3) {
		game_delete(g);
		return false;
	}
	game_delete(g);
	return true;
}

void usage(char* test_main) {
	fprintf(stderr, "Usage: %s <testname> [<...>]\n", test_main);
	exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]) {
	if (argc == 1) usage(argv[0]);
	// start test
	fprintf(stderr, "=> Start test \"%s\"\n", argv[1]);
	bool cool = false;
	if (0 == strcmp("dummy", argv[1])) {
		cool = (argv[1]);
	} else if (0 == strcmp("game_new_empty", argv[1])) {
		cool = test_game_new_empty();
	} else if (0 == strcmp("game_new", argv[1])) {
		cool = test_game_new();
	} else if (0 == strcmp("game_delete", argv[1])) {
		cool = test_game_delete();
	} else if (0 == strcmp("game_is_over", argv[1])) {
		cool = test_game_is_over();
	} else if (0 == strcmp("game_copy", argv[1])) {
		cool = test_game_copy();
	} else if (0 == strcmp("game_get_current_nb_tents_all", argv[1])) {
		cool = test_game_get_current_nb_tents_all();
	}
	// print test result

	if (cool) {
		fprintf(stderr, "Test \"%s\" finished: SUCCESS\n", argv[1]);
		return EXIT_SUCCESS;
	} else {
		fprintf(stderr, "Test \"%s\" finished: FAILURE\n", argv[1]);
		return EXIT_FAILURE;
	}
}