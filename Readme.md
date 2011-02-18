# CoreJSON Framework

CoreJSON is iOS, OSX (will also work on Linux) Core Foundation based fast parser and generator based on `libyajl` C library.

Comparison with other JSON frameworks:

| Framework            | Read (min) | Read (mean) | Read (max)  
|:---------------------|-----------:|------------:|-----------:
| JSONKit              |      3.978 |       4.032 |     4.829
| CoreJSON             |      6.789 |       7.617 |     7.750
| Apple JSON (private) |      7.609 |       7.643 |     7.840
| YAJL-objc            |      7.158 |       8.033 |    43.361
| JSON Framework       |      9.737 |       9.803 |    10.181
| TouchJSON            |     13.800 |      13.883 |    14.363

_Tests performed on iPad Simulator with https://github.com/samsoffes/json-benchmarks_

## Usage

    CoreJSONRef json = JSONCreateWithString(NULL, <<NSString or CFStringRef>>);
    id object = [(id)JSONGetObject(json) retain];
    JSONRelease(json);
    
    // Do whatever you want with object, when done, [object release] it
    ...

## Using in your projects

For your own (non Mac AppStore) OSX projects the quick way is to:

1. `brew install yajl`
2. add `/usr/local/lib` to `Library Search Path` and `/usr/local/include` to `Header Search Path`
3. Just drop `CoreJSON.h` and `CoreJSON.c` to your project and have fun

For OSX and iOS (Mac AppStore/AppStore) projects you need to include `libyajl` and drop `CoreJSON.h` and `CoreJSON.c` files to your project.
One way to do it:

1. Go to your project's directory (for which you're using `git`, right? ;) and `git submodule add git://github.com/mirek/CoreJSON.git CoreJSON`
2. From Xcode add `CoreJSON.h` and `CoreJSON.c` files to your project
3. If you're already using `libyajl` in your project, you are good to go. If not, add `libyajl` files
