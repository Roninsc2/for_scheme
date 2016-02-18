#pragma once
#include <string>
#include <exception>

class Exception : public std::exception {
public:
    virtual const char* what() const throw() override {
        return "Lexer error";
    }
};

class ExceptionLexerEOF : public Exception {
public:
    ExceptionLexerEOF(std::string value, size_t line, size_t column) {
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

class ExceptionLexerIncorrectChar : public Exception {
public:
    ExceptionLexerIncorrectChar(std::string value, size_t line, size_t column) {
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

class ParserExceptionIncorrectToken : public Exception {
public:
    ParserExceptionIncorrectToken(std::string tokVal) {
        errorString = tokVal;
    }
    virtual const char* what() const throw() override {
        std::string str = ("Input incorrect token : " + errorString);
        return str.c_str();
    }
public:
    std::string errorString;

};


class ParserExceptionTokenEOF : public Exception {
public:
    ParserExceptionTokenEOF(std::string tokVal) {
        errorString = tokVal;
    }
    virtual const char* what() const throw() override {
        std::string str = ("Tokens are end, max size is: " + errorString);
        return str.c_str();
    }
public:
    std::string errorString;

};

