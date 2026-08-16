/*----------------------------------------------------------------------------------------
  Database rc
----------------------------------------------------------------------------------------*/

#include "rcdatabase.h"

struct rcRootType_ rcRootData;
uint8 rcModuleID;
struct rcRootFields rcRoots;
struct rcScreenVertexFields rcScreenVertexs;
struct rcChunkFields rcChunks;
struct rcStripFields rcStrips;
struct rcScreenAABBFields rcScreenAABBs;

/*----------------------------------------------------------------------------------------
  Constructor/Destructor hooks.
----------------------------------------------------------------------------------------*/
rcRootCallbackType rcRootConstructorCallback;
rcRootCallbackType rcRootDestructorCallback;
rcScreenVertexCallbackType rcScreenVertexConstructorCallback;
rcScreenVertexCallbackType rcScreenVertexDestructorCallback;
rcChunkCallbackType rcChunkConstructorCallback;
rcChunkCallbackType rcChunkDestructorCallback;
rcStripCallbackType rcStripConstructorCallback;
rcStripCallbackType rcStripDestructorCallback;
rcScreenAABBCallbackType rcScreenAABBConstructorCallback;
rcScreenAABBCallbackType rcScreenAABBDestructorCallback;

/*----------------------------------------------------------------------------------------
  Destroy Root including everything in it. Remove from parents.
----------------------------------------------------------------------------------------*/
void rcRootDestroy(
    rcRoot Root)
{
    rcChunk Chunk_;
    uint32 xChunk;
    rcStrip Strip_;
    uint32 xStrip;

    if(rcRootDestructorCallback != NULL) {
        rcRootDestructorCallback(Root);
    }
    for(xChunk = 0; xChunk < rcRootGetUsedChunk(Root); xChunk++) {
        Chunk_ = rcRootGetiChunk(Root, xChunk);
        if(Chunk_ != rcChunkNull) {
            rcChunkSetRoot(Chunk_, rcRootNull);
        }
    }
    for(xStrip = 0; xStrip < rcRootGetUsedStrip(Root); xStrip++) {
        Strip_ = rcRootGetiStrip(Root, xStrip);
        if(Strip_ != rcStripNull) {
            rcStripSetRoot(Strip_, rcRootNull);
        }
    }
    rcRootFree(Root);
}

/*----------------------------------------------------------------------------------------
  Default constructor wrapper for the database manager.
----------------------------------------------------------------------------------------*/
static uint64 allocRoot(void)
{
    rcRoot Root = rcRootAlloc();

    return rcRoot2Index(Root);
}

/*----------------------------------------------------------------------------------------
  Destructor wrapper for the database manager.
----------------------------------------------------------------------------------------*/
static void destroyRoot(
    uint64 objectIndex)
{
    rcRootDestroy(rcIndex2Root((uint32)objectIndex));
}

/*----------------------------------------------------------------------------------------
  Allocate the field arrays of Root.
----------------------------------------------------------------------------------------*/
static void allocRoots(void)
{
    rcSetAllocatedRoot(2);
    rcSetUsedRoot(1);
    rcSetFirstFreeRoot(rcRootNull);
    rcRoots.Frame_index = utNewAInitFirst(uint32, (rcAllocatedRoot()));
    rcRoots.ChunkIndex_ = utNewAInitFirst(uint32, (rcAllocatedRoot()));
    rcRoots.NumChunk = utNewAInitFirst(uint32, (rcAllocatedRoot()));
    rcSetUsedRootChunk(0);
    rcSetAllocatedRootChunk(2);
    rcSetFreeRootChunk(0);
    rcRoots.Chunk = utNewAInitFirst(rcChunk, rcAllocatedRootChunk());
    rcRoots.UsedChunk = utNewAInitFirst(uint32, (rcAllocatedRoot()));
    rcRoots.StripIndex_ = utNewAInitFirst(uint32, (rcAllocatedRoot()));
    rcRoots.NumStrip = utNewAInitFirst(uint32, (rcAllocatedRoot()));
    rcSetUsedRootStrip(0);
    rcSetAllocatedRootStrip(2);
    rcSetFreeRootStrip(0);
    rcRoots.Strip = utNewAInitFirst(rcStrip, rcAllocatedRootStrip());
    rcRoots.UsedStrip = utNewAInitFirst(uint32, (rcAllocatedRoot()));
    rcRoots.FreeList = utNewAInitFirst(rcRoot, (rcAllocatedRoot()));
}

/*----------------------------------------------------------------------------------------
  Realloc the arrays of properties for class Root.
----------------------------------------------------------------------------------------*/
static void reallocRoots(
    uint32 newSize)
{
    utResizeArray(rcRoots.Frame_index, (newSize));
    utResizeArray(rcRoots.ChunkIndex_, (newSize));
    utResizeArray(rcRoots.NumChunk, (newSize));
    utResizeArray(rcRoots.UsedChunk, (newSize));
    utResizeArray(rcRoots.StripIndex_, (newSize));
    utResizeArray(rcRoots.NumStrip, (newSize));
    utResizeArray(rcRoots.UsedStrip, (newSize));
    utResizeArray(rcRoots.FreeList, (newSize));
    rcSetAllocatedRoot(newSize);
}

/*----------------------------------------------------------------------------------------
  Allocate more Roots.
----------------------------------------------------------------------------------------*/
void rcRootAllocMore(void)
{
    reallocRoots((uint32)(rcAllocatedRoot() + (rcAllocatedRoot() >> 1)));
}

/*----------------------------------------------------------------------------------------
  Compact the Root.Chunk heap to free memory.
----------------------------------------------------------------------------------------*/
void rcCompactRootChunks(void)
{
    uint32 elementSize = sizeof(rcChunk);
    uint32 usedHeaderSize = (sizeof(rcRoot) + elementSize - 1)/elementSize;
    uint32 freeHeaderSize = (sizeof(rcRoot) + sizeof(uint32) + elementSize - 1)/elementSize;
    rcChunk *toPtr = rcRoots.Chunk;
    rcChunk *fromPtr = toPtr;
    rcRoot Root;
    uint32 size;

    while(fromPtr < rcRoots.Chunk + rcUsedRootChunk()) {
        memcpy(&Root, fromPtr, sizeof(rcRoot));
        if(Root != rcRootNull) {
            /* Need to move it to toPtr */
            size = utMax(rcRootGetNumChunk(Root) + usedHeaderSize, freeHeaderSize);
            memmove((void *)toPtr, (void *)fromPtr, size*elementSize);
            rcRootSetChunkIndex_(Root, toPtr - rcRoots.Chunk + usedHeaderSize);
            toPtr += size;
        } else {
            /* Just skip it */
            memcpy(&size, (char *)fromPtr + sizeof(rcRoot), sizeof(uint32));
            size = utMax(size, freeHeaderSize);
        }
        fromPtr += size;
    }
    rcSetUsedRootChunk(toPtr - rcRoots.Chunk);
    rcSetFreeRootChunk(0);
}

/*----------------------------------------------------------------------------------------
  Allocate more memory for the Root.Chunk heap.
----------------------------------------------------------------------------------------*/
static void allocMoreRootChunks(
    uint32 spaceNeeded)
{
    uint32 freeSpace = rcAllocatedRootChunk() - rcUsedRootChunk();

    if((rcFreeRootChunk() << 2) > rcUsedRootChunk()) {
        rcCompactRootChunks();
        freeSpace = rcAllocatedRootChunk() - rcUsedRootChunk();
    }
    if(freeSpace < spaceNeeded) {
        rcSetAllocatedRootChunk(rcAllocatedRootChunk() + spaceNeeded - freeSpace +
            (rcAllocatedRootChunk() >> 1));
        utResizeArray(rcRoots.Chunk, rcAllocatedRootChunk());
    }
}

/*----------------------------------------------------------------------------------------
  Allocate memory for a new Root.Chunk array.
----------------------------------------------------------------------------------------*/
void rcRootAllocChunks(
    rcRoot Root,
    uint32 numChunks)
{
    uint32 freeSpace = rcAllocatedRootChunk() - rcUsedRootChunk();
    uint32 elementSize = sizeof(rcChunk);
    uint32 usedHeaderSize = (sizeof(rcRoot) + elementSize - 1)/elementSize;
    uint32 freeHeaderSize = (sizeof(rcRoot) + sizeof(uint32) + elementSize - 1)/elementSize;
    uint32 spaceNeeded = utMax(numChunks + usedHeaderSize, freeHeaderSize);

#if defined(DD_DEBUG)
    utAssert(rcRootGetNumChunk(Root) == 0);
#endif
    if(numChunks == 0) {
        return;
    }
    if(freeSpace < spaceNeeded) {
        allocMoreRootChunks(spaceNeeded);
    }
    rcRootSetChunkIndex_(Root, rcUsedRootChunk() + usedHeaderSize);
    rcRootSetNumChunk(Root, numChunks);
    memcpy(rcRoots.Chunk + rcUsedRootChunk(), &Root, sizeof(rcRoot));
    {
        uint32 xValue;
        for(xValue = (uint32)(rcRootGetChunkIndex_(Root)); xValue < rcRootGetChunkIndex_(Root) + numChunks; xValue++) {
            rcRoots.Chunk[xValue] = rcChunkNull;
        }
    }
    rcSetUsedRootChunk(rcUsedRootChunk() + spaceNeeded);
}

