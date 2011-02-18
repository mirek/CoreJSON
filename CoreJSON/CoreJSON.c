//
// CoreJSON.c
// CoreJSON Framework
//
// Copyright 2011 Mirek Rusin <mirek [at] me [dot] com>
//                http://github.com/mirek/CoreJSON
//

#include "CoreJSON.h"

#pragma Internal string helper for fast UTF8 buffer access

inline __JSONUTF8String __JSONUTF8StringMake(CFAllocatorRef allocator, CFStringRef string) {
  __JSONUTF8String utf8String;
  utf8String.allocator = allocator;
  utf8String.string = string;
  utf8String.maximumSize = CFStringGetMaximumSizeForEncoding(CFStringGetLength(string), kCFStringEncodingUTF8) + 1;
  if ((utf8String.pointer = (const unsigned char *)CFStringGetCStringPtr(string, kCFStringEncodingUTF8))) {
    utf8String.buffer = NULL;
  } else {
    utf8String.buffer = CFAllocatorAllocate(allocator, utf8String.maximumSize, 0);
    if (utf8String.buffer) {
      CFStringGetCString(string, (char *)utf8String.buffer, utf8String.maximumSize, kCFStringEncodingUTF8);
    }
  }
  return utf8String;
}

inline const unsigned char *__JSONUTF8StringGetBuffer(__JSONUTF8String utf8String) {
  return utf8String.pointer ? utf8String.pointer : utf8String.buffer;
}

inline CFIndex __JSONUTF8StringGetMaximumSize(__JSONUTF8String utf8String) {
  return utf8String.maximumSize;
}

inline void __JSONUTF8StringDestroy(__JSONUTF8String utf8String) {
  if (utf8String.buffer) {
    CFAllocatorDeallocate(utf8String.allocator, (void *)utf8String.buffer);
  }
}

#pragma Internal stack

inline __JSONStackEntryRef __JSONStackEntryCreate(CFAllocatorRef allocator, CFIndex index, CFIndex valuesInitialSize, CFIndex keysInitialSize) {
  __JSONStackEntryRef entry = CFAllocatorAllocate(allocator, sizeof(__JSONStackEntry), 0);
  if (entry) {
    entry->allocator = allocator ? CFRetain(allocator) : NULL;
    entry->retainCount = 1;
    entry->index = index;
    entry->valuesIndex = 0;
    if ((entry->valuesSize = valuesInitialSize))
      entry->values = CFAllocatorAllocate(entry->allocator, entry->valuesSize, 0);
    else
      entry->values = NULL;
    entry->keysIndex = 0;
    if ((entry->keysSize = keysInitialSize))
      entry->keys = CFAllocatorAllocate(entry->allocator, entry->keysSize, 0);
    else
      entry->keys = NULL;
  }
  return entry;
}

inline __JSONStackEntryRef __JSONStackEntryRetain(__JSONStackEntryRef entry) {
  entry->retainCount++;
  return entry;
}

inline CFIndex __JSONStackEntryRelease(__JSONStackEntryRef entry) {
  if (--entry->retainCount == 0) {
    CFAllocatorRef allocator = entry->allocator;
    if (entry->values)
      CFAllocatorDeallocate(allocator, entry->values);
    if (entry->keys)
      CFAllocatorDeallocate(allocator, entry->keys);
    CFAllocatorDeallocate(allocator, entry);
    if (allocator)
      CFRelease(allocator);
  }
  return entry->retainCount;
}

inline bool __JSONStackEntryAppendValue(__JSONStackEntryRef entry, CFTypeRef value) {
  bool success = 0;
  if (entry) {
    if (entry->valuesIndex == entry->valuesSize) { // Reallocate more space
      CFIndex largerSize = entry->valuesSize ? entry->valuesSize << 1 : 1024;
      CFTypeRef *largerValues = CFAllocatorReallocate(entry->allocator, entry->values, largerSize, 0);
      if (largerValues) {
        entry->valuesSize = largerSize;
        entry->values = largerValues;
      }
    }
    if (entry->valuesIndex < entry->valuesSize) {
      entry->values[entry->valuesIndex++] = value;
      success = 1;
    }
  }
  return success;
}

inline bool __JSONStackEntryAppendKey(__JSONStackEntryRef entry, CFTypeRef key) {
  bool success = 0;
  if (entry) {
    if (entry->keysIndex == entry->keysSize) { // Reallocate more space
      CFIndex largerSize = entry->keysSize ? entry->keysSize << 1 : 1024;
      CFTypeRef *largerKeys = CFAllocatorReallocate(entry->allocator, entry->keys, largerSize, 0);
      if (largerKeys) {
        entry->keysSize = largerSize;
        entry->keys = largerKeys;
      }
    }
    if (entry->keysIndex < entry->keysSize) {
      entry->keys[entry->keysIndex++] = key;
      success = 1;
    }
  }
  return success;
}

