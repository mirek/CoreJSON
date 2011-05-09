//
// CoreJSON.h
// CoreJSON Framework
//
// Copyright 2011 Mirek Rusin <mirek [at] me [dot] com>
//                http://github.com/mirek/CoreJSON
//
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
//

#include <CoreFoundation/CoreFoundation.h>
#include <yajl/yajl_parse.h>
#include <yajl/yajl_gen.h>

#define CORE_JSON_STACK_INITIAL_SIZE              YAJL_MAX_DEPTH
#define CORE_JSON_STACK_ENTRY_KEYS_INITIAL_SIZE   1024
#define CORE_JSON_STACK_ENTRY_VALUES_INITIAL_SIZE 1024
#define CORE_JSON_ELEMENTS_INITIAL_SIZE           4096

#pragma Helper stack for parsing

// Internal stack to hold containers when parsing. Practically all functions in most
// cases are O(1), unless internal buffers are full and need to be reallocated.
//
// This stucture is also used to create CFArrayRef for arrays and CFDictionaryRef for
// maps by a single create call.
typedef struct {
  CFAllocatorRef allocator;
  CFIndex        retainCount;
  
  CFIndex        index;
  
  CFIndex       *values;
  CFIndex        valuesIndex;
  CFIndex        valuesSize;
  
  CFIndex       *keys;
  CFIndex        keysIndex;
  CFIndex        keysSize;
} __JSONStackEntry;

typedef __JSONStackEntry *__JSONStackEntryRef;

__JSONStackEntryRef __JSONStackEntryCreate       (CFAllocatorRef allocator, CFIndex index, CFIndex valuesInitialSize, CFIndex keysInitialSize);
__JSONStackEntryRef __JSONStackEntryRetain       (__JSONStackEntryRef entry);
CFIndex             __JSONStackEntryRelease      (__JSONStackEntryRef entry);
bool                __JSONStackEntryAppendValue  (__JSONStackEntryRef entry, CFIndex value);
bool                __JSONStackEntryAppendKey    (__JSONStackEntryRef entry, CFIndex key);
CFTypeRef          *__JSONStackEntryCreateValues (__JSONStackEntryRef entry, CFTypeRef *elements);
CFTypeRef          *__JSONStackEntryCreateKeys   (__JSONStackEntryRef entry, CFTypeRef *elements);

typedef struct {
  CFAllocatorRef       allocator;
  CFIndex              retainCount;
  __JSONStackEntryRef *stack;
  CFIndex              index;
  CFIndex              size;
} __JSONStack;

typedef __JSONStack *__JSONStackRef;

__JSONStackRef      __JSONStackCreate           (CFAllocatorRef allocator, CFIndex initialSize);
__JSONStackRef      __JSONStackRelease          (__JSONStackRef stack);
__JSONStackEntryRef __JSONStackGetTop           (__JSONStackRef stack);
bool                __JSONStackPush             (__JSONStackRef stack, __JSONStackEntryRef entry);
__JSONStackEntryRef __JSONStackPop              (__JSONStackRef stack);
bool                __JSONStackAppendValueAtTop (__JSONStackRef stack, CFIndex value);
bool                __JSONStackAppendKeyAtTop   (__JSONStackRef stack, CFIndex key);

#pragma Internal callbacks for libyajl parser

int __JSONParserAppendStringWithBytes    (void *context, const unsigned char *value, size_t length);
int __JSONParserAppendNull               (void *context);
int __JSONParserAppendBooleanWithInteger (void *context, int value);
int __JSONParserAppendNumberWithBytes    (void *context, const char *value, size_t length);
int __JSONParserAppendNumberWithLong     (void *context, long value);
int __JSONParserAppendNumberWithDouble   (void *context, double value);
int __JSONParserAppendMapKeyWithBytes    (void *context, const unsigned char *value, size_t length);
int __JSONParserAppendMapStart           (void *context);
int __JSONParserAppendMapEnd             (void *context);
int __JSONParserAppendArrayStart         (void *context);
int __JSONParserAppendArrayEnd           (void *context);

