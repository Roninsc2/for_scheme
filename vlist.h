#pragma once
#include <memory>

enum EPairType {
    PT_Int,//0
    PT_Double,//1
    PT_String,//2
    PT_Symbol,//3
    PT_Char,//4
    PT_Bool,//5
    PT_List,
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
    void ConvetToPair(TPairType* val);
    size_t ListLength();
    bool isList = true;
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
    EPairType Type = PT_Int;
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
    EPairType Type = PT_List;
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
    EPairType Type = PT_Double;
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
    EPairType Type = PT_Char;
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
    EPairType Type = PT_String;
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
    EPairType Type = PT_Symbol;
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
    EPairType Type = PT_Bool;
    bool value;

};
