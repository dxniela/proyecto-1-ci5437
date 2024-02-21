#include <vector>
#include "../../psvn-for-ci5437/global/priority_queue.hpp"
#include "../node.hpp"
#include <stdbool.h>
#include <algorithm>
#include <fstream>
#include <string>
#include <time.h>

#define MAX_LINE_LENGTH 999

using namespace std;

int64_t nodes_expanded;

// Realiza una búsqueda en profundidad limitada por un límite de costo
Node *dfsVisit(Node *n, unsigned bound, unsigned *nextBound, const int history)
{
	unsigned f = (*n).g + heuristic((*n).state);
	unsigned t = INT_MAX;
	int ruleid;
	int c_history;
	ruleid_iterator_t iter;
	state_t child;
	Node *next;

	if (f > bound)
	{
		*nextBound = f;
		return NULL;
	}

	if (is_goal(&(*n).state))
	{
		*nextBound = (*n).g;
		return n;
	}

	++nodes_expanded;

	init_fwd_iter(&iter, &(*n).state);
	while ((ruleid = next_ruleid(&iter)) >= 0)
	{
		apply_fwd_rule(ruleid, &(*n).state, &child);

		if (!fwd_rule_valid_for_history(history, ruleid))
			continue;
		c_history = next_fwd_history(history, ruleid);

		Node n_child(child, n, ((*n).g + 1));

		next = dfsVisit(&n_child, bound, nextBound, c_history);

		if (next != NULL)
		{
			return next;
		}

		t = std::min(t, *nextBound);
	}

	*nextBound = t;
	return NULL;
}

// Función principal del algoritmo IDA*
Node idaStar(state_t *start)
{
	Node root(*start, NULL, 0);
	unsigned bound = heuristic(*start);
	unsigned nextBound;
	Node *final;

	while (1)
	{
		final = dfsVisit(&root, bound, &nextBound, init_history);

		if (final != NULL)
		{
			return *final;
		}
		bound = nextBound;
	}
}

// Función para procesar el archivo de prueba
void process_test_file(string filename)
{
	std::string line;
	ssize_t nchars;
	float runTime;
	state_t start;
	std::ifstream file;
	std::clock_t startTime, endTime, timeSpan;

	file.open(filename);

	if (!file.is_open())
	{
		std::cout << "Error: Invalid filename: " << filename << "\n";
		return;
	}

	std::cout << "Instance \t\t\t Solved   Time \t   Nodes Expanded   Distance\t\n";
	std::cout << "-----------------------------------------------------------------------------\n";

	while (getline(file, line))
	{

		nchars = read_state(line.c_str(), &start);
		if (nchars <= 0)
		{
			std::cout << "Error: invalid state entered.\n";
			continue;
		}

		startTime = clock();
		nodes_expanded = 0;

		// A STAR BEGINS
		Node goal = idaStar(&start);
		// A STAR ENDS

		endTime = clock();

		timeSpan = endTime - startTime;

		runTime = timeSpan / (double)CLOCKS_PER_SEC;

		std::cout << line << "\t True \t" << runTime << "\t" << nodes_expanded << "\t\t" << goal.g << "\n";
	}

	file.close();
}

// Función principal
int main(int argc, char **argv)
{
	char str[MAX_LINE_LENGTH + 1];

	loadPDB();

	std::cout << "Please enter a test file followed by ENTER: ";
	if (!fgets(str, sizeof str, stdin))
	{
		std::cout << "Error: empty input line.\n";
		return 0;
	}

	str[strlen(str) - 1] = '\0';

	process_test_file(str);

	return 0;
}