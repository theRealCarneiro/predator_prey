#include <string.h>
#include <stdio.h>
#include "creature.h"

int GEN_PROC_BUNNY, GEN_PROC_FOX, GEN_FOOD_FOX, N_GEN, L, C, N;


int main() {
	list bunny;
	list fox;

	char obj[10];
	int x, y;

	char* file_path = "input.txt";
	FILE* file;
	file = fopen(file_path, "r");
	fscanf(file, "%d", &GEN_PROC_BUNNY);
	fscanf(file, "%d", &GEN_PROC_FOX);
	fscanf(file, "%d", &GEN_FOOD_FOX);
	fscanf(file, "%d", &N_GEN);
	fscanf(file, "%d", &L);
	fscanf(file, "%d", &C);
	fscanf(file, "%d", &N);

	creature grid[L][C];
	for (int i = 0; i < L; i++) {
		for (int j = 0; j < C; j++) {
			creature c = {i, j, EMPTY, 0, 0};
			grid[i][j] = c;
		}
	}

	bunny = create_list(L * C);
	fox = create_list(L * C);

	while(fscanf(file, "%s %d %d\n", obj, &x, &y) == 3){

		/*printf("%s %d %d\n", obj, x, y);*/
		// rock
		if (!strcmp("ROCHA", obj)) {
			grid[x][y].type = ROCK;

		// bunny
		} 
		else if (!strcmp("COELHO", obj)) {
			grid[x][y].type = BUNNY;
			append_list(&bunny, grid[x][y]);

		}
		// fox
		else if (!strcmp("RAPOSA", obj)) {
			grid[x][y].type = FOX;
			append_list(&fox, grid[x][y]);
		}

	}

	fclose(file);

	// Main Loop

	// Generations
	print_grid(L, C, grid);
	for (int g = 0; g < N_GEN; g++) {
		/*printf("\n------------\n");*/
		list bunny_new = move_creatures(L, C, grid, g, bunny);
		solve_conflict(L, C, grid, g, GEN_PROC_BUNNY, GEN_FOOD_FOX, bunny, &fox, &bunny_new);
		print_grid(L, C, grid);
		/*printf("\n------------\n");*/

		list fox_new = move_creatures(L, C, grid, g, fox);
		solve_conflict(L, C, grid, g, GEN_PROC_FOX, GEN_FOOD_FOX, fox, &bunny_new, &fox_new);
		print_grid(L, C, grid);

		destroy_list(bunny);
		destroy_list(fox);
		bunny = bunny_new;
		fox = fox_new;
	}
	print_grid(L, C, grid);

	return 0;
}
