//
//  FolderReader.hpp
//  Database5
//
//  Created by rick gessner on 4/4/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef FolderReader_h
#define FolderReader_h

#include <string>
#include <filesystem>
#include <iostream>
#include <fstream>

namespace fs =  std::filesystem;

namespace ECE141 {
  
  using FileVisitor = std::function<bool(const std::string)>;

  class FolderReader {
  public:
            FolderReader(std::string aPath) : path(aPath) {}
    virtual ~FolderReader() {}
    
    virtual bool exists(const std::string &aFilename) {
      std::ifstream theStream(aFilename);
      return !theStream ? false : true;
    }
    
    virtual void each(const std::string &anExt,
                      const FileVisitor &aVisitor) const {
        fs::path thePath = path;                                            // grabs the path from object instantiation
        for (auto &theItem : fs::directory_iterator(path)) {                // iterates all the files and folders located in this path
            if (!(theItem.is_directory())) {                                // if the item is not a directory
                fs::path temp = theItem.path();                             // grab the path of this item /.../.../example.txt
                std::string theExtension = temp.extension().u8string();     // grab the item's extension and convert to string
                if (0 < anExt.size() && 0 == anExt.compare(theExtension)) {                     
                    aVisitor(temp.stem().u8string());                          // invoke the stem of the item into the closure
                }
            }
        }
    };
    
    std::string path;
  };
  
}

#endif /* FolderReader_h */
