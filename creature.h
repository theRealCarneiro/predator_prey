#define EMPTY 0
#define ROCK 1
#define BUNNY 2
#define FOX 3

typedef struct creature{
	int x;				// target x
	int y;				// target y
	int oldx;			// current x
	int oldy;			// current y
	int type;			// type of creature (defined above)
	int id;				// index in creature list
	int proc_age;		// generations since was born/last time procreated
	int hung_age;		// generations since was born/fed
} creature;

typedef struct settings {
	int GEN_PROC_BUNNY;	// number of generations until a bunny procreates
	int GEN_PROC_FOX;	// number of generations until a fox procreates
	int GEN_FOOD_FOX;	// number of generations until a fox starves
	int N_GEN;			// number of generations 
	int L;				// number of lines in matrix
	int C;				// number of columns in matrix
	int N;				// number of creatures in grid
	int C_GEN;			// current generation
} settings;

typedef struct list {
	int size;			// total list size
	int used;			// number of elements
	creature* array;	// array
} list;

// Malloc list 
list create_list(int size);

// Free list
void destroy_list(list l);

// Add creature to list, reallocs it if necessary
void append_creature(list* l, creature c);

// Removes all creatures that have an invalid id
void fix_list(list* l);

// Prints all info about a creature
void print_creature(creature c);

// Prints all creatures in list
void print_list(list* l);

// Moves a specific creature (does not update grid)
creature move_creature(settings s, creature* grid, creature c);

// Moves a list of creatures (does not update grid)
void move_creatures(settings s, creature* grid, list* l);

// Make two creatures brawl and see who wins
int brawl(creature atack, creature defense);

// Moves creatures in grid and solves conflicts
void solve_conflict(settings s, creature* grid, list* l_other, list* l);

// Print current state of grid
void print_grid(settings s, creature* grid);
