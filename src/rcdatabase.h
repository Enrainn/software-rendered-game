/*----------------------------------------------------------------------------------------
  Module header file for: rc module
----------------------------------------------------------------------------------------*/
#ifndef RCDATABASE_H

#define RCDATABASE_H

#if defined __cplusplus
extern "C" {
#endif

#ifndef DD_UTIL_H
#include "ddutil.h"
#endif

extern uint8 rcModuleID;
/* Class reference definitions */
#if (defined(DD_DEBUG) && !defined(DD_NOSTRICT)) || defined(DD_STRICT)
typedef struct _struct_rcRoot{char val;} *rcRoot;
#define rcRootNull ((rcRoot)0)
typedef struct _struct_rcScreenVertex{char val;} *rcScreenVertex;
#define rcScreenVertexNull ((rcScreenVertex)0)
typedef struct _struct_rcChunk{char val;} *rcChunk;
#define rcChunkNull ((rcChunk)0)
typedef struct _struct_rcStrip{char val;} *rcStrip;
#define rcStripNull ((rcStrip)0)
typedef struct _struct_rcScreenAABB{char val;} *rcScreenAABB;
#define rcScreenAABBNull ((rcScreenAABB)0)
#else
typedef uint32 rcRoot;
#define rcRootNull 0
typedef uint32 rcScreenVertex;
#define rcScreenVertexNull 0
typedef uint32 rcChunk;
#define rcChunkNull 0
typedef uint32 rcStrip;
#define rcStripNull 0
typedef uint32 rcScreenAABB;
#define rcScreenAABBNull 0
#endif

/* ChunkType enumerated type */
typedef enum {
    RC_CHUNK_COLORED = 0,
    RC_CHUNK_TEXTURED = 1
} rcChunkType;

#include "rctypedef.h"

/* Constructor/Destructor hooks. */
typedef void (*rcRootCallbackType)(rcRoot);
extern rcRootCallbackType rcRootConstructorCallback;
typedef void (*rcScreenVertexCallbackType)(rcScreenVertex);
extern rcScreenVertexCallbackType rcScreenVertexConstructorCallback;
extern rcScreenVertexCallbackType rcScreenVertexDestructorCallback;
typedef void (*rcChunkCallbackType)(rcChunk);
extern rcChunkCallbackType rcChunkConstructorCallback;
typedef void (*rcStripCallbackType)(rcStrip);
extern rcStripCallbackType rcStripConstructorCallback;
extern rcStripCallbackType rcStripDestructorCallback;
typedef void (*rcScreenAABBCallbackType)(rcScreenAABB);
extern rcScreenAABBCallbackType rcScreenAABBConstructorCallback;

/*----------------------------------------------------------------------------------------
  Root structure
----------------------------------------------------------------------------------------*/
struct rcRootType_ {
    uint32 hash; /* This depends only on the structure of the database */
    uint32 usedRoot, allocatedRoot;
    uint32 usedRootChunk, allocatedRootChunk, freeRootChunk;
    uint32 usedRootStrip, allocatedRootStrip, freeRootStrip;
    rcScreenVertex firstFreeScreenVertex;
    uint32 usedScreenVertex, allocatedScreenVertex;
    uint32 usedChunk, allocatedChunk;
    rcStrip firstFreeStrip;
    uint32 usedStrip, allocatedStrip;
    uint32 usedStripChunk, allocatedStripChunk, freeStripChunk;
    uint32 usedScreenAABB, allocatedScreenAABB;
};
extern struct rcRootType_ rcRootData;

utInlineC uint32 rcHash(void) {return rcRootData.hash;}
utInlineC uint32 rcUsedRoot(void) {return rcRootData.usedRoot;}
utInlineC uint32 rcAllocatedRoot(void) {return rcRootData.allocatedRoot;}
utInlineC void rcSetUsedRoot(uint32 value) {rcRootData.usedRoot = value;}
utInlineC void rcSetAllocatedRoot(uint32 value) {rcRootData.allocatedRoot = value;}
utInlineC uint32 rcUsedRootChunk(void) {return rcRootData.usedRootChunk;}
utInlineC uint32 rcAllocatedRootChunk(void) {return rcRootData.allocatedRootChunk;}
utInlineC uint32 rcFreeRootChunk(void) {return rcRootData.freeRootChunk;}
utInlineC void rcSetUsedRootChunk(uint32 value) {rcRootData.usedRootChunk = value;}
utInlineC void rcSetAllocatedRootChunk(uint32 value) {rcRootData.allocatedRootChunk = value;}
utInlineC void rcSetFreeRootChunk(int32 value) {rcRootData.freeRootChunk = value;}
utInlineC uint32 rcUsedRootStrip(void) {return rcRootData.usedRootStrip;}
utInlineC uint32 rcAllocatedRootStrip(void) {return rcRootData.allocatedRootStrip;}
utInlineC uint32 rcFreeRootStrip(void) {return rcRootData.freeRootStrip;}
utInlineC void rcSetUsedRootStrip(uint32 value) {rcRootData.usedRootStrip = value;}
utInlineC void rcSetAllocatedRootStrip(uint32 value) {rcRootData.allocatedRootStrip = value;}
utInlineC void rcSetFreeRootStrip(int32 value) {rcRootData.freeRootStrip = value;}
utInlineC rcScreenVertex rcFirstFreeScreenVertex(void) {return rcRootData.firstFreeScreenVertex;}
utInlineC void rcSetFirstFreeScreenVertex(rcScreenVertex value) {rcRootData.firstFreeScreenVertex = (value);}
utInlineC uint32 rcUsedScreenVertex(void) {return rcRootData.usedScreenVertex;}
utInlineC uint32 rcAllocatedScreenVertex(void) {return rcRootData.allocatedScreenVertex;}
utInlineC void rcSetUsedScreenVertex(uint32 value) {rcRootData.usedScreenVertex = value;}
utInlineC void rcSetAllocatedScreenVertex(uint32 value) {rcRootData.allocatedScreenVertex = value;}
utInlineC uint32 rcUsedChunk(void) {return rcRootData.usedChunk;}
utInlineC uint32 rcAllocatedChunk(void) {return rcRootData.allocatedChunk;}
utInlineC void rcSetUsedChunk(uint32 value) {rcRootData.usedChunk = value;}
utInlineC void rcSetAllocatedChunk(uint32 value) {rcRootData.allocatedChunk = value;}
utInlineC rcStrip rcFirstFreeStrip(void) {return rcRootData.firstFreeStrip;}
utInlineC void rcSetFirstFreeStrip(rcStrip value) {rcRootData.firstFreeStrip = (value);}
utInlineC uint32 rcUsedStrip(void) {return rcRootData.usedStrip;}
utInlineC uint32 rcAllocatedStrip(void) {return rcRootData.allocatedStrip;}
utInlineC void rcSetUsedStrip(uint32 value) {rcRootData.usedStrip = value;}
utInlineC void rcSetAllocatedStrip(uint32 value) {rcRootData.allocatedStrip = value;}
utInlineC uint32 rcUsedStripChunk(void) {return rcRootData.usedStripChunk;}
utInlineC uint32 rcAllocatedStripChunk(void) {return rcRootData.allocatedStripChunk;}
utInlineC uint32 rcFreeStripChunk(void) {return rcRootData.freeStripChunk;}
utInlineC void rcSetUsedStripChunk(uint32 value) {rcRootData.usedStripChunk = value;}
utInlineC void rcSetAllocatedStripChunk(uint32 value) {rcRootData.allocatedStripChunk = value;}
utInlineC void rcSetFreeStripChunk(int32 value) {rcRootData.freeStripChunk = value;}
utInlineC uint32 rcUsedScreenAABB(void) {return rcRootData.usedScreenAABB;}
utInlineC uint32 rcAllocatedScreenAABB(void) {return rcRootData.allocatedScreenAABB;}
utInlineC void rcSetUsedScreenAABB(uint32 value) {rcRootData.usedScreenAABB = value;}
utInlineC void rcSetAllocatedScreenAABB(uint32 value) {rcRootData.allocatedScreenAABB = value;}

/* Validate macros */
#if defined(DD_DEBUG)
utInlineC rcRoot rcValidRoot(rcRoot Root) {
    utAssert(utLikely(Root != rcRootNull && (uint32)(Root - (rcRoot)0) < rcRootData.usedRoot));
    return Root;}
utInlineC rcScreenVertex rcValidScreenVertex(rcScreenVertex ScreenVertex) {
    utAssert(utLikely(ScreenVertex != rcScreenVertexNull && (uint32)(ScreenVertex - (rcScreenVertex)0) < rcRootData.usedScreenVertex));
    return ScreenVertex;}
utInlineC rcChunk rcValidChunk(rcChunk Chunk) {
    utAssert(utLikely(Chunk != rcChunkNull && (uint32)(Chunk - (rcChunk)0) < rcRootData.usedChunk));
    return Chunk;}
utInlineC rcStrip rcValidStrip(rcStrip Strip) {
    utAssert(utLikely(Strip != rcStripNull && (uint32)(Strip - (rcStrip)0) < rcRootData.usedStrip));
    return Strip;}
utInlineC rcScreenAABB rcValidScreenAABB(rcScreenAABB ScreenAABB) {
    utAssert(utLikely(ScreenAABB != rcScreenAABBNull && (uint32)(ScreenAABB - (rcScreenAABB)0) < rcRootData.usedScreenAABB));
    return ScreenAABB;}
#else
utInlineC rcRoot rcValidRoot(rcRoot Root) {return Root;}
utInlineC rcScreenVertex rcValidScreenVertex(rcScreenVertex ScreenVertex) {return ScreenVertex;}
utInlineC rcChunk rcValidChunk(rcChunk Chunk) {return Chunk;}
utInlineC rcStrip rcValidStrip(rcStrip Strip) {return Strip;}
utInlineC rcScreenAABB rcValidScreenAABB(rcScreenAABB ScreenAABB) {return ScreenAABB;}
#endif

/* Object ref to integer conversions */
#if (defined(DD_DEBUG) && !defined(DD_NOSTRICT)) || defined(DD_STRICT)
utInlineC uint32 rcRoot2Index(rcRoot Root) {return Root - (rcRoot)0;}
utInlineC uint32 rcRoot2ValidIndex(rcRoot Root) {return rcValidRoot(Root) - (rcRoot)0;}
utInlineC rcRoot rcIndex2Root(uint32 xRoot) {return (rcRoot)(xRoot + (rcRoot)(0));}
utInlineC uint32 rcScreenVertex2Index(rcScreenVertex ScreenVertex) {return ScreenVertex - (rcScreenVertex)0;}
utInlineC uint32 rcScreenVertex2ValidIndex(rcScreenVertex ScreenVertex) {return rcValidScreenVertex(ScreenVertex) - (rcScreenVertex)0;}
utInlineC rcScreenVertex rcIndex2ScreenVertex(uint32 xScreenVertex) {return (rcScreenVertex)(xScreenVertex + (rcScreenVertex)(0));}
utInlineC uint32 rcChunk2Index(rcChunk Chunk) {return Chunk - (rcChunk)0;}
utInlineC uint32 rcChunk2ValidIndex(rcChunk Chunk) {return rcValidChunk(Chunk) - (rcChunk)0;}
utInlineC rcChunk rcIndex2Chunk(uint32 xChunk) {return (rcChunk)(xChunk + (rcChunk)(0));}
utInlineC uint32 rcStrip2Index(rcStrip Strip) {return Strip - (rcStrip)0;}
utInlineC uint32 rcStrip2ValidIndex(rcStrip Strip) {return rcValidStrip(Strip) - (rcStrip)0;}
utInlineC rcStrip rcIndex2Strip(uint32 xStrip) {return (rcStrip)(xStrip + (rcStrip)(0));}
utInlineC uint32 rcScreenAABB2Index(rcScreenAABB ScreenAABB) {return ScreenAABB - (rcScreenAABB)0;}
utInlineC uint32 rcScreenAABB2ValidIndex(rcScreenAABB ScreenAABB) {return rcValidScreenAABB(ScreenAABB) - (rcScreenAABB)0;}
utInlineC rcScreenAABB rcIndex2ScreenAABB(uint32 xScreenAABB) {return (rcScreenAABB)(xScreenAABB + (rcScreenAABB)(0));}
#else
utInlineC uint32 rcRoot2Index(rcRoot Root) {return Root;}
utInlineC uint32 rcRoot2ValidIndex(rcRoot Root) {return rcValidRoot(Root);}
utInlineC rcRoot rcIndex2Root(uint32 xRoot) {return xRoot;}
utInlineC uint32 rcScreenVertex2Index(rcScreenVertex ScreenVertex) {return ScreenVertex;}
utInlineC uint32 rcScreenVertex2ValidIndex(rcScreenVertex ScreenVertex) {return rcValidScreenVertex(ScreenVertex);}
utInlineC rcScreenVertex rcIndex2ScreenVertex(uint32 xScreenVertex) {return xScreenVertex;}
utInlineC uint32 rcChunk2Index(rcChunk Chunk) {return Chunk;}
utInlineC uint32 rcChunk2ValidIndex(rcChunk Chunk) {return rcValidChunk(Chunk);}
utInlineC rcChunk rcIndex2Chunk(uint32 xChunk) {return xChunk;}
utInlineC uint32 rcStrip2Index(rcStrip Strip) {return Strip;}
utInlineC uint32 rcStrip2ValidIndex(rcStrip Strip) {return rcValidStrip(Strip);}
utInlineC rcStrip rcIndex2Strip(uint32 xStrip) {return xStrip;}
utInlineC uint32 rcScreenAABB2Index(rcScreenAABB ScreenAABB) {return ScreenAABB;}
utInlineC uint32 rcScreenAABB2ValidIndex(rcScreenAABB ScreenAABB) {return rcValidScreenAABB(ScreenAABB);}
utInlineC rcScreenAABB rcIndex2ScreenAABB(uint32 xScreenAABB) {return xScreenAABB;}
#endif

/*----------------------------------------------------------------------------------------
  Fields for class Root.
----------------------------------------------------------------------------------------*/
struct rcRootFields {
    uint32 *Frame_index;
    uint32 *ChunkIndex_;
    uint32 *NumChunk;
    rcChunk *Chunk;
    uint32 *UsedChunk;
    uint32 *StripIndex_;
    uint32 *NumStrip;
    rcStrip *Strip;
    uint32 *UsedStrip;
};
extern struct rcRootFields rcRoots;

void rcRootAllocMore(void);
void rcRootCopyProps(rcRoot rcOldRoot, rcRoot rcNewRoot);
void rcRootAllocChunks(rcRoot Root, uint32 numChunks);
void rcRootResizeChunks(rcRoot Root, uint32 numChunks);
void rcRootFreeChunks(rcRoot Root);
void rcCompactRootChunks(void);
void rcRootAllocStrips(rcRoot Root, uint32 numStrips);
void rcRootResizeStrips(rcRoot Root, uint32 numStrips);
void rcRootFreeStrips(rcRoot Root);
void rcCompactRootStrips(void);
utInlineC uint32 rcRootGetFrame_index(rcRoot Root) {return rcRoots.Frame_index[rcRoot2ValidIndex(Root)];}
utInlineC void rcRootSetFrame_index(rcRoot Root, uint32 value) {rcRoots.Frame_index[rcRoot2ValidIndex(Root)] = value;}
utInlineC uint32 rcRootGetChunkIndex_(rcRoot Root) {return rcRoots.ChunkIndex_[rcRoot2ValidIndex(Root)];}
utInlineC void rcRootSetChunkIndex_(rcRoot Root, uint32 value) {rcRoots.ChunkIndex_[rcRoot2ValidIndex(Root)] = value;}
utInlineC uint32 rcRootGetNumChunk(rcRoot Root) {return rcRoots.NumChunk[rcRoot2ValidIndex(Root)];}
utInlineC void rcRootSetNumChunk(rcRoot Root, uint32 value) {rcRoots.NumChunk[rcRoot2ValidIndex(Root)] = value;}
#if defined(DD_DEBUG)
utInlineC uint32 rcRootCheckChunkIndex(rcRoot Root, uint32 x) {utAssert(x < rcRootGetNumChunk(Root)); return x;}
#else
utInlineC uint32 rcRootCheckChunkIndex(rcRoot Root, uint32 x) {return x;}
#endif
utInlineC rcChunk rcRootGetiChunk(rcRoot Root, uint32 x) {return rcRoots.Chunk[
    rcRootGetChunkIndex_(Root) + rcRootCheckChunkIndex(Root, x)];}
utInlineC rcChunk *rcRootGetChunk(rcRoot Root) {return rcRoots.Chunk + rcRootGetChunkIndex_(Root);}
#define rcRootGetChunks rcRootGetChunk
utInlineC void rcRootSetChunk(rcRoot Root, rcChunk *valuePtr, uint32 numChunk) {
    rcRootResizeChunks(Root, numChunk);
    memcpy(rcRootGetChunks(Root), valuePtr, numChunk*sizeof(rcChunk));}
utInlineC void rcRootSetiChunk(rcRoot Root, uint32 x, rcChunk value) {
    rcRoots.Chunk[rcRootGetChunkIndex_(Root) + rcRootCheckChunkIndex(Root, (x))] = value;}
utInlineC uint32 rcRootGetUsedChunk(rcRoot Root) {return rcRoots.UsedChunk[rcRoot2ValidIndex(Root)];}
utInlineC void rcRootSetUsedChunk(rcRoot Root, uint32 value) {rcRoots.UsedChunk[rcRoot2ValidIndex(Root)] = value;}
utInlineC uint32 rcRootGetStripIndex_(rcRoot Root) {return rcRoots.StripIndex_[rcRoot2ValidIndex(Root)];}
utInlineC void rcRootSetStripIndex_(rcRoot Root, uint32 value) {rcRoots.StripIndex_[rcRoot2ValidIndex(Root)] = value;}
utInlineC uint32 rcRootGetNumStrip(rcRoot Root) {return rcRoots.NumStrip[rcRoot2ValidIndex(Root)];}
utInlineC void rcRootSetNumStrip(rcRoot Root, uint32 value) {rcRoots.NumStrip[rcRoot2ValidIndex(Root)] = value;}
#if defined(DD_DEBUG)
utInlineC uint32 rcRootCheckStripIndex(rcRoot Root, uint32 x) {utAssert(x < rcRootGetNumStrip(Root)); return x;}
#else
utInlineC uint32 rcRootCheckStripIndex(rcRoot Root, uint32 x) {return x;}
#endif
utInlineC rcStrip rcRootGetiStrip(rcRoot Root, uint32 x) {return rcRoots.Strip[
    rcRootGetStripIndex_(Root) + rcRootCheckStripIndex(Root, x)];}
utInlineC rcStrip *rcRootGetStrip(rcRoot Root) {return rcRoots.Strip + rcRootGetStripIndex_(Root);}
#define rcRootGetStrips rcRootGetStrip
utInlineC void rcRootSetStrip(rcRoot Root, rcStrip *valuePtr, uint32 numStrip) {
    rcRootResizeStrips(Root, numStrip);
    memcpy(rcRootGetStrips(Root), valuePtr, numStrip*sizeof(rcStrip));}
utInlineC void rcRootSetiStrip(rcRoot Root, uint32 x, rcStrip value) {
    rcRoots.Strip[rcRootGetStripIndex_(Root) + rcRootCheckStripIndex(Root, (x))] = value;}
utInlineC uint32 rcRootGetUsedStrip(rcRoot Root) {return rcRoots.UsedStrip[rcRoot2ValidIndex(Root)];}
utInlineC void rcRootSetUsedStrip(rcRoot Root, uint32 value) {rcRoots.UsedStrip[rcRoot2ValidIndex(Root)] = value;}
utInlineC void rcRootSetConstructorCallback(void(*func)(rcRoot)) {rcRootConstructorCallback = func;}
utInlineC rcRootCallbackType rcRootGetConstructorCallback(void) {return rcRootConstructorCallback;}
utInlineC rcRoot rcFirstRoot(void) {return rcRootData.usedRoot == 1? rcRootNull : rcIndex2Root(1);}
utInlineC rcRoot rcLastRoot(void) {return rcRootData.usedRoot == 1? rcRootNull :
    rcIndex2Root(rcRootData.usedRoot - 1);}
utInlineC rcRoot rcNextRoot(rcRoot Root) {return rcRoot2ValidIndex(Root) + 1 == rcRootData.usedRoot? rcRootNull :
    Root + 1;}
utInlineC rcRoot rcPrevRoot(rcRoot Root) {return rcRoot2ValidIndex(Root) == 1? rcRootNull : Root - 1;}
#define rcForeachRoot(var) \
    for(var = rcIndex2Root(1); rcRoot2Index(var) != rcRootData.usedRoot; var++)
#define rcEndRoot
utInlineC void rcRootFreeAll(void) {rcSetUsedRoot(1); rcSetUsedRootChunk(0); rcSetUsedRootStrip(0);}
utInlineC rcRoot rcRootAllocRaw(void) {
    rcRoot Root;
    if(rcRootData.usedRoot == rcRootData.allocatedRoot) {
        rcRootAllocMore();
    }
    Root = rcIndex2Root(rcRootData.usedRoot);
    rcSetUsedRoot(rcUsedRoot() + 1);
    return Root;}
utInlineC rcRoot rcRootAlloc(void) {
    rcRoot Root = rcRootAllocRaw();
    rcRootSetFrame_index(Root, 0);
    rcRootSetChunkIndex_(Root, 0);
    rcRootSetNumChunk(Root, 0);
    rcRootSetNumChunk(Root, 0);
    rcRootSetUsedChunk(Root, 0);
    rcRootSetStripIndex_(Root, 0);
    rcRootSetNumStrip(Root, 0);
    rcRootSetNumStrip(Root, 0);
    rcRootSetUsedStrip(Root, 0);
    if(rcRootConstructorCallback != NULL) {
        rcRootConstructorCallback(Root);
    }
    return Root;}

/*----------------------------------------------------------------------------------------
  Fields for class ScreenVertex.
----------------------------------------------------------------------------------------*/
struct rcScreenVertexFields {
    float *X;
    float *Y;
    float *Z;
    float *Inv_w;
    rcScreenVertex *FreeList;
};
extern struct rcScreenVertexFields rcScreenVertexs;

void rcScreenVertexAllocMore(void);
void rcScreenVertexCopyProps(rcScreenVertex rcOldScreenVertex, rcScreenVertex rcNewScreenVertex);
utInlineC float rcScreenVertexGetX(rcScreenVertex ScreenVertex) {return rcScreenVertexs.X[rcScreenVertex2ValidIndex(ScreenVertex)];}
utInlineC void rcScreenVertexSetX(rcScreenVertex ScreenVertex, float value) {rcScreenVertexs.X[rcScreenVertex2ValidIndex(ScreenVertex)] = value;}
utInlineC float rcScreenVertexGetY(rcScreenVertex ScreenVertex) {return rcScreenVertexs.Y[rcScreenVertex2ValidIndex(ScreenVertex)];}
utInlineC void rcScreenVertexSetY(rcScreenVertex ScreenVertex, float value) {rcScreenVertexs.Y[rcScreenVertex2ValidIndex(ScreenVertex)] = value;}
utInlineC float rcScreenVertexGetZ(rcScreenVertex ScreenVertex) {return rcScreenVertexs.Z[rcScreenVertex2ValidIndex(ScreenVertex)];}
utInlineC void rcScreenVertexSetZ(rcScreenVertex ScreenVertex, float value) {rcScreenVertexs.Z[rcScreenVertex2ValidIndex(ScreenVertex)] = value;}
utInlineC float rcScreenVertexGetInv_w(rcScreenVertex ScreenVertex) {return rcScreenVertexs.Inv_w[rcScreenVertex2ValidIndex(ScreenVertex)];}
utInlineC void rcScreenVertexSetInv_w(rcScreenVertex ScreenVertex, float value) {rcScreenVertexs.Inv_w[rcScreenVertex2ValidIndex(ScreenVertex)] = value;}
utInlineC rcScreenVertex rcScreenVertexGetFreeList(rcScreenVertex ScreenVertex) {return rcScreenVertexs.FreeList[rcScreenVertex2ValidIndex(ScreenVertex)];}
utInlineC void rcScreenVertexSetFreeList(rcScreenVertex ScreenVertex, rcScreenVertex value) {rcScreenVertexs.FreeList[rcScreenVertex2ValidIndex(ScreenVertex)] = value;}
utInlineC void rcScreenVertexSetConstructorCallback(void(*func)(rcScreenVertex)) {rcScreenVertexConstructorCallback = func;}
utInlineC rcScreenVertexCallbackType rcScreenVertexGetConstructorCallback(void) {return rcScreenVertexConstructorCallback;}
utInlineC void rcScreenVertexSetDestructorCallback(void(*func)(rcScreenVertex)) {rcScreenVertexDestructorCallback = func;}
utInlineC rcScreenVertexCallbackType rcScreenVertexGetDestructorCallback(void) {return rcScreenVertexDestructorCallback;}
utInlineC rcScreenVertex rcScreenVertexNextFree(rcScreenVertex ScreenVertex) {return ((rcScreenVertex *)(void *)(rcScreenVertexs.FreeList))[rcScreenVertex2ValidIndex(ScreenVertex)];}
utInlineC void rcScreenVertexSetNextFree(rcScreenVertex ScreenVertex, rcScreenVertex value) {
    ((rcScreenVertex *)(void *)(rcScreenVertexs.FreeList))[rcScreenVertex2ValidIndex(ScreenVertex)] = value;}
utInlineC void rcScreenVertexFree(rcScreenVertex ScreenVertex) {
    rcScreenVertexSetNextFree(ScreenVertex, rcRootData.firstFreeScreenVertex);
    rcSetFirstFreeScreenVertex(ScreenVertex);}
void rcScreenVertexDestroy(rcScreenVertex ScreenVertex);
utInlineC rcScreenVertex rcScreenVertexAllocRaw(void) {
    rcScreenVertex ScreenVertex;
    if(rcRootData.firstFreeScreenVertex != rcScreenVertexNull) {
        ScreenVertex = rcRootData.firstFreeScreenVertex;
        rcSetFirstFreeScreenVertex(rcScreenVertexNextFree(ScreenVertex));
    } else {
        if(rcRootData.usedScreenVertex == rcRootData.allocatedScreenVertex) {
            rcScreenVertexAllocMore();
        }
        ScreenVertex = rcIndex2ScreenVertex(rcRootData.usedScreenVertex);
        rcSetUsedScreenVertex(rcUsedScreenVertex() + 1);
    }
    return ScreenVertex;}
utInlineC rcScreenVertex rcScreenVertexAlloc(void) {
    rcScreenVertex ScreenVertex = rcScreenVertexAllocRaw();
    rcScreenVertexSetX(ScreenVertex, 0);
    rcScreenVertexSetY(ScreenVertex, 0);
    rcScreenVertexSetZ(ScreenVertex, 0);
    rcScreenVertexSetInv_w(ScreenVertex, 0);
    rcScreenVertexSetFreeList(ScreenVertex, rcScreenVertexNull);
    if(rcScreenVertexConstructorCallback != NULL) {
        rcScreenVertexConstructorCallback(ScreenVertex);
    }
    return ScreenVertex;}

/*----------------------------------------------------------------------------------------
  Fields for class Chunk.
----------------------------------------------------------------------------------------*/
struct rcChunkFields {
    rcChunkType *Type;
    float *Depth_sort_key;
    float *X0;
    float *Y0;
    float *Z0;
    float *Inv_w0;
    float *X1;
    float *Y1;
    float *Z1;
    float *Inv_w1;
    float *X2;
    float *Y2;
    float *Z2;
    float *Inv_w2;
    uint32 *Color;
    TexturePtr *Texture;
    Vec2 *Uv0;
    Vec2 *Uv1;
    Vec2 *Uv2;
    rcRoot *Root;
    uint32 *RootIndex;
    rcScreenAABB *BoundsScreenAABB;
    rcStrip *Strip;
    uint32 *StripIndex;
};
extern struct rcChunkFields rcChunks;

void rcChunkAllocMore(void);
void rcChunkCopyProps(rcChunk rcOldChunk, rcChunk rcNewChunk);
utInlineC rcChunkType rcChunkGetType(rcChunk Chunk) {return rcChunks.Type[rcChunk2ValidIndex(Chunk)];}
utInlineC void rcChunkSetType(rcChunk Chunk, rcChunkType value) {rcChunks.Type[rcChunk2ValidIndex(Chunk)] = value;}
utInlineC float rcChunkGetDepth_sort_key(rcChunk Chunk) {return rcChunks.Depth_sort_key[rcChunk2ValidIndex(Chunk)];}
utInlineC void rcChunkSetDepth_sort_key(rcChunk Chunk, float value) {rcChunks.Depth_sort_key[rcChunk2ValidIndex(Chunk)] = value;}
utInlineC float rcChunkGetX0(rcChunk Chunk) {return rcChunks.X0[rcChunk2ValidIndex(Chunk)];}
utInlineC void rcChunkSetX0(rcChunk Chunk, float value) {rcChunks.X0[rcChunk2ValidIndex(Chunk)] = value;}
utInlineC float rcChunkGetY0(rcChunk Chunk) {return rcChunks.Y0[rcChunk2ValidIndex(Chunk)];}
utInlineC void rcChunkSetY0(rcChunk Chunk, float value) {rcChunks.Y0[rcChunk2ValidIndex(Chunk)] = value;}
utInlineC float rcChunkGetZ0(rcChunk Chunk) {return rcChunks.Z0[rcChunk2ValidIndex(Chunk)];}
utInlineC void rcChunkSetZ0(rcChunk Chunk, float value) {rcChunks.Z0[rcChunk2ValidIndex(Chunk)] = value;}
utInlineC float rcChunkGetInv_w0(rcChunk Chunk) {return rcChunks.Inv_w0[rcChunk2ValidIndex(Chunk)];}
utInlineC void rcChunkSetInv_w0(rcChunk Chunk, float value) {rcChunks.Inv_w0[rcChunk2ValidIndex(Chunk)] = value;}
utInlineC float rcChunkGetX1(rcChunk Chunk) {return rcChunks.X1[rcChunk2ValidIndex(Chunk)];}
utInlineC void rcChunkSetX1(rcChunk Chunk, float value) {rcChunks.X1[rcChunk2ValidIndex(Chunk)] = value;}
utInlineC float rcChunkGetY1(rcChunk Chunk) {return rcChunks.Y1[rcChunk2ValidIndex(Chunk)];}
utInlineC void rcChunkSetY1(rcChunk Chunk, float value) {rcChunks.Y1[rcChunk2ValidIndex(Chunk)] = value;}
utInlineC float rcChunkGetZ1(rcChunk Chunk) {return rcChunks.Z1[rcChunk2ValidIndex(Chunk)];}
utInlineC void rcChunkSetZ1(rcChunk Chunk, float value) {rcChunks.Z1[rcChunk2ValidIndex(Chunk)] = value;}
utInlineC float rcChunkGetInv_w1(rcChunk Chunk) {return rcChunks.Inv_w1[rcChunk2ValidIndex(Chunk)];}
utInlineC void rcChunkSetInv_w1(rcChunk Chunk, float value) {rcChunks.Inv_w1[rcChunk2ValidIndex(Chunk)] = value;}
utInlineC float rcChunkGetX2(rcChunk Chunk) {return rcChunks.X2[rcChunk2ValidIndex(Chunk)];}
utInlineC void rcChunkSetX2(rcChunk Chunk, float value) {rcChunks.X2[rcChunk2ValidIndex(Chunk)] = value;}
utInlineC float rcChunkGetY2(rcChunk Chunk) {return rcChunks.Y2[rcChunk2ValidIndex(Chunk)];}
utInlineC void rcChunkSetY2(rcChunk Chunk, float value) {rcChunks.Y2[rcChunk2ValidIndex(Chunk)] = value;}
utInlineC float rcChunkGetZ2(rcChunk Chunk) {return rcChunks.Z2[rcChunk2ValidIndex(Chunk)];}
utInlineC void rcChunkSetZ2(rcChunk Chunk, float value) {rcChunks.Z2[rcChunk2ValidIndex(Chunk)] = value;}
utInlineC float rcChunkGetInv_w2(rcChunk Chunk) {return rcChunks.Inv_w2[rcChunk2ValidIndex(Chunk)];}
utInlineC void rcChunkSetInv_w2(rcChunk Chunk, float value) {rcChunks.Inv_w2[rcChunk2ValidIndex(Chunk)] = value;}
utInlineC uint32 rcChunkGetColor(rcChunk Chunk) {return rcChunks.Color[rcChunk2ValidIndex(Chunk)];}
utInlineC void rcChunkSetColor(rcChunk Chunk, uint32 value) {rcChunks.Color[rcChunk2ValidIndex(Chunk)] = value;}
utInlineC TexturePtr rcChunkGetTexture(rcChunk Chunk) {return rcChunks.Texture[rcChunk2ValidIndex(Chunk)];}
utInlineC void rcChunkSetTexture(rcChunk Chunk, TexturePtr value) {rcChunks.Texture[rcChunk2ValidIndex(Chunk)] = value;}
utInlineC Vec2 rcChunkGetUv0(rcChunk Chunk) {return rcChunks.Uv0[rcChunk2ValidIndex(Chunk)];}
utInlineC void rcChunkSetUv0(rcChunk Chunk, Vec2 value) {rcChunks.Uv0[rcChunk2ValidIndex(Chunk)] = value;}
utInlineC Vec2 rcChunkGetUv1(rcChunk Chunk) {return rcChunks.Uv1[rcChunk2ValidIndex(Chunk)];}
utInlineC void rcChunkSetUv1(rcChunk Chunk, Vec2 value) {rcChunks.Uv1[rcChunk2ValidIndex(Chunk)] = value;}
utInlineC Vec2 rcChunkGetUv2(rcChunk Chunk) {return rcChunks.Uv2[rcChunk2ValidIndex(Chunk)];}
utInlineC void rcChunkSetUv2(rcChunk Chunk, Vec2 value) {rcChunks.Uv2[rcChunk2ValidIndex(Chunk)] = value;}
utInlineC rcRoot rcChunkGetRoot(rcChunk Chunk) {return rcChunks.Root[rcChunk2ValidIndex(Chunk)];}
utInlineC void rcChunkSetRoot(rcChunk Chunk, rcRoot value) {rcChunks.Root[rcChunk2ValidIndex(Chunk)] = value;}
utInlineC uint32 rcChunkGetRootIndex(rcChunk Chunk) {return rcChunks.RootIndex[rcChunk2ValidIndex(Chunk)];}
utInlineC void rcChunkSetRootIndex(rcChunk Chunk, uint32 value) {rcChunks.RootIndex[rcChunk2ValidIndex(Chunk)] = value;}
utInlineC rcScreenAABB rcChunkGetBoundsScreenAABB(rcChunk Chunk) {return rcChunks.BoundsScreenAABB[rcChunk2ValidIndex(Chunk)];}
utInlineC void rcChunkSetBoundsScreenAABB(rcChunk Chunk, rcScreenAABB value) {rcChunks.BoundsScreenAABB[rcChunk2ValidIndex(Chunk)] = value;}
utInlineC rcStrip rcChunkGetStrip(rcChunk Chunk) {return rcChunks.Strip[rcChunk2ValidIndex(Chunk)];}
utInlineC void rcChunkSetStrip(rcChunk Chunk, rcStrip value) {rcChunks.Strip[rcChunk2ValidIndex(Chunk)] = value;}
utInlineC uint32 rcChunkGetStripIndex(rcChunk Chunk) {return rcChunks.StripIndex[rcChunk2ValidIndex(Chunk)];}
utInlineC void rcChunkSetStripIndex(rcChunk Chunk, uint32 value) {rcChunks.StripIndex[rcChunk2ValidIndex(Chunk)] = value;}
utInlineC void rcChunkSetConstructorCallback(void(*func)(rcChunk)) {rcChunkConstructorCallback = func;}
utInlineC rcChunkCallbackType rcChunkGetConstructorCallback(void) {return rcChunkConstructorCallback;}
utInlineC rcChunk rcFirstChunk(void) {return rcRootData.usedChunk == 1? rcChunkNull : rcIndex2Chunk(1);}
utInlineC rcChunk rcLastChunk(void) {return rcRootData.usedChunk == 1? rcChunkNull :
    rcIndex2Chunk(rcRootData.usedChunk - 1);}
utInlineC rcChunk rcNextChunk(rcChunk Chunk) {return rcChunk2ValidIndex(Chunk) + 1 == rcRootData.usedChunk? rcChunkNull :
    Chunk + 1;}
utInlineC rcChunk rcPrevChunk(rcChunk Chunk) {return rcChunk2ValidIndex(Chunk) == 1? rcChunkNull : Chunk - 1;}
#define rcForeachChunk(var) \
    for(var = rcIndex2Chunk(1); rcChunk2Index(var) != rcRootData.usedChunk; var++)
#define rcEndChunk
utInlineC void rcChunkFreeAll(void) {rcSetUsedChunk(1);}
utInlineC rcChunk rcChunkAllocRaw(void) {
    rcChunk Chunk;
    if(rcRootData.usedChunk == rcRootData.allocatedChunk) {
        rcChunkAllocMore();
    }
    Chunk = rcIndex2Chunk(rcRootData.usedChunk);
    rcSetUsedChunk(rcUsedChunk() + 1);
    return Chunk;}
utInlineC rcChunk rcChunkAlloc(void) {
    rcChunk Chunk = rcChunkAllocRaw();
    rcChunkSetType(Chunk, RC_CHUNK_COLORED);
    rcChunkSetDepth_sort_key(Chunk, 0);
    rcChunkSetX0(Chunk, 0);
    rcChunkSetY0(Chunk, 0);
    rcChunkSetZ0(Chunk, 0);
    rcChunkSetInv_w0(Chunk, 0);
    rcChunkSetX1(Chunk, 0);
    rcChunkSetY1(Chunk, 0);
    rcChunkSetZ1(Chunk, 0);
    rcChunkSetInv_w1(Chunk, 0);
    rcChunkSetX2(Chunk, 0);
    rcChunkSetY2(Chunk, 0);
    rcChunkSetZ2(Chunk, 0);
    rcChunkSetInv_w2(Chunk, 0);
    rcChunkSetColor(Chunk, 0);
    rcChunkSetTexture(Chunk, NULL);
    rcChunkSetUv0(Chunk, (Vec2){0});
    rcChunkSetUv1(Chunk,  (Vec2){0});
    rcChunkSetUv2(Chunk,  (Vec2){0});
    rcChunkSetRoot(Chunk, rcRootNull);
    rcChunkSetRootIndex(Chunk, UINT32_MAX);
    rcChunkSetBoundsScreenAABB(Chunk, rcScreenAABBNull);
    rcChunkSetStrip(Chunk, rcStripNull);
    rcChunkSetStripIndex(Chunk, UINT32_MAX);
    if(rcChunkConstructorCallback != NULL) {
        rcChunkConstructorCallback(Chunk);
    }
    return Chunk;}

/*----------------------------------------------------------------------------------------
  Fields for class Strip.
----------------------------------------------------------------------------------------*/
struct rcStripFields {
    int32 *X_min;
    int32 *X_max;
    rcRoot *Root;
    uint32 *RootIndex;
    uint32 *ChunkIndex_;
    uint32 *NumChunk;
    rcChunk *Chunk;
    uint32 *UsedChunk;
};
extern struct rcStripFields rcStrips;

void rcStripAllocMore(void);
void rcStripCopyProps(rcStrip rcOldStrip, rcStrip rcNewStrip);
void rcStripAllocChunks(rcStrip Strip, uint32 numChunks);
void rcStripResizeChunks(rcStrip Strip, uint32 numChunks);
void rcStripFreeChunks(rcStrip Strip);
void rcCompactStripChunks(void);
utInlineC int32 rcStripGetX_min(rcStrip Strip) {return rcStrips.X_min[rcStrip2ValidIndex(Strip)];}
utInlineC void rcStripSetX_min(rcStrip Strip, int32 value) {rcStrips.X_min[rcStrip2ValidIndex(Strip)] = value;}
utInlineC int32 rcStripGetX_max(rcStrip Strip) {return rcStrips.X_max[rcStrip2ValidIndex(Strip)];}
utInlineC void rcStripSetX_max(rcStrip Strip, int32 value) {rcStrips.X_max[rcStrip2ValidIndex(Strip)] = value;}
utInlineC rcRoot rcStripGetRoot(rcStrip Strip) {return rcStrips.Root[rcStrip2ValidIndex(Strip)];}
utInlineC void rcStripSetRoot(rcStrip Strip, rcRoot value) {rcStrips.Root[rcStrip2ValidIndex(Strip)] = value;}
utInlineC uint32 rcStripGetRootIndex(rcStrip Strip) {return rcStrips.RootIndex[rcStrip2ValidIndex(Strip)];}
utInlineC void rcStripSetRootIndex(rcStrip Strip, uint32 value) {rcStrips.RootIndex[rcStrip2ValidIndex(Strip)] = value;}
utInlineC uint32 rcStripGetChunkIndex_(rcStrip Strip) {return rcStrips.ChunkIndex_[rcStrip2ValidIndex(Strip)];}
utInlineC void rcStripSetChunkIndex_(rcStrip Strip, uint32 value) {rcStrips.ChunkIndex_[rcStrip2ValidIndex(Strip)] = value;}
utInlineC uint32 rcStripGetNumChunk(rcStrip Strip) {return rcStrips.NumChunk[rcStrip2ValidIndex(Strip)];}
utInlineC void rcStripSetNumChunk(rcStrip Strip, uint32 value) {rcStrips.NumChunk[rcStrip2ValidIndex(Strip)] = value;}
#if defined(DD_DEBUG)
utInlineC uint32 rcStripCheckChunkIndex(rcStrip Strip, uint32 x) {utAssert(x < rcStripGetNumChunk(Strip)); return x;}
#else
utInlineC uint32 rcStripCheckChunkIndex(rcStrip Strip, uint32 x) {return x;}
#endif
utInlineC rcChunk rcStripGetiChunk(rcStrip Strip, uint32 x) {return rcStrips.Chunk[
    rcStripGetChunkIndex_(Strip) + rcStripCheckChunkIndex(Strip, x)];}
utInlineC rcChunk *rcStripGetChunk(rcStrip Strip) {return rcStrips.Chunk + rcStripGetChunkIndex_(Strip);}
#define rcStripGetChunks rcStripGetChunk
utInlineC void rcStripSetChunk(rcStrip Strip, rcChunk *valuePtr, uint32 numChunk) {
    rcStripResizeChunks(Strip, numChunk);
    memcpy(rcStripGetChunks(Strip), valuePtr, numChunk*sizeof(rcChunk));}
utInlineC void rcStripSetiChunk(rcStrip Strip, uint32 x, rcChunk value) {
    rcStrips.Chunk[rcStripGetChunkIndex_(Strip) + rcStripCheckChunkIndex(Strip, (x))] = value;}
utInlineC uint32 rcStripGetUsedChunk(rcStrip Strip) {return rcStrips.UsedChunk[rcStrip2ValidIndex(Strip)];}
utInlineC void rcStripSetUsedChunk(rcStrip Strip, uint32 value) {rcStrips.UsedChunk[rcStrip2ValidIndex(Strip)] = value;}
utInlineC void rcStripSetConstructorCallback(void(*func)(rcStrip)) {rcStripConstructorCallback = func;}
utInlineC rcStripCallbackType rcStripGetConstructorCallback(void) {return rcStripConstructorCallback;}
utInlineC void rcStripSetDestructorCallback(void(*func)(rcStrip)) {rcStripDestructorCallback = func;}
utInlineC rcStripCallbackType rcStripGetDestructorCallback(void) {return rcStripDestructorCallback;}
utInlineC rcStrip rcStripNextFree(rcStrip Strip) {return ((rcStrip *)(void *)(rcStrips.Root))[rcStrip2ValidIndex(Strip)];}
utInlineC void rcStripSetNextFree(rcStrip Strip, rcStrip value) {
    ((rcStrip *)(void *)(rcStrips.Root))[rcStrip2ValidIndex(Strip)] = value;}
utInlineC void rcStripFree(rcStrip Strip) {
    rcStripFreeChunks(Strip);
    rcStripSetNextFree(Strip, rcRootData.firstFreeStrip);
    rcSetFirstFreeStrip(Strip);}
void rcStripDestroy(rcStrip Strip);
utInlineC rcStrip rcStripAllocRaw(void) {
    rcStrip Strip;
    if(rcRootData.firstFreeStrip != rcStripNull) {
        Strip = rcRootData.firstFreeStrip;
        rcSetFirstFreeStrip(rcStripNextFree(Strip));
    } else {
        if(rcRootData.usedStrip == rcRootData.allocatedStrip) {
            rcStripAllocMore();
        }
        Strip = rcIndex2Strip(rcRootData.usedStrip);
        rcSetUsedStrip(rcUsedStrip() + 1);
    }
    return Strip;}
utInlineC rcStrip rcStripAlloc(void) {
    rcStrip Strip = rcStripAllocRaw();
    rcStripSetX_min(Strip, 0);
    rcStripSetX_max(Strip, 0);
    rcStripSetRoot(Strip, rcRootNull);
    rcStripSetRootIndex(Strip, UINT32_MAX);
    rcStripSetChunkIndex_(Strip, 0);
    rcStripSetNumChunk(Strip, 0);
    rcStripSetNumChunk(Strip, 0);
    rcStripSetUsedChunk(Strip, 0);
    if(rcStripConstructorCallback != NULL) {
        rcStripConstructorCallback(Strip);
    }
    return Strip;}

/*----------------------------------------------------------------------------------------
  Fields for class ScreenAABB.
----------------------------------------------------------------------------------------*/
struct rcScreenAABBFields {
    int32 *X_min;
    int32 *X_max;
    int32 *Y_min;
    int32 *Y_max;
    rcChunk *CachedChunk;
};
extern struct rcScreenAABBFields rcScreenAABBs;

void rcScreenAABBAllocMore(void);
void rcScreenAABBCopyProps(rcScreenAABB rcOldScreenAABB, rcScreenAABB rcNewScreenAABB);
utInlineC int32 rcScreenAABBGetX_min(rcScreenAABB ScreenAABB) {return rcScreenAABBs.X_min[rcScreenAABB2ValidIndex(ScreenAABB)];}
utInlineC void rcScreenAABBSetX_min(rcScreenAABB ScreenAABB, int32 value) {rcScreenAABBs.X_min[rcScreenAABB2ValidIndex(ScreenAABB)] = value;}
utInlineC int32 rcScreenAABBGetX_max(rcScreenAABB ScreenAABB) {return rcScreenAABBs.X_max[rcScreenAABB2ValidIndex(ScreenAABB)];}
utInlineC void rcScreenAABBSetX_max(rcScreenAABB ScreenAABB, int32 value) {rcScreenAABBs.X_max[rcScreenAABB2ValidIndex(ScreenAABB)] = value;}
utInlineC int32 rcScreenAABBGetY_min(rcScreenAABB ScreenAABB) {return rcScreenAABBs.Y_min[rcScreenAABB2ValidIndex(ScreenAABB)];}
utInlineC void rcScreenAABBSetY_min(rcScreenAABB ScreenAABB, int32 value) {rcScreenAABBs.Y_min[rcScreenAABB2ValidIndex(ScreenAABB)] = value;}
utInlineC int32 rcScreenAABBGetY_max(rcScreenAABB ScreenAABB) {return rcScreenAABBs.Y_max[rcScreenAABB2ValidIndex(ScreenAABB)];}
utInlineC void rcScreenAABBSetY_max(rcScreenAABB ScreenAABB, int32 value) {rcScreenAABBs.Y_max[rcScreenAABB2ValidIndex(ScreenAABB)] = value;}
utInlineC rcChunk rcScreenAABBGetCachedChunk(rcScreenAABB ScreenAABB) {return rcScreenAABBs.CachedChunk[rcScreenAABB2ValidIndex(ScreenAABB)];}
utInlineC void rcScreenAABBSetCachedChunk(rcScreenAABB ScreenAABB, rcChunk value) {rcScreenAABBs.CachedChunk[rcScreenAABB2ValidIndex(ScreenAABB)] = value;}
utInlineC void rcScreenAABBSetConstructorCallback(void(*func)(rcScreenAABB)) {rcScreenAABBConstructorCallback = func;}
utInlineC rcScreenAABBCallbackType rcScreenAABBGetConstructorCallback(void) {return rcScreenAABBConstructorCallback;}
utInlineC rcScreenAABB rcFirstScreenAABB(void) {return rcRootData.usedScreenAABB == 1? rcScreenAABBNull : rcIndex2ScreenAABB(1);}
utInlineC rcScreenAABB rcLastScreenAABB(void) {return rcRootData.usedScreenAABB == 1? rcScreenAABBNull :
    rcIndex2ScreenAABB(rcRootData.usedScreenAABB - 1);}
utInlineC rcScreenAABB rcNextScreenAABB(rcScreenAABB ScreenAABB) {return rcScreenAABB2ValidIndex(ScreenAABB) + 1 == rcRootData.usedScreenAABB? rcScreenAABBNull :
    ScreenAABB + 1;}
utInlineC rcScreenAABB rcPrevScreenAABB(rcScreenAABB ScreenAABB) {return rcScreenAABB2ValidIndex(ScreenAABB) == 1? rcScreenAABBNull : ScreenAABB - 1;}
#define rcForeachScreenAABB(var) \
    for(var = rcIndex2ScreenAABB(1); rcScreenAABB2Index(var) != rcRootData.usedScreenAABB; var++)
#define rcEndScreenAABB
utInlineC void rcScreenAABBFreeAll(void) {rcSetUsedScreenAABB(1);}
utInlineC rcScreenAABB rcScreenAABBAllocRaw(void) {
    rcScreenAABB ScreenAABB;
    if(rcRootData.usedScreenAABB == rcRootData.allocatedScreenAABB) {
        rcScreenAABBAllocMore();
    }
    ScreenAABB = rcIndex2ScreenAABB(rcRootData.usedScreenAABB);
    rcSetUsedScreenAABB(rcUsedScreenAABB() + 1);
    return ScreenAABB;}
utInlineC rcScreenAABB rcScreenAABBAlloc(void) {
    rcScreenAABB ScreenAABB = rcScreenAABBAllocRaw();
    rcScreenAABBSetX_min(ScreenAABB, 0);
    rcScreenAABBSetX_max(ScreenAABB, 0);
    rcScreenAABBSetY_min(ScreenAABB, 0);
    rcScreenAABBSetY_max(ScreenAABB, 0);
    rcScreenAABBSetCachedChunk(ScreenAABB, rcChunkNull);
    if(rcScreenAABBConstructorCallback != NULL) {
        rcScreenAABBConstructorCallback(ScreenAABB);
    }
    return ScreenAABB;}

/*----------------------------------------------------------------------------------------
  Relationship macros between classes.
----------------------------------------------------------------------------------------*/
#define rcForeachRootChunk(pVar, cVar) { \
    uint32 _xChunk; \
    for(_xChunk = 0; _xChunk < rcRootGetUsedChunk(pVar); _xChunk++) { \
        cVar = rcRootGetiChunk(pVar, _xChunk); \
        if(cVar != rcChunkNull) {
#define rcEndRootChunk }}}
#define rcForeachRootStrip(pVar, cVar) { \
    uint32 _xStrip; \
    for(_xStrip = 0; _xStrip < rcRootGetUsedStrip(pVar); _xStrip++) { \
        cVar = rcRootGetiStrip(pVar, _xStrip); \
        if(cVar != rcStripNull) {
#define rcEndRootStrip }}}
void rcRootInsertChunk(rcRoot Root, uint32 x, rcChunk _Chunk);
void rcRootAppendChunk(rcRoot Root, rcChunk _Chunk);
void rcRootRemoveChunk(rcRoot Root, rcChunk _Chunk);
void rcRootInsertStrip(rcRoot Root, uint32 x, rcStrip _Strip);
void rcRootAppendStrip(rcRoot Root, rcStrip _Strip);
void rcRootRemoveStrip(rcRoot Root, rcStrip _Strip);
utInlineC void rcChunkInsertBoundsScreenAABB(rcChunk Chunk, rcScreenAABB _ScreenAABB) {rcChunkSetBoundsScreenAABB(Chunk, _ScreenAABB); rcScreenAABBSetCachedChunk(_ScreenAABB, Chunk);}
utInlineC void rcChunkRemoveBoundsScreenAABB(rcChunk Chunk, rcScreenAABB _ScreenAABB) {rcChunkSetBoundsScreenAABB(Chunk, rcScreenAABBNull); rcScreenAABBSetCachedChunk(_ScreenAABB, rcChunkNull);}
#define rcForeachStripChunk(pVar, cVar) { \
    uint32 _xChunk; \
    for(_xChunk = 0; _xChunk < rcStripGetUsedChunk(pVar); _xChunk++) { \
        cVar = rcStripGetiChunk(pVar, _xChunk); \
        if(cVar != rcChunkNull) {
#define rcEndStripChunk }}}
void rcStripInsertChunk(rcStrip Strip, uint32 x, rcChunk _Chunk);
void rcStripAppendChunk(rcStrip Strip, rcChunk _Chunk);
void rcStripRemoveChunk(rcStrip Strip, rcChunk _Chunk);
void rcDatabaseStart(void);
void rcDatabaseStop(void);
utInlineC void rcDatabaseSetSaved(bool value) {utModuleSetSaved(utModules + rcModuleID, value);}
#if defined __cplusplus
}
#endif

#endif
