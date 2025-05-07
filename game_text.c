#include <stdlib.h>
#include <stdio.h>
#include "game_aux.h"
#include "game_ext.h"

void print_help() { printf("\nThere is no help\n\n."); }

void check_and_play_move(game g, uint i, uint j, square s) {
	if (game_check_move(g, i, j, s) != ILLEGAL) {
		game_play_move(g, i, j, s);
	} else {
		printf("\nWait, this is illegal !\n\n");
	}
}

int main(void) {

	game g = game_default();
	game_print(g);

	while (!game_is_over(g)) {
		char c;
		int i, j;
		int ret = scanf(" %c", &c);
		if (c == 't' || c == 'g' || c == 'e') ret += scanf("%d%d", &i, &j);
		if (ret == 1) {
			switch (c) {
			case 'z':
				game_undo(g);
				break;
			case 'y':
				game_redo(g);
				break;
			case 'h':
				print_help();
				break;
			case 'r':
				game_restart(g);
				break;
			case 'q':
				printf("SHAME\n");
				game_delete(g);
				return EXIT_SUCCESS;
				break;
			default:
				printf("wrong input");
				break;
			}
		} else if (ret == 3) {
			switch (c) {
			case 't':
				check_and_play_move(g, i, j, TENT);
				break;
			case 'g':
				check_and_play_move(g, i, j, GRASS);
				break;
			case 'e':
				check_and_play_move(g, i, j, EMPTY);
				break;
			default:
				printf("wrong input");
				break;
			}
		} else {
			printf("wrong input");
		}

		game_print(g);
	}
	printf("CONGRATULATIONS\n");
	return EXIT_SUCCESS;
}