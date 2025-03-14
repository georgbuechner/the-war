/**
 * @author fux
 */

#ifndef FIELD_FIELD_H_
#define FIELD_FIELD_H_ 

#include <nlohmann/json.hpp>
#include "dtos/army.h"
#include "dtos/unit.h"
#include "utils/defines.h"

class Field {
  public: 
    Field(nlohmann::json config);

    // methods 
    nlohmann::json GetFieldJson();

  private: 
    std::shared_ptr<Army> _red;
    std::shared_ptr<Army> _blue;

    static std::map<int, std::shared_ptr<Base>> CreateBasesFromConfig(
        const std::vector<nlohmann::json>& config, bf::BLOCK block);
    static void CreateInitialUnits(bf::t_pos, std::map<int, std::shared_ptr<Unit>>& units, 
        short faktor, bf::BLOCK block, int& max_num_units);

};

#endif
