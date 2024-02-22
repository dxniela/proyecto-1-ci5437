#include <vector>
#include "../priority_queue.hpp"
#include <stdbool.h>
#include <fstream>
#include <string>
#include <time.h>
#include <chrono>
#include <iomanip>

#define MAX_LINE_LENGTH 999
#define MAX_RUN_TIME 600000

using namespace std::chrono;
using namespace std;

int64_t nodes_expanded;

// Función para verificar si se ha excedido el tiempo máximo de ejecución
int64_t checkMaxRuntime(system_clock::time_point &startTime)
{
	int64_t currentTime = duration_cast<milliseconds>(high_resolution_clock::now() - startTime).count();
	return currentTime > MAX_RUN_TIME ? currentTime : -1;
}

// Función para verificar si un estado es el estado objetivo
bool is_goal_reached(state_t *state)
{
	return is_goal(state);
}

// Función para expandir un estado y agregar sus hijos a la cola de prioridad
void expand_state(state_t *state, PriorityQueue<state_t> &q, state_map_t *distance, int g)
{
	state_t child;
	int ruleid;
	ruleid_iterator_t iter;

	init_fwd_iter(&iter, state);
	while ((ruleid = next_ruleid(&iter)) >= 0)
	{
		apply_fwd_rule(ruleid, state, &child);

		int child_g = g + get_fwd_rule_cost(ruleid);
		int child_h = heuristicas(child);
		int child_f = child_g + child_h;

		if (child_h < INT_MAX)
		{
			q.Add(child_f, child_g, child);
		}
	}
}

// Implementación del algoritmo A*
int aStar(state_t *start)
{
	system_clock::time_point startTime = high_resolution_clock::now();
	PriorityQueue<state_t> q;
	state_map_t *distance = new_state_map();
	if (distance == NULL)
	{ // Verifica si la memoria fue asignada correctamente
		cout << "Error: No se pudo asignar memoria para el mapa de estado.\n" << endl;
		return -1;
	}

	state_t state;
	int g;

	/* Agrega el estado inicial */
	state_map_add(distance, start, 0);
	q.Add(0, 0, *start);

	while (!q.Empty())
	{

		/* Verifica si se ha excedido el tiempo máximo de ejecución */
		int64_t runTime = checkMaxRuntime(startTime);
		if (runTime != -1)
		{
			cout << "Tiempo de corrida excedido: " << runTime << " ms\n" << endl;
			break;
		}

		/* Obtiene la distancia actual desde el inicio */
		g = q.CurrentPriority();

		/* Obtiene el estado */
		state = q.Top();
		q.Pop();

		int *old_distance = state_map_get(distance, &state);
		++nodes_expanded;

		if ((old_distance == NULL) || (g < *old_distance) || (!compare_states(&state, start)))
		{
			state_map_add(distance, &state, g);

			if (is_goal_reached(&state))
			{
				cout << "A* ha expandido: " << nodes_expanded << " nodos." << endl;
				cout << "Movimientos mínimos realizados: " << g << endl;
				cout << "Tiempo de corrida: " << duration_cast<milliseconds>(high_resolution_clock::now() - startTime).count() << " ms\n\n" << endl;
				return g;
			}

			expand_state(&state, q, distance, g);
		}
	}

	return -1;
}

// Función para procesar el archivo de prueba
void process_test_file(string filename)
{
	string line;
	int d;
	ssize_t nchars;
	state_t start;
	ifstream file;
	clock_t startTime, endTime, timeSpan;
	float runTime;

	cargarPDBS();

	file.open(filename);

	if (!file.is_open())
	{
		cout << "Error: Nombre de archivo inválido: " << filename << endl;
		return;
	}

	cout << "Instancia \t\t\t Resuelto   Tiempo \t   Nodos Expandidos   Distancia\t" << endl;
	cout << "-----------------------------------------------------------------------------" << endl;

	while (getline(file, line))
	{
		
		nchars = read_state(line.c_str(), &start);
		if (nchars <= 0)
		{
			cout << "Error: estado inválido ingresado." << endl;
			continue;
		}

		startTime = clock();
		nodes_expanded = 0;

		// INICIO DE A*
		d = aStar(&start);
		// FIN DE A*

		endTime = clock();

		timeSpan = endTime - startTime;

		runTime = timeSpan / (double)CLOCKS_PER_SEC;

		if (d < 0)
		{
			cout << line << "\t Falso \t " << runTime << " ms \t " << nodes_expanded << "\t\t " << d << endl;
		}
		else
		{
			cout << line << "\t Verdadero \t " << runTime << " ms \t " << nodes_expanded << "\t\t " << d << endl;
		}
	}

	file.close();
}

// Función principal
int main(int argc, char **argv)
{
	string filename;

	cout << "Por favor, ingrese un archivo de prueba seguido de ENTER: ";
	getline(cin, filename);

	process_test_file(filename);

	return 0;
}