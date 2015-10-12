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
        State_Num, //0
        State_Bool, //1
        State_Ident, //2
        State_Lbkt, //3
        State_Rbkt, //4
        State_Space, //5
        State_NewLine, //6
        State_Char, //7
        State_String, //8
        State_Symbol, //9
        State_End, // 10
        State_Error, //11
    };
    const size_t StatesCount = 12;

    enum Events {
        Event_Error,
        Event_Bool,
        Event_Space, // ' '
        Event_NewLine, //'\n'
        Event_Didgit, //0-9
        Event_Lbkt, // '('
        Event_Rbkt, // ')'
        Event_Letter, //a-zA-Z
        Event_Special, // $|%|&|*|/|:|>|<|=|?|^|_|~|!|+|-
        Event_Point, //'.'
        Event_Exponent, // "e+", // "e-"
        Event_Symbol, // '\''
        Event_String, // "\""
        Event_Char,
        Event_End
    };

    const size_t EventsCount = 15;

    struct Transition {
        Transition(){}
        Transition(States state);

        States nextState;
    };
    States currentState;
    struct Token {
        Token(States s, std::string v) {
            state = s;
            value = v;
        }

        States state;
        std::string value;
    };
    std::vector< Token > Tokens;
    std::vector < std::vector < Transition > > statesTable;
    std::ifstream fin;
private:
    void Read();
    void ProcessEvent(Events event, std::string& value);
    void AddRule(std::string string, States state);
    void GetRule(std::string currentRule, States State);
    void AddTransition(States from, Events event, States to);
};

