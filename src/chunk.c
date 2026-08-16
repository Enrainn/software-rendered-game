#include "chunk.h"
#include <stdlib.h>
#include <math.h>
#include "rcdatabase.h"

#if defined(__has_include)
#  if __has_include("rcdatabase.h")
#    include "rcdatabase.h"
#    define CHUNKS_HAVE_DATADRAW 1
#  endif
#endif

#ifndef CHUNKS_HAVE_DATADRAW
#  define CHUNKS_HAVE_DATADRAW 0
#endif

static int chunk_compare(const void *a, const void *b) {
    float da = ((const Chunk *)a)->depth_sort_key;
    float db = ((const Chunk *)b)->depth_sort_key;
    if (da < db) return -1;
    if (da > db) return  1;
    return 0;
}

void chunks_sort_front_to_back(Chunk *chunks, int count) {
    qsort(chunks, count, sizeof(Chunk), chunk_compare);
}

ScreenAABB chunk_screen_aabb(const Chunk *chunk) {
    ScreenAABB bb;
    bb.x_min = (int)floorf(minf(chunk->verts[0].x,
                                minf(chunk->verts[1].x, chunk->verts[2].x)));
    bb.x_max = (int)ceilf(maxf(chunk->verts[0].x,
                                maxf(chunk->verts[1].x, chunk->verts[2].x)));
    bb.y_min = (int)floorf(minf(chunk->verts[0].y,
                                minf(chunk->verts[1].y, chunk->verts[2].y)));
    bb.y_max = (int)ceilf(maxf(chunk->verts[0].y,
                                maxf(chunk->verts[1].y, chunk->verts[2].y)));
    return bb;
}


void chunks_datadraw_start(void) {
#if CHUNKS_HAVE_DATADRAW
    rcDatabaseStart();
#endif
}

void chunks_datadraw_record_frame(const Chunk *chunks, int chunk_count, uint32_t frame_index) {
#if CHUNKS_HAVE_DATADRAW
    rcRootFreeAll();
    rcChunkFreeAll();
    rcScreenAABBFreeAll();

    rcRoot root = rcRootAlloc();
    rcRootSetFrame_index(root, frame_index);

    for (int i = 0; i < chunk_count; i++) {
        const Chunk *src = &chunks[i];
        rcChunk dst = rcChunkAlloc();

        rcChunkSetType(dst, src->type == CHUNK_COLORED ? RC_CHUNK_COLORED : RC_CHUNK_TEXTURED);
        rcChunkSetDepth_sort_key(dst, src->depth_sort_key);
        rcChunkSetX0(dst, src->verts[0].x);
        rcChunkSetY0(dst, src->verts[0].y);
        rcChunkSetZ0(dst, src->verts[0].z);
        rcChunkSetInv_w0(dst, src->verts[0].inv_w);
        rcChunkSetX1(dst, src->verts[1].x);
        rcChunkSetY1(dst, src->verts[1].y);
        rcChunkSetZ1(dst, src->verts[1].z);
        rcChunkSetInv_w1(dst, src->verts[1].inv_w);
        rcChunkSetX2(dst, src->verts[2].x);
        rcChunkSetY2(dst, src->verts[2].y);
        rcChunkSetZ2(dst, src->verts[2].z);
        rcChunkSetInv_w2(dst, src->verts[2].inv_w);

        if (src->type == CHUNK_COLORED) {
            rcChunkSetColor(dst, src->colored.color);
        } else {
            rcChunkSetTexture(dst, src->textured.texture);
            rcChunkSetUv0(dst, src->textured.uvs[0]);
            rcChunkSetUv1(dst, src->textured.uvs[1]);
            rcChunkSetUv2(dst, src->textured.uvs[2]);
        }

        ScreenAABB bb = chunk_screen_aabb(src);
        rcScreenAABB bounds = rcScreenAABBAlloc();
        rcScreenAABBSetX_min(bounds, bb.x_min);
        rcScreenAABBSetX_max(bounds, bb.x_max);
        rcScreenAABBSetY_min(bounds, bb.y_min);
        rcScreenAABBSetY_max(bounds, bb.y_max);
        rcChunkInsertBoundsScreenAABB(dst, bounds);

        rcRootAppendChunk(root, dst);
    }
#else
    (void)chunks;
    (void)chunk_count;
    (void)frame_index;
#endif
}

void chunks_datadraw_stop(void) {
#if CHUNKS_HAVE_DATADRAW
    rcDatabaseStop();
#endif
}
