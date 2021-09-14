#pragma once

#include "IAppSystem.hpp"
#include "IVModelRender.hpp"
#include "../Utils/Virtual.hpp"

#define DECLARE_POINTER_HANDLE(name) struct name##__ { int unused; }; typedef struct name##__ *name
#define MAXSTUDIOSKINS		32

// These are given to FindMaterial to reference the texture groups that Show up on the 
#define TEXTURE_GROUP_LIGHTMAP						            "Lightmaps"
#define TEXTURE_GROUP_WORLD							            "World textures"
#define TEXTURE_GROUP_MODEL							            "Model textures" 
#define TEXTURE_GROUP_VGUI							            "VGUI textures"
#define TEXTURE_GROUP_PARTICLE						            "Particle textures"
#define TEXTURE_GROUP_DECAL							            "Decal textures"
#define TEXTURE_GROUP_SKYBOX						            "SkyBox textures"
#define TEXTURE_GROUP_CLIENT_EFFECTS				            "ClientEffect textures"
#define TEXTURE_GROUP_OTHER							            "Other textures" 
#define TEXTURE_GROUP_PRECACHED						            "Precached"
#define TEXTURE_GROUP_CUBE_MAP						            "CubeMap textures"
#define TEXTURE_GROUP_RENDER_TARGET					            "RenderTargets"
#define TEXTURE_GROUP_UNACCOUNTED					            "Unaccounted textures"
#define TEXTURE_GROUP_STATIC_PROP                               "StaticProp textures"
#define TEXTURE_GROUP_STATIC_INDEX_BUFFER			            "Static Indices"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_DISP		            "Displacement Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_COLOR	            "Lighting Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_WORLD	            "World Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_MODELS	            "Model Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_OTHER	            "Other Verts"
#define TEXTURE_GROUP_DYNAMIC_INDEX_BUFFER			            "Dynamic Indices"
#define TEXTURE_GROUP_DYNAMIC_VERTEX_BUFFER			            "Dynamic Verts"
#define TEXTURE_GROUP_DEPTH_BUFFER					            "DepthBuffer"
#define TEXTURE_GROUP_VIEW_MODEL					            "ViewModel"
#define TEXTURE_GROUP_PIXEL_SHADERS					            "Pixel Shaders"
#define TEXTURE_GROUP_VERTEX_SHADERS				            "Vertex Shaders"
#define TEXTURE_GROUP_RENDER_TARGET_SURFACE			            "RenderTarget Surfaces"
#define TEXTURE_GROUP_MORPH_TARGETS					            "Morph Targets"

//-----------------------------------------------------------------------------
// forward declarations
//-----------------------------------------------------------------------------
class C_Material;
class IMesh;
class IVertexBuffer;
class IIndexBuffer;
struct MaterialSystem_Config_t;
class VMatrix;
class matrix3x4_t;

class ITexture
{
private:
    template <typename T, typename ... args_t>
    constexpr T CallVFunc(void* thisptr, std::size_t nIndex, args_t... argList)
    {
        using VirtualFn = T(__thiscall*)(void*, decltype(argList)...);
        return (*static_cast<VirtualFn**>(thisptr))[ nIndex ](thisptr, argList...);
    }
private:
    std::byte	pad0[ 0x50 ];		 // 0x0000
public:
    Texture_t** pTextureHandles; // 0x0050

    int GetActualWidth()
    {
        return CallVFunc<int>(this, 3);
    }

    int GetActualHeight()
    {
        return CallVFunc<int>(this, 4);
    }

    void IncrementReferenceCount()
    {
        CallVFunc<void>(this, 10);
    }

    void DecrementReferenceCount()
    {
        CallVFunc<void>(this, 11);
    }
};

struct MaterialSystemHWID_t;
class C_KeyValues;
class IShader;
class IVertexTexture;
class IMorph;
class IMatRenderContext;
class ICallQueue;
struct MorphWeight_t;
class IFileList;
struct VertexStreamSpec_t;
struct ShaderStencilState_t;
struct MeshInstanceData_t;
class IClientMaterialSystem;
class CPaintMaterial;
class IPaintMapDataManager;
class IPaintMapTextureManager;
class GPUMemoryStats;
struct AspectRatioInfo_t;
struct CascadedShadowMappingState_t;

class C_MaterialProxyFactory;
class ITexture;
class C_MaterialSystemHardwareConfig;
class CShadowMgr;

