//zach lib, random functions and data structures for use wherever
//Inspired by the stb library and talks by Sean Barrett

// Copyright (C) 2018 Zachary Wells
//
// This software may be modified and distributed under the terms
// of the MIT license. See the LICENSE file for details.

#ifndef __ZAC_INCLUDE_H
#define __ZAC_INCLUDE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif

#define zac_max(a, b) ((a) > (b) ? (a) : (b))
#define zac_min(a, b) ((a) < (b) ? (a) : (b))

typedef unsigned char zac_byte;
typedef unsigned int  zac_uint;

#ifdef ZAC_IMPLEMENTATION

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef zac_assert
#define zac_assert(e, r) assert(e)
#endif
#ifndef zac_malloc
#define zac_malloc(s) malloc(s)
#endif
#ifndef zac_realloc
#define zac_realloc(p, s) realloc(p, s)
#endif
#ifndef zac_free
#define zac_free(p) free(p)
#endif

#endif

#ifdef ZAC_IMPLEMENTATION
#define Z(...) __VA_ARGS__
#else
#define Z(...)
#endif

//Resizeable arrays
//std::vector replacement
typedef struct zac_array {
	int len, cap;
} zac_array;

#define zac_array_head(a)      (((zac_array*)(a))[-1])
#define zac_array_len(a)       ((a) ? zac_array_head(a).len : 0)
#define zac_array_cap(a)       ((a) ? zac_array_head(a).cap : 0)
#define zac_array_elem_size(a) (sizeof(*(a)))
#define zac_array_end(a) ((a) + zac_array_len(a) - 1)

#define zac_array_resize(a, c) ((a) = zac_array__alloc((zac_array*)(a), zac_array_elem_size(a), c))
#define zac_array_shrink(a)    (zac_array_resize(a, zac_array_len(a)))
#define zac_array_free(a)      (zac_array_resize(a, 0))

#define zac_array_hasSpace(a, n) (zac_array_cap(a) >= (zac_array_len(a) + (n)))
#define zac_array_getSpace(a, n) ((zac_array_hasSpace(a, n) ? 0 : zac_array_resize(a, zac_array_len(a) + (n))), \
								zac_array_head(a).len += (n), zac_array_end(a) - (n) + 1)

#define zac_array_push(a, e) ((zac_array_hasSpace(a, 1) ? 0 : zac_array_resize(a, zac_array_len(a)*3/2+1)), \
								zac_array_head(a).len++, *zac_array_end(a) = (e))

void *zac_array__alloc(zac_array *arr, int size, int cap) Z({
	if (cap == 0) {
		zac_free(arr ? (arr-1) : arr);
		return NULL;
	}
	
	int len = zac_array_len(arr);
	
	zac_array *out = zac_realloc(arr ? (arr-1) : arr, sizeof(zac_array) + (size * cap));
	zac_assert(out != NULL, "Allocation failed!");
	out->len = zac_min(len, cap);
	out->cap = cap;
	return out + 1;
});


//Hash map (tbd)
typedef unsigned long zac_hash;

zac_hash zac_hash_str(const char *str) Z({
	zac_hash hash = 5381, c;
	
	while ((c = *str++)) {
		hash = ((hash << 5) + hash) + c;
	}
	
	return hash;
});


//File IO helper functions
typedef void zac_file; //Wrapper for C FILE*

int zac_io_getSize(const zac_file *file) Z({
	FILE *fp = (FILE*)file;
	int pos = ftell(fp);
	fseek(fp, 0, SEEK_END);
	int len = ftell(fp);
	fseek(fp, pos, SEEK_SET);
	return len;
});

//Read entire file as ascii char array
char *zac_io_reads(const char *fileName, int *length) Z({
	FILE *file = fopen(fileName, "rb");
	int len = -1;
	char *out = NULL;
	
	if (file != NULL) {
		len = zac_io_getSize(file);
		
		out = zac_malloc(sizeof(char) * len + 1);
		fread(out, sizeof(char), len, file);
		out[len] = '\0';
		
		fclose(file);
	}
	
	if (length != NULL) {
		*length = len;
	}
	
	return out;
});
int zac_io_readsb(char *buffer, int length, const char *fileName) Z({
	FILE *file = fopen(fileName, "rb");
	int len = -1;
	
	if (file != NULL) {
		len = zac_min(zac_io_getSize(file), length - 1);
		
		fread(buffer, sizeof(char), len, file);
		buffer[len++] = '\0';
		
		fclose(file);
	}
	
	return len;
});
//TODO: make array parameter wrapper
// int zac_io_readsa(char *array, const char *fileName) Z({
// 	FILE *file = fopen(fileName, "rb");
// 	int len = -1;
	
// 	if (file != NULL) {
// 		len = zac_io_getSize(file);
		
// 		fread(zac_array_getSpace(array, len + 1), sizeof(char), len, file);
// 		array[len++] = '\0';
		
// 		fclose(file);
// 	}
	
// 	return len;
// });

#undef Z

//Undefine just incase
#ifdef ZAC_IMPLEMENTATION
#undef ZAC_IMPLEMENTATION
#endif

#ifdef __cplusplus
}
#endif
#endif