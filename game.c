#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "game.h"
#include "game_ext.h"
#include "queue.h"

/**
 * @brief The game structure
 * @
 **/
typedef struct game_s {
	square** grid;				 /**< the game grid */
	uint* expected_nb_tents_row; /**< values expected for each row */
	uint* expected_nb_tents_col; /**< values expected for each column */
	uint nb_of_rows;			 /**< the game height */
	uint nb_of_cols;			 /**< the game width */
	bool is_wrapping;			 /**< option for wrapping */
	bool diagadj;				 /**< option for diagonal tents */
	queue* undo_queue;			 /**< the queue used to store possible undos */
	queue* redo_queue;			 /**< the queue used to store possible redos*/
} game_s;

/**
 * @brief Used to store moves for undo and redo
 **/
typedef struct move_s {
	square square;
	int i;
	int j;
} move;

/**
 * @brief Used to store coordinates.
 **/
typedef struct couple_s {
	int x;
	int y;
} couple;

////////////////// AUX FUNCTIONS //////////////////////////////////////////////

// REALLY can't believe I need to do this : in C the '%' operator is NOT a
// modulo but a remainder therefore I need to make a modulo function myself
int static mod(int a, int b) {
	int r = a % b;
	return r < 0 ? r + b : r;
}

/**
 * @brief Looks for square in the vicinity of (i,j)
 * @details This function looks for the square s in the orthogonal (and diagonal
 * when it applies) neighbors of (i,j), useful for rule 1 and rule 4.
 * @param g the game
 * @param i row index
 * @param j column index
 * @param s the square to find
 * @param rule4 if true ignore diagonals
 * @pre @p g must be a valid pointer toward a game structure.
 * @pre @p i < game width
 * @pre @p j < game height
 * @returns true if s is present, false if not
 */
bool static aux_check_square(cgame g, uint i, uint j, square s, bool rule4) {
	int nb_rows = game_nb_rows(g);
	int nb_cols = game_nb_cols(g);
	int I = i;
	int J = j;
	couple card[4] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};
	couple diag[4] = {{1, 1}, {-1, 1}, {1, -1}, {-1, -1}};

	for (int n = 0; n < 4; n++) {
		int x1 = card[n].x;
		int y1 = card[n].y;
		int x2 = diag[n].x;
		int y2 = diag[n].y;

		if (game_is_wrapping(g) || (I + x1 >= 0 && J + y1 >= 0 &&
									I + x1 < nb_rows && J + y1 < nb_cols)) {
			if (game_get_square(g, mod(I + x1, nb_rows),
								mod(J + y1, nb_cols)) == s) {
				return true;
			}
		}

		if (!game_is_diagadj(g) && !rule4) {
			if (game_is_wrapping(g) || (I + x2 >= 0 && J + y2 >= 0 &&
										I + x2 < nb_rows && J + y2 < nb_cols)) {
				if (game_get_square(g, mod(I + x2, nb_rows),
									mod(J + y2, nb_cols)) == s) {
					return true;
				}
			}
		}
	}
	return false;
}

/**
 * @brief Gets the current number of empty squares in a given row.
 * @param g the game
 * @param i row index
 * @return the current number of empty squares on this row
 * @pre @p g must be a valid pointer toward a game structure.
 * @pre @p i < game width
 **/
uint static aux_get_current_nb_empty_row(cgame g, uint i) {
	uint n = 0;
	for (int j = 0; j < game_nb_cols(g); j++)
		if (game_get_square(g, i, j) == EMPTY) n++;

	return n;
}

/**
 * @brief Gets the current number of empty squares in a given column.
 * @param g the game
 * @param j column index
 * @return the current number of empty squares on this column
 * @pre @p g must be a valid pointer toward a game structure.
 * @pre @p j < game height
 **/
uint static aux_get_current_nb_empty_col(cgame g, uint j) {
	uint n = 0;
	for (int i = 0; i < game_nb_rows(g); i++)
		if (game_get_square(g, i, j) == EMPTY) n++;

	return n;
}

/**
 * @brief Gets the current number of empty squares on the whole grid.
 * @param g the game
 * @return the current number of empty squares on this game
 * @pre @p g must be a valid pointer toward a game structure.
 **/
uint static aux_get_current_nb_empty_all(cgame g) {
	uint n = 0;
	for (int i = 0; i < game_nb_rows(g); ++i)
		n += aux_get_current_nb_empty_row(g, i);
	return n;
}

/**
 * @brief Gets the current number of trees on the whole grid.
 * @param g the game
 * @return the current number of trees on this game
 * @pre @p g must be a valid pointer toward a game structure.
 **/