/*----------------------------------------------------------------------------------------
  Wrapper around rcRootGetChunks for the database manager.
----------------------------------------------------------------------------------------*/
static void *getRootChunks(
    uint64 objectNumber,
    uint32 *numValues)
{
    rcRoot Root = rcIndex2Root((uint32)objectNumber);

    *numValues = rcRootGetNumChunk(Root);
    return rcRootGetChunks(Root);
}

/*----------------------------------------------------------------------------------------
  Wrapper around rcRootAllocChunks for the database manager.
----------------------------------------------------------------------------------------*/
static void *allocRootChunks(
    uint64 objectNumber,
    uint32 numValues)
{
    rcRoot Root = rcIndex2Root((uint32)objectNumber);

    rcRootSetChunkIndex_(Root, 0);
    rcRootSetNumChunk(Root, 0);
    if(numValues == 0) {
        return NULL;
    }
    rcRootAllocChunks(Root, numValues);
    return rcRootGetChunks(Root);
}

/*----------------------------------------------------------------------------------------
  Free memory used by the Root.Chunk array.
----------------------------------------------------------------------------------------*/
void rcRootFreeChunks(
    rcRoot Root)
{
    uint32 elementSize = sizeof(rcChunk);
    uint32 usedHeaderSize = (sizeof(rcRoot) + elementSize - 1)/elementSize;
    uint32 freeHeaderSize = (sizeof(rcRoot) + sizeof(uint32) + elementSize - 1)/elementSize;
    uint32 size = utMax(rcRootGetNumChunk(Root) + usedHeaderSize, freeHeaderSize);
    rcChunk *dataPtr = rcRootGetChunks(Root) - usedHeaderSize;

    if(rcRootGetNumChunk(Root) == 0) {
        return;
    }
    memset(dataPtr, 0, sizeof(rcRoot));
    memcpy((char *)dataPtr + sizeof(rcRoot), &size, sizeof(uint32));
    rcRootSetNumChunk(Root, 0);
    rcSetFreeRootChunk(rcFreeRootChunk() + size);
}

/*----------------------------------------------------------------------------------------
  Resize the Root.Chunk array.
----------------------------------------------------------------------------------------*/
void rcRootResizeChunks(
    rcRoot Root,
    uint32 numChunks)
{
    if (rcRootGetNumChunk(Root) == numChunks) {
      return;
    }
    uint32 freeSpace;
    uint32 elementSize = sizeof(rcChunk);
    uint32 usedHeaderSize = (sizeof(rcRoot) + elementSize - 1)/elementSize;
    uint32 freeHeaderSize = (sizeof(rcRoot) + sizeof(uint32) + elementSize - 1)/elementSize;
    uint32 newSize = utMax(numChunks + usedHeaderSize, freeHeaderSize);
    uint32 oldSize = utMax(rcRootGetNumChunk(Root) + usedHeaderSize, freeHeaderSize);
    rcChunk *dataPtr;

    if(numChunks == 0) {
        if(rcRootGetNumChunk(Root) != 0) {
            rcRootFreeChunks(Root);
        }
        return;
    }
    if(rcRootGetNumChunk(Root) == 0) {
        rcRootAllocChunks(Root, numChunks);
        return;
    }
    freeSpace = rcAllocatedRootChunk() - rcUsedRootChunk();
    if(freeSpace < newSize) {
        allocMoreRootChunks(newSize);
    }
    dataPtr = rcRootGetChunks(Root) - usedHeaderSize;
    memcpy((void *)(rcRoots.Chunk + rcUsedRootChunk()), dataPtr,
        elementSize*utMin(oldSize, newSize));
    if(newSize > oldSize) {
        {
            uint32 xValue;
            for(xValue = (uint32)(rcUsedRootChunk() + oldSize); xValue < rcUsedRootChunk() + oldSize + newSize - oldSize; xValue++) {
                rcRoots.Chunk[xValue] = rcChunkNull;
            }
        }
    }
    memset(dataPtr, 0, sizeof(rcRoot));
    memcpy((char *)dataPtr + sizeof(rcRoot), &oldSize, sizeof(uint32));
    rcSetFreeRootChunk(rcFreeRootChunk() + oldSize);
    rcRootSetChunkIndex_(Root, rcUsedRootChunk() + usedHeaderSize);
    rcRootSetNumChunk(Root, numChunks);
    rcSetUsedRootChunk(rcUsedRootChunk() + newSize);
}

/*----------------------------------------------------------------------------------------
  Compact the Root.Strip heap to free memory.
----------------------------------------------------------------------------------------*/
void rcCompactRootStrips(void)
{
    uint32 elementSize = sizeof(rcStrip);
    uint32 usedHeaderSize = (sizeof(rcRoot) + elementSize - 1)/elementSize;
    uint32 freeHeaderSize = (sizeof(rcRoot) + sizeof(uint32) + elementSize - 1)/elementSize;
    rcStrip *toPtr = rcRoots.Strip;
    rcStrip *fromPtr = toPtr;
    rcRoot Root;
    uint32 size;

    while(fromPtr < rcRoots.Strip + rcUsedRootStrip()) {
        memcpy(&Root, fromPtr, sizeof(rcRoot));
        if(Root != rcRootNull) {
            /* Need to move it to toPtr */
            size = utMax(rcRootGetNumStrip(Root) + usedHeaderSize, freeHeaderSize);
            memmove((void *)toPtr, (void *)fromPtr, size*elementSize);
            rcRootSetStripIndex_(Root, toPtr - rcRoots.Strip + usedHeaderSize);
            toPtr += size;
        } else {
            /* Just skip it */
            memcpy(&size, (char *)fromPtr + sizeof(rcRoot), sizeof(uint32));
            size = utMax(size, freeHeaderSize);
        }
        fromPtr += size;
    }
    rcSetUsedRootStrip(toPtr - rcRoots.Strip);
    rcSetFreeRootStrip(0);
}

/*----------------------------------------------------------------------------------------
  Allocate more memory for the Root.Strip heap.
----------------------------------------------------------------------------------------*/
static void allocMoreRootStrips(
    uint32 spaceNeeded)
{
    uint32 freeSpace = rcAllocatedRootStrip() - rcUsedRootStrip();

    if((rcFreeRootStrip() << 2) > rcUsedRootStrip()) {
        rcCompactRootStrips();
        freeSpace = rcAllocatedRootStrip() - rcUsedRootStrip();
    }
    if(freeSpace < spaceNeeded) {
        rcSetAllocatedRootStrip(rcAllocatedRootStrip() + spaceNeeded - freeSpace +
            (rcAllocatedRootStrip() >> 1));
        utResizeArray(rcRoots.Strip, rcAllocatedRootStrip());
    }
}

/*----------------------------------------------------------------------------------------
  Allocate memory for a new Root.Strip array.
----------------------------------------------------------------------------------------*/
void rcRootAllocStrips(
    rcRoot Root,
    uint32 numStrips)
{
    uint32 freeSpace = rcAllocatedRootStrip() - rcUsedRootStrip();
    uint32 elementSize = sizeof(rcStrip);
    uint32 usedHeaderSize = (sizeof(rcRoot) + elementSize - 1)/elementSize;
    uint32 freeHeaderSize = (sizeof(rcRoot) + sizeof(uint32) + elementSize - 1)/elementSize;
    uint32 spaceNeeded = utMax(numStrips + usedHeaderSize, freeHeaderSize);

#if defined(DD_DEBUG)
    utAssert(rcRootGetNumStrip(Root) == 0);
#endif
    if(numStrips == 0) {
        return;
    }
    if(freeSpace < spaceNeeded) {
        allocMoreRootStrips(spaceNeeded);
    }
    rcRootSetStripIndex_(Root, rcUsedRootStrip() + usedHeaderSize);
    rcRootSetNumStrip(Root, numStrips);
    memcpy(rcRoots.Strip + rcUsedRootStrip(), &Root, sizeof(rcRoot));
    {
        uint32 xValue;
        for(xValue = (uint32)(rcRootGetStripIndex_(Root)); xValue < rcRootGetStripIndex_(Root) + numStrips; xValue++) {
            rcRoots.Strip[xValue] = rcStripNull;
        }
    }
    rcSetUsedRootStrip(rcUsedRootStrip() + spaceNeeded);
}

