#pragma once

#define MV_ID(dt) dt ## _mv
#define MV_MANY_ID(dt) dt ## _mv_many
#define CP_ID(dt) dt ## _cp
#define CP_MANY_ID(dt) dt ## _cp_many
#define CLR_ID(dt) dt ## _clr
#define CLR_MANY_ID(dt) dt ## _clr_many
#define EQ_ID(dt) dt ## _eq
#define CMP_ID(dt) dt ## _cmp

#define MV_MANY_USING_MV_DECL(dt) void MV_MANY_ID(dt)(dt *dest, dt *src, size_t count)
#define MV_MANY_USING_MV_DEFN(dt) void MV_MANY_ID(dt)(dt *dest, dt *src, size_t count) \
    { \
        for(size_t i = 0; i < count; i++) \
        { \
            MV_ID(dt)(dest + i, src + i); \
        } \
    }
#define CP_MANY_USING_CP_DECL(dt) void CP_MANY_ID(dt)(dt *dest, dt *src, size_t count)
#define CP_MANY_USING_CP_DEFN(dt) void CP_MANY_ID(dt)(dt *dest, dt *src, size_t count) \
    { \
        for(size_t i = 0; i < count; i++) \
        { \
            CP_ID(dt)(dest + i, src + i); \
        } \
    }
#define CLR_MANY_USING_CLR_DECL(dt) void CLR_MANY_ID(dt)(dt *to_clear, size_t count)
#define CLR_MANY_USING_CLR_DEFN(dt) void CLR_MANY_ID(dt)(dt *to_clear, size_t count) \
    { \
        for(size_t i = 0; i < count; i++) \
        { \
            CLR_ID(dt)(to_clear + i); \
        } \
    }

#define POD_MV_DECL(dt) void MV_ID(dt)(dt *dest, dt *src)
#define POD_MV_DEFN(dt) void MV_ID(dt)(dt *dest, dt *src) \
    { \
        *dest = *src; \
    }
#define POD_MV_MANY_DECL(dt) void MV_MANY_ID(dt)(dt *dest, dt *src, size_t count)
#define POD_MV_MANY_DEFN(dt) void MV_MANY_ID(dt)(dt *dest, dt *src, size_t count) \
    { \
        memcpy(dest, src, sizeof(dt) * count); \
    }
#define POD_CP_DECL(dt) void CP_ID(dt)(dt *dest, dt *src)
#define POD_CP_DEFN(dt) void CP_ID(dt)(dt *dest, dt *src) \
    { \
        *dest = *src; \
    }
#define POD_CP_MANY_DECL(dt) void CP_MANY_ID(dt)(dt *dest, dt *src, size_t count)
#define POD_CP_MANY_DEFN(dt) void CP_MANY_ID(dt)(dt *dest, dt *src, size_t count) \
    { \
        memcpy(dest, src, sizeof(dt) * count); \
    }
#define POD_CLR_DECL(dt) void CLR_ID(dt)(dt *to_clear)
#define POD_CLR_DEFN(dt) void CLR_ID(dt)(dt *to_clear){}
#define POD_CLR_MANY_DECL(dt) void CLR_MANY_ID(dt)(dt *to_clear, size_t count)
#define POD_CLR_MANY_DEFN(dt) void CLR_MANY_ID(dt)(dt *to_clear, size_t count){}

#define EQ_FROM_CMP_DECL(dt) bool EQ_ID(dt)(dt *a, dt *b)
#define EQ_FROM_CMP_DEFN(dt) bool EQ_ID(dt)(dt *a, dt *b) \
    { \
        return CMP_ID(dt)(a, b) == 0; \
    }

#define POD_FNS_DECL(dt) \
    POD_MV_DECL(dt); \
    POD_MV_MANY_DECL(dt); \
    POD_CP_DECL(dt); \
    POD_CP_MANY_DECL(dt); \
    POD_CLR_DECL(dt); \
    POD_CLR_MANY_DECL(dt)
#define POD_FNS_DEFN(dt) \
    POD_MV_DEFN(dt) \
    POD_MV_MANY_DEFN(dt) \
    POD_CP_DEFN(dt) \
    POD_CP_MANY_DEFN(dt) \
    POD_CLR_DEFN(dt) \
    POD_CLR_MANY_DEFN(dt)
