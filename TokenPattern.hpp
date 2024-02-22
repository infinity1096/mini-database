#ifndef TONKEN_PATTERN_HPP
#define TONKEN_PATTERN_HPP

#include "Tokenizer.hpp"
#include "Attribute.hpp"
#include <vector>
#include <string>
#include <initializer_list>
#include "Attribute.hpp"
#include "Helpers.hpp"
#include "Row.hpp"


namespace ECE141{
    class TokenPattern {
        public:

        TokenPattern(Tokenizer& atokenizer) : tokenizer(atokenizer) {}
      
        TokenPattern& begins(){
            initialize();
            return *this;
        }

        TokenPattern& hasType(const TokenType& aType){
            if (within_bound_check()){
                auto& theCurrentToken = tokenizer.current();
                sequence_recognized &= (theCurrentToken.type == aType);
            }
            next();
            return *this;
        }
        
        TokenPattern& skip(size_t aCount) {
          if(sequence_recognized) {
            tokenizer.next((int)aCount); //actually skip...
          }
          return *this;
        }
        
        TokenPattern& nextIs(const KWList &aList){
            
            for (auto theKeyword : aList) {
                equalsKeyword(theKeyword);
            }
            
            return *this;
        }

        TokenPattern& equalsKeyword(const Keywords& aKeyword){
            if(within_bound_check()){
                auto& theCurrentToken = tokenizer.current();
                sequence_recognized &= (theCurrentToken.type == TokenType::keyword);
                sequence_recognized &= (theCurrentToken.keyword == aKeyword);
            }
            next();
            return *this;
        }
        
        TokenPattern& equalsOperator(const Operators& anOp){
            if(within_bound_check()){
                auto& theCurrentToken = tokenizer.current();
                sequence_recognized  &= (theCurrentToken.type == TokenType::operators);
                sequence_recognized &= (anOp == Helpers::toOperator(theCurrentToken.data)); // FIXME: 
            }
            next();
            return *this;
        }

        TokenPattern& equalsPunctuation(const char aPunct){
            if (within_bound_check()){
                auto& theCurrentToken = tokenizer.current();
                sequence_recognized &= (theCurrentToken.type == TokenType::punctuation);
                sequence_recognized &= (theCurrentToken.data.size() == 1 && theCurrentToken.data[0] == aPunct);
            }
            next();
            return *this;
        }
        TokenPattern& equalsDataType() {
          Keywords theKeyword = tokenizer.current().keyword;
          static Keywords theKnown[] =
          {Keywords::integer_kw,Keywords::boolean_kw,Keywords::datetime_kw, Keywords::float_kw, Keywords::varchar_kw};
          auto theIt = std::find(std::begin(theKnown),
              std::end(theKnown), theKeyword);
          sequence_recognized &= theIt != std::end(theKnown);

          return *this;
        }
          
        TokenPattern& containsNumber(int32_t& numberOutput){
            if (within_bound_check()){
                auto& theCurrentToken = tokenizer.current();
                sequence_recognized &= (theCurrentToken.type == TokenType::number);
                
                if (theCurrentToken.type == TokenType::number){
                    numberOutput = std::stoi(theCurrentToken.data);
                }
            }
            next();
            return *this;
        }

        TokenPattern& containsCompOperator(Operators& compOp){
            
            std::vector<Operators> compOps = {
                Operators::gt_op,
                Operators::gte_op,
                Operators::lt_op,
                Operators::lte_op, 
                Operators::equal_op,
                Operators::notequal_op,
            };

            if (within_bound_check()){
                auto& theCurrentToken = tokenizer.current();

                bool localResult = (theCurrentToken.type == TokenType::operators) 
                    && std::find(compOps.begin(), compOps.end(), theCurrentToken.op) != compOps.end();

                sequence_recognized &= localResult;
                
                if (localResult){
                    compOp = theCurrentToken.op;
                }
            }

            next();
            return *this;
        }

