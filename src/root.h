#ifndef ROOT_H
#define ROOT_H

// #include <raylib.h>
// #include <raymath.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define STB_DS_IMPLEMENTATION
#include <stb_ds.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float f32;
typedef double f64;

// src files
#include "common.c"
#include "editor.c"
#include "text_renderer.c"
#include "main.c"

#endif
