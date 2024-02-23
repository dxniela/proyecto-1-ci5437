#include "../priority_queue.hpp"
#include <string>
#include <stdbool.h>
#include <fstream>
#include <string>
#include <time.h>
#include <chrono>
#include <iomanip>

using namespace std;

#define MAX_LINE_LENGTH 999

unsigned long int nodes_expanded;

int a_star(state_t *init_state, int (*h)(state_t*)) {
    state_t state, child;
    int ruleid, g, *old_g;
    ruleid_iterator_t iter;
    state_map_t *distances;
    PriorityQueue<state_t> frontier;

    // Distance map
    distances = new_state_map();
    state_map_add(distances, init_state, 0);

    // Min-priority queue on the f-value (g + h)
    frontier.Add(h(init_state), 0, *init_state);

    while (!frontier.Empty()) {
        nodes_expanded++;
        g = frontier.CurrentPriority();

        // Get the state with the lowest f-value
        state = frontier.Top();
        frontier.Pop();
        nodes_expanded++;

        // Current distance 
        g -= h(&state);

        // If the state is a goal state
        if (is_goal(&state)) return g;

        // Get the distance to the state
        old_g = state_map_get(distances, &state);

        // If the state was not visited or if the new distance is lower
        if (old_g == NULL || g <= *old_g) {
            // Update the distance
            state_map_add(distances, &state, g);

            // Expand the state
            init_fwd_iter(&iter, &state);
            while ((ruleid = next_ruleid(&iter)) >= 0) {
                apply_fwd_rule(ruleid, &state, &child);

                // Compute the distance to the child state
                int h_child = h(&child); 
                if (h_child < INT_MAX) {
                    // Add the state to the queue with the new distance
                    int g_child = g + get_fwd_rule_cost(ruleid);
                    int f_child = g_child + h_child;
                    frontier.Add(f_child, g_child, child);
                }
            }
        }
    }

    // No goal state found
    return -1;
}

int main(int argc, char **argv) {
    printf("A*\n");
    char str[MAX_LINE_LENGTH + 1];
    ssize_t n; 
    state_t state; 
    clock_t start, end;
    float time;
	ifstream file;
    string filename;
    string line;

	cout << "Por favor, ingrese un archivo de prueba seguido de ENTER: ";
	getline(cin, filename);

    cargarPDBS();

	file.open(filename);

	if (!file.is_open())
	{
		cout << "Error: Nombre de archivo inválido: " << filename << endl;
		return 0;
	}

	cout << "Instancia \t\t\t Resuelto   Tiempo \t   Nodos Expandidos   Distancia\t" << endl;
	cout << "-----------------------------------------------------------------------------" << endl;

    while (getline(file, line)) {

        n = read_state(line.c_str(), &state);

        if (n <= 0) {
            printf("Error: invalid state.\n");
            continue; 
        }

        nodes_expanded = 0;
        start = clock();
        int d = a_star(&state, heuristicas);
        end = clock();

        time = (float)(end - start) / CLOCKS_PER_SEC;

        if (d >= 0) 
            cout << line << "\t Verdadero \t " << time << " ms \t " << nodes_expanded << "\t\t " << d << endl;
        else 
            cout << line << "\t Falso \t " << time << " ms \t " << nodes_expanded << "\t\t " << d << endl;
    }

    return 0;
}