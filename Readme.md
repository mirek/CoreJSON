# CoreJSON Framework

CoreJSON is iOS, OSX (will also work on Linux) Core Foundation based fast parser and generator based on `libyajl` C library.

Comparison with other JSON frameworks:

| Framework            | Speed (lower means faster) 
|:---------------------|---------------------------:
| CoreJSON             |                   1.100 ms
| JSONKit              |                   4.040 ms
| Apple JSON (private) |                   7.254 ms
| YAJL-objc            |                   7.721 ms
| JSON Framework       |                   9.879 ms
| TouchJSON            |                  14.157 ms

_Tests performed on iOS with https://github.com/samsoffes/json-benchmarks_

## Usage

    CoreJSONRef json = JSONCreateWithString(NULL, <<NSString or CFStringRef>>);
    id object = [(id)JSONGetObject(json) retain];
    JSONRelease(json);
    
    // Do whatever you want with object, when done, [object release] it
    ...

## Using in your projects

1. Go to your project's directory (for which you're using `git`, right? ;) and `git submodule add git://github.com/mirek/CoreJSON.git CoreJSON`
2. From Xcode add `CoreJSON.h` and `CoreJSON.m` files to your project
3. If you're already using `libyajl` in your project, you are good to go. If not, add `libyajl` files