#pragma Internal memory allocation

void *__JSONAllocatorAllocate   (void *ctx, size_t sz);
void  __JSONAllocatorDeallocate (void *ctx, void *ptr);
void *__JSONAllocatorReallocate (void *ctx, void *ptr, size_t sz);

typedef struct {
  CFAllocatorRef     allocator;
  CFIndex            retainCount;

  yajl_handle        yajlParser;
//  yajl_parser_config yajlParserConfig;
  yajl_status        yajlParserStatus;
  yajl_callbacks     yajlParserCallbacks;
  yajl_alloc_funcs   yajlAllocFuncs;
  
  CFIndex            elementsIndex;
  CFIndex            elementsSize;
  CFTypeRef         *elements;

  __JSONStackRef     stack;
  
} __JSON;

typedef __JSON *__JSONRef;

typedef enum JSONReadOptions {
  kJSONReadOptionCheckUTF8                  = 1,
  kJSONReadOptionAllowComments              = 2,
  
  kJSONReadOptionsDefault                   = 0,
  kJSONReadOptionsCheckUTF8AndAllowComments = 3
} JSONReadOptions;

typedef enum JSONWriteOptions {
  kJSONWriteOptionIndent = 1,
  
  kJSONWriteOptionsDefault = 0
} JSONWriteOptions;

#pragma Internal elements array support

CFIndex __JSONElementsAppend                 (__JSONRef json, CFTypeRef value);

#pragma Generator

void __JSONGeneratorAppendString             (CFAllocatorRef allocator, yajl_gen *g, CFStringRef value);
void __JSONGeneratorAppendDoubleTypeNumber   (CFAllocatorRef allocator, yajl_gen *g, CFNumberRef value);
void __JSONGeneratorAppendLongLongTypeNumber (CFAllocatorRef allocator, yajl_gen *g, CFNumberRef value);
void __JSONGeneratorAppendNumber             (CFAllocatorRef allocator, yajl_gen *g, CFNumberRef value);
void __JSONGeneratorAppendArray              (CFAllocatorRef allocator, yajl_gen *g, CFArrayRef value);
void __JSONGeneratorAppendDictionary         (CFAllocatorRef allocator, yajl_gen *g, CFDictionaryRef value);
void __JSONGeneratorAppendValue              (CFAllocatorRef allocator, yajl_gen *g, CFTypeRef value);
void __JSONGeneratorAppendAttributedString   (CFAllocatorRef allocator, yajl_gen *g, CFAttributedStringRef value);
void __JSONGeneratorAppendBoolean            (CFAllocatorRef allocator, yajl_gen *g, CFBooleanRef value);
void __JSONGeneratorAppendNull               (CFAllocatorRef allocator, yajl_gen *g, CFNullRef value);
void __JSONGeneratorAppendURL                (CFAllocatorRef allocator, yajl_gen *g, CFURLRef value);
void __JSONGeneratorAppendUUID               (CFAllocatorRef allocator, yajl_gen *g, CFUUIDRef value);

__JSONRef   __JSONCreate           (CFAllocatorRef allocator, JSONReadOptions options);
bool        __JSONParseWithString  (__JSONRef    json, CFStringRef string, CFErrorRef *error);
CFTypeRef   __JSONCreateObject     (__JSONRef    json);
__JSONRef   __JSONRelease          (__JSONRef    json);

#pragma Public API

CFTypeRef JSONCreateWithString(CFAllocatorRef allocator, CFStringRef string, JSONReadOptions options, CFErrorRef *error);
//CFTypeRef     JSONCreateWithData       (CFAllocatorRef allocator, CFDataRef data);

CFStringRef JSONCreateString(CFAllocatorRef allocator, CFTypeRef value, JSONWriteOptions options, CFErrorRef *error);
//CFDataRef     JSONCreateData           (CFAllocatorRef allocator, CFTypeRef value);
