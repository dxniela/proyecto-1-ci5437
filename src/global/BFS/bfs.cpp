#include <iostream>
#include <queue>
#include <vector>
#include <chrono>
#include <unordered_map>
#include "./bfs.hpp"
#include "../stateComparer.hpp"

#define MAX_RUN_TIME 600000

using namespace std::chrono;
using namespace std;

enum COLOR
{
	WHITE,
	GRAY,
	BLACK
};

vector<int64_t> levelNodes{};
int currentLevelNodes;
int nextLevelNodes;

// Función para verificar si se ha excedido el tiempo máximo de ejecución
int64_t checkMaxRuntime(system_clock::time_point &startTime)
{
	int64_t currentTime = duration_cast<milliseconds>(high_resolution_clock::now() - startTime).count();
	return currentTime > MAX_RUN_TIME ? currentTime : -1;
}

// Función para realizar la búsqueda en anchura
optional<state_t> bfs(state_t root)
{
	system_clock::time_point startTime = high_resolution_clock::now();
	queue<state_t> nodeQueue;
	ruleid_iterator_t iter;

	// Se agrega el nodo raíz a la cola
	nodeQueue.emplace(root);

	// Se limpian los nodos de nivel
	levelNodes.clear();
	currentLevelNodes = 1;
	nextLevelNodes = 1;

	while (!nodeQueue.empty())
	{
		// Obtenemos el siguiente nodo de la cola
		state_t currentNode = nodeQueue.front();
		nodeQueue.pop();
		init_fwd_iter(&iter, &currentNode);

		// Si se han visitado todos los nodos del nivel actual
		if (currentLevelNodes == 0)
		{
			// Se actualizan los nodos de nivel
			levelNodes.push_back(nextLevelNodes);
			currentLevelNodes = nextLevelNodes;
			nextLevelNodes = 0;

			// Se imprime la profundidad y el número de nodos
			cout << "Profundidad: " << levelNodes.size() << endl;
			cout << "Tiene " << currentLevelNodes << " nodo(s)." << endl;
		}

		// Si el nodo actual es un objetivo
		if (is_goal(&currentNode))
		{
			// Se imprime la profundidad, el número de nodos y el tiempo de ejecución
			cout << "Profundidad: " << levelNodes.size() << endl;
			cout << "Tiene " << levelNodes.back() << " nodo(s)." << endl;
			cout << "Tiempo de corrida " << duration_cast<milliseconds>(high_resolution_clock::now() - startTime).count();
			cout << "ms" << endl
								<< endl;

			return optional<state_t>(currentNode);
		}

		// Si se ha excedido el tiempo máximo de ejecución
		int64_t elapsedTime;
		if ((elapsedTime = checkMaxRuntime(startTime)) != -1)
		{
			// Se imprime el tiempo de ejecución y se sale del bucle
			cout << "Tiempo de corrida " << elapsedTime << "ms" << endl;
			break;
		}

		// Para cada acción posible
		int action;
		while ((action = next_ruleid(&iter)) >= 0)
		{
			// Se aplica la acción al nodo actual para obtener un nuevo nodo
			state_t newNode;
			apply_fwd_rule(action, &currentNode, &newNode);

			// Se agrega el nuevo nodo a la cola
			nodeQueue.emplace(newNode);
			nextLevelNodes += 1;
		}
		currentLevelNodes--;
	}

	// Si no se encontró un objetivo
	cout << "No hay objetivo" << endl;
	cout << "Tiempo de corrida " << duration_cast<milliseconds>(high_resolution_clock::now() - startTime).count();
	cout << "ms" << endl
						<< endl;
	return nullopt;
}

