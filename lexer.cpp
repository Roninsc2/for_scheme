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

            case '\t': {
                FindToken(State_Tab, value);
                break;
            }

            case '\n': {
                FindToken(State_NewLine,value);
                break;
            }

            case 39: {
                ReadSymbol(value);
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
            case ' ': {
                FindToken(State_Space, value);
                break;
            }
            default: {
                if (isspecial(c) || isalpha(c)) {
                    ReadIdent(value);
                } else if (isdigit(c)) {
                    ReadNumber(value);
                }
            }
        }
    }
    value = "EOF";
    FindToken(State_EOF, value);
}

char TLexer::GetChar() {
    char c;
    fin.get(c);
    return c;
}

void TLexer::UnGetChar() {
    fin.unget();
}

bool TLexer::FileEOF() {
    return fin.eof();
}

void TLexer::FindToken(States state, std::string& value) {
    if (lastState == state && (state == State_Space || state == State_NewLine
                               ||state == State_Tab))
    {
        value.clear();
    } else {
        Tokens.push_back(Token(state, value));
        std::cout << state << "\t" << value << std::endl;
        value.clear();
        lastState = state;
    }
}

void TLexer::ReadString(std::string &value) {
    bool IsSpecialForString = false;
    while(true) {
        if (FileEOF()) {
            throw new ExceptionEOF(value);
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
            FindToken(State_String, value);
            break;
        } else if (IsSpecialForString) {
            throw new ExceptionIncorrectChar(value);
            break;
        }
    }
}

void TLexer::ReadSymbol(std::string &value) {
    while (true) {
        if (FileEOF()) {
            throw new ExceptionEOF(value);
            break;
        }
        char c = GetChar();
        if (isspecial(c) || isalpha(c) || isdigit(c)) {
            value += c;
        } else {
            UnGetChar();
            FindToken(State_Symbol, value);
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
        FindToken(State_Char, value);
    } else if (c == 't' || c == 'f') {
        FindToken(State_Bool, value);
    } else {
        throw new ExceptionIncorrectChar(value);
    }
}

void TLexer::ReadIdent(std::string &value) {
    while (true) {
        if (FileEOF()) {
            throw new ExceptionEOF(value);
            break;
        }
        char c = GetChar();
        if (isspecial(c) || isdigit(c) || isalpha(c)) {
            value += c;
        } else {
            UnGetChar();
            FindToken(State_Ident, value);
            break;
        }
    }
}

void TLexer::ReadNumber(std::string &value) {
    States state = State_Int;
    while (true) {
        if (FileEOF()) {
            throw new ExceptionEOF(value);
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
                UnGetChar();
                UnGetChar();
                UnGetChar();
                FindToken(state, value);
                break;
            }
        } else {
             UnGetChar();
             FindToken(state, value);
             break;
        }
    }
}