inline __JSONStackRef __JSONStackCreate(CFAllocatorRef allocator, CFIndex initialSize) {
  __JSONStackRef stack = CFAllocatorAllocate(allocator, sizeof(__JSONStack), 0);
  if (stack) {
    stack->allocator = allocator ? CFRetain(allocator) : NULL;
    stack->retainCount = 1;
    stack->size = initialSize;
    stack->index = 0;
    stack->stack = CFAllocatorAllocate(stack->allocator, sizeof(__JSONStackEntryRef) * stack->size, 0);
    memset(stack->stack, 0, sizeof(__JSONStackEntryRef) * stack->size);
  }
  return stack;
}

inline CFIndex __JSONStackRelease(__JSONStackRef stack) {
  CFIndex retainCount = 0;
  if (stack) {
    if ((retainCount = --stack->retainCount) == 0) {
      CFAllocatorRef allocator = stack->allocator;
      if (stack->stack)
        CFAllocatorDeallocate(allocator, stack->stack);
      if (allocator)
        CFRelease(allocator);
    }
  }
  return retainCount;
}

inline __JSONStackEntryRef __JSONStackGetTop(__JSONStackRef stack) {
  if (stack->index > 0)
    return stack->stack[stack->index - 1];
  else
    return NULL;
}

inline bool __JSONStackPush(__JSONStackRef stack, __JSONStackEntryRef entry) {
  bool success = 0;
  if (stack && entry) {
    
    // Do we need more space? Reallocate to 2 * current size.
    if (stack->index == stack->size) {
      CFIndex largerSize = stack->size ? stack->size << 1 : 1024;
      __JSONStackEntryRef *largerStack = CFAllocatorReallocate(stack->allocator, stack->stack, largerSize, 0);
      if (largerStack) {
        stack->size = largerSize;
        stack->stack = largerStack;
      }
    }
    if (stack->index < stack->size) {
      stack->stack[stack->index++] = __JSONStackEntryRetain(entry);
      success = 1;
    }
  }
  return success;
}

inline __JSONStackEntryRef __JSONStackPop(__JSONStackRef stack) {
  __JSONStackEntryRef entry = NULL;
  if (stack) {
    // TODO: __JSONStackEntryRelease(stack->stack[stack->index])
    if (--stack->index >= 0) {
      entry = stack->stack[stack->index];
    } else {
      // TODO: Out of bounds, this is error, should never happen
    }
  }
  return entry;
}

inline bool __JSONStackAppendValueAtTop(__JSONStackRef stack, CFTypeRef value) {
  bool success = 0;
  if (stack) {
    __JSONStackEntryRef entry = __JSONStackGetTop(stack);
    if (entry) {
      __JSONStackEntryAppendValue(entry, value);
      success = 1;
    }
  }
  return success;
}

inline bool __JSONStackAppendKeyAtTop(__JSONStackRef stack, CFTypeRef key) {
  bool success = 0;
  if (stack) {
    __JSONStackEntryRef entry = __JSONStackGetTop(stack);
    if (entry) {
      __JSONStackEntryAppendKey(entry, key);
      success = 1;
    }
  }
  return success;
}

#pragma Parser callbacks

inline int __JSONParserAppendStringWithBytes(void *context, const unsigned char *value, unsigned int length) {
  CoreJSONRef json = (CoreJSONRef)context;
  CFStringRef string = CFStringCreateWithBytes(json->allocator, value, length, kCFStringEncodingUTF8, 0);
  CFArrayAppendValue(json->elements, string);
  __JSONStackAppendValueAtTop(json->stack, string);
  CFRelease(string);
  return 1;
}

inline int __JSONParserAppendNull(void *context) {
  CoreJSONRef json = (CoreJSONRef)context;
  CFArrayAppendValue(json->elements, kCFNull);
  __JSONStackAppendValueAtTop(json->stack, kCFNull);
  return 1;
}

inline int __JSONParserAppendBooleanWithInteger(void *context, int value) {
  CoreJSONRef json = (CoreJSONRef)context;
  CFBooleanRef boolean = value ? kCFBooleanTrue : kCFBooleanFalse;
  CFArrayAppendValue(json->elements, boolean);
  __JSONStackAppendValueAtTop(json->stack, boolean);
  return 1;
}