enum CompiledVtfFlags
{
    TEXTUREFLAGS_POINTSAMPLE = 0x00000001,
    TEXTUREFLAGS_TRILINEAR = 0x00000002,
    TEXTUREFLAGS_CLAMPS = 0x00000004,
    TEXTUREFLAGS_CLAMPT = 0x00000008,
    TEXTUREFLAGS_ANISOTROPIC = 0x00000010,
    TEXTUREFLAGS_HINT_DXT5 = 0x00000020,
    TEXTUREFLAGS_PWL_CORRECTED = 0x00000040,
    TEXTUREFLAGS_NORMAL = 0x00000080,
    TEXTUREFLAGS_NOMIP = 0x00000100,
    TEXTUREFLAGS_NOLOD = 0x00000200,
    TEXTUREFLAGS_ALL_MIPS = 0x00000400,
    TEXTUREFLAGS_PROCEDURAL = 0x00000800,
    TEXTUREFLAGS_ONEBITALPHA = 0x00001000,
    TEXTUREFLAGS_EIGHTBITALPHA = 0x00002000,
    TEXTUREFLAGS_ENVMAP = 0x00004000,
    TEXTUREFLAGS_RENDERTARGET = 0x00008000,
    TEXTUREFLAGS_DEPTHRENDERTARGET = 0x00010000,
    TEXTUREFLAGS_NODEBUGOVERRIDE = 0x00020000,
    TEXTUREFLAGS_SINGLECOPY = 0x00040000,
    TEXTUREFLAGS_PRE_SRGB = 0x00080000,
    TEXTUREFLAGS_UNUSED_00100000 = 0x00100000,
    TEXTUREFLAGS_UNUSED_00200000 = 0x00200000,
    TEXTUREFLAGS_UNUSED_00400000 = 0x00400000,
    TEXTUREFLAGS_NODEPTHBUFFER = 0x00800000,
    TEXTUREFLAGS_UNUSED_01000000 = 0x01000000,
    TEXTUREFLAGS_CLAMPU = 0x02000000,
    TEXTUREFLAGS_VERTEXTEXTURE = 0x04000000,
    TEXTUREFLAGS_SSBUMP = 0x08000000,
    TEXTUREFLAGS_UNUSED_10000000 = 0x10000000,
    TEXTUREFLAGS_BORDER = 0x20000000,
    TEXTUREFLAGS_UNUSED_40000000 = 0x40000000,
    TEXTUREFLAGS_UNUSED_80000000 = 0x80000000
};

enum StandardLightmap_t
{
    MATERIAL_SYSTEM_LIGHTMAP_PAGE_WHITE = -1,
    MATERIAL_SYSTEM_LIGHTMAP_PAGE_WHITE_BUMP = -2,
    MATERIAL_SYSTEM_LIGHTMAP_PAGE_USER_DEFINED = -3
};


struct MaterialSystem_SortInfo_t
{
    C_Material	*material;
    int			lightmapPageID;
};

enum MaterialThreadMode_t
{
    MATERIAL_SINGLE_THREADED,
    MATERIAL_QUEUED_SINGLE_THREADED,
    MATERIAL_QUEUED_THREADED
};

enum
{
    MATERIAL_ADAPTER_NAME_LENGTH = 512
};

struct MaterialTextureInfo_t
{
    int iExcludeInformation;
};

struct ApplicationPerformanceCountersInfo_t
{
    float msMain;
    float msMST;
    float msGPU;
    float msFlip;
    float msTotal;
};

struct ApplicationInstantCountersInfo_t
{
    int m_nCpuActivityMask;
    int m_nDeferredWordsAllocated;
};

struct MaterialAdapterInfo_t
{
    char m_pDriverName[MATERIAL_ADAPTER_NAME_LENGTH];
    unsigned int m_VendorID;
    unsigned int m_DeviceID;
    unsigned int m_SubSysID;
    unsigned int m_Revision;
    int m_nDXSupportLevel;			// This is the *preferred* dx support level
    int m_nMinDXSupportLevel;
    int m_nMaxDXSupportLevel;
    unsigned int m_nDriverVersionHigh;
    unsigned int m_nDriverVersionLow;
};

struct MaterialVideoMode_t
{
    int m_Width;			// if width and height are 0 and you select 
    int m_Height;			// windowed mode, it'll use the window size
    ImageFormat m_Format;	// use ImageFormats (ignored for windowed mode)
    int m_RefreshRate;		// 0 == default (ignored for windowed mode)
};

enum HDRType_t
{
    HDR_TYPE_NONE,
    HDR_TYPE_INTEGER,
    HDR_TYPE_FLOAT,
};

enum RestoreChangeFlags_t
{
    MATERIAL_RESTORE_VERTEX_FORMAT_CHANGED = 0x1,
    MATERIAL_RESTORE_RELEASE_MANAGED_RESOURCES = 0x2,
};

