#pragma once

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

enum Hitboxes
{
	HITBOX_HEAD,
	HITBOX_NECK,
	HITBOX_PELVIS,
	HITBOX_STOMACH,
	HITBOX_LOWER_CHEST,
	HITBOX_CHEST,
	HITBOX_UPPER_CHEST,
	HITBOX_RIGHT_THIGH,
	HITBOX_LEFT_THIGH,
	HITBOX_RIGHT_CALF,
	HITBOX_LEFT_CALF,
	HITBOX_RIGHT_FOOT,
	HITBOX_LEFT_FOOT,
	HITBOX_RIGHT_HAND,
	HITBOX_LEFT_HAND,
	HITBOX_RIGHT_UPPER_ARM,
	HITBOX_RIGHT_FOREARM,
	HITBOX_LEFT_UPPER_ARM,
	HITBOX_LEFT_FOREARM,
	HITBOX_MAX
};


typedef unsigned short MDLHandle_t;


struct mstudiobone_t
{
	int                    sznameindex;
	inline char * const    pszName(void) const { return ((char *)this) + sznameindex; }
	int                    parent;
	int                    bonecontroller[6];    // bone controller index, -1 == none
	Vector                 pos;
	Quaternion             quat;
	RadianEuler            rot;
	// compression scale
	Vector                 posscale;
	Vector                 rotscale;

	matrix3x4_t            poseToBone;
	Quaternion             qAlignment;
	int                    flags;
	int                    proctype;
	int                    procindex;
	mutable int            physicsbone;
	int                    surfacepropidx;
	inline char * const    pszSurfaceProp(void) const { return ((char *)this) + surfacepropidx; }
	inline int             GetSurfaceProp(void) const { return surfacepropLookup; }

	int                    contents;
	int                    surfacepropLookup;
	int                    m_iPad01[7];

	mstudiobone_t() {}
private:
	// No copy constructors allowed
	mstudiobone_t(const mstudiobone_t& vOther);
};


struct mstudiobbox_t
{
  	int		m_nBone;
	int		m_nGroup;
	Vector  m_vecBBMin;
	Vector	m_vecBBMax;
	int		m_nHitboxNameIndex;
	Vector	m_vecRotation;
	float	m_flRadius;
	int		pad2[4];

    const char* GetName()
    {
        if(!m_nHitboxNameIndex) return nullptr;
        return (const char*)((uint8_t*)this + m_nHitboxNameIndex);
    }
};

struct mstudiohitboxset_t
{
    int    sznameindex;
    int    numhitboxes;
    int    hitboxindex;

    const char* GetName()
    {
        if(!sznameindex) return nullptr;
        return (const char*)((uint8_t*)this + sznameindex);
    }

    mstudiobbox_t* GetHitbox(int i)
    {
        if(i > numhitboxes) return nullptr;
        return (mstudiobbox_t*)((uint8_t*)this + hitboxindex) + i;
    }
};

struct model_t
{
    void* fnHandle;
    char szName[260];
    __int32 nLoadFlags;
    __int32 nServerCount;
    __int32 type;
    __int32 flags;
    Vector vecMins, vecMaxs;
    float radius;
    void* m_pKeyValues;
    union
    {
        void* brush;
        MDLHandle_t studio;
        void* sprite;
    };
};

class studiohdr_t
{
public:
    __int32 id;                     //0x0000 
    __int32 version;                //0x0004 
    long    checksum;               //0x0008 
    char    szName[64];             //0x000C 
    __int32 length;                 //0x004C 
    Vector  vecEyePos;              //0x0050 
    Vector  vecIllumPos;            //0x005C 
    Vector  vecHullMin;             //0x0068 
    Vector  vecHullMax;             //0x0074 
    Vector  vecBBMin;               //0x0080 
    Vector  vecBBMax;               //0x008C 
    __int32 flags;                  //0x0098 
    __int32 numbones;               //0x009C 
    __int32 boneindex;              //0x00A0 
    __int32 numbonecontrollers;     //0x00A4 
    __int32 bonecontrollerindex;    //0x00A8 
    __int32 numhitboxsets;          //0x00AC 
    __int32 hitboxsetindex;         //0x00B0 
    __int32 numlocalanim;           //0x00B4 
    __int32 localanimindex;         //0x00B8 
    __int32 numlocalseq;            //0x00BC 
    __int32 localseqindex;          //0x00C0 
    __int32 activitylistversion;    //0x00C4 
    __int32 eventsindexed;          //0x00C8 
    __int32 numtextures;            //0x00CC 
    __int32 textureindex;           //0x00D0

    mstudiohitboxset_t* GetHitboxSet(int i)
    {
        if (i > numhitboxsets) return nullptr;
        return (mstudiohitboxset_t*)((uint8_t*)this + hitboxsetindex) + i;
    }
    mstudiobone_t* GetBone(int i)
    {
        if (i > numbones) return nullptr;
        return (mstudiobone_t*)((uint8_t*)this + boneindex) + i;
    }

};//Size=0x00D4