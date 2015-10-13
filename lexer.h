#pragma once
#include <string>
#include <fstream>
#include <vector>
#include <cctype>
#include <iostream>

class TLexer {

public:
    TLexer(const std::string fileName);
    ~TLexer();
private:
    enum States {
        State_Int, //0
        State_Double,//1
        State_Bool, //2
        State_Ident, //3
        State_Lbkt, //4
        State_Rbkt, //5
        State_Space, //6
        State_NewLine, //7
        State_Char, //8
        State_String, //9
        State_Symbol, //10
        State_Tab,//11
        State_EOF, // 12
        State_Error, //13
    };
    const size_t StatesCount = 14;

    States lastState = State_Space;
    struct Token {
        Token(States s, std::string v) {
            state = s;
            value = v;
        }

        States state;
        std::string value;
    };
    std::vector< Token > Tokens;
    std::ifstream fin;
private:
    void Read();
    char GetChar();
    void UnGetChar();
    void FindToken(States state, std::string& value);
    void ReadString(std::string& value);
    void ReadSymbol(std::string& value);
    void ReadSharp(std::string& value);
    void ReadIdent(std::string& value);
    void ReadNumber(std::string& value);
};
