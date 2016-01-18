#pragma once

#include <string>
#include <vector>
#include <list>
#include <utility>
#include <iostream>
#include <streambuf>

template<class T>
class TSerializer {
public:
    static void Save(std::ostream& out, const T& object) {
        object.Save(out);
    }
    static void Load(std::istream& in, T& object) {
        object.Load(in);
    }
};

template <class T>
static inline void Save(std::ostream& out, const T& t);

template <class T>
static inline void Load(std::istream& in, T& t);


template <class A, class B>
class TSerializer<std::pair<A, B> > {
public:
    static void Save(std::ostream& out, const std::pair<A, B>& object) {
        ::Save(out, object.first);
        ::Save(out, object.second);
    }
    static void Load(std::istream& in, std::pair<A, B>& object) {
        ::Load(in, object.first);
        ::Load(in, object.second);
    }
};

template<class TVec, class TObj>
class TVectorSerializer {
public:
    static inline void Save(std::ostream& out, const TVec& object) {
        unsigned short size = object.size();
        out.write((const char*)(&size), 2);
        for (const auto& obj: object) {
            ::Save(out, obj);
        }
    }

    static inline void Load(std::istream& in, TVec& object) {
        unsigned short size;
        in.read((char*)(&size), 2);
        object.clear();
        for (size_t i = 0; i < size; ++i) {
            TObj obj;
            ::Load(in, obj);
            object.push_back(std::move(obj));
        }
    }
};

template <class T> class TSerializer<std::vector<T> >: public TVectorSerializer<std::vector<T>, T > {};
template <class T> class TSerializer<std::list<T> >: public TVectorSerializer<std::list<T>, T > {};
template <> class TSerializer<std::string>: public TVectorSerializer<std::string, char> {};
//template <> class TSerializer<QString>: public TVectorSerializer<QString, QChar> {};

template <class T>
class TPodSerializer {
public:
    static inline void Save(std::ostream& out, const T& object) {
        out.write((const char*)(&object), sizeof(T));
    }
    static inline void Load(std::istream& in, T& object) {
        in.read((char*)(&object), sizeof(T));
    }
};

template <> class TSerializer<bool>: public TPodSerializer<bool> {};
template <> class TSerializer<char>: public TPodSerializer<char> {};
template <> class TSerializer<unsigned char>: public TPodSerializer<unsigned char> {};
template <> class TSerializer<short>: public TPodSerializer<short> {};
template <> class TSerializer<unsigned short>: public TPodSerializer<unsigned short> {};
template <> class TSerializer<int>: public TPodSerializer<int> {};
template <> class TSerializer<unsigned int>: public TPodSerializer<unsigned int> {};
template <> class TSerializer<long>: public TPodSerializer<long> {};
template <> class TSerializer<unsigned long>: public TPodSerializer<unsigned long> {};

template <> class TSerializer<long long>: public TPodSerializer<long long> {};
template <> class TSerializer<unsigned long long>: public TPodSerializer<unsigned long long> {};

template <> class TSerializer<float>: public TPodSerializer<float> {};
template <> class TSerializer<double>: public TPodSerializer<double> {};
//template <> class TSerializer<QChar>: public TPodSerializer<QChar> {};

template <class T>
static inline void Save(std::ostream& out, const T& t) {
    TSerializer<T>::Save(out, t);
}

template <class T>
static inline void Load(std::istream& in, T& t) {
    TSerializer<T>::Load(in, t);
}

static inline void LoadMany(std::istream& in) {
    return;
}

template<typename T, typename... Targs>
static inline void LoadMany(std::istream& in, T& value, Targs&... args)
{
    ::Load(in, value);
    LoadMany(in, args...);
}

static inline void SaveMany(std::ostream& out) {
    return;
}

template<typename T, typename... Targs>
static inline void SaveMany(std::ostream& out, const T& value, const Targs&... args)
{
    ::Save(out, value);
    SaveMany(out, args...);
}

#define SAVELOAD(...) \
    inline virtual void Save(std::ostream& out) const { \
        ::SaveMany(out, __VA_ARGS__);             \
    } \
 \
    inline virtual void Load(std::istream& in) { \
        ::LoadMany(in, __VA_ARGS__);             \
    }


struct membuf: std::streambuf {
    membuf(char const* base, size_t size) {
        char* p(const_cast<char*>(base));
        this->setg(p, p, p + size);
    }
};
struct imemstream: virtual membuf, std::istream {
    imemstream(char const* base, size_t size)
        : membuf(base, size)
        , std::istream(static_cast<std::streambuf*>(this)) {
    }
};

#define SAVELOAD_POD(TypeName) template <> class TSerializer<TypeName>: public TPodSerializer<TypeName> {};
