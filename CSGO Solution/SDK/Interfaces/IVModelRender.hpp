#pragma once
#include "../Math/QAngle.hpp"
#include "../Math/Vector.hpp"
#include "../Math/VMatrix.hpp"

typedef float Quaternion[4];
typedef float RadianEuler[3];

#define MAX_QPATH  260

#define BONE_CALCULATE_MASK             0x1F
#define BONE_PHYSICALLY_SIMULATED       0x01    // bone is physically simulated when physics are active
#define BONE_PHYSICS_PROCEDURAL         0x02    // procedural when physics is active
#define BONE_ALWAYS_PROCEDURAL          0x04    // bone is always procedurally animated
#define BONE_SCREEN_ALIGN_SPHERE        0x08    // bone aligns to the screen, not constrained in motion.
#define BONE_SCREEN_ALIGN_CYLINDER      0x10    // bone aligns to the screen, constrained by it's own axis.

#define BONE_USED_MASK                  0x0007FF00
#define BONE_USED_BY_ANYTHING           0x0007FF00
#define BONE_USED_BY_HITBOX             0x00000100    // bone (or child) is used by a hit box
#define BONE_USED_BY_ATTACHMENT         0x00000200    // bone (or child) is used by an attachment point
#define BONE_USED_BY_VERTEX_MASK        0x0003FC00
#define BONE_USED_BY_VERTEX_LOD0        0x00000400    // bone (or child) is used by the toplevel model via skinned vertex
#define BONE_USED_BY_VERTEX_LOD1        0x00000800    
#define BONE_USED_BY_VERTEX_LOD2        0x00001000  
#define BONE_USED_BY_VERTEX_LOD3        0x00002000
#define BONE_USED_BY_VERTEX_LOD4        0x00004000
#define BONE_USED_BY_VERTEX_LOD5        0x00008000
#define BONE_USED_BY_VERTEX_LOD6        0x00010000
#define BONE_USED_BY_VERTEX_LOD7        0x00020000
#define BONE_USED_BY_BONE_MERGE         0x00040000    // bone is available for bone merge to occur against it

#define BONE_USED_BY_VERTEX_AT_LOD(lod) ( BONE_USED_BY_VERTEX_LOD0 << (lod) )
#define BONE_USED_BY_ANYTHING_AT_LOD(lod) ( ( BONE_USED_BY_ANYTHING & ~BONE_USED_BY_VERTEX_MASK ) | BONE_USED_BY_VERTEX_AT_LOD(lod) )

#define MAX_NUM_LODS 8
#define MAXSTUDIOBONES		128		// total bones actually used

#define BONE_TYPE_MASK                  0x00F00000
#define BONE_FIXED_ALIGNMENT            0x00100000    // bone can't spin 360 degrees, all interpolation is normalized around a fixed orientation

#define BONE_HAS_SAVEFRAME_POS          0x00200000    // Vector48
#define BONE_HAS_SAVEFRAME_ROT64        0x00400000    // Quaternion64
#define BONE_HAS_SAVEFRAME_ROT32        0x00800000    // Quaternion32


#define HITGROUP_GENERIC 0
#define HITGROUP_HEAD 1
#define HITGROUP_CHEST 2
#define HITGROUP_STOMACH 3
#define HITGROUP_LEFTARM 4    
#define HITGROUP_RIGHTARM 5
#define HITGROUP_LEFTLEG 6
#define HITGROUP_RIGHTLEG 7
#define HITGROUP_GEAR 10

enum modtype_t
{
    mod_bad = 0,
    mod_brush,
    mod_sprite,
    mod_studio
};

typedef unsigned short MDLHandle_t;


class C_StudioHDR;
class ITexture;
class IMatRenderContext;
class DataCacheHandle_t;

enum PreviewImageRetVal_t
{
    MATERIAL_PREVIEW_IMAGE_BAD = 0,
    MATERIAL_PREVIEW_IMAGE_OK,
    MATERIAL_NO_PREVIEW_IMAGE,
};

