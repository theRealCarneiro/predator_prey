#define EMPTY 0
#define ROCK 1
#define BUNNY 2
#define FOX 3

typedef struct creature{
	int x;
	int y;
	int type;
	int proc_age;
	int hung_age;
} creature;

typedef struct list {
	int size;
	int used;
	creature* array;
} list;

list create_list(int size);
void destroy_list(list l);
void append_list(list* l, creature c);
void remove_list(list* l, int id);
void remove_id(list* l, int id);
void print_list(list* l);

creature create_creature(int x, int y, int type);
creature move_creature(int L, int C, creature grid[L][C], creature c, int cur_gen);
list move_creatures(int L, int C, creature grid[L][C], int cur_gen, list l);
void solve_conflict(int L, int C, creature grid[L][C], int cur_gen, int proc_gen,
		int hung_age, list l_old, list* l_other, list* l);

void move_foxes(int L, int C, creature grid[L][C], int cur_gen);
void solve_conflict_foxes(int L, int C, creature grid[L][C],
		int cur_gen, int GEN_PROC_FOX, int GEN_FOOD_FOX);

void print_grid(int L, int C, creature grid[L][C]);
