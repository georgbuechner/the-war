/**
 * @author: fux
 */
#ifndef UTILS_DEFINES_H
#define UTILS_DEFINES_H 

#include <string>
#include <utility>

namespace bf {
  typedef std::pair<unsigned int, unsigned int> t_pos;

  enum BLOCK {
    RED = 0, 
    BLUE
  };

  class Exception {
    public: 
      Exception(const std::string msg, unsigned int rtn_code) : _msg(msg), 
        _rtn_code(rtn_code) { }

      // getter
      const std::string& msg() { return _msg; } 
      unsigned int rtn_code() { return _rtn_code; }

    private: 
      const std::string _msg;
      unsigned int _rtn_code;
  };
};

#endif 


