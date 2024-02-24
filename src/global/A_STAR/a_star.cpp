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
    int ruleid, g, *old;
    ruleid_iterator_t iter;
    state_map_t *distances;
    PriorityQueue<state_t> q;

    distances = new_state_map();
    state_map_add(distances, init_state, 0);

    q.Add(h(init_state), 0, *init_state);

    while (!q.Empty()) {
        nodes_expanded++;
        g = q.CurrentPriority();

        state = q.Top();
        q.Pop();
        nodes_expanded++;

        g -= h(&state);

        if (is_goal(&state)) 
            return g;

        old = state_map_get(distances, &state);

        if (old == NULL || g <= *old) {
            state_map_add(distances, &state, g);

            init_fwd_iter(&iter, &state);
            while ((ruleid = next_ruleid(&iter)) >= 0) {
                apply_fwd_rule(ruleid, &state, &child);

                int h_child = h(&child); 
                if (h_child < INT_MAX) {
                    int g_child = g + get_fwd_rule_cost(ruleid);
                    int f_child = g_child + h_child;
                    q.Add(f_child, g_child, child);
                }
            }
        }
    }

    return -1;
}

int main(int argc, char **argv) {

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