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
    EPairType GetType() {
        return Type;
    }
    EPairType Type;
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
    bool isList();
    void GetListData();
private:
    std::shared_ptr<TPair> list;
};


class TPairTypeInt : public TPairType {

public:
    TPairTypeInt(int val): value(val) {
        Type = PT_Int;
    }
    int GetValue() {
        return value;
    }
private:
    int value;

};

class TPairTypeList : public TPairType {

public:
    TPairTypeList(VList* val): value(val) {
        Type = PT_List;
    }
    ~TPairTypeList(){
        delete value;
    }

    VList* GetValue() {
        return value;
    }
private:
    VList* value;

};

class TPairTypeDouble : public TPairType {

public:
    TPairTypeDouble(double val): value(val) {
        Type = PT_Double;
    }
    double GetValue() {
        return value;
    }
private:
    double value;

};

class TPairTypeChar : public TPairType {

public:
    TPairTypeChar(char val): value(val) {
        Type = PT_Char;
    }
    char GetValue() {
        return value;
    }
private:
    char value;

};

class TPairTypeString : public TPairType {

public:
    TPairTypeString(std::string val): value(val) {
        Type = PT_String;
    }
    std::string GetValue() {
        return value;
    }
private:
    std::string value;

};

class TPairTypeSymbol : public TPairType {

public:
    TPairTypeSymbol(std::string val): value(val) {
        Type = PT_Symbol;
    }
    std::string GetValue() {
        return value;
    }
private:
    std::string value;

};

class TPairTypeBool : public TPairType {

public:
    TPairTypeBool(bool val): value(val) {
        Type = PT_Bool;
    }
    bool GetValue() {
        return value;
    }
private:
    bool value;

};
