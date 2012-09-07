#pragma once

// atle.h
// 9/6/2012
//
// AtlEdict
// See: chiitrans2/atlas.cs

#include <QtCore/QLibrary>

#define ATLE_DLL    "AtleCont"
#define ATLE_ENCODING "SHIFT-JIS"

class Atle : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(Atle)
  typedef Atle Self;
  typedef QObject Base;

  bool loaded_;
  QLibrary *lib_;

  enum { LibBufferSize = 30000 };
  char *libBuffer1_, *libBuffer2_;

public:
  explicit Atle(QObject *parent = nullptr)
    : Base(parent), loaded_(false), lib_(nullptr), libBuffer1_(nullptr), libBuffer2_(nullptr),
      pDestroyEngine(nullptr), pCreateEngine(nullptr), pAtlInitEngineData(nullptr), pFreeAtlasData(nullptr), pTranslatePair(nullptr)
  { }

  ~Atle()
  {
    if (loaded_) unload();
    if (libBuffer1_) delete libBuffer1_;
    if (libBuffer2_) delete libBuffer2_;
  }

protected:
  QLibrary *library()
  {
    if (!lib_)
      lib_ = new QLibrary(ATLE_DLL, this);
    return lib_;
  }

public:
  bool load() { return loaded_ = (library()->load() && atlInitEngineData() && createEngine()); }
  bool isLoaded() { return loaded_; }
  void unload() { if (lib_) destroyEngine(); loaded_ = false; }

  bool reload()
  {
    if (lib_) {
      lib_->deleteLater();
      lib_ = nullptr;
    }
    pDestroyEngine = nullptr;
    pCreateEngine = nullptr;
    pAtlInitEngineData = nullptr;
    pFreeAtlasData = nullptr;
    pTranslatePair = nullptr;

    return load();
  }

  QByteArray translate(const QByteArray &input)
  {
    QByteArray ret;
    if (input.isEmpty())
      return ret;
    char *in = new char[input.size()];
    qMemCopy(in, input.constData(), input.size());
    char *out = nullptr;
    void *buffer = nullptr;
    if (translatePair(in, &out, &buffer) && out)
      ret = out;
    if (out)
      freeAtlasData(out);
    if (buffer)
      freeAtlasData(buffer);
    delete in;
    return ret;
  }

protected:
  void destroyEngine()
  { DestroyEngine(); }

  bool createEngine() // memory leak caused by unmanaged memory
  { return 1 == CreateEngine(1, 1, 0, "General"); } // parameters see Atlas.cs

  bool atlInitEngineData()
  {
    if (!libBuffer1_)
      libBuffer1_ = new char[LibBufferSize];
    if (!libBuffer2_)
      libBuffer2_ = new char[LibBufferSize];
    return !AtlInitEngineData(0, 2, libBuffer1_, 0, libBuffer2_); // parameters see Atlas.cs
  }

  void freeAtlasData(void *out)
  { FreeAtlasData(out, nullptr, nullptr, nullptr); }

  bool
  translatePair(char *in, char **out, void **buffer)
  {
    ulong dwSize = 0;
    bool ret = -1 != TranslatePair(in, out, buffer, &dwSize);
    return ret;
  }

protected:
#define RESOLVE(_api) \
  if (!p##_api) { \
    QLibrary *lib = library(); \
    if (lib->isLoaded()) \
      p##_api = static_cast<P##_api>(lib->resolve(#_api)); \
  }

  // private delegate int DestroyEngineType();
  typedef int (*PDestroyEngine)();
  PDestroyEngine pDestroyEngine;
  int DestroyEngine()
  {
    RESOLVE(DestroyEngine)
    return !pDestroyEngine ? -1 :
            pDestroyEngine();
  }

  // private delegate int CreateEngineType(int x, int dir, int x3, byte[] env);
  typedef int (*PCreateEngine)(int x1, int dir, int x3, const char *env);
  PCreateEngine pCreateEngine;
  int CreateEngine(int x1, int dir, int x3, const char *env)
  {
    RESOLVE(CreateEngine)
    return !pCreateEngine ? -1 : pCreateEngine(x1, dir, x3, env);
  }

  // private delegate int AtlInitEngineDataType(int x1, int x2, IntPtr x3, int x4, IntPtr x5);
  typedef int (*PAtlInitEngineData)(int x1, int x2, void *buffer1, int x4, void *buffer2);
  PAtlInitEngineData pAtlInitEngineData;
  int AtlInitEngineData(int x1, int x2, void *buffer1, int x4, void *buffer2)
  {
    RESOLVE(AtlInitEngineData)
    return !pAtlInitEngineData ? -1 : pAtlInitEngineData(x1, x2, buffer1, x4, buffer2);
  }

  // private delegate int FreeAtlasDataType(System.IntPtr mem, System.IntPtr noSureHowManyArgs, System.IntPtr x3, System.IntPtr x4);
  typedef int (*PFreeAtlasData)(void *mem, void *noSureHowManyArgs, void *x3, void *x4);
  PFreeAtlasData pFreeAtlasData;
  int FreeAtlasData(void *mem, void *noSureHowManyArgs, void *x3, void *x4)
  {
    RESOLVE(FreeAtlasData)
    return !pFreeAtlasData ? -1 : pFreeAtlasData(mem, noSureHowManyArgs, x3, x4);
  }

  // private delegate int TranslatePairType(byte[] @inp, out System.IntPtr @out, out System.IntPtr dunno, out uint maybeSize);
  typedef int (*PTranslatePair)(__in char *in, __out char **out, __out void **buffer, __out ulong *size);
  PTranslatePair pTranslatePair;
  int TranslatePair(__in char *in, __out char **out, __out void **buffer, __out ulong *size)
  {
    RESOLVE(TranslatePair)
    return !pTranslatePair ? -1 :
            pTranslatePair(in, out, buffer, size);
  }

#undef RESOLVE
};

// EOF