enum MaterialVarFlags_t
{
    MATERIAL_VAR_DEBUG = (1 << 0),
    MATERIAL_VAR_NO_DEBUG_OVERRIDE = (1 << 1),
    MATERIAL_VAR_NO_DRAW = (1 << 2),
    MATERIAL_VAR_USE_IN_FILLRATE_MODE = (1 << 3),
    MATERIAL_VAR_VERTEXCOLOR = (1 << 4),
    MATERIAL_VAR_VERTEXALPHA = (1 << 5),
    MATERIAL_VAR_SELFILLUM = (1 << 6),
    MATERIAL_VAR_ADDITIVE = (1 << 7),
    MATERIAL_VAR_ALPHATEST = (1 << 8),
    //MATERIAL_VAR_UNUSED = (1 << 9),
    MATERIAL_VAR_ZNEARER = (1 << 10),
    MATERIAL_VAR_MODEL = (1 << 11),
    MATERIAL_VAR_FLAT = (1 << 12),
    MATERIAL_VAR_NOCULL = (1 << 13),
    MATERIAL_VAR_NOFOG = (1 << 14),
    MATERIAL_VAR_IGNOREZ = (1 << 15),
    MATERIAL_VAR_DECAL = (1 << 16),
    MATERIAL_VAR_ENVMAPSPHERE = (1 << 17), // OBSOLETE
    MATERIAL_VAR_UNUSED = (1 << 18), // UNUSED
    MATERIAL_VAR_ENVMAPCAMERASPACE = (1 << 19), // OBSOLETE
    MATERIAL_VAR_BASEALPHAENVMAPMASK = (1 << 20),
    MATERIAL_VAR_TRANSLUCENT = (1 << 21),
    MATERIAL_VAR_NORMALMAPALPHAENVMAPMASK = (1 << 22),
    MATERIAL_VAR_NEEDS_SOFTWARE_SKINNING = (1 << 23), // OBSOLETE
    MATERIAL_VAR_OPAQUETEXTURE = (1 << 24),
    MATERIAL_VAR_ENVMAPMODE = (1 << 25), // OBSOLETE
    MATERIAL_VAR_SUPPRESS_DECALS = (1 << 26),
    MATERIAL_VAR_HALFLAMBERT = (1 << 27),
    MATERIAL_VAR_WIREFRAME = (1 << 28),
    MATERIAL_VAR_ALLOWALPHATOCOVERAGE = (1 << 29),
    MATERIAL_VAR_ALPHA_MODIFIED_BY_PROXY = (1 << 30),
    MATERIAL_VAR_VERTEXFOG = (1 << 31),
};

typedef unsigned short ModelInstanceHandle_t;
typedef void* LightCacheHandle_t;
typedef void* StudioDecalHandle_t;
typedef int OverrideType_t;
typedef int ImageFormat;
typedef int VertexFormat_t;
typedef int MaterialPropertyTypes_t;

enum MaterialContextType_t
{
    MATERIAL_HARDWARE_CONTEXT,
    MATERIAL_QUEUED_CONTEXT,
    MATERIAL_NULL_CONTEXT
};

enum
{
    MATERIAL_MAX_LIGHT_COUNT = 4,
};

#define CREATERENDERTARGETFLAGS_HDR				0x00000001
#define CREATERENDERTARGETFLAGS_AUTOMIPMAP		0x00000002
#define CREATERENDERTARGETFLAGS_UNFILTERABLE_OK 0x00000004

enum MorphFormatFlags_t
{
    MORPH_POSITION = 0x0001,	// 3D
    MORPH_NORMAL = 0x0002,	// 3D
    MORPH_WRINKLE = 0x0004,	// 1D
    MORPH_SPEED = 0x0008,	// 1D
    MORPH_SIDE = 0x0010,	// 1D
};

#define MAXSTUDIOBONECTRLS			4
#define MAXSTUDIOPOSEPARAM			24
#define MAXSTUDIOSKINS				32		// total textures
#define MAXSTUDIOFLEXCTRL			96		// maximum number of flexcontrollers (input sliders)
#define MAXSTUDIOANIMBLOCKS			256
#define MAXSTUDIOFLEXDESC			1024	// maximum number of low level flexes (actual morph targets)

class CCustomMaterialOwner
{
public:
    virtual ~CCustomMaterialOwner() { }
    virtual void SetCustomMaterial(void* pCustomMaterial, int nIndex = 0) = 0;	// either replaces and existing material (releasing the old one), or adds one to the vector
    virtual void OnCustomMaterialsUpdated() {}
    virtual void DuplicateCustomMaterialsToOther(CCustomMaterialOwner* pOther) const = 0;

public:
    CUtlVector<void*> vecCustomMaterials;
}; // Size: 0x0014

