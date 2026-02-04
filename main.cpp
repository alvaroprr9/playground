#include <stdlib.h>
#include <stdio.h>
#include <cstdint>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

typedef struct
{
    f32 x;
    f32 y;
    f32 mass;
} Particle;

int main()
{
    const u32 n = 3;
    Particle p;
    p.x = 1.f;
    p.y = 6.f;
    p.mass = 23.f;

    Particle *z = (Particle *)malloc(n * sizeof(Particle));

    z->x = 3.f;
    z->y = 0.f;

    for (u32 i = 0; i < n; ++i)
    {
        z[i].x = 1.f;
        z[i].y = 6.f;
        z[i].mass = 23.f;
    }

    free(z);
    return 0;
}
