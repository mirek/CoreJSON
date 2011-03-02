# CoreJSON Framework

CoreJSON is iOS, OSX (will also work on Linux) Core Foundation based fast parser and generator based on `libyajl` C library.

Comparison with other JSON frameworks:

iPhone 4.2 Simulator running on MacBook Pro 3.06GHz Intel Core 2 Duo, OSX SL 10.6.6:

| Framework            | What    | Min     | Mean    | Max 
|:---------------------|--------:|--------:|--------:|--------:
|              JSONKit |    read |   4.032 |   4.571 |  13.548
|             CoreJSON |    read |   4.892 |   5.487 |  10.001
|                 YAJL |    read |   7.637 |   7.670 |   8.084
|           Apple JSON |    read |   7.290 |   8.241 |  14.693
|            TouchJSON |    read |  13.565 |  13.970 |  24.359
|       JSON Framework |    read |   9.866 |  12.001 |  30.772

iPhone 4:

| Framework            | What    | Min     | Mean    | Max 
|:---------------------|--------:|--------:|--------:|--------:
|              JSONKit |    read |  35.887 |  37.185 |  40.931
|             CoreJSON |    read |  59.742 |  64.204 | 132.731
|           Apple JSON |    read | 114.739 | 119.915 | 229.629
|                 YAJL |    read | 120.171 | 124.833 | 262.097
|       JSON Framework |    read | 147.569 | 152.352 | 205.846
|            TouchJSON |    read | 229.279 | 234.811 | 370.086

_Tests performed with https://github.com/samsoffes/json-benchmarks_

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