class ITexture;
class IMatRenderContext;

class IRenderToRTHelperObject
{
public:
    virtual void Draw(const matrix3x4_t& matRootToWorld) = 0;
    virtual bool BoundingSphere(Vector& vecCenter, float& flRadius) = 0;
    virtual ITexture* GetEnvCubeMap() = 0;
};

struct MDLSquenceLayer_t
{
    int		nSequenceIndex;
    float	flWeight;
};

class CMDL
{
public:
    std::byte	pad[ 0x3C ]; // 0x0000
    MDLHandle_t	hModelHandle; // 0x003C
    std::byte	pad0[ 0x8 ]; // 0x003E
    Color		Color; // 0x0046
    std::byte	pad1[ 0x2 ]; // 0x004A
    int			nSkin; // 0x004C
    int			nBody; // 0x0050
    int			nSequence; // 0x0054
    int			nLOD; // 0x0058
    float		flPlaybackRate; // 0x005C
    float		flTime; // 0x0060
    float		flCurrentAnimEndTime; // 0x0064
    float		arrFlexControls[ MAXSTUDIOFLEXCTRL * 4 ]; // 0x0068
    Vector		vecViewTarget; // 0x0668
    bool		bWorldSpaceViewTarget; // 0x0674
    bool		bUseSequencePlaybackFPS; // 0x0675
    std::byte	pad2[ 0x2 ]; // 0x0676
    void* pProxyData; // 0x0678
    float		flTimeBasisAdjustment; // 0x067C
    std::byte	pad3[ 0x4 ]; // 0x0680 --isn't correct after this point iirc
    CUtlVector<int> arrSequenceFollowLoop; // 0x0684
    matrix3x4_t	matModelToWorld; // 0x0698
    bool		bRequestBoneMergeTakeover; // 0x06C8
}; // Size: 0x06C9 // 0x6D0?

template <class T>
class CBaseAutoPtr
{
public:
    CBaseAutoPtr() :
        pObject(nullptr) { }

    CBaseAutoPtr(T* pObject) :
        pObject(pObject) { }

    operator const void* () const
    {
        return pObject;
    }

    operator void* () const
    {
        return pObject;
    }

    operator const T* () const
    {
        return pObject;
    }

    operator const T* ()
    {
        return pObject;
    }

    operator T* ()
    {
        return pObject;
    }

    int	operator=(int i)
    {
        pObject = nullptr;
        return 0;
    }

    T* operator=(T* pSecondObject)
    {
        pObject = pSecondObject;
        return pSecondObject;
    }

    bool operator!() const
    {
        return (!pObject);
    }

    bool operator==(const void* pSecondObject) const
    {
        return (pObject == pSecondObject);
    }

    bool operator!=(const void* pSecondObject) const
    {
        return (pObject != pSecondObject);
    }

    bool operator==(T* pSecondObject) const
    {
        return operator==(static_cast<void*>(pSecondObject));
    }

    bool operator!=(T* pSecondObject) const
    {
        return operator!=(static_cast<void*>(pSecondObject));
    }

    bool operator==(const CBaseAutoPtr<T>& pSecondPtr) const
    {
        return operator==(static_cast<const void*>(pSecondPtr));
    }

    bool operator!=(const CBaseAutoPtr<T>& pSecondPtr) const
    {
        return operator!=(static_cast<const void*>(pSecondPtr));
    }

    T* operator->()
    {
        return pObject;
    }

    T& operator*()
    {
        return *pObject;
    }

    T** operator&()
    {
        return &pObject;
    }

    const T* operator->() const
    {
        return pObject;
    }

    const T& operator*() const
    {
        return *pObject;
    }

    T* const* operator&() const
    {
        return &pObject;
    }

    CBaseAutoPtr(const CBaseAutoPtr<T>& pSecondPtr) :
        pObject(pSecondPtr.pObject) { }

    void operator=(const CBaseAutoPtr<T>& pSecondPtr)
    {
        pObject = pSecondPtr.pObject;
    }

    T* pObject;
};

struct Texture_t
{
    std::byte			pad0[ 0xC ];		// 0x0000
    IDirect3DTexture9* lpRawTexture;	// 0x000C
};


