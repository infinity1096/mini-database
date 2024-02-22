#ifndef STORABLE_HPP
#define STORABLE_HPP

#include <iostream>

#include "Errors.hpp"

namespace ECE141{

  class Storable{
    public:
    virtual StatusResult encode(std::ostream& anOutput)=0;
    virtual StatusResult decode(std::istream& anInput)=0;
  };

}

#endif