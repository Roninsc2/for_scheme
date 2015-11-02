#pragma once
#include <string>
#include <fstream>
#include <vector>
#include <cctype>
#include <exception>

enum States {
    State_Int, //0
    State_Double,//1
    State_Bool, //2
    State_Ident, //3
    State_Lbkt, //4
    State_Rbkt, //5
    State_Char, //6
    State_String, //7
    State_Symbol, //8
    State_Point, //9
    State_EOF, // 10
};
const size_t StatesCount = 11;


struct Token {
    Token(){}
    Token(States s, std::string v) {
        state = s;
        value = v;
    }
    States state;
    std::string value;
};

class TLexer {

public:
    TLexer(const std::string fileName);
    ~TLexer();

private:
    void Read();
    char GetChar();
    void UnGetChar();
    bool FileEOF();
    bool IsEndToken(char c);
    void IsCorrectToken(char c, std::string& value, States state);
    void FindToken(States state, std::string& value);
    void ReadString(std::string& value);
    void ReadSymbol(std::string& value);
    void ReadSharp(std::string& value);
    void ReadIdent(std::string& value);
    void ReadNumber(std::string& value);

private:
    std::ifstream fin;
    size_t PosColumn = 1;
    size_t PosLine = 1;

public:
    std::vector< Token > Tokens;
};

class LexerException : public std::exception {
public:
    virtual const char* what() const throw() override {
        return "Lexer error";
    }
};

class ExceptionEOF : public LexerException {
public:
    ExceptionEOF(std::string value, size_t line, size_t column) {
        errorString = value;
        ErrLine = line;
        ErrColumn = column;
    }
    virtual const char* what() const throw() override {
        std::string str = ("Found premature termination in " + errorString + " in line "
                           + std::to_string(ErrLine) + " in colomn "
                           + std::to_string(ErrColumn));
        return str.c_str();
    }
public:
    std::string errorString;
    size_t ErrLine;
    size_t ErrColumn;
};

class ExceptionIncorrectChar : public LexerException {
public:
    ExceptionIncorrectChar(std::string value, size_t line, size_t column) {
        errorString = value;
        ErrLine = line;
        ErrColumn = column;
    }
    virtual const char* what() const throw() override {
        std::string str = ("Input incorrect character : " + errorString + " in line "
                           + std::to_string(ErrLine) + " in colomn "
                           + std::to_string(ErrColumn));
        return str.c_str();
    }
public:
    std::string errorString;
    size_t ErrLine;
    size_t ErrColumn;
};
