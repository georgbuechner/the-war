#include "dtos/army.h"
#include <iostream>
#include <nlohmann/json.hpp>
#include <numeric>

Army::Army(bf::BLOCK block, std::map<int, std::shared_ptr<Unit>> units, 
    std::map<int, std::shared_ptr<Base>> bases) 
  : _block(block), _chance(0), _units(units), _bases(bases), 
    _initial_num_bases(_bases.size()) {
  _dollars = CalculateTotalStrength();
}

// getter 
int Army::chance() { return _chance; }
int Army::dollars() { return _dollars; }
const std::map<int, std::shared_ptr<Unit>>& Army::units() { return _units; } 
const std::map<int, std::shared_ptr<Base>>& Army::bases() { return _bases; } 

// setter 
void Army::chance(int chance) { _chance = chance; }
void Army::dollars(int dollars) { _dollars = dollars; } 

// Method 
void Army::IncDollars(int amount) { _dollars += amount; }
void Army::DecDollars(int amount) { _dollars -= amount; }
int Army::CalculateTotalStrength() {
  // Add strength from units and size of bases (since bases always have strength = 1)
  return std::accumulate(_units.begin(), _units.end(), _bases.size(), [](const auto& val, const auto& next) {
      return val + next.second->strength(); }); 
}
std::map<int, std::shared_ptr<Unit>> Army::GetAllUnits() { 
 std::map<int, std::shared_ptr<Unit>> all(_units.begin(), _units.end());
 all.insert(_bases.begin(), _bases.end());
 return all;
} 

nlohmann::json Army::ToJson() {
  std::vector<nlohmann::json> all_units_json;
  // It is slightly mor efficient to do this "by heand" instead of using
  // `GetAllUnits()` since this way, we only iterate once.
  for (const auto& base : _bases)
    all_units_json.push_back({{"pos", base.second->pos()}, {"strength", base.second->strength()}});
  for (const auto& unit : _units)
    all_units_json.push_back({{"pos", unit.second->pos()}, {"strength", unit.second->strength()}});
  return {{"num_units", CalculateTotalStrength()}, {"dollars", _dollars},
    {"bases_destroyed", _initial_num_bases-_bases.size()}, {"units", all_units_json}};
}