template <class T>
class CRefPtr : public CBaseAutoPtr<T>
{
    typedef CBaseAutoPtr<T> CBaseClass;
public:
    CRefPtr() { }

    CRefPtr(T* pInit)
        : CBaseClass(pInit) { }

    CRefPtr(const CRefPtr<T>& pRefPtr)
        : CBaseClass(pRefPtr) { }

    ~CRefPtr()
    {
        if (CBaseClass::pObject != nullptr)
            CBaseClass::pObject->Release();
    }

    void operator=(const CRefPtr<T>& pSecondRefPtr)
    {
        CBaseClass::operator=(pSecondRefPtr);
    }

    int operator=(int i)
    {
        return CBaseClass::operator=(i);
    }

    T* operator=(T* pSecond)
    {
        return CBaseClass::operator=(pSecond);
    }

    operator bool() const
    {
        return !CBaseClass::operator!();
    }

    operator bool()
    {
        return !CBaseClass::operator!();
    }

    void SafeRelease()
    {
        if (CBaseClass::pObject != nullptr)
            CBaseClass::pObject->Release();

        CBaseClass::pObject = nullptr;
    }

    void AssignAddReference(T* pFrom)
    {
        if (pFrom != nullptr)
            pFrom->AddReference();

        SafeRelease();
        CBaseClass::pObject = pFrom;
    }

    void AddReferenceAssignTo(T*& pTo)
    {
        if (CBaseClass::pObject != nullptr)
            CBaseClass::pObject->AddReference();

        SafeRelease(pTo);
        pTo = CBaseClass::pObject;
    }
};

class IRefCounted
{
public:
    virtual int AddReference() = 0;
    virtual int Release() = 0;
};

class CRefCounted
{
public:
    virtual void Destructor(char bDelete) = 0;
    virtual bool OnFinalRelease() = 0;

    void Release()
    {
        if (InterlockedDecrement(&vlRefCount) == 0 && OnFinalRelease())
            Destructor(1);
    }

private:
    volatile long vlRefCount;
};


struct LightDesc_t;
class IMatRenderContext : public IRefCounted
{
private:
    template <typename T, typename ... args_t>
    constexpr T CallVFunc(void* thisptr, std::size_t nIndex, args_t... argList)
    {
        using VirtualFn = T(__thiscall*)(void*, decltype(argList)...);
        return (*static_cast<VirtualFn**>(thisptr))[ nIndex ](thisptr, argList...);
    }
public:
    void BeginRender()
    {
        CallVFunc<void>(this, 2);
    }

    void EndRender()
    {
        CallVFunc<void>(this, 3);
    }

    void BindLocalCubemap(ITexture* pTexture)
    {
        CallVFunc<void>(this, 5, pTexture);
    }

    void SetRenderTarget(ITexture* pTexture)
    {
        CallVFunc<void>(this, 6, pTexture);
    }

    ITexture* GetRenderTarget()
    {
        return CallVFunc<ITexture*>(this, 7);
    }

    void ClearBuffers(bool bClearColor, bool bClearDepth, bool bClearStencil = false)
    {
        CallVFunc<void>(this, 12, bClearColor, bClearDepth, bClearStencil);
    }

    void SetLights(int nCount, const LightDesc_t* pLights)
    {
        CallVFunc<void>(this, 17, nCount, pLights);
    }

    void SetAmbientLightCube(Vector4D vecCube[ 6 ])
    {
        CallVFunc<void>(this, 18, vecCube);
    }

    void SetIntRenderingParameter( int a1, int a2 )
    {
        return GetVirtual < void( __thiscall* )( void*, int, int ) >( this, 126 )( this, a1, a2 );
    }

    void Viewport(int x, int y, int iWidth, int iHeight)
    {
        CallVFunc<void>(this, 40, x, y, iWidth, iHeight);
    }

    void GetViewport(int& x, int& y, int& iWidth, int& iHeight)
    {
        CallVFunc<void, int&, int&, int&, int&>(this, 41, x, y, iWidth, iHeight);
    }

    void ClearColor3ub(unsigned char r, unsigned char g, unsigned char b)
    {
        CallVFunc<void>(this, 78, r, g, b);
    }

    void ClearColor4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
    {
        CallVFunc<void>(this, 79, r, g, b, a);
    }

