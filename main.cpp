//
//  main.cpp
//  Database2
//
//  Created by rick gessner on 3/17/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <functional>
#include <variant>
#include <memory>

#include "TestManually.hpp"
#include "TestAutomatic.hpp"

#include "TestIndex.hpp"
#include "TestCache.hpp"

int main(int argc, const char * argv[]) {
  srand(static_cast<uint32_t>(time(0)));
  if(argc>1) {
    
    std::stringstream theOutput;
    ECE141::TestAutomatic theTests(theOutput);

    using TestCall = std::function<bool()>;
    static std::map<std::string, TestCall> theCalls {
      {"App",         [&](){return theTests.doAppTest();}  },
      {"BlockCache",  [&](){return theTests.doBlockCacheTest();}  },
      {"Compile",     [&](){return theTests.doCompileTest();}  },
      {"Delete",      [&](){return theTests.doDeleteTest();}  },
      {"DropTable",   [&](){return theTests.doDropTest();}  },
      {"DBCommands",  [&](){return theTests.doDBCommandsTest();}  },
      {"Index",       [&](){return theTests.doIndexTest();}},
      {"Insert",      [&](){return theTests.doInsertTest();}  },
      {"Joins",       [&](){return theTests.doJoinTest();}  },
      {"Reader",      [&](){return theTests.doReaderTest();}  },
      {"RowCache",    [&](){return theTests.doRowCacheTest();}  },
      {"Select",      [&](){return theTests.doSelectTest();}  },
      {"Tables",      [&](){return theTests.doTablesTest();}  },
      {"Update",      [&](){return theTests.doUpdateTest();}  },
      {"ViewCache",   [&](){return theTests.doViewCacheTest();}  },
      {"customIndex",      [&](){
        ECE141::TestIndex indexTest;
        return indexTest.runTests() == indexTest.getNumTests();
      }},
      {"customCache",      [&](){
        ECE141::TestCache cacheTest;
        return cacheTest.runTests() == cacheTest.getNumTests();
      }}
    };
    
    std::string theCmd(argv[1]);
    if (theCmd == "All") {
      std::map<std::string, bool> results;
      for (auto kv : theCalls) {
          std::cout << "========== TESTING " + kv.first + " ==========\n";
          results[kv.first] = kv.second();
      }

      std::cout << "========== RESULTS ==========\n";

      for (auto kv : results) {
          std::cout << std::setw(16) << std::left << kv.first << "\t : \t" << (kv.second ? "PASS" : "FAIL  <-----") << "\n";
      }
      return 0;
    }

    if(theCalls.count(theCmd)) {
      bool theResult = theCalls[theCmd]();
      const char* theStatus[]={"FAIL","PASS"};
      std::cout << theCmd << " test " << theStatus[theResult] << "\n";
      std::cout << "------------------------------\n"
        << theOutput.str() << "\n";
    }
    else std::cout << "Unknown test\n";
  }
  else {
    doManualTesting();
  }
  return 0;
}