enum RenderTargetSizeMode_t
{
    RT_SIZE_NO_CHANGE = 0,
    RT_SIZE_DEFAULT = 1,
    RT_SIZE_PICMIP = 2,
    RT_SIZE_HDR = 3,
    RT_SIZE_FULL_FRAME_BUFFER = 4,
    RT_SIZE_OFFSCREEN = 5,
    RT_SIZE_FULL_FRAME_BUFFER_ROUNDED_UP = 6
};

enum MaterialRenderTargetDepth_t
{
    MATERIAL_RT_DEPTH_SHARED = 0x0,
    MATERIAL_RT_DEPTH_SEPARATE = 0x1,
    MATERIAL_RT_DEPTH_NONE = 0x2,
    MATERIAL_RT_DEPTH_ONLY = 0x3,
};

typedef void(*MaterialBufferReleaseFunc_t)(int nChangeFlags);	// see RestoreChangeFlags_t
typedef void(*MaterialBufferRestoreFunc_t)(int nChangeFlags);	// see RestoreChangeFlags_t
typedef void(*ModeChangeCallbackFunc_t)(void);
typedef void(*EndFrameCleanupFunc_t)(void);
typedef bool(*EndFramePriorToNextContextFunc_t)(void);
typedef void(*OnLevelShutdownFunc_t)(void *pUserData);

typedef unsigned short MaterialHandle_t;
DECLARE_POINTER_HANDLE(MaterialLock_t);

class C_StudioRender
{
private:
    template <typename T, typename ... args_t>
    constexpr T CallVFunc(void* thisptr, std::size_t nIndex, args_t... argList)
    {
        using VirtualFn = T(__thiscall*)(void*, decltype(argList)...);
        return (*static_cast<VirtualFn**>(thisptr))[ nIndex ](thisptr, argList...);
    }
public:
    void SetAmbientLightColors(const Vector4D* pAmbientOnlyColors)
    {
        CallVFunc<void>(this, 20, pAmbientOnlyColors);
    }
    void SetAmbientLightColors(const Vector* pAmbientOnlyColors)
    {
        CallVFunc<void>(this, 21, pAmbientOnlyColors);
    }
    void SetLocalLights(int nLights, const LightDesc_t* pLights)
    {
        CallVFunc<void>(this, 22, nLights, pLights);
    }

    void SetColorModulation(float const* arrColor)
    {
        CallVFunc<void>(this, 27, arrColor);
    }

    void SetAlphaModulation(float flAlpha)
    {
        CallVFunc<void>(this, 28, flAlpha);
    }

    void ForcedMaterialOverride(C_Material* pMaterial, OverrideType_t nOverrideType = 0, int nOverrides = 0)
    {
        CallVFunc<void>(this, 33, pMaterial, nOverrideType, nOverrides);
    }
};

class C_MaterialSystem : public C_AppSystem
{
public:
private:
    template <typename T, typename ... args_t>
    constexpr T CallVFunc(void* thisptr, std::size_t nIndex, args_t... argList)
    {
        using VirtualFn = T(__thiscall*)(void*, decltype(argList)...);
        return (*static_cast<VirtualFn**>(thisptr))[ nIndex ](thisptr, argList...);
    }
public:
    ImageFormat GetBackBufferFormat()
    {
        return CallVFunc<ImageFormat>(this, 36);
    }

    C_Material* CreateMaterial(const char* szName, C_KeyValues* pKeyValues)
    {
        return CallVFunc<C_Material*>(this, 83, szName, pKeyValues);
    }

    C_Material* FindMaterial(char const* szMaterialName, const char* szTextureGroupName = TEXTURE_GROUP_MODEL, bool bComplain = true, const char* pComplainPrefix = nullptr)
    {
        return CallVFunc<C_Material*>(this, 84, szMaterialName, szTextureGroupName, bComplain, pComplainPrefix);
    }

    MaterialHandle_t FirstMaterial()
    {
        return CallVFunc<MaterialHandle_t>(this, 86);
    }

    MaterialHandle_t NextMaterial(MaterialHandle_t hMaterial)
    {
        return CallVFunc<MaterialHandle_t>(this, 87, hMaterial);
    }

    MaterialHandle_t InvalidMaterial()
    {
        return CallVFunc<MaterialHandle_t>(this, 88);
    }

    C_Material* GetMaterial(MaterialHandle_t hMaterial)
    {
        return CallVFunc<C_Material*>(this, 89, hMaterial);
    }

