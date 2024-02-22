#pragma once

bool operator==(const state_t &a, const state_t &b) {
    return compare_states(&a, &b) == 0;
}

template <>
struct std::hash<state_t>
{
  std::size_t operator()(const state_t& state) const
  {
    return hash_state(&state);
  }
};