#include "game_aux.h"
#include "game_ext.h"
#include <stdio.h>

void game_print(cgame g) {

	printf("   ");
	for (int i = 0; i < game_nb_cols(g); ++i)
		printf("%d", i);
	printf("   \n   ");
	for (int i = 0; i < game_nb_cols(g); ++i)
		printf("-");
	printf("   \n");

	for (int i = 0; i < game_nb_rows(g); ++i) {
		printf("%d |", i);
		for (int j = 0; j < game_nb_cols(g); ++j) {
			square current_square = game_get_square(g, i, j);
			switch (current_square) {
			case EMPTY:
				printf(" ");
				break;
			case TREE:
				printf("x");
				break;
			case TENT:
				printf("*");
				break;
			case GRASS:
				printf("-");
				break;
			default:
				printf(" ");
				break;
			}
		}
		printf("| %d\n", game_get_expected_nb_tents_row(g, i));
	}

	printf("   ");
	for (int i = 0; i < game_nb_cols(g); ++i)
		printf("-");
	printf("   \n   ");
	for (int i = 0; i < game_nb_cols(g); ++i)
		printf("%d", game_get_expected_nb_tents_col(g, i));
	printf("   \n");
}

game game_default(void) {
	uint nb_tents_row[] = {3, 0, 4, 0, 4, 0, 1, 0};
	uint nb_tents_col[] = {4, 0, 1, 2, 1, 1, 2, 1};
	square squares[8][8] = {
		{EMPTY, EMPTY, EMPTY, EMPTY, TREE, TREE, EMPTY, EMPTY},
		{TREE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, TREE},
		{EMPTY, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY},
		{TREE, EMPTY, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY},
		{EMPTY, TREE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
		{TREE, EMPTY, EMPTY, EMPTY, TREE, EMPTY, TREE, EMPTY},
		{EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
		{TREE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY}};

	return game_new(*squares, nb_tents_row, nb_tents_col);
}

game game_default_solution(void) {
	uint nb_tents_row[] = {3, 0, 4, 0, 4, 0, 1, 0};
	uint nb_tents_col[] = {4, 0, 1, 2, 1, 1, 2, 1};
	square squares[8][8] = {
		{TENT, GRASS, GRASS, TENT, TREE, TREE, TENT, GRASS},
		{TREE, GRASS, GRASS, GRASS, GRASS, GRASS, GRASS, TREE},
		{TENT, GRASS, GRASS, TENT, TREE, TENT, GRASS, TENT},
		{TREE, GRASS, GRASS, GRASS, GRASS, TREE, GRASS, GRASS},
		{TENT, TREE, TENT, GRASS, TENT, GRASS, TENT, GRASS},
		{TREE, GRASS, GRASS, GRASS, TREE, GRASS, TREE, GRASS},
		{TENT, GRASS, GRASS, GRASS, GRASS, GRASS, GRASS, GRASS},
		{TREE, GRASS, GRASS, GRASS, GRASS, GRASS, GRASS, GRASS}};

	return game_new(*squares, nb_tents_row, nb_tents_col);
}