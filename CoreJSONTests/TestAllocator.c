//
//  TestAllocator.c
//  SQLite3
//
//  Created by Mirek Rusin on 21/02/2011.
//  Copyright 2011 Inteliv Ltd. All rights reserved.
//

#include "TestAllocator.h"

CFAllocatorRef TestAllocatorCreate() {
  TestAllocatorInfoRef testAllocatorInfo = CFAllocatorAllocate(NULL, sizeof(TestAllocatorInfo), 0);
  testAllocatorInfo->retainCount = 1;
  testAllocatorInfo->allocationsCount = 0;
  testAllocatorInfo->reallocationsCount = 0;
  testAllocatorInfo->deallocationsCount = 0;
  testAllocatorInfo->addresses = CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks);
  testAllocatorInfo->addressBacktraces = CFDictionaryCreateMutable(NULL, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
  
  CFAllocatorContext *context = (CFAllocatorContext *)CFAllocatorAllocate(NULL, sizeof(CFAllocatorContext), 0);
	context->info = testAllocatorInfo;
	context->version = 0;
	context->retain = TestAllocatorRetainCall;
	context->release = TestAllocatorReleaseCall;
	context->copyDescription = TestAllocatorCreateDescriptionCall;
	context->allocate = TestAllocatorAllocateCall;
	context->reallocate = TestAllocatorReallocateCall;
	context->deallocate = TestAllocatorDeallocateCall;
	context->preferredSize = TestAllocatorPreferedSizeCall;
	CFAllocatorRef allocator = CFAllocatorCreate(kCFAllocatorDefault, context);
  
  testAllocatorInfo->allocator = allocator;
  
	return allocator;
}

const void *TestAllocatorRetainCall(const void *info) {
	((TestAllocatorInfoRef)info)->retainCount++;
	return info;
}

void TestAllocatorReleaseCall(const void *info) {
	if (0 == --((TestAllocatorInfoRef)info)->retainCount) {
    CFRelease(((TestAllocatorInfoRef)info)->addresses);
    CFRelease(((TestAllocatorInfoRef)info)->addressBacktraces);
    CFAllocatorDeallocate(NULL, (void *)info);
  }
}

CFStringRef TestAllocatorCreateDescriptionCall(const void *info) {
  return CFStringCreateWithFormat(NULL, NULL, CFSTR("<TestAllocator %i retain count, %i allocations, %i deallocations, %i reallocations, >"),
                                  ((TestAllocatorInfoRef)info)->retainCount,
                                  ((TestAllocatorInfoRef)info)->allocationsCount,
                                  ((TestAllocatorInfoRef)info)->deallocationsCount,
                                  ((TestAllocatorInfoRef)info)->reallocationsCount);
}

void *TestAllocatorAllocateCall(CFIndex allocSize, CFOptionFlags hint, void *info) {
  void *pointer = CFAllocatorAllocate(NULL, allocSize, hint);
  CFStringRef address = TestAllocatorCreateStringWithPointer(pointer);
  CFArrayAppendValue(((TestAllocatorInfoRef)info)->addresses, address);
  CFStringRef backtrace = TestAllocatorCreateTraceString();
  CFMutableArrayRef backtraces = TestAllocatorInfoGetBacktraces(info, address);
  CFArrayAppendValue(backtraces, backtrace);
  CFRelease(backtrace);
  CFRelease(address);
  ((TestAllocatorInfoRef)info)->allocationsCount++;
  return pointer;
}

void *TestAllocatorReallocateCall(void *ptr, CFIndex newsize, CFOptionFlags hint, void *info) {
  ((TestAllocatorInfoRef)info)->reallocationsCount++;
  return CFAllocatorReallocate(NULL, ptr, newsize, hint);
}

void TestAllocatorDeallocateCall(void *ptr, void *info) {
  CFStringRef address = TestAllocatorCreateStringWithPointer(ptr);
  CFMutableArrayRef addresses = TestAllocatorInfoGetAddresses(info);
  CFIndex index = CFArrayGetLastIndexOfValue(addresses, CFRangeMake(0, CFArrayGetCount(addresses)), address);
  if (index == kCFNotFound) {
    CFStringRef backtrace = TestAllocatorCreateTraceString();
    printf("Trying to deallocate pointer which was not allocated by this allocator:\n");
    CFShow(backtrace);
    CFRelease(backtrace);
    assert(0); // Pointer to deallocate has not been found, this is bad
  } else {
    CFArrayRemoveValueAtIndex(addresses, index);
  }
  CFRelease(address);
  ((TestAllocatorInfoRef)info)->deallocationsCount++;
  CFAllocatorDeallocate(NULL, ptr);
}

CFIndex TestAllocatorPreferedSizeCall(CFIndex size, CFOptionFlags hint, void *info) {
  return CFAllocatorGetPreferredSizeForSize(NULL, size, hint);
}

CFIndex TestAllocatorGetAllocationsCount(CFAllocatorRef allocator) {
  CFIndex allocationsCount = -1;
  CFAllocatorContext context;
  CFAllocatorGetContext(allocator, &context);
  allocationsCount = ((TestAllocatorInfoRef)context.info)->allocationsCount;
  return allocationsCount;
}

CFIndex TestAllocatorGetDeallocationsCount(CFAllocatorRef allocator) {
  CFIndex deallocationsCount = -1;
  CFAllocatorContext context;
  CFAllocatorGetContext(allocator, &context);
  deallocationsCount = ((TestAllocatorInfoRef)context.info)->deallocationsCount;
  return deallocationsCount;
}

