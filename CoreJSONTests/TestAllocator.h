//
//  TestAllocator.h
//  SQLite3
//
//  Created by Mirek Rusin on 21/02/2011.
//  Copyright 2011 Inteliv Ltd. All rights reserved.
//

#include <CoreFoundation/CoreFoundation.h>
#include <execinfo.h>

typedef struct TestAllocatorInfo {
  CFAllocatorRef allocator;
  CFIndex retainCount;
  CFIndex allocationsCount;
  CFIndex reallocationsCount;
  CFIndex deallocationsCount;
  
  CFMutableArrayRef addresses;
  CFMutableDictionaryRef addressBacktraces;
  
} TestAllocatorInfo;

typedef TestAllocatorInfo *TestAllocatorInfoRef;

CFAllocatorRef TestAllocatorCreate                ();

const void    *TestAllocatorRetainCall            (const void *info);
void           TestAllocatorReleaseCall           (const void *info);
CFStringRef    TestAllocatorCreateDescriptionCall (const void *info);
void          *TestAllocatorAllocateCall          (CFIndex allocSize, CFOptionFlags hint, void *info);
void          *TestAllocatorReallocateCall        (void *ptr, CFIndex newsize, CFOptionFlags hint, void *info);
void           TestAllocatorDeallocateCall        (void *ptr, void *info);
CFIndex        TestAllocatorPreferedSizeCall      (CFIndex size, CFOptionFlags hint, void *info);

CFIndex        TestAllocatorGetAllocationsCount   (CFAllocatorRef allocator);
CFIndex        TestAllocatorGetDeallocationsCount (CFAllocatorRef allocator);

TestAllocatorInfoRef TestAllocatorGetInfo(CFAllocatorRef allocator);

// Get backtraces mutable array. Empty array is created in the background if it doesn't exist for specified address.
// Returns mutable array with zero or more backtraces, never NULL.
CFMutableArrayRef TestAllocatorInfoGetBacktraces(TestAllocatorInfoRef info, CFStringRef address);
CFMutableArrayRef TestAllocatorGetBacktraces(CFAllocatorRef allocator, CFStringRef address);


CFMutableArrayRef TestAllocatorInfoGetAddresses(TestAllocatorInfoRef info);
CFMutableArrayRef  TestAllocatorGetAddresses(CFAllocatorRef allocator);

CFStringRef TestAllocatorCreateTraceString();


CFMutableDictionaryRef TestAllocatorInfoCreateAddressesAndBacktracesDictionary(TestAllocatorInfoRef info);
CFMutableDictionaryRef TestAllocatorCreateAddressesAndBacktracesDictionary(CFAllocatorRef allocator);

void TestAllocatorPrintAddressesAndBacktraces(CFAllocatorRef allocator);

CFStringRef TestAllocatorCreateStringWithPointer(void *pointer);