        TokenPattern& containsLogicalOperator(Logical& logicalOp){
            
            std::map<Keywords, Logical> logicalOperatorMap = {
                {Keywords::and_kw, Logical::and_op}, 
                {Keywords::or_kw, Logical::or_op}
            };

            if (within_bound_check()){
                auto& theCurrentToken = tokenizer.current();
                bool localResult = (theCurrentToken.type == TokenType::keyword) 
                    && logicalOperatorMap.count(theCurrentToken.keyword) != 0;

                sequence_recognized &= localResult;
                
                if (localResult){
                    logicalOp = logicalOperatorMap[theCurrentToken.keyword];
                }
            }

            next();
            return *this;
        }

        TokenPattern& containsLeftRight(Keywords& leftRight){
            if (within_bound_check()){
                auto& theCurrentToken = tokenizer.current();
                bool localResult = (theCurrentToken.type == TokenType::keyword) 
                    && (theCurrentToken.keyword == Keywords::left_kw || theCurrentToken.keyword == Keywords::right_kw);

                sequence_recognized &= localResult;
                
                if (localResult){
                    leftRight = theCurrentToken.keyword;
                }
            }

            next();
            return *this;
        }

        TokenPattern& containsNumberIdentifier(std::string& numberIdentifier){
            
            if (within_bound_check()){
                auto& theCurrentToken = tokenizer.current();
                bool localResult = (theCurrentToken.type == TokenType::identifier || theCurrentToken.type == TokenType::number); 

                sequence_recognized &= localResult;
                
                if (localResult){
                    numberIdentifier = theCurrentToken.data;
                }
            }

            next();
            return *this;
        }

                
        TokenPattern& containsInt(RowCell& numberOutput, std::vector<RowCell>& valueVec){
            if (within_bound_check()){
                auto& theCurrentToken = tokenizer.current();
                sequence_recognized &= (theCurrentToken.type == TokenType::number);
                for(auto const &theChar : theCurrentToken.data){
                    sequence_recognized &= theChar!='.';
                }
                if(sequence_recognized){
                    MyBasicIntType  numberOutput(stoi(theCurrentToken.data));
                    valueVec.push_back(numberOutput);
                }
            }
            next();
            return *this;
        }
        
        TokenPattern& containsFloat(RowCell& numberOutput, std::vector<RowCell>& valueVec){
            if (within_bound_check()){
                bool has_decimal = false;
                auto& theCurrentToken = tokenizer.current();
                sequence_recognized &= (theCurrentToken.type == TokenType::number);
                for(auto const &theChar : theCurrentToken.data){
                    if(theChar == '.'){
                        has_decimal = true;
                    }
                }
                sequence_recognized = has_decimal;
                if(has_decimal){
                    MyBasicFloatType  numberOutput(stof(theCurrentToken.data));
                    valueVec.push_back(numberOutput);
                }
            }
            next();
            return *this;
        }

        TokenPattern& containsIdentifier(std::string& anIdentifier){
                
            if (within_bound_check()) {
                auto& theCurrentToken = tokenizer.current();
                sequence_recognized &= (theCurrentToken.type == TokenType::identifier);

                if (theCurrentToken.type == TokenType::identifier) {
                    anIdentifier = theCurrentToken.data;

                    // FIXME: added here to handle "" at both sides
                    anIdentifier.erase(std::remove(anIdentifier.begin(), anIdentifier.end(), '"'), anIdentifier.end());
                }
            }
            next();
            return *this;
        }
        
        TokenPattern& containsString(RowCell& anIdentifier, std::vector<RowCell>& valueVec){
            if (within_bound_check()){
                auto& theCurrentToken = tokenizer.current();
                sequence_recognized &= (theCurrentToken.type == TokenType::identifier);
                std::string theTemp = theCurrentToken.data;
                size_t strSize = theTemp.size();
                if (quote == theTemp[0] && quote == theTemp[strSize-1]){
                    theTemp.erase(strSize-1,1);
                    theTemp.erase(0,1);
                    MyBasicVarcharType anIdentifier(theTemp.c_str());
                    valueVec.push_back(anIdentifier);
                }
                else{
                    MyBasicVarcharType anIdentifier(theTemp.c_str());
                    valueVec.push_back(anIdentifier);
                }
            }
            next();
            return *this;
        }
        