inline int __JSONParserAppendNumberWithBytes(void *context, const char *value, unsigned int length) {
  CoreJSONRef json = (CoreJSONRef)context;
  CFNumberRef number = NULL;

  // TODO: anybody how to do it properly?
  bool looksLikeFloat = 0;
  for (int i = 0; i < length; i++)
    if (value[i] == '.' || value[i] == 'e' || value[i] == 'E')
      looksLikeFloat = 1;

  if (looksLikeFloat) {
    double value_ = strtod((char *)value, NULL);
    number = CFNumberCreate(json->allocator, kCFNumberDoubleType, &value_);
  } else {
    long long value_ = strtoll((char *)value, NULL, 0);
    number = CFNumberCreate(json->allocator, kCFNumberLongLongType, &value_);
  }
  
  CFArrayAppendValue(json->elements, number);
  __JSONStackAppendValueAtTop(json->stack, number);
  CFRelease(number);
  return 1;
}

inline int __JSONParserAppendNumberWithLong(void *context, long value) {
  CoreJSONRef json = (CoreJSONRef)context;
  CFNumberRef number = CFNumberCreate(json->allocator, kCFNumberLongType, &value);
  CFArrayAppendValue(json->elements, number);
  __JSONStackAppendValueAtTop(json->stack, number);
  CFRelease(number);
  return 1;
}

inline int __JSONParserAppendNumberWithDouble(void *context, double value) {
  CoreJSONRef json = (CoreJSONRef)context;
  CFNumberRef number = CFNumberCreate(json->allocator, kCFNumberDoubleType, &value);
  CFArrayAppendValue(json->elements, number);
  __JSONStackAppendValueAtTop(json->stack, number);
  CFRelease(number);
  return 1;
}

inline int __JSONParserAppendMapKeyWithBytes(void *context, const unsigned char *value, unsigned int length) {
  CoreJSONRef json = (CoreJSONRef)context;
  CFStringRef string = CFStringCreateWithBytes(json->allocator, value, length, kCFStringEncodingUTF8, 0);
  CFArrayAppendValue(json->elements, string);
  __JSONStackAppendKeyAtTop(json->stack, string);
  CFRelease(string);
  return 1;
}

