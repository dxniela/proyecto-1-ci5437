#include <iostream>
#include <queue>
#include <vector>
#include <chrono>
#include <unordered_map>
#include "bfs.hpp"
#include "../stateComparer.hpp"

#define MAX_RUN_TIME 600000

using namespace std::chrono;

enum COLOR
{
	WHITE,
	GRAY,
	BLACK
};

std::vector<int64_t> levelNodes{};
int currentLevelNodes;
int nextLevelNodes;

// Función para verificar si se ha excedido el tiempo máximo de ejecución
int64_t checkMaxRuntime(system_clock::time_point &startTime)
{
	int64_t currentTime = duration_cast<milliseconds>(high_resolution_clock::now() - startTime).count();
	return currentTime > MAX_RUN_TIME ? currentTime : -1;
}

// Función para realizar la búsqueda en anchura
std::optional<state_t> bfs(state_t root)
{
	system_clock::time_point startTime = high_resolution_clock::now();
	std::queue<state_t> nodeQueue;
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
			std::cout << "Profundidad: " << levelNodes.size() << std::endl;
			std::cout << "Tiene " << currentLevelNodes << " nodo(s)." << std::endl;
		}

		// Si el nodo actual es un objetivo
		if (is_goal(&currentNode))
		{
			// Se imprime la profundidad, el número de nodos y el tiempo de ejecución
			std::cout << "Profundidad: " << levelNodes.size() << std::endl;
			std::cout << "Tiene " << levelNodes.back() << " nodo(s)." << std::endl;
			std::cout << "Tiempo de corrida " << duration_cast<milliseconds>(high_resolution_clock::now() - startTime).count();
			std::cout << "ms" << std::endl
								<< std::endl;

			return std::optional<state_t>(currentNode);
		}

		// Si se ha excedido el tiempo máximo de ejecución
		int64_t elapsedTime;
		if ((elapsedTime = checkMaxRuntime(startTime)) != -1)
		{
			// Se imprime el tiempo de ejecución y se sale del bucle
			std::cout << "Tiempo de corrida " << elapsedTime << "ms" << std::endl;
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
	std::cout << "No hay objetivo" << std::endl;
	std::cout << "Tiempo de corrida " << duration_cast<milliseconds>(high_resolution_clock::now() - startTime).count();
	std::cout << "ms" << std::endl
						<< std::endl;
	return std::nullopt;
}

// Función para realizar la búsqueda en anchura con poda
std::optional<state_t> bfsPrunning(state_t root)
{
	system_clock::time_point startTime = high_resolution_clock::now();
	std::queue<state_t> nodeQueue;
	std::unordered_map<state_t, COLOR> nodeColors;
	ruleid_iterator_t iter;

	// Se marca el nodo raíz como blanco y se agrega a la cola
	nodeColors[root] = COLOR::WHITE;
	nodeQueue.emplace(root);

	// Se limpiam los nodos de nivel
	levelNodes.clear();
	curLevelNodes = 1;
	nexLevelNodes = 1;

	// Mientras la cola no esté vacía
	while (!nodeQueue.empty())
	{
		// Obtenemos el siguiente nodo de la cola
		state_t currentNode = nodeQueue.front();
		nodeQueue.pop();
		init_fwd_iter(&iter, &currentNode);

		// Si se han visitado todos los nodos del nivel actual
		if (curLevelNodes == 0)
		{
			// Se actualizan los nodos de nivel
			levelNodes.push_back(nexLevelNodes);
			curLevelNodes = nexLevelNodes;
			nexLevelNodes = 0;

			// Se imprime la profundidad y el número de nodos
			std::cout << "Profundidad: " << levelNodes.size() << std::endl;
			std::cout << "Tiene " << curLevelNodes << " nodo(s)." << std::endl;
		}

		// Si el nodo actual es un objetivo
		if (is_goal(&currentNode))
		{
			// Se imprime la profundidad, el número de nodos y el tiempo de ejecución
			std::cout << "Profundidad: " << levelNodes.size() << std::endl;
			std::cout << "Tiene " << levelNodes.back() << " nodo(s)." << std::endl;
			std::cout << "Tiempo de corrida " << duration_cast<milliseconds>(high_resolution_clock::now() - startTime).count();
			std::cout << "ms" << std::endl
								<< std::endl;
			// Se devuelve el nodo objetivo
			return std::optional<state_t>(currentNode);
		}

		// Si se ha excedido el tiempo máximo de ejecución
		int64_t elapsedTime;
		if ((elapsedTime = checkMaxRuntime(startTime)) != -1)
		{
			// Se imprime el tiempo de ejecución y se sale del bucle
			std::cout << "Tiempo de corrida " << elapsedTime << "ms" << std::endl;
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
					nexLevelNodes += 1;
				}
			}
			// Si el nuevo nodo no ha sido descubierto
			else
			{
				// Marcamos el nuevo nodo como gris y lo agregamos a la cola
				nodeColors[newNode] = COLOR::GRAY;
				nodeQueue.emplace(newNode);
				nexLevelNodes += 1;
			}
		}

		// Se decrementa el número de nodos del nivel actual
		curLevelNodes--;

		// Se marca el nodo actual como negro
		nodeColors[currentNode] = COLOR::BLACK;
	}

	// Si no se encontró un objetivo
	std::cout << "No hay objetivo" << std::endl;
	std::cout << "Tiempo de corrida " << duration_cast<milliseconds>(high_resolution_clock::now() - startTime).count();
	std::cout << "ms" << std::endl
						<< std::endl;
	return {};
}
