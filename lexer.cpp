#include "lexer.h"
#include <iostream>

TLexer::TLexer(const std::string fileName) {
    fin.open(fileName);
    if (!fin.is_open()) {
        return;
    }
    Read();
}

TLexer::~TLexer() {
    fin.close();
}

bool isspecial(char c) {
    return (c == '+' || c == '-' || c == '/' || c == '*'
            || c == '>' || c == '<' || c == '=' || c == '$'
            || c == '%' || c == '&' || c == '^' || c == ':'
            || c == '?' || c == '_' || c == '~'|| c == '!');
}

bool TLexer::IsEndToken(char c) {
    switch (c) {
        case '\n': {
            return true;
        }
        case '\t': {
            return true;
        }
        case ' ': {
            return true;
        }
        case ')': {
            UnGetChar();
            return true;
        }
        case '(': {
            UnGetChar();
            return true;
        }
        default: {
            return false;
        }
    }
}

void TLexer::IsCorrectToken(char c, std::string& value, States state) {
    if (IsEndToken(c)) {
        FindToken(state, value);
    } else {
        value += c;
        throw new ExceptionLexerIncorrectChar(value, PosLine, PosColumn);
    }
}

void TLexer::Read() {
    char c;
    std::string value = "";
    while (!FileEOF()) {
        c = GetChar();
        value += c;
        switch (c) {
            case 34: {
                ReadString(value);
                break;
            }

            case 39: {
                ReadSymbol(value);
                break;
            }

            case '.' : {
                FindToken(State_Point, value);
                break;
            }

            case '(': {
                FindToken(State_Lbkt,value);
                break;
            }

            case ')': {
                FindToken(State_Rbkt,value);
                break;
            }
            case '#': {
                ReadSharp(value);
                break;
            }
            default: {
                if (isspecial(c) || isalpha(c)) {
                    ReadIdent(value);
                } else if (isdigit(c)) {
                    ReadNumber(value);
                } else if (IsEndToken(c)) {
                    value.erase(value.size() - 1, value.size());
                }
                break;
            }
        }
    }
    value = "EOF";
    FindToken(State_EOF, value);
}

char TLexer::GetChar() {
    char c;
    fin.get(c);
    if (c == '\n') {
        PosLine += 1;
        PosColumn = 1;
    } else {
        PosColumn += 1;
    }
    return c;
}

void TLexer::UnGetChar() {
    fin.unget();
    PosColumn -= 1;
}

bool TLexer::FileEOF() {
    return fin.eof();
}

void TLexer::FindToken(States state, std::string& value) {
    Tokens.push_back(Token(state, value));
    value.clear();
}

void TLexer::ReadString(std::string &value) {
    bool IsSpecialForString = false;
    while(true) {
        if (FileEOF()) {
            throw new ExceptionLexerEOF(value, PosLine, PosColumn);
            break;
        }
        char c = GetChar();
        value += c;
        if (c == '\\' && !IsSpecialForString) {
            IsSpecialForString = true;
        } else if ((c == '\\' || c == '\"')  && IsSpecialForString) {
            IsSpecialForString = false;
            continue;
        } else if (c == '\"' && !IsSpecialForString) {
            c = GetChar();
            IsCorrectToken(c, value, State_String);
            break;
        } else if (IsSpecialForString) {
            throw new ExceptionLexerIncorrectChar(value, PosLine, PosColumn);
            break;
        }
    }
}

void TLexer::ReadSymbol(std::string &value) {
    while (true) {
        if (FileEOF()) {
            throw new ExceptionLexerEOF(value, PosLine, PosColumn);
            break;
        }
        char c = GetChar();
        if (isspecial(c) || isalpha(c) || isdigit(c)) {
            value += c;
        } else {
            IsCorrectToken(c, value, State_Symbol);
            break;
        }
    }
}

void TLexer::ReadSharp(std::string &value) {
    char c = GetChar();
    value += c;
    if (c == '\\') {
        c = GetChar();
        value += c;
        c = GetChar();
        IsCorrectToken(c, value, State_Char);
    } else if (c == 't' || c == 'f') {
        c = GetChar();
        IsCorrectToken(c, value, State_Bool);
    } else {
        throw new ExceptionLexerIncorrectChar(value, PosLine, PosColumn);
    }
}

void TLexer::ReadIdent(std::string &value) {
    while (true) {
        if (FileEOF()) {
            throw new ExceptionLexerEOF(value, PosLine, PosColumn);
            break;
        }
        char c = GetChar();
        if (isspecial(c) || isdigit(c) || isalpha(c)) {
            value += c;
        } else {
            IsCorrectToken(c, value, State_Ident);
            break;
        }
    }
}

void TLexer::ReadNumber(std::string &value) {
    States state = State_Int;
    while (true) {
        if (FileEOF()) {
            throw new ExceptionLexerEOF(value, PosLine, PosColumn);
            break;
        }
        char c = GetChar();
        if (isdigit(c)) {
            value += c;
        } else if (c == '.' && state == State_Int) {
            state = State_Double;
            value += c;
        } else if (c == 'e' && state == State_Double) {
            char c_temp = GetChar();
            char c_temp2 = GetChar();
            if ((c_temp == '+' || c_temp == '-') && isdigit(c_temp2)) {
                value += c;
                value += c_temp;
                value += c_temp2;
            } else {
                throw new ExceptionLexerIncorrectChar(value, PosLine, PosColumn);
                break;
            }
        } else {
             IsCorrectToken(c, value, state);
             break;
        }
    }
}
