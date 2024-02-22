//
//  CommandView.hpp
//  Assignment2
//
//  Created by rick gessner on 2/15/21.
//  Copyright ?2020 rick gessner. All rights reserved.
//

#ifndef CommandView_h
#define CommandView_h

#include "FolderReader.hpp"
#include "View.hpp"
#include "Timer.hpp"
#include <iomanip>

namespace ECE141 {

    // USE: provide view class that lists db files in storage path...
    class CommandView {
    public:
        CommandView() {}

        bool showCreate(std::ostream& anOutput) {
     
            double time = timer.elapsed();
            anOutput << "Query OK, 1 row affected (" << time << " secs)\n";

            return true;
        }

        bool showDrop(std::ostream& anOutput) {

            double time = timer.elapsed();
            anOutput << "Query OK, 0 rows affected (" << time << " secs)\n";

            return true;
        }

        bool showDump(Database* db, std::ostream& anOutput) {
            double time = timer.elapsed();
            db->dump(std::cout);
            anOutput << "0 rows in set (" << time << " sec)\n";
            
            return true;
        }

        bool showAffect(std::ostream& anOutput, size_t numRow) {
     
            double time = timer.elapsed();
            anOutput << numRow << " rows affected (" << time << " secs)\n";

            return true;
        }


        
        Timer timer;


    };

}

#endif /* FolderView_h */
