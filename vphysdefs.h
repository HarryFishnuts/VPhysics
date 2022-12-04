
/* ========== <vphysdefs.h>						==========	*/
/* Bailey Jia-Tao Brown							2022		*/
/* Physics macros, defs, structs and types					*/

#ifndef _VPHYS_DEFS_INCLUDE_
#define _VPHYS_DEFS_INCLUDE_

/* ========== INCLUDES							==========	*/
#include "vcore.h"
#include "vgfx.h"
#include "vtypes.h"


/* ========== API DEFINITION					==========	*/
#ifdef VPHYSICS_EXPORTS
#define VPHYSAPI __declspec(dllexport)
#else
#define VPHYSAPI __declspec(dllimport)
#endif


/* ========== DEFINITIONS						==========	*/
#define PHYSICS_OBJECTS_MAX				0x1000
#define FASTMEM_STACK_BYTES				0x1000
#define PHYSOBJECT_LIST_NODE_SIZE		0x200
#define STATICFRICTION_COEFF_DEFAULT	1.55f
#define VPHYS_EPSILON					0.005f
#define PARTITION_CAPACITY_MIN			0x20
#define PARTITION_CAPACITY_STEP			0x40
#define PARTITION_BUFFER_NODE_SIZE		0x80
#define PARTITION_SIZE_DEFAULT			500.0f
#define PARTITION_MINSCALE_MULT			2.25f

#define BOUND_COLORb					255, 128, 32, 255
#define BOUND_LINESIZE					2.0f
#define PARTITION_COLORb				64, 255, 0, 255
#define PARTITION_LINESIZE				1.0f


/* ========== TYPEDEFS							==========	*/
typedef vPosition vVect;
typedef vVect*    vPVect;
typedef float	  vFloat;
typedef vFloat*   vPFloat;
typedef (*vPXPFPHYSICALUPDATEFUNC)(struct vPhysicial* object);
typedef (*vPXPFPHYSICALCOLLISIONFUNC)(struct vPhysical* self,
	struct vPhysical* collideObject);


/* ========== STRUCTURES						==========	*/
typedef struct vPXWorldBoundMesh
{
	vVect mesh[4];		/* world-space bound quad	*/
	vVect center;		/* center vertex			*/
} vPXWorldBoundMesh, *vPPXWorldBoundMesh;

typedef struct vPXMaterial
{
	vFloat drag;
	vFloat staticFriction;
	vFloat dynamicFriction;
	vFloat bounciness;
} vPXMaterial, *vPPXMaterial;

typedef struct vPXProperties
{
	vUI8  collideLayer;	/* collision layer (ranges from 0 - 255) */

	vBOOL isActive		 : 1;		/* whether the object should be updated				*/
	vBOOL isGhost		 : 1;		/* whether the object should collide w/ nothing		*/
	vBOOL staticPosition : 1;		/* whether the object can be moved					*/
	vBOOL staticRotation : 1;		/* whether the object can be rotated				*/
	vBOOL collideWithParent : 1;	/* whether the object collides w/ it's parent		*/
} vPXProperties, *vPPXProperties;

typedef struct vPhysical
{
	/* ===== PHYSICS METADATA				===== */
	vPObject object;
	vPTR physObjectListPtr;			/* ptr to corresponding element in list				*/

	struct vPhysical* parent;		/* parent physics object							*/
	vUI64 age;						/* ticks spent active								*/

	vBOOL renderableTransformOverride;	/* whether to copy phys transform to rtransform */
	vPGRenderable renderableCache;		/* object's renderable component (if exists)	*/

	/* ==== STATIC PHYSICS PROPERTIES		===== */
	vPXProperties properties;	/* simulation properties							*/
	vPXMaterial   material;	/* physical material properties						*/
	
	/* ==== OBJECT PHYSICS PROPERTIES		===== */
	vGRect bound;					/* bounding rectangle			*/
	vTransform transform;			/* physics transform			*/
	vFloat mass;					/* object mass					*/
	vVect  velocity;				/* change in position			*/
	vVect  acceleration;			/* change of change in position	*/

	/* ==== CALCULATION INTERMEDIATE DATA	===== */
	vVect anticipatedPos;			/* position if velocity is applied	*/
	vPXWorldBoundMesh worldBound;	/* bound turned into a quad mesh	*/

	/* ==== OBJECT CALLBACKS				===== */
	vPXPFPHYSICALUPDATEFUNC	   updateFunc;
	vPXPFPHYSICALCOLLISIONFUNC collisionFunc;

} vPhysical, *vPPhysical;

typedef struct vPXPartition
{
	vBOOL inUse;

	vI32  x, y;	 /* partition coordinates	*/
	vUI8  layer; /* partition layer			*/
	
	vPObject* list;	/* "dyanmic" array of all elements  */
	vUI16 capacity;	/* list capacity (can be increased) */
	vUI16 useage;	/* list useage (always <= capacity) */

} vPXPartiton, *vPPXPartition;

typedef struct _vPXInternals
{
	vBOOL  isInitialized;
	vBOOL  debugMode;
	HANDLE debugModeOutput;
	vUI64  debugFlushInterval;
	vUI64  debugLogCount;

	CRITICAL_SECTION lock;			/* physics lock						*/

	vPWorker physicsThread;			/* worker thread object				*/
	vHNDL physObjectList;			/* dynamic list of phys objects		*/

	vUI16 physComponent;	/* physics component handle	*/

	float partitionSize;	/* space partition size			*/
	vHNDL partitions;		/* dbuffer of space partitions	*/

} _vPXInternals, *vPPXInternals;
_vPXInternals _vphys;	/* INSTANCE	*/

#endif