// Función para realizar la búsqueda en anchura con poda
optional<state_t> bfsPrunning(state_t root)
{
	system_clock::time_point startTime = high_resolution_clock::now();
	queue<state_t> nodeQueue;
	unordered_map<state_t, COLOR> nodeColors;
	ruleid_iterator_t iter;

	// Se marca el nodo raíz como blanco y se agrega a la cola
	nodeColors[root] = COLOR::WHITE;
	nodeQueue.emplace(root);

	// Se limpiam los nodos de nivel
	levelNodes.clear();
	currentLevelNodes = 1;
	nextLevelNodes = 1;

	// Mientras la cola no esté vacía
	while (!nodeQueue.empty())
	{
		// Obtenemos el siguiente nodo de la cola
		state_t currentNode = nodeQueue.front();
		nodeQueue.pop();
		init_fwd_iter(&iter, &currentNode);

		// Si se han visitado todos los nodos del nivel actual
		if (currentLevelNodes == 0)
		{
			// Se actualizan los nodos de nivel
			levelNodes.push_back(nextLevelNodes);
			currentLevelNodes = nextLevelNodes;
			nextLevelNodes = 0;

			// Se imprime la profundidad y el número de nodos
			cout << "Profundidad: " << levelNodes.size() << endl;
			cout << "Tiene " << currentLevelNodes << " nodo(s)." << endl;
		}

		// Si el nodo actual es un objetivo
		if (is_goal(&currentNode))
		{
			// Se imprime la profundidad, el número de nodos y el tiempo de ejecución
			cout << "Profundidad: " << levelNodes.size() << endl;
			cout << "Tiene " << levelNodes.back() << " nodo(s)." << endl;
			cout << "Tiempo de corrida " << duration_cast<milliseconds>(high_resolution_clock::now() - startTime).count();
			cout << "ms" << endl
								<< endl;
			// Se devuelve el nodo objetivo
			return optional<state_t>(currentNode);
		}

		// Si se ha excedido el tiempo máximo de ejecución
		int64_t elapsedTime;
		if ((elapsedTime = checkMaxRuntime(startTime)) != -1)
		{
			// Se imprime el tiempo de ejecución y se sale del bucle
			cout << "Tiempo de corrida " << elapsedTime << "ms" << endl;
			break;
		}

		// Para cada acción posible
		int action;
		while ((action = next_ruleid(&iter)) >= 0)
		{
			// Aplicamos la acción al nodo actual para obtener un nuevo nodo
			state_t newNode;
			apply_fwd_rule(action, &currentNode, &newNode);

			// Si el nuevo nodo ya ha sido descubierto
			if (nodeColors.contains(newNode))
			{
				// Si el nuevo nodo es blanco
				if (nodeColors[newNode] == COLOR::WHITE)
				{
					// Marcamos el nuevo nodo como gris y lo agregamos a la cola
					nodeColors[newNode] = COLOR::GRAY;
					nodeQueue.emplace(newNode);
					nextLevelNodes += 1;
				}
			}
			// Si el nuevo nodo no ha sido descubierto
			else
			{
				// Marcamos el nuevo nodo como gris y lo agregamos a la cola
				nodeColors[newNode] = COLOR::GRAY;
				nodeQueue.emplace(newNode);
				nextLevelNodes += 1;
			}
		}

		// Se decrementa el número de nodos del nivel actual
		currentLevelNodes--;

		// Se marca el nodo actual como negro
		nodeColors[currentNode] = COLOR::BLACK;
	}

	// Si no se encontró un objetivo
	cout << "No hay objetivo" << endl;
	cout << "Tiempo de corrida " << duration_cast<milliseconds>(high_resolution_clock::now() - startTime).count();
	cout << "ms" << endl
						<< endl;
	return {};
}

// Función principal
int main(int argc, char **argv)
{
    state_t goal;
    int finished, goal_id, pruning;

    if (argc != 2) {
        printf("Please enter 1 for parcial pruning or 0 for no pruning.\n\n");
        return -1;
    }

    pruning = atoi(argv[1]);

    if (pruning != 0 && pruning != 1) {
        printf("Please enter 1 for parcial pruning or 0 for no pruning.\n\n");
        return -1;
    }

    printf("Depth\t#Nodes\tBranching Factor\n");
    printf("------------------------------------------------------------------\n");

    first_goal_state(&goal, &goal_id);
;
    if (pruning) 
		bfsPrunning(goal);
	else 
		bfs(goal);

    return 0;
}