/*----------------------------------------------------------------------------------------
  Wrapper around rcRootGetStrips for the database manager.
----------------------------------------------------------------------------------------*/
static void *getRootStrips(
    uint64 objectNumber,
    uint32 *numValues)
{
    rcRoot Root = rcIndex2Root((uint32)objectNumber);

    *numValues = rcRootGetNumStrip(Root);
    return rcRootGetStrips(Root);
}

/*----------------------------------------------------------------------------------------
  Wrapper around rcRootAllocStrips for the database manager.
----------------------------------------------------------------------------------------*/
static void *allocRootStrips(
    uint64 objectNumber,
    uint32 numValues)
{
    rcRoot Root = rcIndex2Root((uint32)objectNumber);

    rcRootSetStripIndex_(Root, 0);
    rcRootSetNumStrip(Root, 0);
    if(numValues == 0) {
        return NULL;
    }
    rcRootAllocStrips(Root, numValues);
    return rcRootGetStrips(Root);
}

/*----------------------------------------------------------------------------------------
  Free memory used by the Root.Strip array.
----------------------------------------------------------------------------------------*/
void rcRootFreeStrips(
    rcRoot Root)
{
    uint32 elementSize = sizeof(rcStrip);
    uint32 usedHeaderSize = (sizeof(rcRoot) + elementSize - 1)/elementSize;
    uint32 freeHeaderSize = (sizeof(rcRoot) + sizeof(uint32) + elementSize - 1)/elementSize;
    uint32 size = utMax(rcRootGetNumStrip(Root) + usedHeaderSize, freeHeaderSize);
    rcStrip *dataPtr = rcRootGetStrips(Root) - usedHeaderSize;

    if(rcRootGetNumStrip(Root) == 0) {
        return;
    }
    memset(dataPtr, 0, sizeof(rcRoot));
    memcpy((char *)dataPtr + sizeof(rcRoot), &size, sizeof(uint32));
    rcRootSetNumStrip(Root, 0);
    rcSetFreeRootStrip(rcFreeRootStrip() + size);
}

/*----------------------------------------------------------------------------------------
  Resize the Root.Strip array.
----------------------------------------------------------------------------------------*/
void rcRootResizeStrips(
    rcRoot Root,
    uint32 numStrips)
{
    if (rcRootGetNumStrip(Root) == numStrips) {
      return;
    }
    uint32 freeSpace;
    uint32 elementSize = sizeof(rcStrip);
    uint32 usedHeaderSize = (sizeof(rcRoot) + elementSize - 1)/elementSize;
    uint32 freeHeaderSize = (sizeof(rcRoot) + sizeof(uint32) + elementSize - 1)/elementSize;
    uint32 newSize = utMax(numStrips + usedHeaderSize, freeHeaderSize);
    uint32 oldSize = utMax(rcRootGetNumStrip(Root) + usedHeaderSize, freeHeaderSize);
    rcStrip *dataPtr;

    if(numStrips == 0) {
        if(rcRootGetNumStrip(Root) != 0) {
            rcRootFreeStrips(Root);
        }
        return;
    }
    if(rcRootGetNumStrip(Root) == 0) {
        rcRootAllocStrips(Root, numStrips);
        return;
    }
    freeSpace = rcAllocatedRootStrip() - rcUsedRootStrip();
    if(freeSpace < newSize) {
        allocMoreRootStrips(newSize);
    }
    dataPtr = rcRootGetStrips(Root) - usedHeaderSize;
    memcpy((void *)(rcRoots.Strip + rcUsedRootStrip()), dataPtr,
        elementSize*utMin(oldSize, newSize));
    if(newSize > oldSize) {
        {
            uint32 xValue;
            for(xValue = (uint32)(rcUsedRootStrip() + oldSize); xValue < rcUsedRootStrip() + oldSize + newSize - oldSize; xValue++) {
                rcRoots.Strip[xValue] = rcStripNull;
            }
        }
    }
    memset(dataPtr, 0, sizeof(rcRoot));
    memcpy((char *)dataPtr + sizeof(rcRoot), &oldSize, sizeof(uint32));
    rcSetFreeRootStrip(rcFreeRootStrip() + oldSize);
    rcRootSetStripIndex_(Root, rcUsedRootStrip() + usedHeaderSize);
    rcRootSetNumStrip(Root, numStrips);
    rcSetUsedRootStrip(rcUsedRootStrip() + newSize);
}

/*----------------------------------------------------------------------------------------
  Copy the properties of Root.
----------------------------------------------------------------------------------------*/
void rcRootCopyProps(
    rcRoot oldRoot,
    rcRoot newRoot)
{
    rcRootSetFrame_index(newRoot, rcRootGetFrame_index(oldRoot));
}

/*----------------------------------------------------------------------------------------
  Add the indexed Chunk to the Root.
----------------------------------------------------------------------------------------*/
void rcRootInsertChunk(
    rcRoot Root,
    uint32 x,
    rcChunk _Chunk)
{
#if defined(DD_DEBUG)
    if(Root == rcRootNull) {
        utExit("Non existent Root");
    }
    if(rcChunkGetRoot(_Chunk) != rcRootNull) {
        utExit("Attempting to add Chunk to Root twice");
    }
#endif
    rcRootSetiChunk(Root, x, _Chunk);
    rcRootSetUsedChunk(Root, utMax(rcRootGetUsedChunk(Root), x + 1));
    rcChunkSetRootIndex(_Chunk, x);
    rcChunkSetRoot(_Chunk, Root);
}

/*----------------------------------------------------------------------------------------
  Add the Chunk to the end of the RootChunk array.
----------------------------------------------------------------------------------------*/
void rcRootAppendChunk(
    rcRoot Root,
    rcChunk _Chunk)
{
    uint32 usedChunk = rcRootGetUsedChunk(Root);

#if defined(DD_DEBUG)
    if(Root == rcRootNull) {
        utExit("Non existent Root");
    }
#endif
    if(usedChunk >= rcRootGetNumChunk(Root)) {
        rcRootResizeChunks(Root, usedChunk + (usedChunk << 1) + 1);
    }
    rcRootSetiChunk(Root, usedChunk, _Chunk);
    rcRootSetUsedChunk(Root, usedChunk + 1);
    rcChunkSetRootIndex(_Chunk, usedChunk);
    rcChunkSetRoot(_Chunk, Root);
}

/*----------------------------------------------------------------------------------------
  Remove the Chunk from the Root.
----------------------------------------------------------------------------------------*/
void rcRootRemoveChunk(
    rcRoot Root,
    rcChunk _Chunk)
{
#if defined(DD_DEBUG)
    if(_Chunk == rcChunkNull) {
        utExit("Non-existent Chunk");
    }
    if(rcChunkGetRoot(_Chunk) != rcRootNull && rcChunkGetRoot(_Chunk) != Root) {
        utExit("Delete Chunk from non-owning Root");
    }
#endif
    rcRootSetiChunk(Root, rcChunkGetRootIndex(_Chunk), rcChunkNull);
    rcChunkSetRootIndex(_Chunk, UINT32_MAX);
    rcChunkSetRoot(_Chunk, rcRootNull);
}

/*----------------------------------------------------------------------------------------
  Add the indexed Strip to the Root.
----------------------------------------------------------------------------------------*/
void rcRootInsertStrip(
    rcRoot Root,
    uint32 x,
    rcStrip _Strip)
{
#if defined(DD_DEBUG)
    if(Root == rcRootNull) {
        utExit("Non existent Root");
    }
    if(rcStripGetRoot(_Strip) != rcRootNull) {
        utExit("Attempting to add Strip to Root twice");
    }
#endif
    rcRootSetiStrip(Root, x, _Strip);
    rcRootSetUsedStrip(Root, utMax(rcRootGetUsedStrip(Root), x + 1));
    rcStripSetRootIndex(_Strip, x);
    rcStripSetRoot(_Strip, Root);
}

/*----------------------------------------------------------------------------------------
  Add the Strip to the end of the RootStrip array.
----------------------------------------------------------------------------------------*/
void rcRootAppendStrip(
    rcRoot Root,
    rcStrip _Strip)
{
    uint32 usedStrip = rcRootGetUsedStrip(Root);

#if defined(DD_DEBUG)
    if(Root == rcRootNull) {
        utExit("Non existent Root");
    }
#endif
    if(usedStrip >= rcRootGetNumStrip(Root)) {
        rcRootResizeStrips(Root, usedStrip + (usedStrip << 1) + 1);
    }
    rcRootSetiStrip(Root, usedStrip, _Strip);
    rcRootSetUsedStrip(Root, usedStrip + 1);
    rcStripSetRootIndex(_Strip, usedStrip);
    rcStripSetRoot(_Strip, Root);
}