    void DrawScreenSpaceRectangle(C_Material* pMaterial, int iDestX, int iDestY, int iWidth, int iHeight, float flTextureX0, float flTextureY0, float flTextureX1, float flTextureY1, int iTextureWidth, int iTextureHeight, void* pClientRenderable = nullptr, int nXDice = 1, int nYDice = 1)
    {
        CallVFunc<void>(this, 114, pMaterial, iDestX, iDestY, iWidth, iHeight, flTextureX0, flTextureY0, flTextureX1, flTextureY1, iTextureWidth, iTextureHeight, pClientRenderable, nXDice, nYDice);
    }

    void PushRenderTargetAndViewport()
    {
        CallVFunc<void>(this, 119);
    }

    void PopRenderTargetAndViewport()
    {
        CallVFunc<void>(this, 120);
    }

    void SetLightingOrigin(/*Vector vecLightingOrigin*/float x, float y, float z)
    {
        CallVFunc<void>(this, 158, x, y, z);
    }

};


class C_MergedMDL : public IRenderToRTHelperObject
{
public:
    virtual ~C_MergedMDL() { }
    virtual void SetMDL(MDLHandle_t hModelHandle, CCustomMaterialOwner* pCustomMaterialOwner = nullptr, void* pProxyData = nullptr) = 0;
    virtual void SetMDL(const char* szModelPath, CCustomMaterialOwner* pCustomMaterialOwner = nullptr, void* pProxyData = nullptr) = 0;

    void SetMergedMDL( const char* szModelPath, CCustomMaterialOwner* pCustomMaterialOwner = nullptr, void* pProxyData = nullptr );

    void SetupBonesForAttachmentQueries();

    void SetSequence(const int nSequence, const bool bUseSequencePlaybackFPS)
    {
        this->RootMDL.nSequence = nSequence;
        this->RootMDL.bUseSequencePlaybackFPS = bUseSequencePlaybackFPS;
        this->RootMDL.flTimeBasisAdjustment = this->RootMDL.flTime;
    }

    void SetSkin(int nSkin)
    {
        this->RootMDL.nSkin = nSkin;
    }

public:
    CMDL RootMDL; // 0x0000
    CUtlVector<CMDL> vecMergedModels; // 0x069C
    float arrPoseParameters[ MAXSTUDIOPOSEPARAM ]; // 0x06E9
    int	nSequenceLayers; // 0x0749
    MDLSquenceLayer_t sequenceLayers[ 8 ]; // 0x074D -> // end: 0x78D
}; // Expected Size: 0x075C

class IClientRenderable;
class C_Material;
class C_MaterialVar {
public:
    void SetVecValue(float r, float g, float b) {
        using original_fn = void(__thiscall*)(C_MaterialVar*, float, float, float);
        return (*(original_fn**)this)[11](this, r, g, b);
    }
};

struct FileHandle_t;
class C_KeyValues;
struct model_t;
struct mstudioanimdesc_t;
struct mstudioseqdesc_t
{
    //DECLARE_BYTESWAP_DATADESC();
    int					baseptr;
    int					szlabelindex;
    int					szactivitynameindex;
    int					flags;		// looping/non-looping flags
    int					activity;	// initialized at loadtime to game DLL values
    int					actweight;

    int					numevents;
    int					eventindex;

    Vector				bbmin;		// per sequence bounding box
    Vector				bbmax;

    int					numblends;

    // Index into array of shorts which is groupsize[0] x groupsize[1] in length
    int					animindexindex;

    int					movementindex;	// [blend] float array for blended movement
    int					groupsize[2];
    int					paramindex[2];	// X, Y, Z, XR, YR, ZR
    float				paramstart[2];	// local (0..1) starting value
    float				paramend[2];	// local (0..1) ending value
    int					paramparent;

    float				fadeintime;		// ideal cross fate in time (0.2 default)
    float				fadeouttime;	// ideal cross fade out time (0.2 default)

    int					localentrynode;		// transition node at entry
    int					localexitnode;		// transition node at exit
    int					nodeflags;		// transition rules

    float				entryphase;		// used to match entry gait
    float				exitphase;		// used to match exit gait

    float				lastframe;		// frame that should generation EndOfSequence

    int					nextseq;		// auto advancing sequences
    int					pose;			// index of delta animation between end and nextseq

    int					numikrules;

    int					numautolayers;	//
    int					autolayerindex;

    int					weightlistindex;

    // FIXME: make this 2D instead of 2x1D arrays
    int					posekeyindex;