uint static aux_get_current_nb_tree_all(cgame g) {
	uint n = 0;
	for (int i = 0; i < game_nb_rows(g); ++i)
		for (int j = 0; j < game_nb_cols(g); ++j)
			if (game_get_square(g, i, j) == TREE) n++;
	return n;
}

/**
 * @brief Checks if the whole grid complies with rule 1
 * @param g the game
 * @return true if yes false if no
 * @pre @p g must be a valid pointer toward a game structure.
 **/
bool static aux_rule1_on_grid(cgame g) {
	for (int i = 0; i < game_nb_rows(g); ++i) {
		for (int j = 0; j < game_nb_cols(g); j++) {
			if (game_get_square(g, i, j) == TENT &&
				aux_check_square(g, i, j, TENT, false))
				return false;
		}
	}
	return true;
}

/**
 * @brief Checks if the whole grid complies with rule 2
 * @param g the game
 * @return true if yes false if no
 * @pre @p g must be a valid pointer toward a game structure.
 **/
bool static aux_rule2(cgame g) {
	for (int i = 0; i < game_nb_rows(g); ++i) {
		if (game_get_expected_nb_tents_row(g, i) !=
			game_get_current_nb_tents_row(g, i))
			return false;
	}
	for (int j = 0; j < game_nb_cols(g); ++j) {
		if (game_get_expected_nb_tents_col(g, j) !=
			game_get_current_nb_tents_col(g, j))
			return false;
	}
	return true;
}

/**
 * @brief Checks if the whole grid complies with rule 3
 * @param g the game
 * @return true if yes false if no
 * @pre @p g must be a valid pointer toward a game structure.
 **/
bool static aux_rule3(cgame g) {
	return (aux_get_current_nb_tree_all(g) == game_get_current_nb_tents_all(g));
}

/**
 * @brief Checks if the whole grid complies with rule 4
 * @param g the game
 * @return true if yes false if no
 * @pre @p g must be a valid pointer toward a game structure.
 **/
bool static aux_rule4_on_grid(cgame g) {
	for (int i = 0; i < game_nb_rows(g); ++i) {
		for (int j = 0; j < game_nb_cols(g); j++) {

			if (game_get_square(g, i, j) == TENT &&
				!aux_check_square(g, i, j, TREE, true))
				return false;

			if (game_get_square(g, i, j) == TREE &&
				!aux_check_square(g, i, j, TENT, true))
				return false;
		}
	}
	return true;
}

/**
 * @brief Checks for rule 4 when playing GRASS (works not so well)
 * @param g the game
 * @param i row index
 * @param j column index
 * @pre @p g must be a valid pointer toward a game structure.
 * @pre @p i < game width
 * @pre @p j < game height
 * @returns true if ok is present, false if not
 */
bool static aux_rule4_on_grass(cgame g, uint i, uint j) {
	int nb_rows = game_nb_rows(g);
	int nb_cols = game_nb_cols(g);
	int I = i;
	int J = j;

	couple card[4] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};

	for (int n = 0; n < 4; n++) {
		int x1 = card[n].x;
		int y1 = card[n].y;

		if (game_is_wrapping(g) || (I + x1 >= 0 && J + y1 >= 0 &&
									I + x1 < nb_rows && J + y1 < nb_cols)) {
			if (game_get_square(g, mod(I + x1, nb_rows),
								mod(J + y1, nb_cols)) == TREE) {
				if (aux_check_square(g, mod(I + x1, nb_rows),
									 mod(J + y1, nb_cols), TENT, true)) {
					return true;
				}
			}
		}
	}
	return false;
}

/**
 * @brief Allocates and adds a move to the undo queue
 * @param g the game
 * @param i the row index
 * @param j the column index
 * @param s the square value
 * @pre @p g must be a valid pointer toward a game structure.
 * @pre @p i < game width
 * @pre @p j < game height
 * @pre @p s must be either EMPTY, GRASS or TENT (but not TREE).
 **/
void aux_add_to_undo(game g, uint i, uint j, square s) {
	move* m = malloc(sizeof(move));
	m->i = i;
	m->j = j;
	m->square = s;
	queue_push_head(g->undo_queue, m);
}

/**
 * @brief Allocates and adds a move to the redo queue
 * @param g the game
 * @param i the row index
 * @param j the column index
 * @param s the square value
 * @pre @p g must be a valid pointer toward a game structure.
 * @pre @p i < game width
 * @pre @p j < game height
 * @pre @p s must be either EMPTY, GRASS or TENT (but not TREE).
 **/
