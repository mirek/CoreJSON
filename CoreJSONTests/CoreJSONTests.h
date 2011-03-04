//
//  CoreJSONTests.h
//  CoreJSONTests
//
//  Created by Mirek Rusin on 16/02/2011.
//  Copyright 2011 Inteliv Ltd. All rights reserved.
//

#import <SenTestingKit/SenTestingKit.h>
#import "CoreJSON.h"
#import "time.h"
#import "TestAllocator.h"

#define MAX_FRAMES 100

//void demangle(const char* symbol) {
//  size_t size;
//  int status;
//  char temp[128];
//  char* demangled;
//  //first, try to demangle a c++ name
//  if (1 == sscanf(symbol, "%*[^(]%*[^_]%127[^)+]", temp)) {
//    if (NULL != (demangled = abi::__cxa_demangle(temp, NULL, &size, &status))) {
//      std::string result(demangled);
//      free(demangled);
//      return result;
//    }
//  }
//  //if that didn't work, try to get a regular c symbol
//  if (1 == sscanf(symbol, "%127s", temp)) {
//    return temp;
//  }
//  
//  //if all else fails, just return the symbol
//  return symbol;
//}

@interface CoreJSONTests : SenTestCase {
@private
  CFAllocatorRef testAllocator;
}

@end
