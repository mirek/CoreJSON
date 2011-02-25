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
  // ...
}

- (void) tearDown {
  // ...
  [super tearDown];
}

- (void) testGenerator {
  
  id object = [NSDictionary dictionaryWithObjectsAndKeys: [NSNumber numberWithLongLong: 9223372036854775807], @"key1", nil];
  
  NSLog(@"=====\nParsed:\n%@\n", JSONCreateString(NULL, object));
}

- (void) testSimpleStuff {
  {
    CoreJSONRef json = JSONCreateWithString(NULL, (CFStringRef)@"[1, 3, 5]");
    NSArray *array = (NSArray *)JSONGetObject(json);
    STAssertTrue([array count] == 3, @"Array should have 3 elements");
    STAssertTrue([[NSNumber numberWithInt: 1] isEqualToNumber: [array objectAtIndex: 0]], @"First element should be 1");
    STAssertTrue([[NSNumber numberWithInt: 3] isEqualToNumber: [array objectAtIndex: 1]], @"Second element should be 3");
    STAssertTrue([[NSNumber numberWithInt: 5] isEqualToNumber: [array objectAtIndex: 2]], @"Third element should be 5");
  }
  {
    CoreJSONRef json = JSONCreateWithString(NULL, (CFStringRef)@"{ \"a\": 1, \"b\": 3, \"c\": 5 }");
    NSDictionary *dictionary = (NSDictionary *)JSONGetObject(json);
    STAssertTrue([dictionary count] == 3, @"Dictionary should have 3 entries");
    STAssertTrue([[dictionary objectForKey: @"b"] isEqualToNumber: [NSNumber numberWithInt: 3]], @"'b' value should be 3");
  }
}


- (void) testUTF8Strings {
  {
    CoreJSONRef json = JSONCreateWithString(NULL, (CFStringRef)@"[\"a’la\"]");
    NSArray *array = (NSArray *)JSONGetObject(json);
    STAssertTrue([array count] == 1, @"Array should have 1 element");
    STAssertTrue([[array objectAtIndex: 0] isEqualToString: @"a’la"], @"UTF8 string a’la expected");
  }
}

- (void) testLargeNumbers {
  {
    CoreJSONRef json = JSONCreateWithString(NULL, (CFStringRef)@"[4294967297, 9223372036854775807, -9223372036854775807]");
    NSArray *array = (NSArray *)JSONGetObject(json);
    STAssertTrue([array count] == 3, @"Array should have 3 elements");
    STAssertEquals([[array objectAtIndex: 0] longLongValue], 4294967297, @"Large number 2^32+1 expected");
    STAssertEquals([[array objectAtIndex: 1] longLongValue], 9223372036854775807, @"Large number 2^(64-1) expected");
    STAssertEquals([[array objectAtIndex: 2] longLongValue], -9223372036854775807, @"Large number 2^(64-1) expected");
  }
}

- (void) testFloats {
  {
    CoreJSONRef json = JSONCreateWithString(NULL, (CFStringRef)@"[3.14159265, 1.61803399, -57.2957795]");
    NSArray *array = (NSArray *)JSONGetObject(json);
    STAssertTrue([array count] == 3, @"Array should have 3 elements");
    STAssertEquals([[array objectAtIndex: 0] doubleValue], 3.14159265, @"Large number 2^32+1 expected");
    STAssertEquals([[array objectAtIndex: 1] doubleValue], 1.61803399, @"Large number 2^(64-1) expected");
    STAssertEquals([[array objectAtIndex: 2] doubleValue], -57.2957795, @"Large number 2^(64-1) expected");
  }
}

@end