CFStringRef TestAllocatorCreateTraceString() {
  CFStringRef trace = NULL;
  CFMutableArrayRef array = CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks);
  void *addresses[1024];
  int size = backtrace(addresses, 1024);
  char **symbols = backtrace_symbols(addresses, size);
  for (int i = 0; i < size; i++) {
    CFStringRef symbol = CFStringCreateWithCString(NULL, symbols[i], kCFStringEncodingUTF8);
    CFArrayAppendValue(array, symbol);
    CFRelease(symbol);
  }
  free(symbols);
  trace = CFStringCreateByCombiningStrings(NULL, array, CFSTR("\n"));
  CFRelease(array);
  return trace;
}

CFMutableArrayRef TestAllocatorGetBacktraces(CFAllocatorRef allocator, CFStringRef address) {
  CFMutableArrayRef backtraces = NULL;
  TestAllocatorInfoRef info = TestAllocatorGetInfo(allocator);
  if (info)
    backtraces = TestAllocatorInfoGetBacktraces(info, address);
  return backtraces;
}

CFMutableArrayRef TestAllocatorInfoGetBacktraces(TestAllocatorInfoRef info, CFStringRef address) {
  CFMutableArrayRef backtraces = NULL;
  if (info) {
    backtraces = (CFMutableArrayRef)CFDictionaryGetValue(info->addressBacktraces, address);
    if (backtraces == NULL) {
      CFMutableArrayRef emptyBacktraces = CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks);
      CFDictionarySetValue(info->addressBacktraces, address, emptyBacktraces);
      CFRelease(emptyBacktraces);
      
      // Retained by the dictionary, no worries about this line
      backtraces = emptyBacktraces;
    }
  }
  return backtraces;
}

TestAllocatorInfoRef TestAllocatorGetInfo(CFAllocatorRef allocator) {
  CFAllocatorContext context;
  CFAllocatorGetContext(allocator, &context);
  return context.info;
}

CFMutableArrayRef TestAllocatorInfoGetAddresses(TestAllocatorInfoRef info) {
  return ((TestAllocatorInfoRef)info)->addresses;
}

CFMutableArrayRef TestAllocatorGetAddresses(CFAllocatorRef allocator) {
  CFMutableArrayRef addresses = NULL;
  TestAllocatorInfoRef info = TestAllocatorGetInfo(allocator);
  if (info)
    addresses = TestAllocatorInfoGetAddresses(info);
  return addresses;
}

CFMutableDictionaryRef TestAllocatorInfoCreateAddressesAndBacktracesDictionary(TestAllocatorInfoRef info) {
  CFMutableDictionaryRef dictionary = CFDictionaryCreateMutable(NULL, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
  if (info) {
    CFIndex n = CFArrayGetCount(info->addresses);
    for (CFIndex i = 0; i < n; i++) {
      CFStringRef address = CFArrayGetValueAtIndex(info->addresses, i);
      if (NULL == CFDictionaryGetValue(dictionary, address))
        CFDictionarySetValue(dictionary, address, CFDictionaryGetValue(info->addressBacktraces, address));
    }
  }
  return dictionary;
}

CFMutableDictionaryRef TestAllocatorCreateAddressesAndBacktracesDictionary(CFAllocatorRef allocator) {
  CFMutableDictionaryRef dictionary = NULL;
  TestAllocatorInfoRef info = TestAllocatorGetInfo(allocator);
  if (info)
    dictionary = TestAllocatorInfoCreateAddressesAndBacktracesDictionary(info);
  return dictionary;
}

void TestAllocatorPrintAddressesAndBacktraces(CFAllocatorRef allocator) {
  CFMutableDictionaryRef dictionary = TestAllocatorCreateAddressesAndBacktracesDictionary(allocator);
  CFIndex n = CFDictionaryGetCount(dictionary);
  if (n > 0) {
    CFStringRef *keys = CFAllocatorAllocate(NULL, sizeof(CFStringRef) * n, 0);
    CFArrayRef *values = CFAllocatorAllocate(NULL, sizeof(CFArrayRef) * n, 0);
    CFDictionaryGetKeysAndValues(dictionary, (const void **)keys, (const void **)values);
    for (CFIndex i = 0; i < n; i++) {
      CFIndex keyLength = CFStringGetMaximumSizeForEncoding(CFStringGetLength(keys[i]), kCFStringEncodingUTF8);
      char *key = CFAllocatorAllocate(NULL, keyLength, 0);
      CFStringGetCString(keys[i], key, keyLength, kCFStringEncodingUTF8);
      
      printf("\n%s address not deallocated, allocated from %ld place(s):\n", key, CFArrayGetCount(values[i]));
      
      for (CFIndex j = 0; j < CFArrayGetCount(values[i]); j++) {
        CFIndex valueLength = CFStringGetMaximumSizeForEncoding(CFStringGetLength(CFArrayGetValueAtIndex(values[i], j)), kCFStringEncodingUTF8);
        char *value = CFAllocatorAllocate(NULL, valueLength, 0);
        CFStringGetCString(CFArrayGetValueAtIndex(values[i], j), value, valueLength, kCFStringEncodingUTF8);
        
        printf("%s\n", value);
        
        CFAllocatorDeallocate(NULL, value);
      }
      
      CFAllocatorDeallocate(NULL, key);
    }
    CFAllocatorDeallocate(NULL, values);
    CFAllocatorDeallocate(NULL, keys);
  }
  CFRelease(dictionary);
}

CFStringRef TestAllocatorCreateStringWithPointer(void *pointer) {
  char address[12];
  sprintf(address, "%p", pointer);
  return CFStringCreateWithCString(NULL, address, kCFStringEncodingASCII);
}
