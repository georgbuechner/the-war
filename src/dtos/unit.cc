#include "dtos/unit.h" 

// UNIT 
int Unit::ID = 0;

Unit::Unit(bf::t_pos pos, unsigned int strength, bf::BLOCK block) : _id(++ID), 
  _block(block), _pos(pos), _strength(strength) {}

// getter 
int Unit::id() { return _id; }
bf::t_pos Unit::pos() { return _pos; } 
unsigned int Unit::strength() { return _strength; }
bf::BLOCK Unit::block() { return _block; }

// setter 
void Unit::pos(bf::t_pos pos) { _pos = pos; }
void Unit::strength(unsigned int strength) { _strength = strength; }


// BASE 

Base::Base(bf::t_pos pos, std::map<std::string, unsigned int> finanziers, 
    bf::BLOCK block) : Unit(pos, 1, block), _finanziers(finanziers) {}

// getter 
bool Base::base() { return true; }
const std::map<std::string, unsigned int>& Base::finanzier() { return _finanziers; }

// methods 
void Base::AddFinanzier(std::string id, unsigned int share) { 
  _finanziers[id] = share;
}
void Base::RemoveFinanzier(const std::string& id) {
  if (_finanziers.contains(id))
    _finanziers.erase(id);
  else 
    throw bf::Exception("Finanzier with id " + id + " not found!", 404);
}
void Base::UpdateFinanzier(const std::string& id, unsigned int share) {
  if (_finanziers.contains(id)) 
    _finanziers.at(id) = share;
  else 
    throw bf::Exception("Finanzier with id " + id + " not found!", 404);
}
