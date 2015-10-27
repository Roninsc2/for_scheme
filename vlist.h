#pragma once
#include <memory>

enum EListType {
    LT_Int,//0
    LT_Double,//1
    LT_String,//2
    LT_Symbol,//3
    LT_Char,//4
    LT_Bool,//5
    LT_List
};

class TListType {
public:
    virtual ~TListType(){}
};

typedef std::shared_ptr<TListType> TListTypePtr;

class VList {

public:
    VList(TListTypePtr  val): value(val){}
    ~VList(){}
    void InsertAfter(VList* after);
    void InsertBefore(VList* before);
    void DeleteList();
    size_t ListLength();
private:
    TListTypePtr value;
    std::shared_ptr<VList> next;
};


class TListTypeInt : public TListType {

public:
    TListTypeInt(int val): value(val) {}
    int GetValue() {
        return value;
    }
    EListType GetType() {
        return Type;
    }
private:
    EListType Type = LT_Int;
    int value;

};

class TListTypeList : public TListType {

public:
    TListTypeList(VList* val): value(val) {}
    ~TListTypeList(){
        delete value;
    }

    VList* GetValue() {
        return value;
    }
    EListType GetType() {
        return Type;
    }
private:
    EListType Type = LT_List;
    VList* value;

};

class TListTypeDouble : public TListType {

public:
    TListTypeDouble(double val): value(val) {}
    double GetValue() {
        return value;
    }
    EListType GetType() {
        return Type;
    }
private:
    EListType Type = LT_Double;
    double value;

};

class TListTypeChar : public TListType {

public:
    TListTypeChar(char val): value(val) {}
    char GetValue() {
        return value;
    }
    EListType GetType() {
        return Type;
    }
private:
    EListType Type = LT_Char;
    char value;

};

class TListTypeString : public TListType {

public:
    TListTypeString(std::string val): value(val) {}
    std::string GetValue() {
        return value;
    }
    EListType GetType() {
        return Type;
    }
private:
    EListType Type = LT_String;
    std::string value;

};

class TListTypeSymbol : public TListType {

public:
    TListTypeSymbol(std::string val): value(val) {}
    std::string GetValue() {
        return value;
    }
    EListType GetType() {
        return Type;
    }
private:
    EListType Type = LT_Symbol;
    std::string value;

};

class TListTypeBool : public TListType {

public:
    TListTypeBool(bool val): value(val) {}
    bool GetValue() {
        return value;
    }
    EListType GetType() {
        return Type;
    }

private:
    EListType Type = LT_Bool;
    bool value;

};