void aux_add_to_redo(game g, uint i, uint j, square s) {
	move* m = malloc(sizeof(move));
	m->i = i;
	m->j = j;
	m->square = s;
	queue_push_head(g->redo_queue, m);
}

void aux_null_game_pointer_handler(cgame g, char const* caller) {
	if (g == NULL) {
		fprintf(stderr,
				"%s : g must be a valid pointer toward a game structure.",
				caller);
		exit(EXIT_FAILURE);
	}
}

void aux_invalid_width_handler(cgame g, uint i, char const* caller) {
	if (i > game_nb_rows(g) - 1) {
		fprintf(stderr, "%s : i must be >= 0 AND < width.", caller);
		exit(EXIT_FAILURE);
	}
}
void aux_invalid_height_handler(cgame g, uint j, char const* caller) {
	if (j > game_nb_cols(g) - 1) {
		fprintf(stderr, "%s : j must be >= 0 AND < height.", caller);
		exit(EXIT_FAILURE);
	}
}

////////////////// MAIN FUNCTIONS
/////////////////////////////////////////////////

game game_new(square* squares, uint* nb_tents_row, uint* nb_tents_col) {
	return game_new_ext(DEFAULT_SIZE, DEFAULT_SIZE, squares, nb_tents_row,
						nb_tents_col, false, false);
}

game game_new_empty(void) {
	return game_new_empty_ext(DEFAULT_SIZE, DEFAULT_SIZE, false, false);
}

game game_copy(cgame g) {
	aux_null_game_pointer_handler(g, __func__);

	square squares[game_nb_cols(g)][game_nb_rows(g)];
	uint nb_tents_row[game_nb_cols(g)];
	uint nb_tents_col[game_nb_rows(g)];

	for (int i = 0; i < game_nb_cols(g); ++i) {
		nb_tents_row[i] = game_get_expected_nb_tents_row(g, i);
		for (int j = 0; j < game_nb_rows(g); ++j) {
			nb_tents_col[j] = game_get_expected_nb_tents_col(g, j);
			squares[i][j] = game_get_square(g, i, j);
		}
	}

	return game_new_ext(game_nb_rows(g), game_nb_cols(g), *squares,
						nb_tents_row, nb_tents_col, game_is_wrapping(g),
						game_is_diagadj(g));
}

bool game_equal(cgame g1, cgame g2) {
	aux_null_game_pointer_handler(g1, __func__);
	aux_null_game_pointer_handler(g2, __func__);

	if (game_nb_rows(g1) != game_nb_rows(g2) ||
		game_nb_cols(g1) != game_nb_cols(g2)) {
		return false;
	}

	if ((game_is_diagadj(g1) != game_is_diagadj(g2)) ||
		game_is_wrapping(g1) != game_is_wrapping(g2))
		return false;

	for (int i = 0; i < game_nb_rows(g1); i++) {
		if (game_get_expected_nb_tents_row(g1, i) !=
			game_get_expected_nb_tents_row(g2, i)) {
			return false;
		}

		for (int j = 0; j < game_nb_cols(g1); j++) {
			if (game_get_expected_nb_tents_col(g1, j) !=
				game_get_expected_nb_tents_col(g2, j)) {
				return false;
			}

			if (game_get_square(g1, i, j) != game_get_square(g2, i, j)) {
				return false;
			}
		}
	}

	return true;
}

void game_delete(game g) {
	aux_null_game_pointer_handler(g, __func__);

	for (int i = 0; i < game_nb_rows(g); ++i) {
		free(g->grid[i]);
	}
	queue_free_full(g->undo_queue, free);
	queue_free_full(g->redo_queue, free);
	free(g->expected_nb_tents_row);
	free(g->expected_nb_tents_col);
	free(g->grid);
	free(g);
}

void game_set_square(game g, uint i, uint j, square s) {
	aux_null_game_pointer_handler(g, __func__);
	aux_invalid_width_handler(g, i, __func__);
	aux_invalid_height_handler(g, j, __func__);

	g->grid[i][j] = s;
}

square game_get_square(cgame g, uint i, uint j) {
	aux_null_game_pointer_handler(g, __func__);
	aux_invalid_width_handler(g, i, __func__);
	aux_invalid_height_handler(g, j, __func__);

	return g->grid[i][j];
}

void game_set_expected_nb_tents_row(game g, uint i, uint nb_tents) {
	aux_null_game_pointer_handler(g, __func__);
	aux_invalid_width_handler(g, i, __func__);

	g->expected_nb_tents_row[i] = nb_tents;
}

void game_set_expected_nb_tents_col(game g, uint j, uint nb_tents) {
	aux_null_game_pointer_handler(g, __func__);
	aux_invalid_height_handler(g, j, __func__);

	g->expected_nb_tents_col[j] = nb_tents;
}