    int					numiklocks;
    int					iklockindex;

    // Key values
    int					keyvalueindex;
    int					keyvaluesize;

    int					cycleposeindex;		// index of pose parameter to use as cycle index

    int					activitymodifierindex;
    int					numactivitymodifiers;

    int animtag_index;
    int num_animtags;

    int					unused[3];		// remove/add as appropriate (grow back to 8 ints on version change!)

    mstudioseqdesc_t() {}
private:
    // No copy constructors allowed
    mstudioseqdesc_t(const mstudioseqdesc_t& vOther);
};

struct Ray_t;
struct DrawModelInfo_t;
struct studiohwdata_t;
struct MaterialLightingState_t;
struct ColorMeshInfo_t;

struct DrawModelState_t
{
    void*                   m_pStudioHdr;
    studiohwdata_t*         m_pStudioHWData;
    IClientRenderable*      m_pRenderable;
    const matrix3x4_t*      m_pModelToWorld;
    StudioDecalHandle_t     m_decals;
    int                     m_drawFlags;
    int                     m_lod;
};

struct StaticPropRenderInfo_t
{
    const matrix3x4_t*      pModelToWorld;
    const model_t*          pModel;
    IClientRenderable*      pRenderable;
    Vector*                 pLightingOrigin;
    short                   skin;
    ModelInstanceHandle_t   instance;
};

struct ModelRenderInfo_t
{
    Vector                  origin;
    QAngle                  angles;
	char                    pad[4];
    IClientRenderable*      pRenderable;
    const model_t*          pModel;
    const matrix3x4_t*      pModelToWorld;
    const matrix3x4_t*      pLightingOffset;
    const Vector*           pLightingOrigin;
    int                     flags;
    int                     entity_index;
    int                     skin;
    int                     body;
    int                     hitboxset;
    ModelInstanceHandle_t   instance;

    ModelRenderInfo_t()
    {
        pModelToWorld = NULL;
        pLightingOffset = NULL;
        pLightingOrigin = NULL;
    }
};

struct LightingQuery_t
{
    Vector                  m_LightingOrigin;
    ModelInstanceHandle_t   m_InstanceHandle;
    bool                    m_bAmbientBoost;
};

struct StaticLightingQuery_t : public LightingQuery_t
{
    IClientRenderable*        m_pRenderable;
};

