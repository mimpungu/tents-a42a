#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "queue.h"

/* ********** TEST DUMMY ********** */

bool test_dummy(char* arg) { return (0 == strcmp(arg, "dummy")); }

/* ********** TEST GAME_DEFAULT ********** */

bool test_game_default() {
	game test = game_default();

	square testsquares[8][8] = {
		{EMPTY, EMPTY, EMPTY, EMPTY, TREE, TREE, EMPTY, EMPTY},
		{TREE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, TREE},
		{EMPTY, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY},
		{TREE, EMPTY, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY},
		{EMPTY, TREE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
		{TREE, EMPTY, EMPTY, EMPTY, TREE, EMPTY, TREE, EMPTY},
		{EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
		{TREE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY}};

	int expected_row[8] = {3, 0, 4, 0, 4, 0, 1, 0};
	int expected_col[8] = {4, 0, 1, 2, 1, 1, 2, 1};

	for (int i = 0; i < DEFAULT_SIZE; ++i) {
		if (game_get_expected_nb_tents_row(test, i) != expected_row[i]) {
			game_delete(test);
			return false;
		}
		if (game_get_expected_nb_tents_col(test, i) != expected_col[i]) {
			game_delete(test);
			return false;
		}
		for (int j = 0; j < DEFAULT_SIZE; ++j) {
			if (game_get_square(test, i, j) != testsquares[i][j]) {
				return false;
			}
		}
	}
	game_delete(test);
	return true;
}

/* ********** TEST GAME_DEFAULT_SOLUTION ********** */

bool test_game_default_solution() {
	game test = game_default_solution();
	square testsquares[8][8] = {
		{TENT, GRASS, GRASS, TENT, TREE, TREE, TENT, GRASS},
		{TREE, GRASS, GRASS, GRASS, GRASS, GRASS, GRASS, TREE},
		{TENT, GRASS, GRASS, TENT, TREE, TENT, GRASS, TENT},
		{TREE, GRASS, GRASS, GRASS, GRASS, TREE, GRASS, GRASS},
		{TENT, TREE, TENT, GRASS, TENT, GRASS, TENT, GRASS},
		{TREE, GRASS, GRASS, GRASS, TREE, GRASS, TREE, GRASS},
		{TENT, GRASS, GRASS, GRASS, GRASS, GRASS, GRASS, GRASS},
		{TREE, GRASS, GRASS, GRASS, GRASS, GRASS, GRASS, GRASS}};

	int expected_row[8] = {3, 0, 4, 0, 4, 0, 1, 0};
	int expected_col[8] = {4, 0, 1, 2, 1, 1, 2, 1};

	for (int i = 0; i < DEFAULT_SIZE; ++i) {
		if (game_get_expected_nb_tents_row(test, i) != expected_row[i]) {
			game_delete(test);
			return false;
		}
		if (game_get_expected_nb_tents_col(test, i) != expected_col[i]) {
			game_delete(test);
			return false;
		}
		for (int j = 0; j < DEFAULT_SIZE; ++j) {
			if (game_get_square(test, i, j) != testsquares[i][j]) {
				game_delete(test);
				return false;
			}
		}
	}
	game_delete(test);
	return true;
}

/* ********** TEST GAME_NEW ********** */

bool test_game_new() {
	uint expected_row[8] = {3, 0, 4, 0, 4, 0, 1, 0};
	uint expected_col[8] = {4, 0, 1, 2, 1, 1, 2, 1};

	square testsquares[8][8] = {
		{EMPTY, EMPTY, EMPTY, EMPTY, TREE, TREE, EMPTY, EMPTY},
		{TREE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, TREE},
		{EMPTY, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY},
		{TREE, EMPTY, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY},
		{EMPTY, TREE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
		{TREE, EMPTY, EMPTY, EMPTY, TREE, EMPTY, TREE, EMPTY},
		{EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
		{TREE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY}};

	game test = game_new(*testsquares, expected_row, expected_col);

	for (int i = 0; i < DEFAULT_SIZE; ++i) {
		if (game_get_expected_nb_tents_row(test, i) != expected_row[i]) {
			fprintf(stderr, "error in expected row %d", i);
			game_delete(test);
			return false;
		}
		if (game_get_expected_nb_tents_col(test, i) != expected_col[i]) {
			fprintf(stderr, "error in expected col %d", i);
			game_delete(test);
			return false;
		}
		for (int j = 0; j < DEFAULT_SIZE; ++j) {
			if (game_get_square(test, i, j) != testsquares[i][j]) {
				fprintf(stderr,
						"error in square [%d][%d] expected %u found %u\n", i, j,
						testsquares[i][j], game_get_square(test, i, j));
				game_delete(test);
				return false;
			} else {
				fprintf(stderr,
						"no error in square [%d][%d] expected %u found %u\n", i,
						j, testsquares[i][j], game_get_square(test, i, j));
			}
		}
	}
	game_delete(test);
	return true;
}

/* ********** TEST GAME_EQUAL ********** */

bool test_game_equal() {
	game test1;
	game test2;

	test1 = game_new_empty_ext(2, 4, false, false);
	test2 = game_new_empty_ext(2, 4, true, false);
	assert(!game_equal(test1, test2));
	game_delete(test1);
	game_delete(test2);
	test2 = game_new_empty_ext(2, 4, true, true);
	test1 = game_new_empty_ext(2, 4, false, false);
	assert(!game_equal(test1, test2));
	game_delete(test1);
	game_delete(test2);
	test1 = game_new_empty_ext(2, 4, true, false);
	test2 = game_new_empty_ext(2, 4, true, true);
	assert(!game_equal(test1, test2));
	game_delete(test1);
	game_delete(test2);
	test1 = game_new_empty_ext(2, 4, false, false);
	test2 = game_new_empty_ext(2, 4, false, true);
	assert(!game_equal(test1, test2));
	game_delete(test1);
	game_delete(test2);
	test1 = game_new_empty_ext(2, 4, true, true);
	test2 = game_new_empty_ext(2, 4, true, true);
	assert(game_equal(test1, test2));
	game_delete(test1);
	game_delete(test2);
	test1 = game_new_empty_ext(3, 4, false, false);
	test2 = game_new_empty_ext(2, 4, false, false);
	assert(!game_equal(test1, test2));
	game_delete(test1);
	game_delete(test2);
	test1 = game_new_empty_ext(2, 3, false, false);
	test2 = game_new_empty_ext(2, 4, false, false);
	assert(!game_equal(test1, test2));
	game_delete(test1);
	game_delete(test2);
	test1 = game_new_empty_ext(2, 4, false, false);
	test2 = game_new_empty_ext(2, 4, false, false);
	game_play_move(test1, 1, 1, TENT);
	assert(!game_equal(test1, test2));
	game_delete(test1);
	game_delete(test2);
	test1 = game_new_empty_ext(2, 4, false, false);
	test2 = game_new_empty_ext(2, 4, false, false);
	game_set_expected_nb_tents_row(test1, 1, 12);
	game_set_expected_nb_tents_row(test2, 1, 13);
	assert(!game_equal(test1, test2));
	game_delete(test1);
	game_delete(test2);
	test1 = game_new_empty_ext(2, 4, false, false);
	test2 = game_new_empty_ext(2, 4, false, false);
	game_set_expected_nb_tents_col(test1, 1, 12);
	game_set_expected_nb_tents_col(test2, 1, 13);
	assert(!game_equal(test1, test2));
	game_delete(test1);
	game_delete(test2);
	test1 = game_new_empty_ext(2, 4, false, false);
	test2 = game_new_empty_ext(2, 4, false, false);
	assert(game_equal(test1, test2));
	game_delete(test1);
	game_delete(test2);
	return true;
}

/* ********** TEST GAME_SET_EXPECTED_NB_TENTS_ROW ********** */

bool test_game_set_expected_nb_tents_row() {
	game test = game_default();

	for (int i = 0; i < DEFAULT_SIZE; ++i) {
		game_set_expected_nb_tents_row(test, i, 4);
		if (game_get_expected_nb_tents_row(test, i) != 4) {
			game_delete(test);
			return false;
		}
	}
	game_delete(test);
	return true;
}

/* ********** TEST GAME_SET_EXPECTED_NB_TENTS_COL ********** */

bool test_game_set_expected_nb_tents_col() {
	game test = game_default();

	for (int i = 0; i < DEFAULT_SIZE; ++i) {
		game_set_expected_nb_tents_col(test, i, 4);
		if (game_get_expected_nb_tents_col(test, i) != 4) {
			game_delete(test);
			return false;
		}
	}
	game_delete(test);
	return true;
}

/* ********** TEST GAME_GET_EXPECTED_NB_TENTS_ROW ********** */

bool test_game_get_expected_nb_tents_row() {
	game test = game_default();
	int expected[8] = {3, 0, 4, 0, 4, 0, 1, 0};
	for (int i = 0; i < DEFAULT_SIZE; ++i) {
		if (game_get_expected_nb_tents_row(test, i) != expected[i]) {
			game_delete(test);
			return false;
		}
	}
	game_delete(test);
	return true;
}

/* ********** TEST GAME_GET_EXPECTED_NB_TENTS_COL ********** */

bool test_game_get_expected_nb_tents_col() {
	game test = game_default();
	int expected[8] = {4, 0, 1, 2, 1, 1, 2, 1};
	for (int i = 0; i < DEFAULT_SIZE; ++i) {
		if (game_get_expected_nb_tents_col(test, i) != expected[i]) {
			game_delete(test);
			return false;
		}
	}
	game_delete(test);
	return true;
}

/* ********** TEST GAME_GET_EXPECTED_NB_TENTS_ALL ********** */

bool test_game_get_expected_nb_tents_all() {
	game test1 = game_default();
	game test2 = game_default_solution();
	if (game_get_expected_nb_tents_all(test1) != 12) {
		game_delete(test1);
		game_delete(test2);
		return false;
	}
	if (game_get_expected_nb_tents_all(test2) != 12) {
		game_delete(test1);
		game_delete(test2);
		return false;
	}
	game_delete(test1);
	game_delete(test2);
	return true;
}

/* ********** TEST GAME_GET_CURRENT_NB_TENTS_ROW ********** */

bool test_game_get_current_nb_tents_row() {
	game test1 = game_default();
	game test2 = game_default_solution();
	int expected_row[8] = {3, 0, 4, 0, 4, 0, 1, 0};

	for (int i = 0; i < DEFAULT_SIZE; ++i) {
		if (game_get_current_nb_tents_row(test1, i) != 0) {
			game_delete(test1);
			game_delete(test2);
			return false;
		}
		if (game_get_current_nb_tents_row(test2, i) != expected_row[i]) {
			game_delete(test1);
			game_delete(test2);
			return false;
		}
	}
	game_delete(test1);
	game_delete(test2);
	return true;
}

/* ********** TEST GAME_GET_CURRENT_NB_TENTS_COL ********** */

bool test_game_get_current_nb_tents_col() {
	game test1 = game_default();
	game test2 = game_default_solution();
	int expected_col[8] = {4, 0, 1, 2, 1, 1, 2, 1};

	for (int i = 0; i < DEFAULT_SIZE; ++i) {
		if (game_get_current_nb_tents_col(test1, i) != 0) {
			game_delete(test1);
			game_delete(test2);
			return false;
		}
		if (game_get_current_nb_tents_col(test2, i) != expected_col[i]) {
			game_delete(test1);
			game_delete(test2);
			return false;
		}
	}
	game_delete(test1);
	game_delete(test2);
	return true;
}

/* ********** TEST GAME_GET_CURRENT_NB_TENTS_ALL ********** */

bool test_game_get_current_nb_tents_all() {
	game test1 = game_default();
	game test2 = game_default_solution();
	if (game_get_current_nb_tents_all(test1) != 0) {
		game_delete(test1);
		game_delete(test2);
		return false;
	}
	if (game_get_current_nb_tents_all(test2) != 12) {
		game_delete(test1);
		game_delete(test2);
		return false;
	}
	game_delete(test1);
	game_delete(test2);
	return true;
}

/* ********** TEST GAME_PLAY_MOVE ********** */

bool test_game_play_move() {
	game test = game_default();

	for (int i = 0; i < DEFAULT_SIZE; ++i) {
		game_play_move(test, 6, i, TENT);
		if (game_get_square(test, 6, i) != TENT) {
			game_delete(test);
			return false;
		}
		game_play_move(test, 6, i, GRASS);
		if (game_get_square(test, 6, i) != GRASS) {
			game_delete(test);
			return false;
		}
		game_play_move(test, 6, i, EMPTY);
		if (game_get_square(test, 6, i) != EMPTY) {
			game_delete(test);
			return false;
		}
		game_play_move(test, i, 3, TENT);
		if (game_get_square(test, i, 3) != TENT) {
			game_delete(test);
			return false;
		}
		game_play_move(test, i, 3, GRASS);
		if (game_get_square(test, i, 3) != GRASS) {
			game_delete(test);
			return false;
		}
		game_play_move(test, i, 3, EMPTY);
		if (game_get_square(test, i, 3) != EMPTY) {
			game_delete(test);
			return false;
		}
	}
	game_delete(test);
	return true;
}

/* ********** TEST GAME_RESTART ********** */

bool test_game_restart() {
	game test1 = game_default_solution();
	game test2 = game_default();
	game_restart(test1);
	assert(game_equal(test1, test2));
	game_undo(test1);
	assert(game_equal(test1, test2));
	game_redo(test1);
	assert(game_equal(test1, test2));
	game_delete(test1);
	game_delete(test2);
	return true;
}

/* ********** TEST GAME_NEW_EMPTY_EXT ********** */

bool test_game_new_empty_ext() {
	game g = game_new_empty_ext(3, 3, false, false);
	assert(game_nb_cols(g) == 3);
	assert(game_nb_rows(g) == 3);
	assert(!game_is_diagadj(g));
	assert(!game_is_wrapping(g));
	for (int i = 0; i < game_nb_rows(g); ++i)
		assert(game_get_expected_nb_tents_row(g, i) == 0);
	for (int i = 0; i < game_nb_cols(g); ++i)
		assert(game_get_expected_nb_tents_col(g, i) == 0);
	for (int i = 0; i < game_nb_rows(g); ++i)
		for (int j = 0; j < game_nb_cols(g); ++j)
			assert(game_get_square(g, i, j) == EMPTY);
	game_delete(g);

	g = game_new_empty_ext(8, 10, true, true);
	assert(game_nb_cols(g) == 10);
	assert(game_nb_rows(g) == 8);
	assert(game_is_diagadj(g));
	assert(game_is_wrapping(g));
	for (int i = 0; i < game_nb_rows(g); ++i)
		assert(game_get_expected_nb_tents_row(g, i) == 0);
	for (int i = 0; i < game_nb_cols(g); ++i)
		assert(game_get_expected_nb_tents_col(g, i) == 0);
	for (int i = 0; i < game_nb_rows(g); ++i)
		for (int j = 0; j < game_nb_cols(g); ++j)
			assert(game_get_square(g, i, j) == EMPTY);

	game_delete(g);
	return true;
}

/* ********** TEST GAME_UNDO ********** */

bool test_game_undo() {
	game g1 = game_default();
	game g2 = game_default();
	game_play_move(g1, 0, 1, TENT);
	game_undo(g1);
	assert(game_equal(g1, g2));
	game_undo(g1);
	assert(game_equal(g1, g2));
	game_delete(g1);
	game_delete(g2);
	return true;
}

/* ********** TEST GAME_REDO ********** */

bool test_game_redo() {
	game g1 = game_default();
	game g2 = game_default();
	game_play_move(g2, 0, 1, TENT);
	game_play_move(g1, 0, 1, TENT);
	game_undo(g1);
	game_redo(g1);
	assert(game_equal(g1, g2));
	game_redo(g1);
	assert(game_equal(g1, g2));
	game_delete(g1);
	game_delete(g2);
	return true;
}

/* ********** TEST GAME_FILL_GRASS_ROW ********** */

bool test_game_fill_grass_row() {
	game g = game_new_empty_ext(4, 4, false, false);
	game_set_square(g, 0, 0, TREE);
	game_fill_grass_row(g, 0);
	assert(game_get_square(g, 0, 0) == TREE);
	assert(game_get_square(g, 0, 1) == GRASS);
	assert(game_get_square(g, 0, 2) == GRASS);
	assert(game_get_square(g, 0, 3) == GRASS);
	game_undo(g);
	assert(game_get_square(g, 0, 0) == TREE);
	assert(game_get_square(g, 0, 1) == GRASS);
	assert(game_get_square(g, 0, 2) == GRASS);
	assert(game_get_square(g, 0, 3) == EMPTY);
	game_redo(g);
	assert(game_get_square(g, 0, 0) == TREE);
	assert(game_get_square(g, 0, 1) == GRASS);
	assert(game_get_square(g, 0, 2) == GRASS);
	assert(game_get_square(g, 0, 3) == GRASS);
	game_delete(g);
	return true;
}

/* ********** TEST GAME_FILL_GRASS_COL ********** */

bool test_game_fill_grass_col() {
	game g = game_new_empty_ext(4, 4, false, false);
	game_set_square(g, 0, 0, TREE);
	game_fill_grass_col(g, 0);
	assert(game_get_square(g, 0, 0) == TREE);
	assert(game_get_square(g, 1, 0) == GRASS);
	assert(game_get_square(g, 2, 0) == GRASS);
	assert(game_get_square(g, 3, 0) == GRASS);
	game_undo(g);
	assert(game_get_square(g, 0, 0) == TREE);
	assert(game_get_square(g, 1, 0) == GRASS);
	assert(game_get_square(g, 2, 0) == GRASS);
	assert(game_get_square(g, 3, 0) == EMPTY);
	game_redo(g);
	assert(game_get_square(g, 0, 0) == TREE);
	assert(game_get_square(g, 1, 0) == GRASS);
	assert(game_get_square(g, 2, 0) == GRASS);
	assert(game_get_square(g, 3, 0) == GRASS);
	game_delete(g);
	return true;
}

/* ********** USAGE ********** */

void usage(int argc, char* argv[]) {
	fprintf(stderr, "Usage: %s <testname> [<...>]\n", argv[0]);
	exit(EXIT_FAILURE);
}

/* ********** MAIN ROUTINE ********** */

int main(int argc, char* argv[]) {
	if (argc == 1) usage(argc, argv);

	fprintf(stderr, "=> Start test \"%s\"\n", argv[1]);
	bool ok = false;
	if (0 == strcmp("dummy", argv[1])) {
		ok = test_dummy(argv[1]);
	} else if (0 == strcmp("game_default", argv[1])) {
		ok = test_game_default();
	} else if (0 == strcmp("game_default_solution", argv[1])) {
		ok = test_game_default_solution();
	} else if (0 == strcmp("game_restart", argv[1])) {
		ok = test_game_restart();
	} else if (0 == strcmp("game_play_move", argv[1])) {
		ok = test_game_play_move();
	} else if (0 == strcmp("game_new", argv[1])) {
		ok = test_game_new();
	} else if (0 == strcmp("game_equal", argv[1])) {
		ok = test_game_equal();
	} else if (0 == strcmp("game_set_expected_nb_tents_row", argv[1])) {
		ok = test_game_set_expected_nb_tents_row();
	} else if (0 == strcmp("game_set_expected_nb_tents_col", argv[1])) {
		ok = test_game_set_expected_nb_tents_col();
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
	} else if (0 == strcmp("game_get_current_nb_tents_all", argv[1])) {
		ok = test_game_get_current_nb_tents_all();
	} else if (0 == strcmp("game_new_empty_ext", argv[1])) {
		ok = test_game_new_empty_ext();
	} else if (0 == strcmp("game_undo", argv[1])) {
		ok = test_game_undo();
	} else if (0 == strcmp("game_redo", argv[1])) {
		ok = test_game_redo();
	} else if (0 == strcmp("game_fill_grass_row", argv[1])) {
		ok = test_game_fill_grass_row();
	} else if (0 == strcmp("game_fill_grass_col", argv[1])) {
		ok = test_game_fill_grass_col();
	} else {
		fprintf(stderr, "Error: test \"%s\" not found!\n", argv[1]);
		exit(EXIT_FAILURE);
	}

	if (ok) {
		fprintf(stderr, "Test \"%s\" finished: SUCCESS\n", argv[1]);
		return EXIT_SUCCESS;
	} else {
		fprintf(stderr, "Test \"%s\" finished: FAILURE\n", argv[1]);
		return EXIT_FAILURE;
	}
}