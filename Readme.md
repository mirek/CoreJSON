# CoreJSON Framework

CoreJSON is iOS, OSX (will also work on Linux) Core Foundation based fast parser and generator based on `libyajl` C library.

Comparison with other JSON frameworks:

![Chart](http://chart.apis.google.com/chart?chxl=1:|TouchJSON|JSON+Framework|YAJL|Apple+JSON|CoreJSON|JSONKit&chxr=0,0,60&chxt=x,y&chbh=a&chs=300x165&cht=bhg&chco=94AAC8&chds=0,237.904&chd=t:46.582,63.192,128.143,129.231,154.504,237.904&chtt=1+Iteration%2C+iPhone+4+iOS+4.2.1)

_Tests performed with https://github.com/samsoffes/json-benchmarks_

## Usage

Parsing:

    CoreJSONRef json = JSONCreateWithString(NULL, <<NSString or CFStringRef>>);
    id object = [(id)JSONGetObject(json) retain];
    JSONRelease(json);
    
    // Do whatever you want with object, when done, [object release] it
    ...

Generating:

    NSString *json = (id)JSONCreateStringWithObject(NULL, <<Whatever>>);
    
    // Do something with json string...
    
    [json release];

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