class C_Material
{
public:
    virtual const char*             GetName() const = 0;
    virtual const char*             GetTextureGroupName() const = 0;
    virtual PreviewImageRetVal_t    GetPreviewImageProperties(int *width, int *height, ImageFormat *imageFormat, bool* isTranslucent) const = 0;
    virtual PreviewImageRetVal_t    GetPreviewImage(unsigned char *data, int width, int height, ImageFormat imageFormat) const = 0;
    virtual int                     GetMappingWidth() = 0;
    virtual int                     GetMappingHeight() = 0;
    virtual int                     GetNumAnimationFrames() = 0;
    virtual bool                    InMaterialPage(void) = 0;
    virtual    void                 GetMaterialOffset(float *pOffset) = 0;
    virtual void                    GetMaterialScale(float *pScale) = 0;
    virtual C_Material*              GetMaterialPage(void) = 0;
    virtual C_MaterialVar*           FindVar(const char *varName, bool *found, bool complain = true) = 0;
    virtual void                    IncrementReferenceCount(void) = 0;
    virtual void                    DecrementReferenceCount(void) = 0;
    inline void                     AddRef() { IncrementReferenceCount(); }
    inline void                     Release() { DecrementReferenceCount(); }
    virtual int                     GetEnumerationID(void) const = 0;
    virtual void                    GetLowResColorSample(float s, float t, float *color) const = 0;
    virtual void                    RecomputeStateSnapshots() = 0;
    virtual bool                    IsTranslucent() = 0;
    virtual bool                    IsAlphaTested() = 0;
    virtual bool                    IsVertexLit() = 0;
    virtual VertexFormat_t          GetVertexFormat() const = 0;
    virtual bool                    HasProxy(void) const = 0;
    virtual bool                    UsesEnvCubemap(void) = 0;
    virtual bool                    NeedsTangentSpace(void) = 0;
    virtual bool                    NeedsPowerOfTwoFrameBufferTexture(bool bCheckSpecificToThisFrame = true) = 0;
    virtual bool                    NeedsFullFrameBufferTexture(bool bCheckSpecificToThisFrame = true) = 0;
    virtual bool                    NeedsSoftwareSkinning(void) = 0;
    virtual void                    AlphaModulate(float alpha) = 0;
    virtual void                    ColorModulate(float r, float g, float b) = 0;
    virtual void                    SetMaterialVarFlag(MaterialVarFlags_t flag, bool on) = 0;
    virtual bool                    GetMaterialVarFlag(MaterialVarFlags_t flag) const = 0;
    virtual void                    GetReflectivity(Vector& reflect) = 0;
    virtual bool                    GetPropertyFlag(MaterialPropertyTypes_t type) = 0;
    virtual bool                    IsTwoSided() = 0;
    virtual void                    SetShader(const char *pShaderName) = 0;
    virtual int                     GetNumPasses(void) = 0;
    virtual int                     GetTextureMemoryBytes(void) = 0;
    virtual void                    Refresh() = 0;
    virtual bool                    NeedsLightmapBlendAlpha(void) = 0;
    virtual bool                    NeedsSoftwareLighting(void) = 0;
    virtual int                     ShaderParamCount() const = 0;
    virtual C_MaterialVar**          GetShaderParams(void) = 0;
    virtual bool                    IsErrorMaterial() const = 0;
    virtual void                    Unused() = 0;
    virtual float                   GetAlphaModulation() = 0;
    virtual void                    GetColorModulation(float *r, float *g, float *b) = 0;
    virtual bool                    IsTranslucentUnderModulation(float fAlphaModulation = 1.0f) const = 0;
    virtual C_MaterialVar*           FindVarFast(char const *pVarName, unsigned int *pToken) = 0;
    virtual void                    SetShaderAndParams(C_KeyValues *pKeyValues) = 0;
    virtual const char*             GetShaderName() const = 0;
    virtual void                    DeleteIfUnreferenced() = 0;
    virtual bool                    IsSpriteCard() = 0;
    virtual void                    CallBindProxy(void *proxyData) = 0;
    virtual void                    RefreshPreservingMaterialVars() = 0;
    virtual bool                    WasReloadedFromWhitelist() = 0;
    virtual bool                    SetTempExcluded(bool bSet, int nExcludedDimensionLimit) = 0;
    virtual int                     GetReferenceCount() const = 0;
};