/*----------------------------------------------------------------------------------------
  Remove the Strip from the Root.
----------------------------------------------------------------------------------------*/
void rcRootRemoveStrip(
    rcRoot Root,
    rcStrip _Strip)
{
#if defined(DD_DEBUG)
    if(_Strip == rcStripNull) {
        utExit("Non-existent Strip");
    }
    if(rcStripGetRoot(_Strip) != rcRootNull && rcStripGetRoot(_Strip) != Root) {
        utExit("Delete Strip from non-owning Root");
    }
#endif
    rcRootSetiStrip(Root, rcStripGetRootIndex(_Strip), rcStripNull);
    rcStripSetRootIndex(_Strip, UINT32_MAX);
    rcStripSetRoot(_Strip, rcRootNull);
}

#if defined(DD_DEBUG)
/*----------------------------------------------------------------------------------------
  Write out all the fields of an object.
----------------------------------------------------------------------------------------*/
void rcShowRoot(
    rcRoot Root)
{
    utDatabaseShowObject("rc", "Root", rcRoot2Index(Root));
}
#endif

/*----------------------------------------------------------------------------------------
  Destroy ScreenVertex including everything in it. Remove from parents.
----------------------------------------------------------------------------------------*/
void rcScreenVertexDestroy(
    rcScreenVertex ScreenVertex)
{
    if(rcScreenVertexDestructorCallback != NULL) {
        rcScreenVertexDestructorCallback(ScreenVertex);
    }
    rcScreenVertexFree(ScreenVertex);
}

/*----------------------------------------------------------------------------------------
  Default constructor wrapper for the database manager.
----------------------------------------------------------------------------------------*/
static uint64 allocScreenVertex(void)
{
    rcScreenVertex ScreenVertex = rcScreenVertexAlloc();

    return rcScreenVertex2Index(ScreenVertex);
}

/*----------------------------------------------------------------------------------------
  Destructor wrapper for the database manager.
----------------------------------------------------------------------------------------*/
static void destroyScreenVertex(
    uint64 objectIndex)
{
    rcScreenVertexDestroy(rcIndex2ScreenVertex((uint32)objectIndex));
}

/*----------------------------------------------------------------------------------------
  Allocate the field arrays of ScreenVertex.
----------------------------------------------------------------------------------------*/
static void allocScreenVertexs(void)
{
    rcSetAllocatedScreenVertex(2);
    rcSetUsedScreenVertex(1);
    rcSetFirstFreeScreenVertex(rcScreenVertexNull);
    rcScreenVertexs.X = utNewAInitFirst(float, (rcAllocatedScreenVertex()));
    rcScreenVertexs.Y = utNewAInitFirst(float, (rcAllocatedScreenVertex()));
    rcScreenVertexs.Z = utNewAInitFirst(float, (rcAllocatedScreenVertex()));
    rcScreenVertexs.Inv_w = utNewAInitFirst(float, (rcAllocatedScreenVertex()));
    rcScreenVertexs.FreeList = utNewAInitFirst(rcScreenVertex, (rcAllocatedScreenVertex()));
}

/*----------------------------------------------------------------------------------------
  Realloc the arrays of properties for class ScreenVertex.
----------------------------------------------------------------------------------------*/
static void reallocScreenVertexs(
    uint32 newSize)
{
    utResizeArray(rcScreenVertexs.X, (newSize));
    utResizeArray(rcScreenVertexs.Y, (newSize));
    utResizeArray(rcScreenVertexs.Z, (newSize));
    utResizeArray(rcScreenVertexs.Inv_w, (newSize));
    utResizeArray(rcScreenVertexs.FreeList, (newSize));
    rcSetAllocatedScreenVertex(newSize);
}

/*----------------------------------------------------------------------------------------
  Allocate more ScreenVertexs.
----------------------------------------------------------------------------------------*/
void rcScreenVertexAllocMore(void)
{
    reallocScreenVertexs((uint32)(rcAllocatedScreenVertex() + (rcAllocatedScreenVertex() >> 1)));
}

/*----------------------------------------------------------------------------------------
  Copy the properties of ScreenVertex.
----------------------------------------------------------------------------------------*/
void rcScreenVertexCopyProps(
    rcScreenVertex oldScreenVertex,
    rcScreenVertex newScreenVertex)
{
    rcScreenVertexSetX(newScreenVertex, rcScreenVertexGetX(oldScreenVertex));
    rcScreenVertexSetY(newScreenVertex, rcScreenVertexGetY(oldScreenVertex));
    rcScreenVertexSetZ(newScreenVertex, rcScreenVertexGetZ(oldScreenVertex));
    rcScreenVertexSetInv_w(newScreenVertex, rcScreenVertexGetInv_w(oldScreenVertex));
}

#if defined(DD_DEBUG)
/*----------------------------------------------------------------------------------------
  Write out all the fields of an object.
----------------------------------------------------------------------------------------*/
void rcShowScreenVertex(
    rcScreenVertex ScreenVertex)
{
    utDatabaseShowObject("rc", "ScreenVertex", rcScreenVertex2Index(ScreenVertex));
}
#endif

/*----------------------------------------------------------------------------------------
  Destroy Chunk including everything in it. Remove from parents.
----------------------------------------------------------------------------------------*/
void rcChunkDestroy(
    rcChunk Chunk)
{
    rcScreenAABB BoundsScreenAABB_;
    rcRoot owningRoot = rcChunkGetRoot(Chunk);
    rcStrip owningStrip = rcChunkGetStrip(Chunk);

    if(rcChunkDestructorCallback != NULL) {
        rcChunkDestructorCallback(Chunk);
    }
    BoundsScreenAABB_ = rcChunkGetBoundsScreenAABB(Chunk);
    if(BoundsScreenAABB_ != rcScreenAABBNull) {
        rcScreenAABBSetCachedChunk(BoundsScreenAABB_, rcChunkNull);
    }
    if(owningRoot != rcRootNull) {
        rcRootRemoveChunk(owningRoot, Chunk);
    }
    if(owningStrip != rcStripNull) {
        rcStripRemoveChunk(owningStrip, Chunk);
    }
    rcChunkFree(Chunk);
}

/*----------------------------------------------------------------------------------------
  Default constructor wrapper for the database manager.
----------------------------------------------------------------------------------------*/
static uint64 allocChunk(void)
{
    rcChunk Chunk = rcChunkAlloc();

    return rcChunk2Index(Chunk);
}

/*----------------------------------------------------------------------------------------
  Destructor wrapper for the database manager.
----------------------------------------------------------------------------------------*/
static void destroyChunk(
    uint64 objectIndex)
{
    rcChunkDestroy(rcIndex2Chunk((uint32)objectIndex));
}

/*----------------------------------------------------------------------------------------
  Allocate the field arrays of Chunk.
----------------------------------------------------------------------------------------*/
static void allocChunks(void)
{
    rcSetAllocatedChunk(2);
    rcSetUsedChunk(1);
    rcSetFirstFreeChunk(rcChunkNull);
    rcChunks.Type = utNewAInitFirst(rcChunkType, (rcAllocatedChunk()));
    rcChunks.Depth_sort_key = utNewAInitFirst(float, (rcAllocatedChunk()));
    rcChunks.X0 = utNewAInitFirst(float, (rcAllocatedChunk()));
    rcChunks.Y0 = utNewAInitFirst(float, (rcAllocatedChunk()));
    rcChunks.Z0 = utNewAInitFirst(float, (rcAllocatedChunk()));
    rcChunks.Inv_w0 = utNewAInitFirst(float, (rcAllocatedChunk()));
    rcChunks.X1 = utNewAInitFirst(float, (rcAllocatedChunk()));
    rcChunks.Y1 = utNewAInitFirst(float, (rcAllocatedChunk()));
    rcChunks.Z1 = utNewAInitFirst(float, (rcAllocatedChunk()));
    rcChunks.Inv_w1 = utNewAInitFirst(float, (rcAllocatedChunk()));
    rcChunks.X2 = utNewAInitFirst(float, (rcAllocatedChunk()));
    rcChunks.Y2 = utNewAInitFirst(float, (rcAllocatedChunk()));
    rcChunks.Z2 = utNewAInitFirst(float, (rcAllocatedChunk()));
    rcChunks.Inv_w2 = utNewAInitFirst(float, (rcAllocatedChunk()));
    rcChunks.Color = utNewAInitFirst(uint32, (rcAllocatedChunk()));
    rcChunks.Texture = utNewAInitFirst(Texture, (rcAllocatedChunk()));
    rcChunks.Uv0 = utNewAInitFirst(Vec2, (rcAllocatedChunk()));
    rcChunks.Uv1 = utNewAInitFirst(Vec2, (rcAllocatedChunk()));
    rcChunks.Uv2 = utNewAInitFirst(Vec2, (rcAllocatedChunk()));
    rcChunks.Root = utNewAInitFirst(rcRoot, (rcAllocatedChunk()));
    rcChunks.RootIndex = utNewAInitFirst(uint32, (rcAllocatedChunk()));
    rcChunks.BoundsScreenAABB = utNewAInitFirst(rcScreenAABB, (rcAllocatedChunk()));
    rcChunks.Strip = utNewAInitFirst(rcStrip, (rcAllocatedChunk()));
    rcChunks.StripIndex = utNewAInitFirst(uint32, (rcAllocatedChunk()));
}

