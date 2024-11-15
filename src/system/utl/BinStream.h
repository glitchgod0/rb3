#ifndef UTL_BINSTREAM_H
#define UTL_BINSTREAM_H
#include "math/Rand2.h"
#include "types.h"
#include "utl/Str.h"
#include "utl/Symbol.h"
#include <utility>
#include <vector>
#include <map>

#define BS_WRITE_OP(var) \
    BinStream& operator<<(var x){ \
        WriteEndian(&x, sizeof(var)); \
        return *this; \
    }

#define BS_READ_OP(var) \
    BinStream& operator>>(var& x){ \
        ReadEndian(&x, sizeof(var)); \
        return *this; \
    }

#define BS_READ_FUNC(var, name) \
    var Read##name(){ \
        var val; \
        *this >> val; \
        return val; \
    }

enum EofType {
    NotEof = 0,
    RealEof = 1,
    TempEof = 2
};

class BinStream {
public:
    /** 
     * @brief The three seek types for BinStream::Seek.
     * @see Seek()
     */
    enum SeekType {
        kSeekBegin = 0,
        kSeekCur = 1,
        kSeekEnd = 2,
    };

    BinStream(bool);
    virtual ~BinStream();
    virtual void Flush() = 0;
    virtual int Tell() = 0;
    virtual EofType Eof() = 0;
    virtual bool Fail() = 0;
    virtual const char* Name() const;
    virtual bool Cached() const { return false; }
    virtual int GetPlatform() const { return 0; }
    virtual void ReadImpl(void*, int) = 0;
    virtual void WriteImpl(const void*, int) = 0;
    virtual void SeekImpl(int, SeekType) = 0;

    /** Endianness flags, denotes whether to swap from native. */
    bool mLittleEndian;
    /** PRNG source, used for encryption */
    Rand2 *mCrypto;

    BinStream &operator<<(const char *);
    BinStream &operator<<(const Symbol &);
    BinStream &operator<<(const class String &);

    /** Reads a `len` length string to `out`.
     * Reads a standard C string of length `len` into `out` from
     * the open file.
     *
     * @param [out] out The pointer to read the string into.
     * @param [in] len The length of text to read.
     */
    void ReadString(char * out, int len);

    BinStream &operator>>(Symbol &);
    BinStream &operator>>(class String &);

    /** Enables read encryption for e.g. archives.
     * Enables read encryption using Rand2, used in .dtb and .ark files.
     */
    void EnableReadEncryption();

    void EnableWriteEncryption();

    void DisableEncryption();

    /** Reads `len` bytes of data into `out`.
     * Reads a length of data specified by `len` from the open file
     * into `out`, using the native endianness (PPC == BE).
     *
     * @param [out] out The pointer to read data into.
     * @param [in] len The length of data to read.
     */
    void Read(void * out, int len);

    void Write(const void *, int);

    /** Seeks to `offset` using `mode`. */
    void Seek(int offset, SeekType mode);

    /** Reads `len` bytes of data into `out`, reversed if mLittleEndian is true.
     * Reads a length of data specified by `len` from the open file
     * into `out`, using the endianness specified by mLittleEndian.
     * (mLE == LE, therefore !mLE == BE)
     *
     * @param [out] out The pointer to read data into.
     * @param [in] len The length of data to read.
     */
    void ReadEndian(void * out, int len);

    void WriteEndian(const void *, int);

    bool LittleEndian() const { return mLittleEndian; }

    BS_WRITE_OP(short);
    BS_WRITE_OP(unsigned short);
    BS_WRITE_OP(int);
    BS_WRITE_OP(unsigned int);
    BS_WRITE_OP(long);
    BS_WRITE_OP(unsigned long);
    BS_WRITE_OP(long long);
    BS_WRITE_OP(unsigned long long);
    BS_WRITE_OP(float);

    BinStream& operator<<(char c){
        Write(&c, 1);
        return *this;
    }

    BinStream& operator<<(unsigned char uc){
        Write(&uc, 1);
        return *this;
    }

    BinStream& operator<<(bool b){
        unsigned char uc = (b) ? 1 : 0;
        Write(&uc, 1);
        return *this;
    }

    BS_READ_OP(int);
    BS_READ_OP(unsigned int);
    BS_READ_OP(short);
    BS_READ_OP(unsigned short);
    BS_READ_OP(long);
    BS_READ_OP(unsigned long);
    BS_READ_OP(long long);
    BS_READ_OP(unsigned long long);
    BS_READ_OP(float);

    BinStream& operator>>(char& out) {
        Read(&out, 1);
        return *this;
    }

    BinStream& operator>>(unsigned char& out) {
        Read(&out, 1);
        return *this;
    }

    BinStream& operator>>(bool& b){
        unsigned char uc;
        *this >> uc;
        b = (uc != 0);
        return *this;
    }

    BS_READ_FUNC(char, Char);
    BS_READ_FUNC(unsigned char, Byte);
    BS_READ_FUNC(short, Short);
    BS_READ_FUNC(unsigned short, UShort);
    BS_READ_FUNC(int, Int);
    BS_READ_FUNC(unsigned int, UInt);
    BS_READ_FUNC(long, Long);
    BS_READ_FUNC(unsigned long, ULong);
    BS_READ_FUNC(long long, LongLong);
    BS_READ_FUNC(unsigned long long, ULongLong);
    BS_READ_FUNC(bool, Bool);
    BS_READ_FUNC(float, Float);
};

// Note: `Allocator` here is actually the size/capacity type parameter on Wii.
// The name is based on Xbox 360 symbols, which show the allocator type instead.
template<class T, class Allocator>
BinStream& operator<<(BinStream& bs, const std::vector<T, Allocator>& vec){
    bs << (int)vec.size();
    for(typename std::vector<T, Allocator>::const_iterator it = vec.begin(); it != vec.end(); it++){
        bs << *it;
    }
    return bs;
}

template<class T, class Allocator>
BinStream& operator>>(BinStream& bs, std::vector<T, Allocator>& vec){
    unsigned int length;
    bs >> length;
    vec.resize(length);

    for(typename std::vector<T, Allocator>::iterator it = vec.begin(); it != vec.end(); it++){
        bs >> *it;
    }

    return bs;
}

template<class T, class Allocator>
BinStream& operator>>(BinStream& bs, std::list<T, Allocator>& list){
    unsigned int length;
    bs >> length;
    list.resize(length);

    for(typename std::list<T, Allocator>::iterator it = list.begin(); it != list.end(); it++){
        bs >> *it;
    }

    return bs;
}

template<class T1, class T2>
BinStream& operator<<(BinStream& bs, const std::map<T1, T2>& map){
    bs << map.size();
    for(typename std::map<T1, T2>::const_iterator it = map.begin(); it != map.end(); ++it){
        bs << it->first << it->second;
    }
    return bs;
}

template<class T1, class T2>
BinStream& operator>>(BinStream& bs, std::map<T1, T2>& map){
    int size;
    bs >> size;
    for(; size != 0; size--){
        T1 key;
        bs >> key;
        bs >> map[key];
    }
    return bs;
}

template <class T1, class T2>
BinStream& operator>>(BinStream& bs, std::pair<T1, T2>& p) {
    bs >> p.first >> p.second;
    return bs;
}

template <class T>
BinStream& operator>>(BinStream& bs, T* t) {
    t->Load(bs);
}

#endif
