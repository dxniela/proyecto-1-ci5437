#pragma once
#include <optional>

using namespace std;

optional<state_t> bfs(state_t root);
optional<state_t> bfsPrunning(state_t root);