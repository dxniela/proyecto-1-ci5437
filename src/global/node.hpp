#ifndef _NODE_HPP
#define _NODE_HPP

class Node
{
private:
	state_t state;
	Node *parent;
	unsigned g;

public:
	Node(state_t new_state, Node *new_parent, unsigned new_g) : state(new_state), parent(new_parent), g(new_g) {}
	virtual ~Node() {}

	// Getters
	state_t getState() const { return state; }
	Node *getParent() const { return parent; }
	unsigned getG() const { return g; }

	// Setters
	void setState(state_t newState) { state = newState; }
	void setParent(Node *newParent) { parent = newParent; }
	void setG(unsigned newG) { g = newG; }
};

#endif