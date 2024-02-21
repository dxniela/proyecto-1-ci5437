#include <vector>
#include "../../psvn-for-ci5437/global/priority_queue.hpp"
#include <stdbool.h>
#include <fstream>
#include <string>
#include <time.h>
#include <chrono>
#include <iomanip>

#define MAX_LINE_LENGTH 999
#define MAX_RUN_TIME 600000

using namespace std::chrono;
using namespace std

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
		int child_h = heuristic(child);
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
		std::cout << "Error: No se pudo asignar memoria para el mapa de estado.\n"
							<< std::endl;
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
			std::cout << "Tiempo de corrida excedido: " << runTime << " ms\n"
								<< std::endl;
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
				std::cout << "A* ha expandido: " << nodes_expanded << " nodos." << std::endl;
				std::cout << "Movimientos mínimos realizados: " << g << std::endl;
				std::cout << "Tiempo de corrida: " << duration_cast<milliseconds>(high_resolution_clock::now() - startTime).count() << " ms\n\n"
									<< std::endl;
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

	loadPDB();

	file.open(filename);

	if (!file.is_open())
	{
		std::cout << "Error: Nombre de archivo inválido: " << filename << std::endl;
		return;
	}

	std::cout << "Instancia \t\t\t Resuelto   Tiempo \t   Nodos Expandidos   Distancia\t" << std::endl;
	std::cout << "-----------------------------------------------------------------------------" << std::endl;

	while (getline(file, line);)
	{
		
		nchars = read_state(line.c_str(), &start);
		if (nchars <= 0)
		{
			std::cout << "Error: estado inválido ingresado." << std::endl;
			continue;
		}

		nodes_expanded = 0;

		// INICIO DE A*
		d = aStar(&start);
		// FIN DE A*

		if (d < 0)
		{
			std::cout << line << "\t Falso \t " << duration_cast<milliseconds>(high_resolution_clock::now() - startTime).count() << " ms \t " << nodes_expanded << "\t\t " << d << std::endl;
		}
		else
		{
			std::cout << line << "\t Verdadero \t " << duration_cast<milliseconds>(high_resolution_clock::now() - startTime).count() << " ms \t " << nodes_expanded << "\t\t " << d << std::endl;
		}
	}

	file.close();
}

// Función principal
int main(int argc, char **argv)
{
	string filename;

	std::cout << "Por favor, ingrese un archivo de prueba seguido de ENTER: ";
	getline(cin, filename);

	process_test_file(filename);

	return 0;
}