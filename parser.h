#pragma once
#include <memory>
#include "lexer.h"
#include "ast.h"

class TParser {

public:
    TParser(const std::string fileName);
    ~TParser();

private:
    OpPlus(std::vector< std::string > values);
    OpMinus(std::vector< std::string > values);
    OpMult(std::vector< std::string > values);
    //OpDiv(std::vector< std::string > values);
    //OpDiv(std::vector< std::string > values);
    //OpEqually(std::vector< std::string > values);


private:
    std::shared_ptr< TLexer > Lexer;
};

