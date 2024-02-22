#include <iostream>

abstraction_t *abs1;
abstraction_t *abs2;
abstraction_t *abs3;

state_map_t *map1;
state_map_t *map2;
state_map_t *map3;

state_t abs_state1;
state_t abs_state2;
state_t abs_state3;

using namespace std;

void cargarPDB(){

	FILE *f1 = fopen("towers-of-hanoi_abst1.pdb", "r");
	FILE *f2 = fopen("towers-of-hanoi_abst2.pdb", "r");
 
	abs1 = read_abstraction_from_file("towers-of-hanoi_abst1.abst");
	map1 = read_state_map(f1);

	abs2 = read_abstraction_from_file("towers-of-hanoi_abst2.abst");
	map2 = read_state_map(f2);

	fclose(f1);
	fclose(f2);
}

unsigned heuristicas(state_t puzzle_state){
	
	abstract_state(abs1, &puzzle_state, &abs_state1);
	abstract_state(abs2, &puzzle_state, &abs_state2);

	unsigned max = max(*state_map_get(map1, &abs_state1), *state_map_get(map2, &abs_state2));

	return (max);
}