        TokenPattern& containsBool(RowCell& anIdentifier, std::vector<RowCell>& valueVec){
            if(within_bound_check()){
                auto& theCurrentToken = tokenizer.current();
                sequence_recognized &= (theCurrentToken.type == TokenType::identifier);
                std::string theTemp = theCurrentToken.data;
                if("true" == theTemp){
                    MyBasicBoolType anIdentifier(true);
                    valueVec.push_back(anIdentifier);
                    next();
                    return *this;
                }
                else if("false" == theTemp){
                    MyBasicBoolType anIdentifier(false);
                    valueVec.push_back(anIdentifier);
                    next();
                    return *this;
                }
            }
            sequence_recognized = false;
            next();
            return *this;
        }
        
        TokenPattern& containsDateTime(RowCell& anIdentifier, std::vector<RowCell>& valueVec){
            if(within_bound_check()){
                auto& theCurrentToken = tokenizer.current();
                std::string theTemp = theCurrentToken.data;
                if(19 != theTemp.size()){
                    sequence_recognized = false;
                    return *this;
                }
                std::string dateFormat = "YYYY-MM-DD hh:mm:ss";
                for(size_t i = 0; i < 19; i++){
                    char theDFChar = dateFormat[0];
                    char theChar = theTemp[0];
                    if(isalpha(theDFChar)){
                        sequence_recognized &= static_cast<bool>(isdigit(theChar));
                    }else if('-' == theDFChar){
                        sequence_recognized &= '-' == theChar;
                    }else if(':' == theDFChar){
                        sequence_recognized &= ':' == theChar;
                    }else if(' ' == theDFChar){
                        sequence_recognized &= ' ' == theChar;
                    }
                    
                }
                if(sequence_recognized){
                    int16_t year = stoi(theTemp.substr(0,4));
                    int16_t month = stoi(theTemp.substr(5,2));
                    int16_t day = stoi(theTemp.substr(8,2));
                    int16_t hour = stoi(theTemp.substr(11,2));
                    int16_t min = stoi(theTemp.substr(14,2));
                    int16_t sec = stoi(theTemp.substr(17,2));
                    
                    MyBasicDateTimeType anIdentifier({year,month,day,hour,min,sec});
                    valueVec.push_back(anIdentifier);
                }
            }
            next();
            return *this;
        }
        
        TokenPattern& containsDataType(DataTypes& aDataType, int32_t& aVarcharLength) {
            
            if (!within_bound_check()){return *this;}
            
            Keywords theKeyword = tokenizer.current().keyword;
            static Keywords theKnown[] = {Keywords::integer_kw,Keywords::boolean_kw,Keywords::datetime_kw,Keywords::show_kw, Keywords::float_kw, Keywords::varchar_kw};
            auto theIt = std::find(std::begin(theKnown), std::end(theKnown), theKeyword);
            
            bool theLocalResult = theIt != std::end(theKnown);
            
            next();

            if (theLocalResult){ // iterator valid
                if (*theIt == Keywords::varchar_kw){
                    // additional verification for varchar
                    TokenPattern pattern(tokenizer);// subsequence starting from current read index
                    
                    if (pattern.begins().equalsPunctuation('(').containsNumber(aVarcharLength).equalsPunctuation(')')){
                        theLocalResult &= aVarcharLength >= 0;
                    }else{
                        theLocalResult = false;
                    }
                }
                else {
                    aVarcharLength = 0;
                }

                aDataType = Helpers::keywordToDataTypes(*theIt);
            }

            sequence_recognized &= theLocalResult;

            return *this;
        }
        