/*----------------------------------------------------------------------------------------
  Realloc the arrays of properties for class Chunk.
----------------------------------------------------------------------------------------*/
static void reallocChunks(
    uint32 newSize)
{
    utResizeArray(rcChunks.Type, (newSize));
    utResizeArray(rcChunks.Depth_sort_key, (newSize));
    utResizeArray(rcChunks.X0, (newSize));
    utResizeArray(rcChunks.Y0, (newSize));
    utResizeArray(rcChunks.Z0, (newSize));
    utResizeArray(rcChunks.Inv_w0, (newSize));
    utResizeArray(rcChunks.X1, (newSize));
    utResizeArray(rcChunks.Y1, (newSize));
    utResizeArray(rcChunks.Z1, (newSize));
    utResizeArray(rcChunks.Inv_w1, (newSize));
    utResizeArray(rcChunks.X2, (newSize));
    utResizeArray(rcChunks.Y2, (newSize));
    utResizeArray(rcChunks.Z2, (newSize));
    utResizeArray(rcChunks.Inv_w2, (newSize));
    utResizeArray(rcChunks.Color, (newSize));
    utResizeArray(rcChunks.Texture, (newSize));
    utResizeArray(rcChunks.Uv0, (newSize));
    utResizeArray(rcChunks.Uv1, (newSize));
    utResizeArray(rcChunks.Uv2, (newSize));
    utResizeArray(rcChunks.Root, (newSize));
    utResizeArray(rcChunks.RootIndex, (newSize));
    utResizeArray(rcChunks.BoundsScreenAABB, (newSize));
    utResizeArray(rcChunks.Strip, (newSize));
    utResizeArray(rcChunks.StripIndex, (newSize));
    rcSetAllocatedChunk(newSize);
}

/*----------------------------------------------------------------------------------------
  Allocate more Chunks.
----------------------------------------------------------------------------------------*/
void rcChunkAllocMore(void)
{
    reallocChunks((uint32)(rcAllocatedChunk() + (rcAllocatedChunk() >> 1)));
}

/*----------------------------------------------------------------------------------------
  Copy the properties of Chunk.
----------------------------------------------------------------------------------------*/
void rcChunkCopyProps(
    rcChunk oldChunk,
    rcChunk newChunk)
{
    rcChunkSetType(newChunk, rcChunkGetType(oldChunk));
    rcChunkSetDepth_sort_key(newChunk, rcChunkGetDepth_sort_key(oldChunk));
    rcChunkSetX0(newChunk, rcChunkGetX0(oldChunk));
    rcChunkSetY0(newChunk, rcChunkGetY0(oldChunk));
    rcChunkSetZ0(newChunk, rcChunkGetZ0(oldChunk));
    rcChunkSetInv_w0(newChunk, rcChunkGetInv_w0(oldChunk));
    rcChunkSetX1(newChunk, rcChunkGetX1(oldChunk));
    rcChunkSetY1(newChunk, rcChunkGetY1(oldChunk));
    rcChunkSetZ1(newChunk, rcChunkGetZ1(oldChunk));
    rcChunkSetInv_w1(newChunk, rcChunkGetInv_w1(oldChunk));
    rcChunkSetX2(newChunk, rcChunkGetX2(oldChunk));
    rcChunkSetY2(newChunk, rcChunkGetY2(oldChunk));
    rcChunkSetZ2(newChunk, rcChunkGetZ2(oldChunk));
    rcChunkSetInv_w2(newChunk, rcChunkGetInv_w2(oldChunk));
    rcChunkSetColor(newChunk, rcChunkGetColor(oldChunk));
    rcChunkSetTexture(newChunk, rcChunkGetTexture(oldChunk));
    rcChunkSetUv0(newChunk, rcChunkGetUv0(oldChunk));
    rcChunkSetUv1(newChunk, rcChunkGetUv1(oldChunk));
    rcChunkSetUv2(newChunk, rcChunkGetUv2(oldChunk));
}

#if defined(DD_DEBUG)
/*----------------------------------------------------------------------------------------
  Write out all the fields of an object.
----------------------------------------------------------------------------------------*/
void rcShowChunk(
    rcChunk Chunk)
{
    utDatabaseShowObject("rc", "Chunk", rcChunk2Index(Chunk));
}
#endif

/*----------------------------------------------------------------------------------------
  Destroy Strip including everything in it. Remove from parents.
----------------------------------------------------------------------------------------*/
void rcStripDestroy(
    rcStrip Strip)
{
    rcChunk Chunk_;
    uint32 xChunk;
    rcRoot owningRoot = rcStripGetRoot(Strip);

    if(rcStripDestructorCallback != NULL) {
        rcStripDestructorCallback(Strip);
    }
    for(xChunk = 0; xChunk < rcStripGetUsedChunk(Strip); xChunk++) {
        Chunk_ = rcStripGetiChunk(Strip, xChunk);
        if(Chunk_ != rcChunkNull) {
            rcChunkSetStrip(Chunk_, rcStripNull);
        }
    }
    if(owningRoot != rcRootNull) {
        rcRootRemoveStrip(owningRoot, Strip);
    }
    rcStripFree(Strip);
}

/*----------------------------------------------------------------------------------------
  Default constructor wrapper for the database manager.
----------------------------------------------------------------------------------------*/
static uint64 allocStrip(void)
{
    rcStrip Strip = rcStripAlloc();

    return rcStrip2Index(Strip);
}

/*----------------------------------------------------------------------------------------
  Destructor wrapper for the database manager.
----------------------------------------------------------------------------------------*/
static void destroyStrip(
    uint64 objectIndex)
{
    rcStripDestroy(rcIndex2Strip((uint32)objectIndex));
}

/*----------------------------------------------------------------------------------------
  Allocate the field arrays of Strip.
----------------------------------------------------------------------------------------*/
static void allocStrips(void)
{
    rcSetAllocatedStrip(2);
    rcSetUsedStrip(1);
    rcSetFirstFreeStrip(rcStripNull);
    rcStrips.X_min = utNewAInitFirst(int32, (rcAllocatedStrip()));
    rcStrips.X_max = utNewAInitFirst(int32, (rcAllocatedStrip()));
    rcStrips.Root = utNewAInitFirst(rcRoot, (rcAllocatedStrip()));
    rcStrips.RootIndex = utNewAInitFirst(uint32, (rcAllocatedStrip()));
    rcStrips.ChunkIndex_ = utNewAInitFirst(uint32, (rcAllocatedStrip()));
    rcStrips.NumChunk = utNewAInitFirst(uint32, (rcAllocatedStrip()));
    rcSetUsedStripChunk(0);
    rcSetAllocatedStripChunk(2);
    rcSetFreeStripChunk(0);
    rcStrips.Chunk = utNewAInitFirst(rcChunk, rcAllocatedStripChunk());
    rcStrips.UsedChunk = utNewAInitFirst(uint32, (rcAllocatedStrip()));
}

/*----------------------------------------------------------------------------------------
  Realloc the arrays of properties for class Strip.
----------------------------------------------------------------------------------------*/
static void reallocStrips(
    uint32 newSize)
{
    utResizeArray(rcStrips.X_min, (newSize));
    utResizeArray(rcStrips.X_max, (newSize));
    utResizeArray(rcStrips.Root, (newSize));
    utResizeArray(rcStrips.RootIndex, (newSize));
    utResizeArray(rcStrips.ChunkIndex_, (newSize));
    utResizeArray(rcStrips.NumChunk, (newSize));
    utResizeArray(rcStrips.UsedChunk, (newSize));
    rcSetAllocatedStrip(newSize);
}

/*----------------------------------------------------------------------------------------
  Allocate more Strips.
----------------------------------------------------------------------------------------*/
void rcStripAllocMore(void)
{
    reallocStrips((uint32)(rcAllocatedStrip() + (rcAllocatedStrip() >> 1)));
}

