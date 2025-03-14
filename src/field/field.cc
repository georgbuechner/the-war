
#include "field/field.h"
#include "dtos/unit.h"
#include "utils/defines.h"
#include "utils/utils.h"
#include <cmath>
#include <csetjmp>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <thread>

#define RED_BASES "pink_bases" 
#define BLUE_BASES "green_bases" 
#define RED_UNITS "pink_battlefields" 
#define BLUE_UNITS "green_battlefields" 

#define MAX_NUM_UNITS 850 


Field::Field(nlohmann::json config) {
  _paused.store(true);
  auto red_bases = CreateBasesFromConfig(config[RED_BASES], bf::BLOCK::RED);
  auto blue_bases = CreateBasesFromConfig(config[BLUE_BASES], bf::BLOCK::RED);
  std::map<int, std::shared_ptr<Unit>> red_units;
  std::map<int, std::shared_ptr<Unit>> blue_units;
  int red_max_num_units = MAX_NUM_UNITS;
  int blue_max_num_units = MAX_NUM_UNITS;
  for (const auto& pos : config[RED_UNITS]) {
    CreateInitialUnits({pos["x"], pos["y"]}, red_units, 1, bf::BLOCK::RED, red_max_num_units);
    CreateInitialUnits({pos["x"], pos["y"]}, blue_units, -1, bf::BLOCK::BLUE, blue_max_num_units);
  }
  for (const auto& pos : config[BLUE_UNITS]) {
    CreateInitialUnits({pos["x"], pos["y"]}, blue_units, 1, bf::BLOCK::BLUE, blue_max_num_units);
    CreateInitialUnits({pos["x"], pos["y"]}, red_units, -1, bf::BLOCK::RED, red_max_num_units);
  }
  _red = std::make_shared<Army>(bf::BLOCK::RED, red_units, red_bases);
  _blue = std::make_shared<Army>(bf::BLOCK::BLUE, blue_units, blue_bases);
  int red_dollars = _red->CalculateTotalStrength() * UNIT_COST;
  int blue_dollars = _blue->CalculateTotalStrength() * UNIT_COST;
  _red->set_dollars(red_dollars + ((red_dollars < blue_dollars) ? (blue_dollars-red_dollars)*2 : 0));
  _blue->set_dollars(blue_dollars + ((blue_dollars < red_dollars) ? (red_dollars-blue_dollars)*2 : 0));

  std::cout << "Created " << _red->CalculateTotalStrength() << " red units" << std::endl;
  std::cout << "Created " << _blue->CalculateTotalStrength() << " blue units" << std::endl;
}

// getter 
bool Field::paused() { return _paused.load(); }

// setter
void Field::set_paused(bool paused) { std::cout << "Setting paused: " << paused; _paused.store(paused); }

// methods

std::map<int, std::shared_ptr<Base>> Field::CreateBasesFromConfig(
    const std::vector<nlohmann::json>& config, bf::BLOCK block) {
  std::map<int, std::shared_ptr<Base>> red_bases;
  for (const auto& base : config) {
    bf::t_pos pos = {base.at("x"), base.at("y")};
    std::map<std::string, unsigned int> finanziers = base.at("finanziers");
    std::shared_ptr<Base> new_base = std::make_shared<Base>(pos, finanziers, block);
    red_bases[new_base->id()] = new_base;
  }
  return red_bases;
}

void Field::CreateInitialUnits(bf::t_pos pos, std::map<int, std::shared_ptr<Unit>>& units, 
  short faktor, bf::BLOCK block, int& max_num_units) {
  const int DIST = 30;
  unsigned int num_units_at_pos = util::Ran(30 + faktor*2, 20 + faktor*2); 
  for (unsigned int i=0; i<num_units_at_pos; i++) {
    unsigned int unit_strength = util::Ran(7 + faktor*1, 3 + faktor*1);
    std::shared_ptr<Unit> new_unit = std::make_shared<Unit>(util::RandomClosePos(pos, DIST), 
        unit_strength, block);
    units[new_unit->id()] = new_unit;
    max_num_units -= unit_strength; 
    if (max_num_units <=0 )
      break;
  }
}

nlohmann::json Field::GetFieldJson() {
  std::shared_lock sl(_lock);
  if (_red->bases().size() == 0)
    return {{"victory", "green"}};
  if (_blue->bases().size() == 0)
    return {{"victory", "pink"}};
  return {{"pink_army", _red->ToJson()}, {"green_army", _blue->ToJson()}, {"victory", ""}};
}

void Field::DoPhases() {
  int counter = 0;
  auto game_over = [&]() -> bool { return _red->bases().size() == 0 || _blue->bases().size() == 0; };
  while(!game_over()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
    std::cout << "...new round started: " << counter << ", " << _paused.load() << std::endl;
    if (!_paused.load()) {
      std::unique_lock ul(_lock);
      MoveArmy(_red, _blue); 
      if (game_over()) break;
      MoveArmy(_blue, _red); 
      if (game_over()) break;
      if (counter++ % 4 == 0) {
        std::cout << "- sending new units" << std::endl;
        SendNewUnits(_red, bf::BLOCK::RED);
        SendNewUnits(_blue, bf::BLOCK::BLUE);
        counter = 1;
      }
      ul.unlock();
    }
  }
}

void Field::MoveArmy(std::shared_ptr<Army> army_1, std::shared_ptr<Army> army_2) {}

void Field::SendNewUnits(std::shared_ptr<Army> army, bf::BLOCK block) {
  const int DIST = 5;
  const int cost = UNIT_COST;
  int money_to_spend = cost + army->dollars()*0.1; 
  while (money_to_spend >= cost ) {
    for (unsigned int i=0; i < army->bases().size(); i++) {
      auto base = army->bases().begin();
      std::advance(base, util::Ran(army->bases().size()-1));
      int possible_strength = 1 + std::round(money_to_spend/(cost*5));
      if (army->dollars() >= possible_strength) {
        bf::t_pos pos = util::RandomClosePos(base->second->pos(), DIST);
        army->AddUnit(std::make_shared<Unit>(pos, possible_strength, block));
        army->DecDollars(possible_strength * cost);
      }
      money_to_spend -= possible_strength * cost;
      if (money_to_spend < cost) 
        break;
    }
  }
}
