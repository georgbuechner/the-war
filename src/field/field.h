/**
 * @author fux
 */

#ifndef FIELD_FIELD_H_
#define FIELD_FIELD_H_ 

#include <atomic>
#include <memory>
#include <nlohmann/json.hpp>
#include <shared_mutex>
#include "dtos/army.h"
#include "dtos/unit.h"
#include "utils/defines.h"

class Field {
  public: 

    Field(nlohmann::json config);
    
    // getter 
    bool paused();

    // setter
    void set_paused(bool paused);

    // methods 
    nlohmann::json GetFieldJson();

    void DoPhases();

  private: 
    std::shared_mutex _lock;
    std::shared_ptr<Army> _red;
    std::shared_ptr<Army> _blue;

    std::atomic<bool> _paused;

    void MoveArmy(std::shared_ptr<Army> army_1, std::shared_ptr<Army> army_2);
    void SendNewUnits(std::shared_ptr<Army> army, bf::BLOCK block);

    static std::map<int, std::shared_ptr<Base>> CreateBasesFromConfig(
        const std::vector<nlohmann::json>& config, bf::BLOCK block);
    static void CreateInitialUnits(bf::t_pos, std::map<int, std::shared_ptr<Unit>>& units, 
        short faktor, bf::BLOCK block, int& max_num_units);
};

#endif
