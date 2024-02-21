#pragma once
#include <optional>

std::optional<state_t> bfs(state_t root);
std::optional<state_t> bfsPrunning(state_t root);