uint game_get_expected_nb_tents_row(cgame g, uint i) {
	aux_null_game_pointer_handler(g, __func__);
	aux_invalid_width_handler(g, i, __func__);

	return g->expected_nb_tents_row[i];
}

uint game_get_expected_nb_tents_col(cgame g, uint j) {
	aux_null_game_pointer_handler(g, __func__);
	aux_invalid_height_handler(g, j, __func__);

	return g->expected_nb_tents_col[j];
}

uint game_get_expected_nb_tents_all(cgame g) {
	aux_null_game_pointer_handler(g, __func__);

	uint count = 0;
	for (uint i = 0; i < game_nb_rows(g); i++)
		count += g->expected_nb_tents_row[i];

	return count;
}

uint game_get_current_nb_tents_row(cgame g, uint i) {
	aux_null_game_pointer_handler(g, __func__);
	aux_invalid_width_handler(g, i, __func__);

	uint count = 0;
	for (int j = 0; j < game_nb_cols(g); j++)
		if (game_get_square(g, i, j) == TENT) count++;

	return count;
}

uint game_get_current_nb_tents_col(cgame g, uint j) {
	aux_null_game_pointer_handler(g, __func__);
	aux_invalid_height_handler(g, j, __func__);

	uint count = 0;
	for (int i = 0; i < game_nb_rows(g); ++i)
		if (game_get_square(g, i, j) == TENT) count++;

	return count;
}

uint game_get_current_nb_tents_all(cgame g) {
	aux_null_game_pointer_handler(g, __func__);

	uint count = 0;
	for (int i = 0; i < game_nb_rows(g); ++i)
		for (int j = 0; j < game_nb_cols(g); ++j)
			if (game_get_square(g, i, j) == TENT) count++;

	return count;
}

void game_play_move(game g, uint i, uint j, square s) {
	aux_null_game_pointer_handler(g, __func__);
	aux_invalid_width_handler(g, i, __func__);
	aux_invalid_height_handler(g, j, __func__);

	if (game_get_square(g, i, j) == TREE || s == TREE) {
		fprintf(stderr, "%s : illegal move.", __func__);
		exit(EXIT_FAILURE);
	}
	aux_add_to_undo(g, i, j, game_get_square(g, i, j));
	queue_clear_full(g->redo_queue, free);
	game_set_square(g, i, j, s);
}

int game_check_move(cgame g, uint i, uint j, square s) {
	aux_null_game_pointer_handler(g, __func__);
	aux_invalid_width_handler(g, i, __func__);
	aux_invalid_height_handler(g, j, __func__);

	if (i >= game_nb_rows(g) || j >= game_nb_cols(g)) {
		exit(EXIT_FAILURE);
	}
	// rule 0
	if ((s == TREE) || (game_get_square(g, i, j) == TREE)) {
		return ILLEGAL;
	} else if (s == EMPTY) {
		return REGULAR;
	}

	// rule 1
	if (s == TENT && aux_check_square(g, i, j, TENT, false)) return LOSING;

	// rule 2
	if (s == TENT && game_get_expected_nb_tents_row(g, i) <
						 game_get_current_nb_tents_row(g, i) + 1)
		return LOSING;

	if (s == TENT && game_get_expected_nb_tents_col(g, j) <
						 game_get_current_nb_tents_col(g, j) + 1)
		return LOSING;

	if (s == GRASS && game_get_expected_nb_tents_row(g, i) <
						  game_get_current_nb_tents_row(g, i) +
							  aux_get_current_nb_empty_row(g, i))
		return LOSING;

	if (s == GRASS && game_get_expected_nb_tents_col(g, j) <
						  game_get_current_nb_tents_col(g, j) +
							  aux_get_current_nb_empty_col(g, j))
		return LOSING;

	// rule 3
	if (s == TENT &&
		aux_get_current_nb_tree_all(g) < game_get_current_nb_tents_all(g) + 1)
		return LOSING;

	if (s == GRASS && game_get_expected_nb_tents_all(g) <=
						  game_get_current_nb_tents_all(g) +
							  aux_get_current_nb_empty_all(g) - 1)
		return LOSING;

	// rule 4
	if (s == TENT && !aux_check_square(g, i, j, TREE, true)) return LOSING;

	if (s == GRASS && !aux_rule4_on_grass(g, i, j)) return LOSING;

	return REGULAR;
}

bool game_is_over(cgame g) {
	aux_null_game_pointer_handler(g, __func__);

	return (aux_rule1_on_grid(g) && aux_rule2(g) && aux_rule3(g) &&
			aux_rule4_on_grid(g));
}

