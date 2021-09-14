#pragma once

#include "../Math/QAngle.hpp"
#include "../Utils/Studio.hpp"

class CPhysCollide;
class CUtlBuffer;
class IClientRenderable;
class C_StudioHDR;
class C_Material;
struct virtualmodel_t;

enum RenderableTranslucencyType_t
{
	RENDERABLE_IS_OPAQUE = 0,
	RENDERABLE_IS_TRANSLUCENT,
	RENDERABLE_IS_TWO_PASS,    // has both translucent and opaque sub-partsa
};

struct RenderableInfo_t
{
	IClientRenderable* m_pRenderable;
	void* m_pAlphaProperty;
	int m_EnumCount;
	int m_nRenderFrame;
	unsigned short m_FirstShadow;
	unsigned short m_LeafList;
	short m_Area;
	uint16_t m_Flags;   // 0x0016
	uint16_t m_Flags2; // 0x0018
	Vector m_vecBloatedAbsMins;
	Vector m_vecBloatedAbsMaxs;
	Vector m_vecAbsMins;
	Vector m_vecAbsMaxs;
	int pad;
};

class C_ModelInfoClient
{
public:
	virtual                                 ~C_ModelInfoClient(void) {}
	virtual const model_t*                  GetModel(int modelindex) const = 0;
	virtual int                             GetModelIndex(const char *name) const = 0;
	virtual const char*                     GetModelName(const model_t *model) const = 0;
	virtual void*							GetVCollide(const model_t *model) const = 0;
	virtual void*		                    GetVCollide(int modelindex) const = 0;
	virtual void                            GetModelBounds(const model_t *model, Vector& mins, Vector& maxs) const = 0;
	virtual void                            GetModelRenderBounds(const model_t *model, Vector& mins, Vector& maxs) const = 0;
	virtual int                             GetModelFrameCount(const model_t *model) const = 0;
	virtual int                             GetModelType(const model_t *model) const = 0;
	virtual void*                           GetModelExtraData(const model_t *model) = 0;
	virtual bool                            ModelHasMaterialProxy(const model_t *model) const = 0;
	virtual bool                            IsTranslucent(model_t const* model) const = 0;
	virtual bool                            IsTranslucentTwoPass(const model_t *model) const = 0;
	virtual void                            Unused0() {};
	virtual void							UNUSED() = 0;
	virtual void							UNUSE11D() = 0;
	virtual RenderableTranslucencyType_t    ComputeTranslucencyType(const model_t *model, int nSkin, int nBody) = 0;
	virtual int                             GetModelMaterialCount(const model_t* model) const = 0;
	virtual void                            GetModelMaterials(const model_t *model, int count, C_Material** ppMaterial) = 0;
	virtual bool                            IsModelVertexLit(const model_t *model) const = 0;
	virtual const char*                     GetModelKeyValueText(const model_t *model) = 0;
	virtual bool                            GetModelKeyValue(const model_t *model, CUtlBuffer &buf) = 0;
	virtual float                           GetModelRadius(const model_t *model) = 0;
	virtual C_StudioHDR*                    GetStudioHdr(MDLHandle_t handle) = 0;
	virtual const void*						FindModel(const void *pStudioHdr, void **cache, const char *modelname) const = 0;
	virtual const void*						FindModel(void *cache) const = 0;
	virtual virtualmodel_t*                 GetVirtualModel(const void *pStudioHdr) const = 0;
	virtual uint8_t*                        GetAnimBlock(const void *pStudioHdr, int iBlock) const = 0;
	virtual void                            GetModelMaterialColorAndLighting(const model_t *model, Vector const& origin, QAngle const& angles, trace_t* pTrace, Vector& lighting, Vector& matColor) = 0;
	virtual void                            GetIlluminationPoint(const model_t *model, IClientRenderable *pRenderable, Vector const& origin, QAngle const& angles, Vector* pLightingCenter) = 0;
	virtual int                             GetModelContents(int modelIndex) const = 0;
	virtual studiohdr_t*							GetStudiomodel(const model_t *mod) = 0;
	virtual int                             GetModelSpriteWidth(const model_t *model) const = 0;
	virtual int                             GetModelSpriteHeight(const model_t *model) const = 0;
	virtual void                            SetLevelScreenFadeRange(float flMinSize, float flMaxSize) = 0;
	virtual void                            GetLevelScreenFadeRange(float *pMinArea, float *pMaxArea) const = 0;
	virtual void                            SetViewScreenFadeRange(float flMinSize, float flMaxSize) = 0;
	virtual unsigned char                   ComputeLevelScreenFade(const Vector &vecAbsOrigin, float flRadius, float flFadeScale) const = 0;
	virtual unsigned char                   ComputeViewScreenFade(const Vector &vecAbsOrigin, float flRadius, float flFadeScale) const = 0;
	virtual int                             GetAutoplayList(const void *pStudioHdr, unsigned short **pAutoplayList) const = 0;
	virtual CPhysCollide*                   GetCollideForVirtualTerrain(int index) = 0;
	virtual bool                            IsUsingFBTexture(const model_t *model, int nSkin, int nBody, IClientRenderable* *pClientRenderable) const = 0;
	virtual const model_t*                  FindOrLoadModel(const char *name) const = 0;
	virtual MDLHandle_t                     GetCacheHandle(const model_t *model) const = 0;
	virtual int                             GetBrushModelPlaneCount(const model_t *model) const = 0;
	virtual void                            GetBrushModelPlane(const model_t *model, int nIndex, cplane_t &plane, Vector *pOrigin) const = 0;
	virtual int                             GetSurfacepropsForVirtualTerrain(int index) = 0;
	virtual bool                            UsesEnvCubemap(const model_t *model) const = 0;
	virtual bool                            UsesStaticLighting(const model_t *model) const = 0;
};