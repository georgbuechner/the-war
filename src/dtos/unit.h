/**
 * @author fux
 */

#ifndef DTOS_UNIT_H_
#define DTOS_UNIT_H_ 

#include "utils/defines.h"
#include <map>
#include <string>

class Unit {
  public:
    Unit(bf::t_pos pos, unsigned int strength, bf::BLOCK block);

    // getter 
    int id();
    bf::t_pos pos();
    unsigned int strength();
    bf::BLOCK block();
    virtual bool base() { return false; }
    virtual const std::map<std::string, unsigned int>& finanzier() {
      static std::map<std::string, unsigned int> m;
      return m;
    }

    // setter 
    void pos(bf::t_pos pos); 
    void strength(unsigned int strength);

    // methods 
    virtual void AddFinanzier(std::string id, unsigned int share) {}
    virtual void RemoveFinanzier(const std::string& id) {}
    virtual void UpdateFinanzier(const std::string& id, unsigned int share) {}

  protected: 
    static int ID; 

  private: 
    const int _id;
    const bf::BLOCK _block;
    bf::t_pos _pos;
    unsigned int _strength;
};

class Base : public Unit {
  public: 
    Base(bf::t_pos pos, std::map<std::string, unsigned int> finanziers, bf::BLOCK);

    // getter 
    bool base();
    const std::map<std::string, unsigned int>& finanzier(); 

    // methods 
    void AddFinanzier(std::string id, unsigned int share); 
    void RemoveFinanzier(const std::string& id); 
    void UpdateFinanzier(const std::string& id, unsigned int share); 

  private: 
    std::map<std::string, unsigned int> _finanziers;
};

#endif
