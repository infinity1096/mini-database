#include "DataStructures.hpp"

namespace ECE141{

  StatusResult String::encode(std::ostream& anOutput){
    Size theStringLength = contained_string.size();
    theStringLength.encode(anOutput);
    anOutput.write(contained_string.c_str(), (theStringLength + 1) * sizeof(char));
    return StatusResult{ Errors::noError };
  }

  StatusResult String::decode(std::istream& anInput){
    Size theStringLength;
    theStringLength.decode(anInput);
    
    char* theStringBuffer = new char[theStringLength + 1];
    anInput.read(theStringBuffer, (theStringLength + 1) * sizeof(char));
    std::string intermediate = theStringBuffer;
    delete[] theStringBuffer;

    contained_string = intermediate;
    return StatusResult{ Errors::noError };
  }

  StatusResult Size::encode(std::ostream& anOutput){
    anOutput.write((char*)(&contained_integer), sizeof(uint32_t));
    return StatusResult{ Errors::noError };
  }

  StatusResult Size::decode(std::istream& anInput){
    anInput.read((char*)(&contained_integer), sizeof(uint32_t));
    return StatusResult{ Errors::noError };
  }

}