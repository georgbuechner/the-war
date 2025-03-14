
#include "field/field.h"
#include "dtos/unit.h"
#include "utils/defines.h"
#include "utils/utils.h"
#include <iostream>
#include <memory>

#define RED_BASES "pink_bases" 
#define BLUE_BASES "green_bases" 
#define RED_UNITS "pink_battlefields" 
#define BLUE_UNITS "green_battlefields" 

#define MAX_NUM_UNITS 850 


Field::Field(nlohmann::json config) {
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
  std::cout << "Created " << _red->CalculateTotalStrength() << " red units" << std::endl;
  std::cout << "Created " << _blue->CalculateTotalStrength() << " blue units" << std::endl;
}

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
  const int DIST = 20;
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
  return {{"pinks", _red->ToJson()}, {"greens", _blue->ToJson()}};
}
