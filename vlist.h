#pragma once
#include <memory>

enum EPairType {
    LT_Int,//0
    LT_Double,//1
    LT_String,//2
    LT_Symbol,//3
    LT_Char,//4
    LT_Bool,//5
    LT_List
};

class TPairType {
public:
    virtual ~TPairType(){}
};

typedef std::shared_ptr<TPairType> TPairTypePtr;

struct TPair {
  TPair(TPairTypePtr val1, TPairTypePtr val2): head(val1),
      tail(val2)
  {
  }
  TPairTypePtr head;
  TPairTypePtr tail;
};

class VList {

public:
    VList(TPairTypePtr  val): list(new TPair(val, nullptr)){}
    ~VList(){}
    void InsertAfter(VList* after);
    void InsertBefore(VList* before);
    void DeleteList();
    size_t ListLength();
private:
    std::shared_ptr<TPair> list;
};


class TPairTypeInt : public TPairType {

public:
    TPairTypeInt(int val): value(val) {}
    int GetValue() {
        return value;
    }
    EPairType GetType() {
        return Type;
    }
private:
    EPairType Type = LT_Int;
    int value;

};

class TPairTypeList : public TPairType {

public:
    TPairTypeList(VList* val): value(val) {}
    ~TPairTypeList(){
        delete value;
    }

    VList* GetValue() {
        return value;
    }
    EPairType GetType() {
        return Type;
    }
private:
    EPairType Type = LT_List;
    VList* value;

};

class TPairTypeDouble : public TPairType {

public:
    TPairTypeDouble(double val): value(val) {}
    double GetValue() {
        return value;
    }
    EPairType GetType() {
        return Type;
    }
private:
    EPairType Type = LT_Double;
    double value;

};

class TPairTypeChar : public TPairType {

public:
    TPairTypeChar(char val): value(val) {}
    char GetValue() {
        return value;
    }
    EPairType GetType() {
        return Type;
    }
private:
    EPairType Type = LT_Char;
    char value;

};

class TPairTypeString : public TPairType {

public:
    TPairTypeString(std::string val): value(val) {}
    std::string GetValue() {
        return value;
    }
    EPairType GetType() {
        return Type;
    }
private:
    EPairType Type = LT_String;
    std::string value;

};

class TPairTypeSymbol : public TPairType {

public:
    TPairTypeSymbol(std::string val): value(val) {}
    std::string GetValue() {
        return value;
    }
    EPairType GetType() {
        return Type;
    }
private:
    EPairType Type = LT_Symbol;
    std::string value;

};

class TPairTypeBool : public TPairType {

public:
    TPairTypeBool(bool val): value(val) {}
    bool GetValue() {
        return value;
    }
    EPairType GetType() {
        return Type;
    }

private:
    EPairType Type = LT_Bool;
    bool value;

};
