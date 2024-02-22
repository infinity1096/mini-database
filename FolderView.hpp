//
//  FolderView.hpp
//  Assignment2
//
//  Created by rick gessner on 2/15/21.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef FolderView_h
#define FolderView_h

#include "FolderReader.hpp"
#include "View.hpp"
#include "Timer.hpp"
#include <iomanip>

namespace ECE141 {

  // USE: provide view class that lists db files in storage path...
  class FolderView : public View {
  public:
    FolderView(std::string aPath, const char *anExtension="db")
      : reader(aPath), extension(anExtension) {}

    size_t max_width = 19;
    std::string padding(std::string content) {
        size_t theLen = content.size();
        std::string str = "| " + content;
        for (size_t i = 0; i < max_width - theLen; i++) {
            str += " ";
        }
        str += "|";
        return str;
    }
   
    virtual bool show(std::ostream &anOutput) {
        Timer timer;
        size_t count = 0;
        std::cout << "+--------------------+\n";
        std::cout << "| DATABASE           |\n";
        std::cout << "+--------------------+\n";
        reader.each(extension, [&](const std::string& aName) -> bool {      // closure 
            std::string theStr = padding(aName);
            std::cout <<theStr<< "\n";
            count++;
            return true;
        });
        std::cout << "+--------------------+\n";
        double time = timer.elapsed();
        anOutput << count << " rows in set (" << time << " sec)\n";
        
        
      return true;
    }
    
    FolderReader  reader;
    const char    *extension;
    
  };

}

#endif /* FolderView_h */
