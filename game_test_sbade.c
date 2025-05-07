#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "game.h"
#include "game_aux.h"
#include <string.h>

/*Game_test de BADE Séliane*/

bool test_dummy(char* arg) { return (strcmp(arg, "dummy") == 0); }

bool test_game_equal() {
	game g1 = game_default();
	game g2 = game_default();
	for (uint x = 0; x < DEFAULT_SIZE; x++) {
		if (game_get_expected_nb_tents_row(g1, x) !=
			game_get_expected_nb_tents_row(g2, x)) {
			game_delete(g1);
			game_delete(g2);
			return false;
		}
		for (uint y = 0; y < DEFAULT_SIZE; y++) {
			if (game_get_expected_nb_tents_col(g1, y) !=
				game_get_expected_nb_tents_col(g2, y)) {
				game_delete(g1);
				game_delete(g2);
				return false;
			}
			if (game_get_square(g1, x, y) != game_get_square(g2, x, y)) {
				game_delete(g1);
				game_delete(g2);
				return false;
			}
		}
	}
	game_delete(g1);
	game_delete(g2);
	return true;
}

bool test_game_get_expected_nb_tents_row() {
	game g = game_default();
	if (game_get_expected_nb_tents_row(g, 0) != 3) {
		game_delete(g);
		return false;
	}
	game_delete(g);
	return true;
}

bool test_game_get_expected_nb_tents_col() {
	game g = game_default();
	if (game_get_expected_nb_tents_col(g, 0) != 4) {
		game_delete(g);
		return false;
	}
	game_delete(g);
	return true;
}

bool test_game_get_expected_nb_tents_all() {
	game g = game_default();
	int n = 0;
	int s = 0;
	int l = 0;
	for (int x = 0; x < DEFAULT_SIZE; x++) {
		n = n + game_get_expected_nb_tents_row(g, x);
	}
	for (int y = 0; y < DEFAULT_SIZE; y++) {
		s = s + game_get_expected_nb_tents_col(g, y);
	}
	l = n + s;
	if ((l) != 24) {
		game_delete(g);
		return false;
	}
	game_delete(g);
	return true;
}

bool test_game_get_current_nb_tents_row() {
	game g = game_default();
	if (game_check_move(g, 2, 0, TENT) == REGULAR) {
		game_play_move(g, 2, 0, TENT);
		if (game_get_current_nb_tents_row(g, 2) != 1) {
			return false;
		}
	}
	return true;
}

bool test_game_get_current_nb_tents_col() {
	game g = game_default();
	if (game_check_move(g, 2, 0, TENT) == REGULAR) {
		game_play_move(g, 2, 0, TENT);
		if (game_get_current_nb_tents_col(g, 0) != 1) {
			return false;
		}
	}
	return true;
}

void usage(char* seli) {
	fprintf(stderr, "usage: %s <testgame> [<...>]\n", seli);
	exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]) {
	if (argc == 1) {
		usage(argv[0]);
	}
	fprintf(stderr, "=> Start test \"%s\"\n", argv[1]);
	bool ok = false;
	if (0 == strcmp("dummy", argv[1])) {
		ok = test_dummy(argv[1]);
	} else if (0 == strcmp("game_equal", argv[1])) {
		ok = test_game_equal();
	} else if (0 == strcmp("game_get_expected_nb_tents_row", argv[1])) {
		ok = test_game_get_expected_nb_tents_row();
	} else if (0 == strcmp("game_get_expected_nb_tents_col", argv[1])) {
		ok = test_game_get_expected_nb_tents_col();
	} else if (0 == strcmp("game_get_expected_nb_tents_all", argv[1])) {
		ok = test_game_get_expected_nb_tents_all();
	} else if (0 == strcmp("game_get_current_nb_tents_row", argv[1])) {
		ok = test_game_get_current_nb_tents_row();
	} else if (0 == strcmp("game_get_current_nb_tents_col", argv[1])) {
		ok = test_game_get_current_nb_tents_col();
	}
	if (ok) {
		fprintf(stderr, "Test \"%s\" SUCCESS\n", argv[1]);
		return EXIT_SUCCESS;
	} else {
		fprintf(stderr, "Test \"%s\" FAILURE\n", argv[1]);
		return EXIT_FAILURE;
	}
}