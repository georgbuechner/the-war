/**
 * @author fux
 */

#ifndef DTOS_ARMY_H_
#define DTOS_ARMY_H_ 

#include "dtos/unit.h"
#include "utils/defines.h"
#include <map>
#include <memory>
#include <nlohmann/json_fwd.hpp>

class Army {
  public:
    Army(bf::BLOCK block, std::map<int, std::shared_ptr<Unit>> units, 
        std::map<int, std::shared_ptr<Base>> bases);

    // getter 
    int chance();
    int dollars();
    const std::map<int, std::shared_ptr<Unit>>& units();
    const std::map<int, std::shared_ptr<Base>>& bases();

    // setter 
    void chance(int chance);
    void dollars(int dollars);

    // Method 
    void IncDollars(int amount);
    void DecDollars(int amount);
    int CalculateTotalStrength();
    std::map<int, std::shared_ptr<Unit>> GetAllUnits();
    nlohmann::json ToJson();

  private: 
    const bf::BLOCK _block;
    int _chance;
    int _dollars;
    std::map<int, std::shared_ptr<Unit>> _units;
    std::map<int, std::shared_ptr<Base>> _bases;
    const unsigned int _initial_num_bases;
};

#endif