/*----------------------------------------------------------------------------------------
  Compact the Strip.Chunk heap to free memory.
----------------------------------------------------------------------------------------*/
void rcCompactStripChunks(void)
{
    uint32 elementSize = sizeof(rcChunk);
    uint32 usedHeaderSize = (sizeof(rcStrip) + elementSize - 1)/elementSize;
    uint32 freeHeaderSize = (sizeof(rcStrip) + sizeof(uint32) + elementSize - 1)/elementSize;
    rcChunk *toPtr = rcStrips.Chunk;
    rcChunk *fromPtr = toPtr;
    rcStrip Strip;
    uint32 size;

    while(fromPtr < rcStrips.Chunk + rcUsedStripChunk()) {
        memcpy(&Strip, fromPtr, sizeof(rcStrip));
        if(Strip != rcStripNull) {
            /* Need to move it to toPtr */
            size = utMax(rcStripGetNumChunk(Strip) + usedHeaderSize, freeHeaderSize);
            memmove((void *)toPtr, (void *)fromPtr, size*elementSize);
            rcStripSetChunkIndex_(Strip, toPtr - rcStrips.Chunk + usedHeaderSize);
            toPtr += size;
        } else {
            /* Just skip it */
            memcpy(&size, (char *)fromPtr + sizeof(rcStrip), sizeof(uint32));
            size = utMax(size, freeHeaderSize);
        }
        fromPtr += size;
    }
    rcSetUsedStripChunk(toPtr - rcStrips.Chunk);
    rcSetFreeStripChunk(0);
}

/*----------------------------------------------------------------------------------------
  Allocate more memory for the Strip.Chunk heap.
----------------------------------------------------------------------------------------*/
static void allocMoreStripChunks(
    uint32 spaceNeeded)
{
    uint32 freeSpace = rcAllocatedStripChunk() - rcUsedStripChunk();

    if((rcFreeStripChunk() << 2) > rcUsedStripChunk()) {
        rcCompactStripChunks();
        freeSpace = rcAllocatedStripChunk() - rcUsedStripChunk();
    }
    if(freeSpace < spaceNeeded) {
        rcSetAllocatedStripChunk(rcAllocatedStripChunk() + spaceNeeded - freeSpace +
            (rcAllocatedStripChunk() >> 1));
        utResizeArray(rcStrips.Chunk, rcAllocatedStripChunk());
    }
}

/*----------------------------------------------------------------------------------------
  Allocate memory for a new Strip.Chunk array.
----------------------------------------------------------------------------------------*/
void rcStripAllocChunks(
    rcStrip Strip,
    uint32 numChunks)
{
    uint32 freeSpace = rcAllocatedStripChunk() - rcUsedStripChunk();
    uint32 elementSize = sizeof(rcChunk);
    uint32 usedHeaderSize = (sizeof(rcStrip) + elementSize - 1)/elementSize;
    uint32 freeHeaderSize = (sizeof(rcStrip) + sizeof(uint32) + elementSize - 1)/elementSize;
    uint32 spaceNeeded = utMax(numChunks + usedHeaderSize, freeHeaderSize);

#if defined(DD_DEBUG)
    utAssert(rcStripGetNumChunk(Strip) == 0);
#endif
    if(numChunks == 0) {
        return;
    }
    if(freeSpace < spaceNeeded) {
        allocMoreStripChunks(spaceNeeded);
    }
    rcStripSetChunkIndex_(Strip, rcUsedStripChunk() + usedHeaderSize);
    rcStripSetNumChunk(Strip, numChunks);
    memcpy(rcStrips.Chunk + rcUsedStripChunk(), &Strip, sizeof(rcStrip));
    {
        uint32 xValue;
        for(xValue = (uint32)(rcStripGetChunkIndex_(Strip)); xValue < rcStripGetChunkIndex_(Strip) + numChunks; xValue++) {
            rcStrips.Chunk[xValue] = rcChunkNull;
        }
    }
    rcSetUsedStripChunk(rcUsedStripChunk() + spaceNeeded);
}

/*----------------------------------------------------------------------------------------
  Wrapper around rcStripGetChunks for the database manager.
----------------------------------------------------------------------------------------*/
static void *getStripChunks(
    uint64 objectNumber,
    uint32 *numValues)
{
    rcStrip Strip = rcIndex2Strip((uint32)objectNumber);

    *numValues = rcStripGetNumChunk(Strip);
    return rcStripGetChunks(Strip);
}

/*----------------------------------------------------------------------------------------
  Wrapper around rcStripAllocChunks for the database manager.
----------------------------------------------------------------------------------------*/
static void *allocStripChunks(
    uint64 objectNumber,
    uint32 numValues)
{
    rcStrip Strip = rcIndex2Strip((uint32)objectNumber);

    rcStripSetChunkIndex_(Strip, 0);
    rcStripSetNumChunk(Strip, 0);
    if(numValues == 0) {
        return NULL;
    }
    rcStripAllocChunks(Strip, numValues);
    return rcStripGetChunks(Strip);
}

/*----------------------------------------------------------------------------------------
  Free memory used by the Strip.Chunk array.
----------------------------------------------------------------------------------------*/
void rcStripFreeChunks(
    rcStrip Strip)
{
    uint32 elementSize = sizeof(rcChunk);
    uint32 usedHeaderSize = (sizeof(rcStrip) + elementSize - 1)/elementSize;
    uint32 freeHeaderSize = (sizeof(rcStrip) + sizeof(uint32) + elementSize - 1)/elementSize;
    uint32 size = utMax(rcStripGetNumChunk(Strip) + usedHeaderSize, freeHeaderSize);
    rcChunk *dataPtr = rcStripGetChunks(Strip) - usedHeaderSize;

    if(rcStripGetNumChunk(Strip) == 0) {
        return;
    }
    memset(dataPtr, 0, sizeof(rcStrip));
    memcpy((char *)dataPtr + sizeof(rcStrip), &size, sizeof(uint32));
    rcStripSetNumChunk(Strip, 0);
    rcSetFreeStripChunk(rcFreeStripChunk() + size);
}

/*----------------------------------------------------------------------------------------
  Resize the Strip.Chunk array.
----------------------------------------------------------------------------------------*/
void rcStripResizeChunks(
    rcStrip Strip,
    uint32 numChunks)
{
    if (rcStripGetNumChunk(Strip) == numChunks) {
      return;
    }
    uint32 freeSpace;
    uint32 elementSize = sizeof(rcChunk);
    uint32 usedHeaderSize = (sizeof(rcStrip) + elementSize - 1)/elementSize;
    uint32 freeHeaderSize = (sizeof(rcStrip) + sizeof(uint32) + elementSize - 1)/elementSize;
    uint32 newSize = utMax(numChunks + usedHeaderSize, freeHeaderSize);
    uint32 oldSize = utMax(rcStripGetNumChunk(Strip) + usedHeaderSize, freeHeaderSize);
    rcChunk *dataPtr;

    if(numChunks == 0) {
        if(rcStripGetNumChunk(Strip) != 0) {
            rcStripFreeChunks(Strip);
        }
        return;
    }
    if(rcStripGetNumChunk(Strip) == 0) {
        rcStripAllocChunks(Strip, numChunks);
        return;
    }
    freeSpace = rcAllocatedStripChunk() - rcUsedStripChunk();
    if(freeSpace < newSize) {
        allocMoreStripChunks(newSize);
    }
    dataPtr = rcStripGetChunks(Strip) - usedHeaderSize;
    memcpy((void *)(rcStrips.Chunk + rcUsedStripChunk()), dataPtr,
        elementSize*utMin(oldSize, newSize));
    if(newSize > oldSize) {
        {
            uint32 xValue;
            for(xValue = (uint32)(rcUsedStripChunk() + oldSize); xValue < rcUsedStripChunk() + oldSize + newSize - oldSize; xValue++) {
                rcStrips.Chunk[xValue] = rcChunkNull;
            }
        }
    }
    memset(dataPtr, 0, sizeof(rcStrip));
    memcpy((char *)dataPtr + sizeof(rcStrip), &oldSize, sizeof(uint32));
    rcSetFreeStripChunk(rcFreeStripChunk() + oldSize);
    rcStripSetChunkIndex_(Strip, rcUsedStripChunk() + usedHeaderSize);
    rcStripSetNumChunk(Strip, numChunks);
    rcSetUsedStripChunk(rcUsedStripChunk() + newSize);
}

/*----------------------------------------------------------------------------------------
  Copy the properties of Strip.
----------------------------------------------------------------------------------------*/
void rcStripCopyProps(
    rcStrip oldStrip,
    rcStrip newStrip)
{
    rcStripSetX_min(newStrip, rcStripGetX_min(oldStrip));
    rcStripSetX_max(newStrip, rcStripGetX_max(oldStrip));
}

/*----------------------------------------------------------------------------------------
  Add the indexed Chunk to the Strip.
----------------------------------------------------------------------------------------*/
void rcStripInsertChunk(
    rcStrip Strip,
    uint32 x,
    rcChunk _Chunk)
{
#if defined(DD_DEBUG)
    if(Strip == rcStripNull) {
        utExit("Non existent Strip");
    }
    if(rcChunkGetStrip(_Chunk) != rcStripNull) {
        utExit("Attempting to add Chunk to Strip twice");
    }
#endif
    rcStripSetiChunk(Strip, x, _Chunk);
    rcStripSetUsedChunk(Strip, utMax(rcStripGetUsedChunk(Strip), x + 1));
    rcChunkSetStripIndex(_Chunk, x);
    rcChunkSetStrip(_Chunk, Strip);
}