inline int __JSONParserAppendMapStart(void *context) {
  CoreJSONRef json = (CoreJSONRef)context;
  
  // Placeholder for the CFDictionaryRef which will come later
  CFMutableDictionaryRef dictionary = CFDictionaryCreateMutable(json->allocator, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
  CFArrayAppendValue(json->elements, dictionary);
  __JSONStackAppendValueAtTop(json->stack, dictionary);
  CFRelease(dictionary);
  
  // New container to the stack
  __JSONStackPush(json->stack, __JSONStackEntryCreate(json->allocator, CFArrayGetCount(json->elements) - 1, 1024, 1024));
  
  return 1;
}

inline int __JSONParserAppendMapEnd(void *context) {
  CoreJSONRef json = (CoreJSONRef)context;
  __JSONStackEntryRef entry = __JSONStackPop(json->stack);
  
  // TODO: Optimize creating dict
  // CFDictionaryRef dictionary = CFDictionaryCreate(json->allocator, entry->keys, entry->values, entry->valuesIndex, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
  // CFArraySetValueAtIndex(json->elements, entry->index, dictionary);
  
  CFMutableDictionaryRef d = (CFMutableDictionaryRef)CFArrayGetValueAtIndex(json->elements, entry->index);
  for (int i = 0; i < entry->keysIndex; i++)
    CFDictionaryAddValue(d, entry->keys[i], entry->values[i]);
  
  __JSONStackEntryRelease(entry);
  return 1;
}

inline int __JSONParserAppendArrayStart(void *context) {
  CoreJSONRef json = (CoreJSONRef)context;
  
  // Let's save this space in elements for the array. When we
  // reach the end of the array, we'll create one in this place.
  CFMutableArrayRef array = CFArrayCreateMutable(json->allocator, 0, &kCFTypeArrayCallBacks);
  CFArrayAppendValue(json->elements, array);
  __JSONStackAppendValueAtTop(json->stack, array);
  CFRelease(array);
  
  // Add the container to the stack. Container element is just
  // 3-value array [container index in the elements array, 
  __JSONStackEntryRef entry = __JSONStackEntryCreate(json->allocator, CFArrayGetCount(json->elements) - 1, 1024, 0);
  __JSONStackPush(json->stack, entry);
  __JSONStackEntryRelease(entry);
  
  return 1;
}

inline int __JSONParserAppendArrayEnd(void *context) {
  CoreJSONRef json = (CoreJSONRef)context;
  __JSONStackEntryRef entry = __JSONStackPop(json->stack);
  
  CFArrayRef array = CFArrayCreate(json->allocator, entry->values, entry->valuesIndex, &kCFTypeArrayCallBacks);
  CFMutableArrayRef a2 = (CFMutableArrayRef)CFArrayGetValueAtIndex(json->elements, entry->index);
  CFArrayAppendArray(a2, array, CFRangeMake(0, CFArrayGetCount(array)));
  //CFArraySetValueAtIndex(json->elements, entry->index, array);
  CFRelease(array);
  
  __JSONStackEntryRelease(entry);
  return 1;
}

#pragma Public API

inline CoreJSONRef JSONCreate(CFAllocatorRef allocator) {
  CoreJSONRef json = CFAllocatorAllocate(allocator, sizeof(CoreJSON), 0);
  if (json) {
    json->allocator = allocator ? CFRetain(allocator) : NULL;
    
    json->yajlParserCallbacks.yajl_null        = __JSONParserAppendNull;
    json->yajlParserCallbacks.yajl_boolean     = __JSONParserAppendBooleanWithInteger;
    
    // Set number or integer and double. Never all 3.
    json->yajlParserCallbacks.yajl_number      = __JSONParserAppendNumberWithBytes;
    json->yajlParserCallbacks.yajl_integer     = NULL; // __JSONParserAppendNumberWithLong
    json->yajlParserCallbacks.yajl_double      = NULL; // __JSONParserAppendNumberWithDouble
    
    json->yajlParserCallbacks.yajl_start_map   = __JSONParserAppendMapStart;
    json->yajlParserCallbacks.yajl_map_key     = __JSONParserAppendMapKeyWithBytes;
    json->yajlParserCallbacks.yajl_end_map     = __JSONParserAppendMapEnd;
    
    json->yajlParserCallbacks.yajl_start_array = __JSONParserAppendArrayStart;
    json->yajlParserCallbacks.yajl_end_array   = __JSONParserAppendArrayEnd;
    
    json->yajlParserCallbacks.yajl_string      = __JSONParserAppendStringWithBytes;
    
    json->yajlParserConfig.allowComments = 1;
    json->yajlParserConfig.checkUTF8 = 1;
    json->yajlParser = yajl_alloc(&json->yajlParserCallbacks, &json->yajlParserConfig, NULL, (void *)json);
    
    json->yajlGeneratorConfig.beautify = 1;
    json->yajlGeneratorConfig.indentString = "  ";
    json->yajlGenerator = yajl_gen_alloc(&json->yajlGeneratorConfig, NULL);
    
    json->elements = CFArrayCreateMutable(json->allocator, 0, &kCFTypeArrayCallBacks);
    json->stack = __JSONStackCreate(json->allocator, CORE_JSON_STACK_MAX_DEPTH);
  }
  return json;
}

inline CFIndex JSONRelease(CoreJSONRef json) {
  CFIndex retainCount = 0;
  if (json) {
    if ((retainCount = --json->retainCount) == 0) {
      CFAllocatorRef allocator = json->allocator;
      
      yajl_free(json->yajlParser);
      // TODO: release generator
      
      CFRelease(json->elements);
      __JSONStackRelease(json->stack);
      CFAllocatorDeallocate(allocator, json);
      
      if (allocator)
        CFRelease(allocator);
    }
  }
  return retainCount;
}

inline void JSONParseWithString(CoreJSONRef json, CFStringRef string) {
  
  // Let's make sure we've got a clean plate first
  CFArrayRemoveAllValues(json->elements);
  
  __JSONUTF8String utf8 = __JSONUTF8StringMake(json->allocator, string);
  if ((json->yajlParserStatus = yajl_parse(json->yajlParser, __JSONUTF8StringGetBuffer(utf8), (unsigned int)__JSONUTF8StringGetMaximumSize(utf8))) != yajl_status_ok) {
    // TODO: Error stuff
    //printf("ERROR: %s\n", yajl_get_error(json->yajlParser, 1, __JSONUTF8StringGetBuffer(utf8), __JSONUTF8StringGetMaximumSize(utf8)));
  }
  __JSONUTF8StringDestroy(utf8);
  
  json->yajlParserStatus = yajl_parse_complete(json->yajlParser);
  
  yajl_free(json->yajlParser);
}

inline CoreJSONRef JSONCreateWithString(CFAllocatorRef allocator, CFStringRef string) {
  CoreJSONRef json = JSONCreate(allocator);
  if (json) {
    JSONParseWithString(json, string);
  }
  return json;
}

inline CFTypeRef JSONGetObject(CoreJSONRef json) {
  return CFArrayGetValueAtIndex(json->elements, 0);
}