        TokenPattern& equalsConstraint() {
          Token theToken = tokenizer.current();
          Keywords theKeyword = theToken.keyword;
          static Keywords theKnown[] =
          {Keywords::primary_kw, Keywords::auto_increment_kw, Keywords::key_kw, Keywords::not_kw, Keywords::null_kw};
          auto theIt = std::find(std::begin(theKnown),
              std::end(theKnown), theKeyword);
          sequence_recognized &= theIt != std::end(theKnown);
            next();
            return *this;
        }
        
        TokenPattern& isValidAttribute(std::vector<Attribute> &anAttrs){
            std::vector<Token> theTokens;
            if(within_bound_check()){
                auto& theCurrentToken = tokenizer.current();
                while(theCurrentToken.data != "," && theCurrentToken.data != ")"){
                    theTokens.push_back(theCurrentToken);
                    next();
                    if(within_bound_check()){
                        theCurrentToken = tokenizer.current();
                        continue;
                    }
                    else{break;}
                }
                TokenPattern thePattern(tokenizer);


                bool notAcceptable = true;
                
                while(notAcceptable){
                    
                /*
                if(thePattern.begins().hasType(TokenType::identifier) // has all 5 constraints
                   .equalsDataType().equalsConstraint().equalsConstraint().equalsConstraint().equalsConstraint().equalsConstraint()){
                       std::string theName = theTokens[0].data;
                       DataTypes theType = Helpers::keywordToDataTypes(theTokens[1].keyword);
                       Attribute theAttr = Attribute(theName, theType);
                       
                       anAttrs.push_back(theAttr);
                       return *this;
                }
                else if(thePattern.begins().hasType(TokenType::identifier) // has 4 constraints
                   .equalsDataType().equalsConstraint().equalsConstraint().equalsConstraint().equalsConstraint()){
                       return *this;
                }
                else if(thePattern.begins().hasType(TokenType::identifier) // has 3 constraints
                   .equalsDataType().equalsConstraint().equalsConstraint().equalsConstraint()){
                       return *this;
                }
                else if(thePattern.begins().hasType(TokenType::identifier) // has 2 constraints
                   .equalsDataType().equalsConstraint().equalsConstraint()){
                       return *this;
                }
                else if(thePattern.begins().hasType(TokenType::identifier) // has 1 constraint
                   .equalsDataType().equalsConstraint()){
                       return *this;
                }
                else if(thePattern.begins().hasType(TokenType::identifier) // has 0 constraints
                   .equalsDataType()){
                       return *this;
                }
                else{
                    sequence_recognized = false;

                DataTypes type;
                int32_t varcharLength;
                if(thePattern.begins().hasType(TokenType::identifier)
                   .containsDataType(type, varcharLength)){
                       return *this;
                }
                else if(thePattern.begins().hasType(TokenType::identifier)
                    .containsDataType(type, varcharLength).equalsConstraint()){
                    return *this;

                }
                 */
            }
            }
            
            return *this;
        }

        //  TokenPattern aPattern;
        //  aPattern(aTokenList);
        //  if (aPattern.begins().equalsKeyword(Keywords::create_kw).theNext().hasType()...){
        // 
        //}

        operator bool(){
            if (!sequence_recognized){
                tokenizer.startFrom(tokenizer_start_index);
            }

            return sequence_recognized;
        }

        
        void initialize(){
            tokenizer_start_index = tokenizer.getIndex();
            sequence_recognized = true;
        }

        bool within_bound_check(){
            if(tokenizer.getIndex() < tokenizer.size()){
                return true;
            }else{
                sequence_recognized = false;
                return false;
            }
        }

        TokenPattern& next(){
            tokenizer.next();
            return *this;
        }

        bool sequence_recognized = true;
        size_t tokenizer_start_index = 0;
        Tokenizer& tokenizer;

    };
}

#endif