/*----------------------------------------------------------------------------------------
  Add the Chunk to the end of the StripChunk array.
----------------------------------------------------------------------------------------*/
void rcStripAppendChunk(
    rcStrip Strip,
    rcChunk _Chunk)
{
    uint32 usedChunk = rcStripGetUsedChunk(Strip);

#if defined(DD_DEBUG)
    if(Strip == rcStripNull) {
        utExit("Non existent Strip");
    }
#endif
    if(usedChunk >= rcStripGetNumChunk(Strip)) {
        rcStripResizeChunks(Strip, usedChunk + (usedChunk << 1) + 1);
    }
    rcStripSetiChunk(Strip, usedChunk, _Chunk);
    rcStripSetUsedChunk(Strip, usedChunk + 1);
    rcChunkSetStripIndex(_Chunk, usedChunk);
    rcChunkSetStrip(_Chunk, Strip);
}

/*----------------------------------------------------------------------------------------
  Remove the Chunk from the Strip.
----------------------------------------------------------------------------------------*/
void rcStripRemoveChunk(
    rcStrip Strip,
    rcChunk _Chunk)
{
#if defined(DD_DEBUG)
    if(_Chunk == rcChunkNull) {
        utExit("Non-existent Chunk");
    }
    if(rcChunkGetStrip(_Chunk) != rcStripNull && rcChunkGetStrip(_Chunk) != Strip) {
        utExit("Delete Chunk from non-owning Strip");
    }
#endif
    rcStripSetiChunk(Strip, rcChunkGetStripIndex(_Chunk), rcChunkNull);
    rcChunkSetStripIndex(_Chunk, UINT32_MAX);
    rcChunkSetStrip(_Chunk, rcStripNull);
}

#if defined(DD_DEBUG)
/*----------------------------------------------------------------------------------------
  Write out all the fields of an object.
----------------------------------------------------------------------------------------*/
void rcShowStrip(
    rcStrip Strip)
{
    utDatabaseShowObject("rc", "Strip", rcStrip2Index(Strip));
}
#endif

/*----------------------------------------------------------------------------------------
  Destroy ScreenAABB including everything in it. Remove from parents.
----------------------------------------------------------------------------------------*/
void rcScreenAABBDestroy(
    rcScreenAABB ScreenAABB)
{
    rcChunk owningCachedChunk = rcScreenAABBGetCachedChunk(ScreenAABB);

    if(rcScreenAABBDestructorCallback != NULL) {
        rcScreenAABBDestructorCallback(ScreenAABB);
    }
    if(owningCachedChunk != rcChunkNull) {
        rcChunkSetBoundsScreenAABB(owningCachedChunk, rcScreenAABBNull);
    }
    rcScreenAABBFree(ScreenAABB);
}

/*----------------------------------------------------------------------------------------
  Default constructor wrapper for the database manager.
----------------------------------------------------------------------------------------*/
static uint64 allocScreenAABB(void)
{
    rcScreenAABB ScreenAABB = rcScreenAABBAlloc();

    return rcScreenAABB2Index(ScreenAABB);
}

/*----------------------------------------------------------------------------------------
  Destructor wrapper for the database manager.
----------------------------------------------------------------------------------------*/
static void destroyScreenAABB(
    uint64 objectIndex)
{
    rcScreenAABBDestroy(rcIndex2ScreenAABB((uint32)objectIndex));
}

/*----------------------------------------------------------------------------------------
  Allocate the field arrays of ScreenAABB.
----------------------------------------------------------------------------------------*/
static void allocScreenAABBs(void)
{
    rcSetAllocatedScreenAABB(2);
    rcSetUsedScreenAABB(1);
    rcSetFirstFreeScreenAABB(rcScreenAABBNull);
    rcScreenAABBs.X_min = utNewAInitFirst(int32, (rcAllocatedScreenAABB()));
    rcScreenAABBs.X_max = utNewAInitFirst(int32, (rcAllocatedScreenAABB()));
    rcScreenAABBs.Y_min = utNewAInitFirst(int32, (rcAllocatedScreenAABB()));
    rcScreenAABBs.Y_max = utNewAInitFirst(int32, (rcAllocatedScreenAABB()));
    rcScreenAABBs.CachedChunk = utNewAInitFirst(rcChunk, (rcAllocatedScreenAABB()));
}

/*----------------------------------------------------------------------------------------
  Realloc the arrays of properties for class ScreenAABB.
----------------------------------------------------------------------------------------*/
static void reallocScreenAABBs(
    uint32 newSize)
{
    utResizeArray(rcScreenAABBs.X_min, (newSize));
    utResizeArray(rcScreenAABBs.X_max, (newSize));
    utResizeArray(rcScreenAABBs.Y_min, (newSize));
    utResizeArray(rcScreenAABBs.Y_max, (newSize));
    utResizeArray(rcScreenAABBs.CachedChunk, (newSize));
    rcSetAllocatedScreenAABB(newSize);
}

/*----------------------------------------------------------------------------------------
  Allocate more ScreenAABBs.
----------------------------------------------------------------------------------------*/
void rcScreenAABBAllocMore(void)
{
    reallocScreenAABBs((uint32)(rcAllocatedScreenAABB() + (rcAllocatedScreenAABB() >> 1)));
}

/*----------------------------------------------------------------------------------------
  Copy the properties of ScreenAABB.
----------------------------------------------------------------------------------------*/
void rcScreenAABBCopyProps(
    rcScreenAABB oldScreenAABB,
    rcScreenAABB newScreenAABB)
{
    rcScreenAABBSetX_min(newScreenAABB, rcScreenAABBGetX_min(oldScreenAABB));
    rcScreenAABBSetX_max(newScreenAABB, rcScreenAABBGetX_max(oldScreenAABB));
    rcScreenAABBSetY_min(newScreenAABB, rcScreenAABBGetY_min(oldScreenAABB));
    rcScreenAABBSetY_max(newScreenAABB, rcScreenAABBGetY_max(oldScreenAABB));
}

#if defined(DD_DEBUG)
/*----------------------------------------------------------------------------------------
  Write out all the fields of an object.
----------------------------------------------------------------------------------------*/
void rcShowScreenAABB(
    rcScreenAABB ScreenAABB)
{
    utDatabaseShowObject("rc", "ScreenAABB", rcScreenAABB2Index(ScreenAABB));
}
#endif

/*----------------------------------------------------------------------------------------
  Free memory used by the rc database.
----------------------------------------------------------------------------------------*/
void rcDatabaseStop(void)
{
    utFree(rcRoots.Frame_index);
    utFree(rcRoots.ChunkIndex_);
    utFree(rcRoots.NumChunk);
    utFree(rcRoots.Chunk);
    utFree(rcRoots.UsedChunk);
    utFree(rcRoots.StripIndex_);
    utFree(rcRoots.NumStrip);
    utFree(rcRoots.Strip);
    utFree(rcRoots.UsedStrip);
    utFree(rcRoots.FreeList);
    utFree(rcScreenVertexs.X);
    utFree(rcScreenVertexs.Y);
    utFree(rcScreenVertexs.Z);
    utFree(rcScreenVertexs.Inv_w);
    utFree(rcScreenVertexs.FreeList);
    utFree(rcChunks.Type);
    utFree(rcChunks.Depth_sort_key);
    utFree(rcChunks.X0);
    utFree(rcChunks.Y0);
    utFree(rcChunks.Z0);
    utFree(rcChunks.Inv_w0);
    utFree(rcChunks.X1);
    utFree(rcChunks.Y1);
    utFree(rcChunks.Z1);
    utFree(rcChunks.Inv_w1);
    utFree(rcChunks.X2);
    utFree(rcChunks.Y2);
    utFree(rcChunks.Z2);
    utFree(rcChunks.Inv_w2);
    utFree(rcChunks.Color);
    utFree(rcChunks.Texture);
    utFree(rcChunks.Uv0);
    utFree(rcChunks.Uv1);
    utFree(rcChunks.Uv2);
    utFree(rcChunks.Root);
    utFree(rcChunks.RootIndex);
    utFree(rcChunks.BoundsScreenAABB);
    utFree(rcChunks.Strip);
    utFree(rcChunks.StripIndex);
    utFree(rcStrips.X_min);
    utFree(rcStrips.X_max);
    utFree(rcStrips.Root);
    utFree(rcStrips.RootIndex);
    utFree(rcStrips.ChunkIndex_);
    utFree(rcStrips.NumChunk);
    utFree(rcStrips.Chunk);
    utFree(rcStrips.UsedChunk);
    utFree(rcScreenAABBs.X_min);
    utFree(rcScreenAABBs.X_max);
    utFree(rcScreenAABBs.Y_min);
    utFree(rcScreenAABBs.Y_max);
    utFree(rcScreenAABBs.CachedChunk);
    utUnregisterModule(rcModuleID);
}

