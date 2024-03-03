//
// Created by cp176 on 2/26/2024.
//

#ifndef FLATDAWG_TYPES_H
#define FLATDAWG_TYPES_H

#include <stdbool.h>
#include <stdint.h>

#define ASSERT(condition, message) if (!(condition)) { fprintf(stderr, "Assertion failed: %s\n", message); return 1; }
typedef float    f32;
typedef double   f64;
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;
typedef size_t   usize;
typedef ssize_t  isize;

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

#define dot(v0, v1) ({ const v2 _v0 = (v0), _v1 = (v1); (_v0.x * _v1.x) + (_v0.y * _v1.y); })
#define length(v) ({ const v2 _v = (v); sqrtf(dot(_v, _v)); })
#define normalize(u) ({ const v2 _u = (u); const f32 l = length(_u); (v2) { _u.x / l, _u.y / l }; })
#define min(a, b) ({ __typeof__(a) _a = (a), _b = (b); _a < _b ? _a : _b; })
#define max(a, b) ({ __typeof__(a) _a = (a), _b = (b); _a > _b ? _a : _b; })
#define sign(a) ({ __typeof__(a) _a = (a); (__typeof__(a))(_a < 0 ? -1 : (_a > 0 ? 1 : 0)); })

typedef struct v2_s { f32 x, y; } v2;
typedef struct v2i_s { i32 x, y; } v2i;

typedef struct deltaTime_s {
    f32 delta;
    f32 time;
} deltaTime;

#endif //FLATDAWG_TYPES_H
