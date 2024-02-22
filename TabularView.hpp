
//
//  TabularView.hpp
//
//  Created by rick gessner on 4/1/22.
//  Copyright © 2022 rick gessner. All rights reserved.
//

#ifndef TabularView_h
#define TabularView_h

#include <iostream>
#include <sstream>
#include <iomanip>
#include <map>
#include <vector>
#include <string>
#include "View.hpp"
#include "Row.hpp"
#include "Timer.hpp"
#include "Application.hpp"
#include "Entity.hpp"
#include "Row.hpp"
#include "BasicTypes.hpp"


namespace ECE141 {

  // USE: general tabular view (with columns)
  class TabularView : public View{
  public:
      TabularView(std::ostream& anOutput, Entity& anEntity, RowCollection& rowCollection) :
          entity(anEntity), rows(rowCollection), output(anOutput) {}
              
    // USE: create header for tablular view...
    TabularView& showHeader(std::ostream& tempOutput) {
        std::vector<size_t> field_spacing = entity.computeFieldSizes();
        
        std::vector<size_t> header_spacing;
        header_spacing.push_back(0);
        for (auto space : field_spacing){
            header_spacing[0] += space + 1; // +1 for ending '|' at each cell after std::setw
        }
        if (header_spacing.size() >= 1){
            header_spacing[0] -= 1;
        }

        std::vector<std::string> table_name_row;
        table_name_row.push_back(std::string("Rows in ") + (entity.getName()));

        show_hline(header_spacing, tempOutput).show_string_vector_line(table_name_row, header_spacing, tempOutput);

        std::vector<std::string> field_name_row;

        AttributeList attributes= entity.getAttributes();
        for (auto attribute : attributes){
            field_name_row.push_back(attribute.getName());
        }

        show_hline(field_spacing, tempOutput).show_string_vector_line(field_name_row, field_spacing, tempOutput).show_hline(field_spacing, tempOutput);

        return *this;
    }
    
    
    // USE: this function shows all the fields in a row...
    TabularView& showRow(const Row &aRow, std::vector<size_t> fieldSpacing, std::ostream& tempOutput){
        std::vector<std::string> theRowStrings = aRow.toStringVec();
        
        show_string_vector_line(theRowStrings, fieldSpacing, tempOutput);

        return *this;
    }

    // USE: this is the main show() for the view, where it presents all the rows...
    bool show(std::ostream &aStream) {
        timer.reset();
        
        std::stringstream ss;

        showHeader(ss);
        
        std::vector<size_t> field_spacing = entity.computeFieldSizes();

        for (auto& theRow : rows) {
            showRow(*theRow, field_spacing, ss);
        }

        show_hline(field_spacing, ss);
        std::cout << ss.rdbuf() << "\n";
        
        output << rows.size() << " rows in set ("
        << std::fixed << timer.elapsed() << " sec.)\n";
      return true;
    }
    
  protected:
    TabularView& show_string_vector_line(std::vector<std::string> elements, std::vector<size_t> spacing, std::ostream& tempOutput){
        
        if (elements.size() != spacing.size()){
            std::cout << "Error in tabular view show(): number of string and spacing mismatch! \n";
            return *this;
        }
        
        tempOutput << "|";
        for(size_t i = 0; i < elements.size(); i++){
            tempOutput << std::setw(spacing[i]) << std::left << elements[i] << "|";
        }
        tempOutput << "\n";

        return *this;
    }

    TabularView& show_hline(std::vector<size_t> spacing, std::ostream& tempOutput){
        tempOutput << "+";
        for (size_t space : spacing){
            std::string line(space, '-');
            tempOutput << line << "+";
        }
        tempOutput << "\n";

        return *this;
    }
    
    Entity              entity;
    RowCollection&      rows;
    std::ostream        &output;
    std::vector<size_t> col_width;
    Timer               timer;
  };

}

#endif /* TabularView_h */