    int	GetNumMaterials()
    {
        return CallVFunc<int>(this, 90);
    }

    ITexture* FindTexture(char const* szTextureName, const char* szTextureGroupName, bool bComplain = true, int nAdditionalCreationFlags = 0)
    {
        return CallVFunc<ITexture*>(this, 91, szTextureName, szTextureGroupName, bComplain, nAdditionalCreationFlags);
    }

    void BeginRenderTargetAllocation()
    {
        CallVFunc<void>(this, 94);
    }

    void EndRenderTargetAllocation()
    {
        CallVFunc<void>(this, 95);
    }

    void ForceBeginRenderTargetAllocation()
    {
        const bool bOldState = DisableRenderTargetAllocationForever();

        DisableRenderTargetAllocationForever() = false;
        BeginRenderTargetAllocation();
        DisableRenderTargetAllocationForever() = bOldState;
    }

    void ForceEndRenderTargetAllocation()
    {
        const bool bOldState = DisableRenderTargetAllocationForever();

        DisableRenderTargetAllocationForever() = false;
        EndRenderTargetAllocation();
        DisableRenderTargetAllocationForever() = bOldState;
    }

    ITexture* CreateNamedRenderTargetTextureEx(const char* szName, int iWidth, int iHeight, RenderTargetSizeMode_t sizeMode, ImageFormat format, MaterialRenderTargetDepth_t depth = MATERIAL_RT_DEPTH_SHARED, unsigned int fTextureFlags = 0U, unsigned int fRenderTargetFlags = CREATERENDERTARGETFLAGS_HDR)
    {
        return CallVFunc<ITexture*>(this, 97, szName, iWidth, iHeight, sizeMode, format, depth, fTextureFlags, fRenderTargetFlags);
    }

    // must be called between the above begin-end calls
    ITexture* CreateNamedRenderTargetTextureEx2(const char* szName, int iWidth, int iHeight, RenderTargetSizeMode_t sizeMode, ImageFormat format, MaterialRenderTargetDepth_t depth = MATERIAL_RT_DEPTH_SHARED, unsigned int fTextureFlags = 0U, unsigned int fRenderTargetFlags = CREATERENDERTARGETFLAGS_HDR)
    {
        return CallVFunc<ITexture*>(this, 99, szName, iWidth, iHeight, sizeMode, format, depth, fTextureFlags, fRenderTargetFlags);
    }

    IMatRenderContext* GetRenderContext()
    {
        return CallVFunc<IMatRenderContext*>(this, 115);
    }

    void FinishRenderTargetAllocation()
    {
        CallVFunc<void>(this, 136);
    }

    // i realize if i call this all textures will be unloaded and load time will suffer horribly
    void ReEnableRenderTargetAllocation()
    {
        CallVFunc<void>(this, 137);
    }

    bool& DisableRenderTargetAllocationForever();
};



class CMatRenderContextPtr : public CRefPtr<IMatRenderContext>
{
    typedef CRefPtr<IMatRenderContext> CBaseClass;
public:
    CMatRenderContextPtr() = default;

    CMatRenderContextPtr(IMatRenderContext* pInit) : CBaseClass(pInit)
    {
        if (CBaseClass::pObject != nullptr)
            CBaseClass::pObject->BeginRender();
    }

    CMatRenderContextPtr(C_MaterialSystem* pFrom) : CBaseClass(pFrom->GetRenderContext())
    {
        if (CBaseClass::pObject != nullptr)
            CBaseClass::pObject->BeginRender();
    }

    ~CMatRenderContextPtr()
    {
        if (CBaseClass::pObject != nullptr)
            CBaseClass::pObject->EndRender();
    }

    IMatRenderContext* operator=(IMatRenderContext* pSecondContext)
    {
        if (pSecondContext != nullptr)
            pSecondContext->BeginRender();

        return CBaseClass::operator=(pSecondContext);
    }

    void SafeRelease()
    {
        if (CBaseClass::pObject != nullptr)
            CBaseClass::pObject->EndRender();

        CBaseClass::SafeRelease();
    }

    void AssignAddReference(IMatRenderContext* pFrom)
    {
        if (CBaseClass::pObject)
            CBaseClass::pObject->EndRender();

        CBaseClass::AssignAddReference(pFrom);
        CBaseClass::pObject->BeginRender();
    }

    void GetFrom(C_MaterialSystem* pFrom)
    {
        AssignAddReference(pFrom->GetRenderContext());
    }

private:
    CMatRenderContextPtr(const CMatRenderContextPtr& pRefPtr);
    void operator=(const CMatRenderContextPtr& pSecondRefPtr);
};