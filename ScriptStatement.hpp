#ifndef SCRIPT_STATEMENT_HPP
#define SCRIPT_STATEMENT_HPP

#include <string>
#include <iostream>

#include "Tokenizer.hpp"
#include "Statement.hpp"
#include "Application.hpp"


namespace ECE141{

    class ScriptStatement : public Statement {
        public:
        ScriptStatement(Application* anApp) : app(anApp) {};
        ScriptStatement(const ScriptStatement &aCopy);
        ScriptStatement& operator=(const ScriptStatement& other);
      
        virtual ~ScriptStatement();
        
        virtual StatusResult  parse(Tokenizer &aTokenizer) override;
        
        virtual const char*   getStatementName() const override{return "script_statement";}
        virtual StatusResult  run(std::ostream &aStream) override;
        
        private:
        Application* app;
        std::string path;
    };

}

#endif