#include "lekser.h"
#include <iostream>

TLexer::TLexer(const std::string fileName) {
    fin.open(fileName);
    if (!fin.is_open()) {
        return;
    }
    currentState = State_Space;
    statesTable.resize(StatesCount);
    for (size_t i = 0; i < StatesCount; i++) {
        statesTable[i].resize(EventsCount);
        for (size_t j = 0; j < EventsCount; j++) {
            statesTable[i][j] = Transition(static_cast<States>(11));
        }
    }
    AddRule("{(}{)}{ }/n/l+/s+/d+{'}{\"}{#\\}{#t}{#f}", State_End);
    AddRule("{(}{)}{ }/n/d+{.}{e+}{e-}/d", State_Num);
    AddRule("{(}{)}{ }/n", State_Bool);
    AddRule("{(}{)}{ }/n/l+/s+/l/s/d", State_Ident);
    AddRule("{(}{)}{ }/n/l+/s+/d+{'}{\"}{#\\}{#t}{#f}", State_Lbkt);
    AddRule("{(}{)}{ }/n/l+/s+/d+{'}{\"}{#\\}{#t}{#f}", State_Rbkt);
    AddRule("{(}{)}{ }/n/l+/s+/d+{'}{\"}{#\\}{#t}{#f}", State_Space);
    AddRule("{(}{)}{ }/n/l+/s+/d+{'}{\"}{#\\}{#t}{#f}", State_NewLine);
    AddRule("{(}{)}{ }/n", State_Char);
    AddRule("{(}{)}{ }/n", State_String);
    AddRule("{(}{)}{ }/n/l/s/d", State_Symbol);
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
    while(fin.get(c)) {
        value += c;
        if (int(c) == 34 && currentState != State_String) {
            ProcessEvent(Event_End, value);
            ProcessEvent(Event_String, value);
            bool IsSpecialForString = false;
            while(1) {
                if (!fin.get(c)) {
                    ProcessEvent(Event_Error, value);
                    break;
                }
                value += c;
                if (c == '\\' && !IsSpecialForString) {
                    IsSpecialForString = true;
                } else if (c == '\\' && !IsSpecialForString) {
                    ProcessEvent(Event_Error, value);
                    break;
                } else if (c == '\"' && !IsSpecialForString) {
                    break;
                } else if (IsSpecialForString && (c == '\\' || c == '\"')) {
                    IsSpecialForString = false;
                }
            }
        } else if (int(c) == '\n') {
            ProcessEvent(Event_NewLine, value);
        } else if (c == 39) {
          ProcessEvent(Event_Symbol, value);
        } else if (c == '.' && currentState == State_Num) {
            ProcessEvent(Event_Point, value);
        } else if ((c == 'e')
                   && currentState == State_Num)
        {
            fin.get(c);
            value += c;
            if (c == '+' || c == '-') {
                ProcessEvent(Event_Exponent, value);
            }
        } else if (isspecial(c)) {
            ProcessEvent(Event_Special, value);
        } else if (isspace(c)) {
            ProcessEvent(Event_Space, value);
        } else if (isdigit(c)) {
            ProcessEvent(Event_Didgit, value);
        } else if (c == '(') {
            ProcessEvent(Event_Lbkt, value);
        } else if (c == ')') {
            ProcessEvent(Event_Rbkt, value);
        } else if (isalpha(c)) {
            ProcessEvent(Event_Letter, value);
        } else if (c == '#') {
            ProcessEvent(Event_End, value);
            fin.get(c);
            value += c;
            if (c == '\\') {
                fin.get(c);
                value += c;
                ProcessEvent(Event_Char, value);
            } else if (c == 't' || c == 'f') {
                ProcessEvent(Event_Bool, value);
            } else {
                ProcessEvent(Event_Error, value);
            }
        } else {
            ProcessEvent(Event_Error, value);
        }
    }
    value += " ";
    ProcessEvent(Event_Space, value);
}

void TLexer::AddTransition(States from, Events event, States to) {
    statesTable[from][event] = Transition(to);
}

void TLexer::ProcessEvent(Events event, std::string& value) {
    States NextState = statesTable[currentState][event].nextState;
    if (NextState == State_Error) {
        std::cout << "Find Error" << std::endl;
    } else if (NextState == State_End) {
        Tokens.push_back(Token(currentState, value.substr(0, value.size()-1)));
        std::cout << int(currentState) << "\t" << value.substr(0, value.size()-1) << std::endl;
        value.erase(0, value.size()-1);
        currentState = NextState;
    } else if (currentState == State_End) {
      currentState = NextState;
    } else if (NextState == State_Space && currentState == State_Space) {
        value.erase(0,1);
    } else if (NextState !=  currentState) {
        Tokens.push_back(Token(currentState, value.substr(0, value.size()-1)));
        std::cout << int(currentState) << "\t" << value.substr(0, value.size()-1) << std::endl;
        value.erase(0, value.size()-1);
        currentState = NextState;
    }
}


TLexer::Transition::Transition(States state):
    nextState(state)
{
}

void TLexer::AddRule(std::string rule, States state) {
    std::string temp = "";
    AddTransition(state, Event_End, State_End);
    AddTransition(state, Event_Error, State_Error);
    for (size_t i = 0; i < rule.length(); i++) {
        if (rule[i] == '/') {
            GetRule(temp, state);
            temp = "/";
        } else if (rule[i] ==  '{') {
            GetRule(temp, state);
            temp = "";
        } else if (rule[i] == '}') {
            GetRule(temp, state);
            temp = "";
        } else if (rule[i] == '*') {
            GetRule(temp, state);
            temp = "";
        } else {
            temp += rule[i];
        }
    }
}

void TLexer::GetRule(std::string currentRule, States state) {
    if (currentRule == "") {
        return;
    }
    if (currentRule == "/d") {
        AddTransition(state, Event_Didgit, state);
        return;
    }
    if (currentRule == "/s") {
        AddTransition(state, Event_Special, state);
        return;
    }
    if (currentRule == "/l") {
        AddTransition(state, Event_Letter, state);
        return;
    }
    if (currentRule == "/d+") {
        AddTransition(state, Event_Didgit, State_Num);
        return;
    }
    if (currentRule == "/s+") {
        AddTransition(state, Event_Special, State_Ident);
        return;
    }
    if (currentRule == "/l+") {
        AddTransition(state, Event_Letter, State_Ident);
        return;
    }
    if (currentRule == ".") {
        AddTransition(state, Event_Point, State_Num);
        return;
    }
    if (currentRule == "e+" || currentRule == "e-") {
        AddTransition(state, Event_Exponent, State_Num);
        return;
    }
    if (currentRule == "#t" || currentRule == "#f") {
        AddTransition(state, Event_Bool, State_Bool);
        return;
    }
    if (currentRule == "#\\") {
        AddTransition(state, Event_Char, State_Char);
        return;
    }
    if (currentRule == "/n") {
        AddTransition(state, Event_NewLine, State_NewLine);
        return;
    }
    if (currentRule == " ") {
        AddTransition(state, Event_Space, State_Space);
        return;
    }
    if (currentRule == "\'") {
        AddTransition(state, Event_Symbol, State_Symbol);
        return;
    }
    if (currentRule == "\"") {
        AddTransition(state, Event_String, State_String);
        return;
    }
    if (currentRule == "(") {
        AddTransition(state, Event_Lbkt, State_Lbkt);
        return;
    }
    if (currentRule == ")") {
        AddTransition(state, Event_Rbkt, State_Rbkt);
        return;
    }
}