/*----------------------------------------------------------------------------------------
  Allocate memory used by the rc database.
----------------------------------------------------------------------------------------*/
void rcDatabaseStart(void)
{
    if(!utInitialized()) {
        utStart();
    }
    rcRootData.hash = 0xf3b9da1a;
    rcModuleID = utRegisterModule("rc", false, rcHash(), 5, 52, 1, sizeof(struct rcRootType_),
        &rcRootData, rcDatabaseStart, rcDatabaseStop);
    utRegisterEnum("ChunkType", 2);
    utRegisterEntry("CHUNK_COLORED", 0);
    utRegisterEntry("CHUNK_TEXTURED", 1);
    utRegisterClass("Root", 10, &rcRootData.usedRoot, &rcRootData.allocatedRoot,
        &rcRootData.firstFreeRoot, 9, 4, allocRoot, destroyRoot);
    utRegisterField("Frame_index", &rcRoots.Frame_index, sizeof(uint32), UT_UINT, NULL);
    utRegisterField("ChunkIndex_", &rcRoots.ChunkIndex_, sizeof(uint32), UT_UINT, NULL);
    utSetFieldHidden();
    utRegisterField("NumChunk", &rcRoots.NumChunk, sizeof(uint32), UT_UINT, NULL);
    utSetFieldHidden();
    utRegisterField("Chunk", &rcRoots.Chunk, sizeof(rcChunk), UT_POINTER, "Chunk");
    utRegisterArray(&rcRootData.usedRootChunk, &rcRootData.allocatedRootChunk,
        getRootChunks, allocRootChunks, rcCompactRootChunks);
    utRegisterField("UsedChunk", &rcRoots.UsedChunk, sizeof(uint32), UT_UINT, NULL);
    utRegisterField("StripIndex_", &rcRoots.StripIndex_, sizeof(uint32), UT_UINT, NULL);
    utSetFieldHidden();
    utRegisterField("NumStrip", &rcRoots.NumStrip, sizeof(uint32), UT_UINT, NULL);
    utSetFieldHidden();
    utRegisterField("Strip", &rcRoots.Strip, sizeof(rcStrip), UT_POINTER, "Strip");
    utRegisterArray(&rcRootData.usedRootStrip, &rcRootData.allocatedRootStrip,
        getRootStrips, allocRootStrips, rcCompactRootStrips);
    utRegisterField("UsedStrip", &rcRoots.UsedStrip, sizeof(uint32), UT_UINT, NULL);
    utRegisterField("FreeList", &rcRoots.FreeList, sizeof(rcRoot), UT_POINTER, "Root");
    utSetFieldHidden();
    utRegisterClass("ScreenVertex", 5, &rcRootData.usedScreenVertex, &rcRootData.allocatedScreenVertex,
        &rcRootData.firstFreeScreenVertex, 14, 4, allocScreenVertex, destroyScreenVertex);
    utRegisterField("X", &rcScreenVertexs.X, sizeof(float), UT_FLOAT, NULL);
    utRegisterField("Y", &rcScreenVertexs.Y, sizeof(float), UT_FLOAT, NULL);
    utRegisterField("Z", &rcScreenVertexs.Z, sizeof(float), UT_FLOAT, NULL);
    utRegisterField("Inv_w", &rcScreenVertexs.Inv_w, sizeof(float), UT_FLOAT, NULL);
    utRegisterField("FreeList", &rcScreenVertexs.FreeList, sizeof(rcScreenVertex), UT_POINTER, "ScreenVertex");
    utSetFieldHidden();
    utRegisterClass("Chunk", 24, &rcRootData.usedChunk, &rcRootData.allocatedChunk,
        &rcRootData.firstFreeChunk, 34, 4, allocChunk, destroyChunk);
    utRegisterField("Type", &rcChunks.Type, sizeof(rcChunkType), UT_ENUM, "ChunkType");
    utRegisterField("Depth_sort_key", &rcChunks.Depth_sort_key, sizeof(float), UT_FLOAT, NULL);
    utRegisterField("X0", &rcChunks.X0, sizeof(float), UT_FLOAT, NULL);
    utRegisterField("Y0", &rcChunks.Y0, sizeof(float), UT_FLOAT, NULL);
    utRegisterField("Z0", &rcChunks.Z0, sizeof(float), UT_FLOAT, NULL);
    utRegisterField("Inv_w0", &rcChunks.Inv_w0, sizeof(float), UT_FLOAT, NULL);
    utRegisterField("X1", &rcChunks.X1, sizeof(float), UT_FLOAT, NULL);
    utRegisterField("Y1", &rcChunks.Y1, sizeof(float), UT_FLOAT, NULL);
    utRegisterField("Z1", &rcChunks.Z1, sizeof(float), UT_FLOAT, NULL);
    utRegisterField("Inv_w1", &rcChunks.Inv_w1, sizeof(float), UT_FLOAT, NULL);
    utRegisterField("X2", &rcChunks.X2, sizeof(float), UT_FLOAT, NULL);
    utRegisterField("Y2", &rcChunks.Y2, sizeof(float), UT_FLOAT, NULL);
    utRegisterField("Z2", &rcChunks.Z2, sizeof(float), UT_FLOAT, NULL);
    utRegisterField("Inv_w2", &rcChunks.Inv_w2, sizeof(float), UT_FLOAT, NULL);
    utRegisterField("Color", &rcChunks.Color, sizeof(uint32), UT_UINT, NULL);
    utRegisterField("Texture", &rcChunks.Texture, sizeof(Texture), UT_TYPEDEF, NULL);
    utRegisterField("Uv0", &rcChunks.Uv0, sizeof(Vec2), UT_TYPEDEF, NULL);
    utRegisterField("Uv1", &rcChunks.Uv1, sizeof(Vec2), UT_TYPEDEF, NULL);
    utRegisterField("Uv2", &rcChunks.Uv2, sizeof(Vec2), UT_TYPEDEF, NULL);
    utRegisterField("Root", &rcChunks.Root, sizeof(rcRoot), UT_POINTER, "Root");
    utRegisterField("RootIndex", &rcChunks.RootIndex, sizeof(uint32), UT_UINT, NULL);
    utRegisterField("BoundsScreenAABB", &rcChunks.BoundsScreenAABB, sizeof(rcScreenAABB), UT_POINTER, "ScreenAABB");
    utRegisterField("Strip", &rcChunks.Strip, sizeof(rcStrip), UT_POINTER, "Strip");
    utRegisterField("StripIndex", &rcChunks.StripIndex, sizeof(uint32), UT_UINT, NULL);
    utRegisterClass("Strip", 8, &rcRootData.usedStrip, &rcRootData.allocatedStrip,
        &rcRootData.firstFreeStrip, 41, 4, allocStrip, destroyStrip);
    utRegisterField("X_min", &rcStrips.X_min, sizeof(int32), UT_INT, NULL);
    utRegisterField("X_max", &rcStrips.X_max, sizeof(int32), UT_INT, NULL);
    utRegisterField("Root", &rcStrips.Root, sizeof(rcRoot), UT_POINTER, "Root");
    utRegisterField("RootIndex", &rcStrips.RootIndex, sizeof(uint32), UT_UINT, NULL);
    utRegisterField("ChunkIndex_", &rcStrips.ChunkIndex_, sizeof(uint32), UT_UINT, NULL);
    utSetFieldHidden();
    utRegisterField("NumChunk", &rcStrips.NumChunk, sizeof(uint32), UT_UINT, NULL);
    utSetFieldHidden();
    utRegisterField("Chunk", &rcStrips.Chunk, sizeof(rcChunk), UT_POINTER, "Chunk");
    utRegisterArray(&rcRootData.usedStripChunk, &rcRootData.allocatedStripChunk,
        getStripChunks, allocStripChunks, rcCompactStripChunks);
    utRegisterField("UsedChunk", &rcStrips.UsedChunk, sizeof(uint32), UT_UINT, NULL);
    utRegisterClass("ScreenAABB", 5, &rcRootData.usedScreenAABB, &rcRootData.allocatedScreenAABB,
        &rcRootData.firstFreeScreenAABB, 51, 4, allocScreenAABB, destroyScreenAABB);
    utRegisterField("X_min", &rcScreenAABBs.X_min, sizeof(int32), UT_INT, NULL);
    utRegisterField("X_max", &rcScreenAABBs.X_max, sizeof(int32), UT_INT, NULL);
    utRegisterField("Y_min", &rcScreenAABBs.Y_min, sizeof(int32), UT_INT, NULL);
    utRegisterField("Y_max", &rcScreenAABBs.Y_max, sizeof(int32), UT_INT, NULL);
    utRegisterField("CachedChunk", &rcScreenAABBs.CachedChunk, sizeof(rcChunk), UT_POINTER, "Chunk");
    allocRoots();
    allocScreenVertexs();
    allocChunks();
    allocStrips();
    allocScreenAABBs();
}

