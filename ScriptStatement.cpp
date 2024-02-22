#include "ScriptStatement.hpp"

#include <fstream>

#include "TokenPattern.hpp"
#include "Config.hpp"

namespace ECE141{

    ScriptStatement::ScriptStatement(const ScriptStatement &aCopy){
        *this = aCopy;
    }

    ScriptStatement& ScriptStatement::operator=(const ScriptStatement& other){
        app = other.app;
        path = other.path;
        return *this;
    }

    ScriptStatement::~ScriptStatement(){}

    StatusResult  ScriptStatement::parse(Tokenizer &aTokenizer) {
        TokenPattern thePattern(aTokenizer);

        if (!thePattern.begins().equalsKeyword(Keywords::script_kw)){
            return StatusResult{Errors::syntaxError};
        }

        // append string as part of path until [;] is reached
        path = "";
       
        while (aTokenizer.more()){
            if(thePattern.begins().equalsPunctuation(';')){
                
                return StatusResult{Errors::noError};
            }

            path += aTokenizer.current().data;
            aTokenizer.next();
        }

        return StatusResult{Errors::syntaxError};
    }

    StatusResult  ScriptStatement::run(std::ostream &aStream) {
        std::fstream fileStream;
        fileStream.open(Config::getStoragePath() + "/" + path, std::ios::in);

        if (!fileStream.good()){
            return StatusResult{Errors::invalidArguments};
        }
        std::string line;
        std::stack<std::string> reverse;
        while (std::getline(fileStream, line)){
            reverse.push(line);
        }

        while (reverse.size() > 0) {
            app->pushCommandToStack(reverse.top());
            reverse.pop();
        }

        return StatusResult{Errors::noError};
    }

}