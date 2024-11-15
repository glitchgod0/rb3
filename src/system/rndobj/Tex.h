#ifndef RNDOBJ_TEX_H
#define RNDOBJ_TEX_H

#include "obj/Object.h"
#include "rndobj/Bitmap.h"
#include "utl/BinStream.h"
#include "utl/FilePath.h"
#include "utl/Loader.h"

class RndTex : public Hmx::Object {
public:
    enum Type {
        kRegular = 1,
        kRendered = 2,
        kMovie = 4,
        kBackBuffer = 8,
        kFrontBuffer = 0x18,
        kRenderedNoZ = 0x22,
        kShadowMap = 0x42,
        kDepthVolumeMap = 0xA2,
        kDensityMap = 0x122,
        kScratch = 0x200,
        kDeviceTexture = 0x1000
    };

    RndTex();
    virtual ~RndTex();
    OBJ_CLASSNAME(RndTex)
    OBJ_SET_TYPE(RndTex)
    virtual DataNode Handle(DataArray*, bool);
    virtual bool SyncProperty(DataNode &, DataArray *, int, PropOp);
    virtual void Save(BinStream &);
    virtual void Copy(const Hmx::Object*, CopyType);
    virtual void Load(BinStream&);
    virtual void Print();
    virtual void PreLoad(BinStream&);
    virtual void PostLoad(BinStream&);
    virtual void SetMipMapK(float f){ mMipMapK = f; }
    virtual void LockBitmap(RndBitmap&, int);
    virtual void UnlockBitmap() {}
    virtual void MakeDrawTarget() {}
    virtual void FinishDrawTarget() {}
    virtual void Compress(bool) {}
    virtual bool TexelsLock(void*&) {return false;}
    virtual void TexelsUnlock() {}
    virtual int TexelsPitch() const { return 0; }
    virtual void Select(int) {}
    virtual void PresyncBitmap() ;
    virtual void SyncBitmap() ;

    DataNode OnSetRendered(const DataArray*);
    DataNode OnSetBitmap(const DataArray*);

    void SetBitmap(int, int, int, RndTex::Type, bool, const char*);
    void SetBitmap(const RndBitmap&, const char*, bool);
    void SetBitmap(FileLoader*);
    void SetBitmap(const FilePath&);
    void SaveBitmap(const char*);
    void SetPowerOf2();

    static const char* CheckSize(int, int, int, int, RndTex::Type, bool);
    static void PlatformBppOrder(const char*, int&, int&, bool);
    inline bool IsRenderTarget() { return mType & kRendered; }
    int Width() const { return mWidth; }
    int Height() const { return mHeight; }
    Type GetType() const { return mType; }
    const FilePath& File() const { return mFilepath; }

    NEW_OVERLOAD
    NEW_OBJ(RndTex)
    DELETE_OVERLOAD;
    static void Init(){
        REGISTER_OBJ_FACTORY(RndTex)
    }

    RndBitmap mBitmap;
    float mMipMapK; // 0x38
    Type mType; // 0x3C
    int mWidth; // 0x40
    int mHeight; // 0x44
    int mBpp; // 0x48
    FilePath mFilepath; // 0x4C
    int mNumMips; // 0x58
    bool mIsPowerOf2; // 0x5C
    bool mOptimizeForPS3; // 0x5D
    FileLoader* mLoader; // 0x60

    DECLARE_REVS
};

bool UseBottomMip();
void CopyBottomMip(RndBitmap&, const RndBitmap&);
const char* CheckDim(int, RndTex::Type, bool);
TextStream& operator<<(TextStream&, RndTex::Type);

#endif // RNDOBJ_TEX_H
