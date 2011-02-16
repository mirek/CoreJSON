//
// CoreJSON.h
// CoreJSON Framework
//
// Copyright 2011 Mirek Rusin <mirek [at] me [dot] com>
//                http://github.com/mirek/CoreJSON
//

#import <CoreFoundation/CoreFoundation.h>
#import <yajl/yajl_parse.h>
#import <yajl/yajl_gen.h>

#define CORE_JSON_STACK_MAX_DEPTH 1024

#pragma Internal string helper for fast UTF8 buffer access

// Internal helper structure supporting fast and easy access to CFStringRef's
// string buffer. Whenever possible getting the buffer is O(1) without copying
// CFStringRef's buffer. If not possible buffer is being copied.
//
// Provided CFStringRef has to be valid for the lifetime of the struct.
// 
// When finished invoke __JSONUTF8StringDestroy(...) to deallocate internal
// data properly. Internal members of this struct should not be accessed directly.
// Use provided functions instead.
//
// This structure is not opaque and is not intended to be passed as function
// argument. CFStringRef should be passed instead. The structure should be
// used inside the function to get access to UTF8 buffer of CFStringRef.
typedef struct {
  CFAllocatorRef allocator;
  CFStringRef string;
  const unsigned char *pointer;
  const unsigned char *buffer;
  CFIndex maximumSize;
} __JSONUTF8String;

// Internal function, use it to instantiate __JSONUTF8String structure to get
// fast and easy access to CFStringRef's UTF8 buffer.
//
// The function takes advantage of Core Foundation CFStringRef and tries to get buffer
// pointer in 0(1). If not possible, the buffer is being copied to internally allocated
// storage.
//
// Returns properly initialized __JSONUTF8String struct. Use __JSONUTF8StringGetBuffer
// to get the UTF8 buffer. Invoke __JSONUTF8StringDestroy to deallocate this struct properly.
__JSONUTF8String     __JSONUTF8StringMake           (CFAllocatorRef allocator, CFStringRef string);

// Internal function, get the internal buffer of associated CFStringRef.
const unsigned char *__JSONUTF8StringGetBuffer      (__JSONUTF8String utf8String);

// Get the size of the internal buffer
CFIndex              __JSONUTF8StringGetMaximumSize (__JSONUTF8String utf8String);

// Deallocate internal members of __JSONUTF8String stuct.
void                 __JSONUTF8StringDestroy        (__JSONUTF8String utf8String);

#pragma Helper stack for parsing

// Internal stack to hold containers when parsing. Practically all functions in most
// cases are O(1), unless internal buffers are full and need to be reallocated.
//
// This stucture is also used to create CFArrayRef for arrays and CFDictionaryRef for
// maps by a single create call.
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
bool                __JSONStackPush             (__JSONStackRef stack, __JSONStackEntryRef entry);
__JSONStackEntryRef __JSONStackPop              (__JSONStackRef stack);
bool                __JSONStackAppendValueAtTop (__JSONStackRef stack, CFTypeRef value);
bool                __JSONStackAppendKeyAtTop   (__JSONStackRef stack, CFTypeRef key);

#pragma Internal callbacks for libyajl parser

int JSONParserAppendStringWithBytes    (void *context, const unsigned char *value, unsigned int length);
int JSONParserAppendNull               (void *context);
int JSONParserAppendBooleanWithInteger (void *context, int value);
int JSONParserAppendNumberWithLong     (void *context, long value);
int JSONParserAppendNumberWithDouble   (void *context, double value);
int JSONParserAppendMapKeyWithBytes    (void *context, const unsigned char *value, unsigned int length);
int JSONParserAppendMapStart           (void *context);
int JSONParserAppendMapEnd             (void *context);
int JSONParserAppendArrayStart         (void *context);
int JSONParserAppendArrayEnd           (void *context);

#pragma Public API

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

extern CoreJSONRef JSONCreate           (CFAllocatorRef allocator);
extern CoreJSONRef JSONCreateWithString (CFAllocatorRef allocator, CFStringRef string);
extern void        JSONParseWithString  (CoreJSONRef json, CFStringRef string);
