//
// CoreJSON.h
// CoreJSON
//
// Copyright 2011 Mirek Rusin <mirek [at] me [dot] com> http://github.com/mirek/CoreJSON
//

#import <Foundation/Foundation.h>
#import "yajl_parse.h"
#import "yajl_gen.h"

#define CORE_JSON_STACK_MAX_DEPTH 1024

#pragma UTF8 string helper for fast buffer access

typedef struct {
  CFAllocatorRef allocator;
  CFStringRef string;
  const unsigned char *pointer;
  const unsigned char *buffer;
  CFIndex maximumSize;
} __JSONUTF8String;

__JSONUTF8String     __JSONUTF8StringMake           (CFAllocatorRef allocator, CFStringRef string);
const unsigned char *__JSONUTF8StringGetBuffer      (__JSONUTF8String utf8String);
CFIndex              __JSONUTF8StringGetMaximumSize (__JSONUTF8String utf8String);
void                 __JSONUTF8StringDestroy        (__JSONUTF8String utf8String);

#pragma Helper stack for parsing

typedef struct {
  CFAllocatorRef allocator;
  CFIndex    retainCount;
  
  CFIndex    index;
  
  CFTypeRef *values;
  CFIndex    valuesIndex;
  CFIndex    valuesLength;
  
  CFTypeRef *keys;
  CFIndex    keysIndex;
  CFIndex    keysLength;
} __JSONStackEntry;

typedef __JSONStackEntry *__JSONStackEntryRef;

__JSONStackEntryRef __JSONStackEntryCreate      (CFAllocatorRef allocator, CFIndex index, CFIndex valuesLength, CFIndex keysLength);
__JSONStackEntryRef __JSONStackEntryRetain      (__JSONStackEntryRef entry);
CFIndex             __JSONStackEntryRelease     (__JSONStackEntryRef entry);
__JSONStackEntryRef __JSONStackEntryReleaseRef  (__JSONStackEntryRef *entry);
void                __JSONStackEntryAppendValue (__JSONStackEntryRef entry, CFTypeRef value);
void                __JSONStackEntryAppendKey   (__JSONStackEntryRef entry, CFTypeRef key);

typedef struct {
  CFAllocatorRef       allocator;
  __JSONStackEntryRef *stack;
  CFIndex              index;
  CFIndex              size;
} __JSONStack;

typedef __JSONStack *__JSONStackRef;

__JSONStackRef      __JSONStackCreate           (CFAllocatorRef allocator, CFIndex maxDepth);
__JSONStackEntryRef __JSONStackGetTop           (__JSONStackRef stack);
BOOL                __JSONStackPush             (__JSONStackRef stack, __JSONStackEntryRef entry);
__JSONStackEntryRef __JSONStackPop              (__JSONStackRef stack);
BOOL                __JSONStackAppendValueAtTop (__JSONStackRef stack, CFTypeRef value);
BOOL                __JSONStackAppendKeyAtTop   (__JSONStackRef stack, CFTypeRef key);

typedef struct {
  CFAllocatorRef allocator;
  CFIndex retainCount;
  
  yajl_handle        yajlParser;
  yajl_parser_config yajlParserConfig;
  yajl_status        yajlParserStatus;
  yajl_callbacks     yajlParserCallbacks;
  
  yajl_gen           yajlGenerator;
  yajl_gen_config    yajlGeneratorConfig;
  yajl_status        yajlGeneratorStatus;
  
  CFMutableArrayRef  elements;
  __JSONStackRef     stack;
  
} CoreJSON;

typedef CoreJSON *CoreJSONRef;

#pragma Parser callbacks

int         JSONParserAppendStringWithBytes    (void *context, const unsigned char *value, unsigned int length);
int         JSONParserAppendNull               (void *context);
int         JSONParserAppendBooleanWithInteger (void *context, int value);
int         JSONParserAppendNumberWithLong     (void *context, long value);
int         JSONParserAppendNumberWithDouble   (void *context, double value);
int         JSONParserAppendMapKeyWithBytes    (void *context, const unsigned char *value, unsigned int length);
int         JSONParserAppendMapStart           (void *context);
int         JSONParserAppendMapEnd             (void *context);
int         JSONParserAppendArrayStart         (void *context);
int         JSONParserAppendArrayEnd           (void *context);

#pragma API functions

CoreJSONRef JSONCreate           (CFAllocatorRef allocator);
CoreJSONRef JSONCreateWithString (CFAllocatorRef allocator, CFStringRef string);
void        JSONParseWithString  (CoreJSONRef json, CFStringRef string);