class C_ModelRender
{
public:
    virtual int                     DrawModel(int flags, IClientRenderable *pRenderable, ModelInstanceHandle_t instance, int entity_index, const model_t *model, Vector const& origin, QAngle const& angles, int skin, int body, int hitboxset, const matrix3x4_t *modelToWorld = NULL, const matrix3x4_t *pLightingOffset = NULL) = 0;
    virtual void                    ForcedMaterialOverride(C_Material *newMaterial, OverrideType_t nOverrideType = 0, int nOverrides = 0) = 0;
    virtual bool                    IsForcedMaterialOverride(void) = 0;
    virtual void                    SetViewTarget(const C_StudioHDR *pStudioHdr, int nBodyIndex, const Vector& target) = 0;
    virtual ModelInstanceHandle_t   CreateInstance(IClientRenderable *pRenderable, LightCacheHandle_t *pCache = NULL) = 0;
    virtual void                    DestroyInstance(ModelInstanceHandle_t handle) = 0;
    virtual void                    SetStaticLighting(ModelInstanceHandle_t handle, LightCacheHandle_t* pHandle) = 0;
    virtual LightCacheHandle_t      GetStaticLighting(ModelInstanceHandle_t handle) = 0;
    virtual bool                    ChangeInstance(ModelInstanceHandle_t handle, IClientRenderable *pRenderable) = 0;
    virtual void                    AddDecal(ModelInstanceHandle_t handle, Ray_t const& ray, Vector const& decalUp, int decalIndex, int body, bool noPokeThru, int maxLODToDecal) = 0;
    virtual void                    RemoveAllDecals(ModelInstanceHandle_t handle) = 0;
    virtual bool                    ModelHasDecals(ModelInstanceHandle_t handle) = 0;
    virtual void                    RemoveAllDecalsFromAllModels() = 0;
    virtual matrix3x4_t*            DrawModelShadowSetup(IClientRenderable *pRenderable, int body, int skin, DrawModelInfo_t *pInfo, matrix3x4_t *pCustomBoneToWorld = NULL) = 0;
    virtual void                    DrawModelShadow(IClientRenderable *pRenderable, const DrawModelInfo_t &info, matrix3x4_t *pCustomBoneToWorld = NULL) = 0;
    virtual bool                    RecomputeStaticLighting(ModelInstanceHandle_t handle) = 0;
    virtual void                    ReleaseAllStaticPropColorData(void) = 0;
    virtual void                    RestoreAllStaticPropColorData(void) = 0;
    virtual int                     DrawModelEx(ModelRenderInfo_t &pInfo) = 0;
    virtual int                     DrawModelExStaticProp(ModelRenderInfo_t &pInfo) = 0;
    virtual bool                    DrawModelSetup(ModelRenderInfo_t &pInfo, DrawModelState_t *pState, matrix3x4_t **ppBoneToWorldOut) = 0;
    virtual void                    DrawModelExecute(IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld = NULL) = 0;
    virtual void                    SetupLighting(const Vector &vecCenter) = 0;
    virtual int                     DrawStaticPropArrayFast(StaticPropRenderInfo_t *pProps, int count, bool bShadowDepth) = 0;
    virtual void                    SuppressEngineLighting(bool bSuppress) = 0;
    virtual void                    SetupColorMeshes(int nTotalVerts) = 0;
    virtual void                    SetupLightingEx(const Vector &vecCenter, ModelInstanceHandle_t handle) = 0;
    virtual bool                    GetBrightestShadowingLightSource(const Vector &vecCenter, Vector& lightPos, Vector& lightBrightness, bool bAllowNonTaggedLights) = 0;
    virtual void                    ComputeLightingState(int nCount, const LightingQuery_t *pQuery, MaterialLightingState_t *pState, ITexture **ppEnvCubemapTexture) = 0;
    virtual void                    GetModelDecalHandles(StudioDecalHandle_t *pDecals, int nDecalStride, int nCount, const ModelInstanceHandle_t *pHandles) = 0;
    virtual void                    ComputeStaticLightingState(int nCount, const StaticLightingQuery_t *pQuery, MaterialLightingState_t *pState, MaterialLightingState_t *pDecalState, ColorMeshInfo_t **ppStaticLighting, ITexture **ppEnvCubemapTexture, DataCacheHandle_t *pColorMeshHandles) = 0;
    virtual void                    CleanupStaticLightingState(int nCount, DataCacheHandle_t *pColorMeshHandles) = 0;
};

enum LightType_t {
    MATERIAL_LIGHT_DISABLE = 0,
    MATERIAL_LIGHT_POINT,
    MATERIAL_LIGHT_DIRECTIONAL,
    MATERIAL_LIGHT_SPOT,
};

class lightpos_t;
class LightDesc_t
{
public:
    LightType_t m_Type;                 //0x0000
    Vector m_Color;                     //0x0004
    Vector m_Position;                  //0x0010
    Vector m_Direction;                 //0x001C
    float m_Range;                      //0x0028
    float m_Falloff;                    //0x002C
    float m_Attenuation0;               //0x0030
    float m_Attenuation1;               //0x0034
    float m_Attenuation2;               //0x0038
    float m_Theta;                      //0x003C
    float m_Phi;                        //0x0040
    float m_ThetaDot;                   //0x0044
    float m_PhiDot;                     //0x0048
    float m_OneOverThetaDotMinusPhiDot; //0x004C
    __int32 m_Flags;                    //0x0050
    float m_RangeSquared;               //0x0054
};

struct MaterialLightingState_t
{
    Vector m_vecAmbientCube[6]; // ambient, and lights that aren't in locallight[]
    Vector m_vecLightingOrigin; // The position from which lighting state was computed
    int m_nLocalLightCount;
    LightDesc_t m_LightDescription[ 4 ];
};

struct DrawModelInfo_t {
	studiohdr_t* m_pStudioHdr;
	studiohwdata_t* m_pHardwareData;
	StudioDecalHandle_t m_Decals;
	int m_Skin;
	int m_Body;
	int m_HitboxSet;
	IClientRenderable* m_pClientEntity;
	int m_Lod;
	ColorMeshInfo_t* m_pColorMeshes;
	bool m_bStaticLighting;
	MaterialLightingState_t m_LightingState;
};