void game_fill_grass_row(game g, uint i) {
	aux_null_game_pointer_handler(g, __func__);
	aux_invalid_width_handler(g, i, __func__);

	for (uint j = 0; j < game_nb_cols(g); j++) {
		if (game_get_square(g, i, j) != TREE &&
			game_get_square(g, i, j) != TENT) {
			game_play_move(g, i, j, GRASS);
		}
	}
}

void game_fill_grass_col(game g, uint j) {
	aux_null_game_pointer_handler(g, __func__);
	aux_invalid_height_handler(g, j, __func__);

	for (uint i = 0; i < game_nb_rows(g); i++) {
		if (game_get_square(g, i, j) != TREE &&
			game_get_square(g, i, j) != TENT) {
			game_play_move(g, i, j, GRASS);
		}
	}
}

void game_restart(game g) {
	aux_null_game_pointer_handler(g, __func__);

	if (!queue_is_empty(g->undo_queue)) queue_clear_full(g->undo_queue, free);
	if (!queue_is_empty(g->redo_queue)) queue_clear_full(g->redo_queue, free);
	for (int i = 0; i < game_nb_rows(g); ++i)
		for (int j = 0; j < game_nb_cols(g); ++j) {
			if (game_get_square(g, i, j) != TREE)
				game_set_square(g, i, j, EMPTY);
		}
}

game game_new_ext(uint nb_rows, uint nb_cols, square* squares,
				  uint* nb_tents_row, uint* nb_tents_col, bool wrapping,
				  bool diagadj) {
	game_s* g = malloc(sizeof(game_s));
	g->undo_queue = queue_new();
	g->redo_queue = queue_new();
	g->nb_of_rows = nb_rows;
	g->nb_of_cols = nb_cols;
	g->is_wrapping = wrapping;
	g->diagadj = diagadj;
	g->expected_nb_tents_row = malloc(nb_rows * sizeof(uint));
	g->expected_nb_tents_col = malloc(nb_cols * sizeof(uint));

	g->grid = malloc(nb_rows * sizeof(square*));
	for (int i = 0; i < nb_rows; ++i) {
		g->grid[i] = malloc(nb_cols * sizeof(square));
		game_set_expected_nb_tents_row(g, i, nb_tents_row[i]);
		for (int j = 0; j < nb_cols; ++j) {
			game_set_expected_nb_tents_col(g, j, nb_tents_col[j]);
			game_set_square(g, i, j, squares[i * nb_cols + j]);
		}
	}
	return g;
}

game game_new_empty_ext(uint nb_rows, uint nb_cols, bool wrapping,
						bool diagadj) {
	game_s* g = malloc(sizeof(game_s));
	g->undo_queue = queue_new();
	g->redo_queue = queue_new();
	g->nb_of_rows = nb_rows;
	g->nb_of_cols = nb_cols;
	g->is_wrapping = wrapping;
	g->diagadj = diagadj;
	g->expected_nb_tents_row = calloc(nb_rows, sizeof(uint));
	g->expected_nb_tents_col = calloc(nb_cols, sizeof(uint));
	g->grid = malloc(nb_rows * sizeof(square*));
	for (int i = 0; i < nb_rows; ++i) {
		g->grid[i] = calloc(nb_cols, sizeof(square));
	}
	return g;
}

uint game_nb_rows(cgame g) {
	aux_null_game_pointer_handler(g, __func__);
	return g->nb_of_rows;
}

uint game_nb_cols(cgame g) {
	aux_null_game_pointer_handler(g, __func__);
	return g->nb_of_cols;
}

bool game_is_wrapping(cgame g) {
	aux_null_game_pointer_handler(g, __func__);
	return g->is_wrapping;
}

bool game_is_diagadj(cgame g) {
	aux_null_game_pointer_handler(g, __func__);
	return g->diagadj;
}

void game_undo(game g) {
	aux_null_game_pointer_handler(g, __func__);
	if (!queue_is_empty(g->undo_queue)) {
		move* m = (queue_pop_head(g->undo_queue));
		aux_add_to_redo(g, m->i, m->j, game_get_square(g, m->i, m->j));
		game_set_square(g, m->i, m->j, m->square);
		free(m);
	}
}

void game_redo(game g) {
	aux_null_game_pointer_handler(g, __func__);
	if (!queue_is_empty(g->redo_queue)) {
		move* m = (queue_pop_head(g->redo_queue));
		aux_add_to_undo(g, m->i, m->j, game_get_square(g, m->i, m->j));
		game_set_square(g, m->i, m->j, m->square);
		free(m);
	}
}
