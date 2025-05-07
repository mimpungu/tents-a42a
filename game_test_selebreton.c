#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "queue.h"

bool test_dummy(char* arg) { return (strcmp(arg, "dummy") == 0); }

bool test_game_get_square() {
	game g = game_default();
	game_play_move(g, 0, 0, TENT);
	game_play_move(g, 6, 1, TENT);
	game_play_move(g, 0, 1, GRASS);
	square testgame[8][8] = {
		{TENT, GRASS, EMPTY, EMPTY, TREE, TREE, EMPTY, EMPTY},
		{TREE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, TREE},
		{EMPTY, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY},
		{TREE, EMPTY, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY},
		{EMPTY, TREE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
		{TREE, EMPTY, EMPTY, EMPTY, TREE, EMPTY, TREE, EMPTY},
		{EMPTY, TENT, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
		{TREE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY}};

	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			if (game_get_square(g, i, j) != testgame[i][j]) {
				game_delete(g);
				return false;
			}
		}
	}
	game_delete(g);
	return true;
}

bool test_game_set_square() {
	game g = game_default();
	square testgame[8][8] = {
		{GRASS, TENT, EMPTY, TREE, TREE, TREE, EMPTY, EMPTY},
		{EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, TREE},
		{EMPTY, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY},
		{TREE, EMPTY, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY},
		{EMPTY, TREE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
		{TREE, EMPTY, EMPTY, EMPTY, TREE, EMPTY, TREE, EMPTY},
		{EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
		{TREE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY}};
	game_set_square(g, 0, 0, GRASS);
	game_set_square(g, 0, 1, TENT);
	game_set_square(g, 0, 3, TREE);
	game_set_square(g, 1, 0, EMPTY);
	bool ok = ((game_get_square(g, 0, 0) == testgame[0][0]) &&
			   (game_get_square(g, 0, 1) == testgame[0][1]) &&
			   (game_get_square(g, 0, 3) == testgame[0][3]) &&
			   (game_get_square(g, 1, 0) == testgame[1][0]));
	game_delete(g);
	return ok;
}

bool test_game_fill_grass_row() {
	game g = game_default();
	game_set_square(g, 0, 0, TENT);
	square testgame[1][8] = {
		{TENT, GRASS, GRASS, GRASS, TREE, TREE, GRASS, GRASS}};
	game_fill_grass_row(g, 0);
	bool ok = ((game_get_square(g, 0, 0) == testgame[0][0]) &&
			   (game_get_square(g, 0, 1) == testgame[0][1]) &&
			   (game_get_square(g, 0, 2) == testgame[0][2]) &&
			   (game_get_square(g, 0, 3) == testgame[0][3]) &&
			   (game_get_square(g, 0, 4) == testgame[0][4]) &&
			   (game_get_square(g, 0, 5) == testgame[0][5]) &&
			   (game_get_square(g, 0, 6) == testgame[0][6]) &&
			   (game_get_square(g, 0, 7) == testgame[0][7]));
	game_delete(g);
	return ok;
}

bool test_game_fill_grass_col() {
	game g = game_default();
	game_set_square(g, 0, 0, TENT);
	square testgame[1][8] = {
		{TENT, TREE, GRASS, TREE, GRASS, TREE, GRASS, TREE}};
	game_fill_grass_col(g, 0);
	game_print(g);
	bool ok = ((game_get_square(g, 0, 0) == testgame[0][0]) &&
			   (game_get_square(g, 1, 0) == testgame[0][1]) &&
			   (game_get_square(g, 2, 0) == testgame[0][2]) &&
			   (game_get_square(g, 3, 0) == testgame[0][3]) &&
			   (game_get_square(g, 4, 0) == testgame[0][4]) &&
			   (game_get_square(g, 5, 0) == testgame[0][5]) &&
			   (game_get_square(g, 6, 0) == testgame[0][6]) &&
			   (game_get_square(g, 7, 0) == testgame[0][7]));
	game_delete(g);
	return ok;
}

bool test_game_check_move() {
	bool res = true;
	game g = game_default();

	// Check illégal move
	if (game_check_move(g, 0, 4, TENT) == 1 ||
		game_check_move(g, 0, 1, TREE) == 1) {
		res = false;
	}

	// Check Loosing move
	// Tent next to another tent
	game_restart(g);
	game_play_move(g, 2, 2, TENT);
	if (game_check_move(g, 1, 1, TENT) == 1 ||
		game_check_move(g, 1, 2, TENT) == 1 ||
		game_check_move(g, 1, 3, TENT) == 1 ||
		game_check_move(g, 2, 1, TENT) == 1 ||
		game_check_move(g, 2, 3, TENT) == 1 ||
		game_check_move(g, 3, 1, TENT) == 1 ||
		game_check_move(g, 3, 2, TENT) == 1 ||
		game_check_move(g, 3, 3, TENT) == 1) {
		printf("i6\n");
		res = false;
	}

	// Too much tents
	game_restart(g);
	uint nbExpectedTent = game_get_expected_nb_tents_all(g);
	uint tenttoplace = nbExpectedTent;
	for (uint i = 0; i < 8; i++) {
		for (uint j = 0; j < 8; j++) {
			if (tenttoplace > 0) {
				if (game_get_square(g, i, j) != 1) {
					game_play_move(g, i, j, TENT);
					tenttoplace--;
				}
			}
		}
	}
	if (game_check_move(g, 7, 7, TENT) == 1) {
		printf("fff\n");
		res = false;
	}

	// too much tent row
	game_restart(g);
	nbExpectedTent = game_get_expected_nb_tents_row(g, 2);

	tenttoplace = nbExpectedTent;
	for (uint j = 0; j < 8; j++) {
		if (tenttoplace > 0) {
			if (game_get_square(g, 2, j) != 1) {
				game_play_move(g, 2, j, TENT);
				tenttoplace--;
			}
		}
	}
	if (game_check_move(g, 2, 7, TENT) == REGULAR) {
		printf("ici\n");
		res = false;
	}

	// too much tent column
	game_restart(g);
	nbExpectedTent = game_get_expected_nb_tents_col(g, 2);

	tenttoplace = nbExpectedTent;
	for (uint j = 0; j < 8; j++) {
		if (tenttoplace > 0) {
			if (game_get_square(g, j, 2) != 1) {
				game_play_move(g, j, 2, TENT);
				tenttoplace--;
			}
		}
	}
	if (game_check_move(g, 7, 2, TENT) == 1) {
		printf("ici2\n");
		res = false;
	}

	// no place next to tree for a tent
	game_restart(g);
	game_play_move(g, 2, 3, GRASS);
	game_play_move(g, 2, 5, GRASS);
	game_play_move(g, 1, 4, GRASS);
	if (game_check_move(g, 3, 4, GRASS) == REGULAR) {
		printf("\nici3\n");
		res = false;
	}
	// place a tent not around a tree
	game_restart(g);
	if (game_check_move(g, 6, 5, TENT) == REGULAR) {
		printf("ici4\n");
		res = false;
	}

	// To much  grass in a row
	game_restart(g);
	uint row = 6;
	nbExpectedTent = game_get_expected_nb_tents_row(g, row);
	uint Grasstoplace = 8 - nbExpectedTent;
	for (uint i = 0; i < Grasstoplace; i++) {
		if (game_get_square(g, row, i) == 1) {
			Grasstoplace += 1;

		} else {
			game_play_move(g, row, i, GRASS);
		}
	}
	if (game_check_move(g, row, 7, GRASS) == 1) {
		printf("ici5\n");
		res = false;
	}

	// To much grass in a col
	game_restart(g);
	uint col = 2;
	nbExpectedTent = game_get_expected_nb_tents_col(g, col);
	Grasstoplace = 8 - nbExpectedTent;
	for (uint i = 0; i < Grasstoplace; i++) {
		if (game_get_square(g, i, col) == 1) {
			Grasstoplace += 1;

		} else {
			game_play_move(g, i, col, GRASS);
		}
	}
	if (game_check_move(g, 7, col, GRASS) == 1) {
		printf("ici6\n");
		res = false;
	}
	game_delete(g);
	return res;
}

void usage(char* executable) {
	fprintf(stderr, "Usage: %s <testname> [<...>]\n", executable);
	exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]) {
	if (argc == 1) {
		usage(argv[0]);
	}

	fprintf(stderr, "=> Start test \"%s\"\n", argv[1]);
	bool test_res = false;

	if (strcmp(argv[1], "dummy") == 0) {
		test_res = test_dummy(argv[1]);
	} else if (strcmp(argv[1], "game_get_square") == 0) {
		test_res = test_game_get_square();
	} else if (strcmp(argv[1], "game_set_square") == 0) {
		test_res = test_game_set_square();
	} else if (strcmp(argv[1], "game_fill_grass_row") == 0) {
		test_res = test_game_fill_grass_row();
	} else if (strcmp(argv[1], "game_fill_grass_col") == 0) {
		test_res = test_game_fill_grass_col();
	} else if (strcmp(argv[1], "game_check_move") == 0) {
		test_res = test_game_check_move();
	} else {
		fprintf(stderr, "Error: test for function \"%s\" not found!\n",
				argv[1]);
		exit(EXIT_FAILURE);
	}

	if (test_res) {
		fprintf(stderr, "Test \"%s\" finished: SUCCESS\n", argv[1]);
		return EXIT_SUCCESS;
	} else {
		fprintf(stderr, "Test \"%s\" finished: FAILURE\n", argv[1]);
		return EXIT_FAILURE;
	}
}