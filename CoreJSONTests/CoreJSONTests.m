//
//  CoreJSONTests.m
//  CoreJSONTests
//
//  Created by Mirek Rusin on 16/02/2011.
//  Copyright 2011 Inteliv Ltd. All rights reserved.
//

#import "CoreJSONTests.h"


@implementation CoreJSONTests

- (void) setUp {
  [super setUp];
  testAllocator = TestAllocatorCreate();
}

- (void) tearDown {
  STAssertTrue(TestAllocatorGetAllocationsCount(testAllocator) > 0, @"Allocations count should be more than 0");
  STAssertTrue(TestAllocatorGetDeallocationsCount(testAllocator) > 0, @"Deallocations count should be more than 0");
  STAssertEquals(TestAllocatorGetAllocationsCount(testAllocator), TestAllocatorGetDeallocationsCount(testAllocator), @"Allocations/deallocations mismatch");

  if (TestAllocatorGetAllocationsCount(testAllocator) != TestAllocatorGetDeallocationsCount(testAllocator))
    TestAllocatorPrintAddressesAndBacktraces(testAllocator);
  
  CFRelease(testAllocator);
  [super tearDown];
}

- (void) testGenerator {
  CFUUIDRef uuid = CFUUIDCreate(NULL);
  
  id object = [NSDictionary dictionaryWithObjectsAndKeys:
               [NSNumber numberWithLongLong: 9223372036854775807], @"number",
               uuid, @"uuid",
               nil];
  
  NSError *error = nil;
  NSString *json = (NSString *)JSONCreateString(testAllocator, object, kJSONWriteOptionsDefault, (CFErrorRef *)&error);
  
  [object release];
  
  [json release];
  [error release];
  
  CFRelease(uuid);
}

- (void) testSimpleStuff {
  {
    NSError *error = nil;
    NSArray *array = (NSArray *)JSONCreateWithString(testAllocator, (CFStringRef)@"[1, 3, 5]", kJSONReadOptionsDefault, (CFErrorRef *)&error);
    STAssertNil(error, @"Error should be nil");
    STAssertNotNil(array, @"Should be array but have nil");
    STAssertTrue([array count] == 3, @"Array should have 3 elements");
    STAssertTrue([[NSNumber numberWithInt: 1] isEqualToNumber: [array objectAtIndex: 0]], @"First element should be 1");
    STAssertTrue([[NSNumber numberWithInt: 3] isEqualToNumber: [array objectAtIndex: 1]], @"Second element should be 3");
    STAssertTrue([[NSNumber numberWithInt: 5] isEqualToNumber: [array objectAtIndex: 2]], @"Third element should be 5");
    [error release];
    [array release];
  }
  {
    NSError *error = nil;
    NSDictionary *dictionary = (NSDictionary *)JSONCreateWithString(testAllocator, (CFStringRef)@"{ \"a\": 1, \"b\": 3, \"c\": 5 }", kJSONReadOptionsDefault, (CFErrorRef *)&error);
    STAssertNil(error, @"Error should be nil");
    STAssertNotNil(dictionary, @"Should be dictionary but have nil");
    STAssertTrue([dictionary count] == 3, @"Dictionary should have 3 entries");
    STAssertTrue([[dictionary objectForKey: @"b"] isEqualToNumber: [NSNumber numberWithInt: 3]], @"'b' value should be 3");
    [error release];
    [dictionary release];
  }
}

- (void) testUTF8Strings {
  {
    NSError *error = nil;
    NSArray *array = (NSArray *)JSONCreateWithString(testAllocator, (CFStringRef)@"[\"a’la\"]", kJSONReadOptionsDefault, (CFErrorRef *)&error);
    STAssertNil(error, @"Error should be nil");
    STAssertNotNil(array, @"Should be array but have nil");
    STAssertTrue([array count] == 1, @"Array should have 1 element");
    STAssertTrue([[array objectAtIndex: 0] isEqualToString: @"a’la"], @"UTF8 string a’la expected");
    [error release];
    [array release];
  }
}

- (void) testLargeNumbers {
  {
    NSError *error = nil;
    NSArray *array = (NSArray *)JSONCreateWithString(testAllocator, (CFStringRef)@"[4294967297, 9223372036854775807, -9223372036854775807]", kJSONReadOptionsDefault, (CFErrorRef *)&error);
    STAssertTrue([array count] == 3, @"Array should have 3 elements");
    STAssertEquals([[array objectAtIndex: 0] longLongValue], 4294967297, @"Large number 2^32+1 expected");
    STAssertEquals([[array objectAtIndex: 1] longLongValue], 9223372036854775807, @"Large number 2^(64-1) expected");
    STAssertEquals([[array objectAtIndex: 2] longLongValue], -9223372036854775807, @"Large number 2^(64-1) expected");
    [error release];
    [array release];
  }
}

- (void) __testFloats {
  {
    NSError *error = nil;
    NSArray *array = (NSArray *)JSONCreateWithString(testAllocator, (CFStringRef)@"[3.14159265, 1.61803399, -57.2957795]", kJSONReadOptionsDefault, (CFErrorRef *)&error);
    STAssertTrue([array count] == 3, @"Array should have 3 elements");
    STAssertEquals([[array objectAtIndex: 0] doubleValue], 3.14159265, @"Large number 2^32+1 expected");
    STAssertEquals([[array objectAtIndex: 1] doubleValue], 1.61803399, @"Large number 2^(64-1) expected");
    STAssertEquals([[array objectAtIndex: 2] doubleValue], -57.2957795, @"Large number 2^(64-1) expected");
    [error release];
    [array release];
  }
}

@end
