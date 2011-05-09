# CoreJSON Framework

CoreJSON is iOS and OSX Core Foundation based fast parser and generator based on `libyajl` C library.

Comparison with other JSON frameworks:

![Chart](http://chart.apis.google.com/chart?chf=bg,s,67676700&chxl=1:|TouchJSON|JSON+Framework|YAJL|Apple+JSON|CoreJSON|JSONKit&chxr=0,0,60&chxt=x,y&chbh=a&chs=300x165&cht=bhg&chco=94AAC8&chds=0,237.904&chd=t:46.582,63.192,128.143,129.231,154.504,237.904&chtt=1+Iteration%2C+iPhone+4+iOS+4.2.1)

_Tests performed with https://github.com/samsoffes/json-benchmarks_

## Usage

Parsing in Objective-C:

    NSError *error = nil;
    id object = (id)JSONCreateWithString(NULL, (CFStringRef)@"[foo, bar]", kJSONReadOptionsDefault, (CFErrorRef *)&error);
    if (object) {
      // Do something with object (NSArray)
      [object release];
    }

Parsing in C:

    CFErrorRef error = NULL;
    CFTypeRef object = JSONCreateWithString(NULL, CFSTR("[foo, bar]"), kJSONReadOptionsDefault, &error);
    if (object) {
      // Do something with object
      CFRelease(object);
    }

Generating in Objective-C:

    NSArray *array = [NSArray arrayWithObjects: @"foo", @"bar", nil];
    NSError *error = nil;
    NSString *json = (id)JSONCreateString(NULL, array, kJSONWriteOptionsDefault, (CFErrorRef *)&error);
    if (json) {
      // Do something with json string
      [json release];
    }
    [array release];

Generating in C:

    CFTypeRef values[] = { CFSTR("foo"), CFSTR("bar") };
    CFArrayRef array = CFArrayCreateMutable(NULL, values, 2, &kCFTypeArrayCallBacks);
    CFErrorRef error = NULL;
    CFStringRef json = JSONCreateString(NULL, array, kJSONWriteOptionsDefault, &error);
    if (json) {
      // Do something with json string
      CFRelease(json);
    }
    CFRelease(array);
    
_You should also take care of `error` object_

## Options

`JSONReadOptions`:

* `kJSONReadOptionCheckUTF8                  = 1` -- Check UTF8 strings
* `kJSONReadOptionAllowComments              = 2` -- Allow `/* comments */`
* `kJSONReadOptionsDefault                   = 0` -- Default options (don't check UTF8 strings and do not allow comments)
* `kJSONReadOptionsCheckUTF8AndAllowComments = 3` -- Check UTF8 strings and allow comments

`JSONWriteOptions`:

* `kJSONWriteOptionIndent   = 1` -- Indent generated JSON string
* `kJSONWriteOptionsDefault = 0` -- Default options (do not indent JSON string)

## Using in your projects

There are just 2 files `CoreJSON.h` and `CoreJSON.c` you'll need together with `libyajl`.

For your own (non Mac AppStore) OSX projects the quick way is to:

1. `brew install yajl`
2. add `/usr/local/lib` to `Library Search Path` and `/usr/local/include` to `Header Search Path`
3. Just drop `CoreJSON.h` and `CoreJSON.c` to your project and have fun

For OSX and iOS (Mac AppStore/AppStore) projects you need to include `libyajl` and drop `CoreJSON.h` and `CoreJSON.c` files to your project.
One way to do it:

1. Go to your project's directory (for which you're using `git`, right? ;) and `git submodule add git://github.com/mirek/CoreJSON.git CoreJSON`
2. From Xcode add `CoreJSON.h` and `CoreJSON.c` files to your project
3. If you're already using `libyajl` in your project, you are good to go. If not, add `libyajl` files

## License

    The MIT License

    Copyright 2011 Mirek Rusin <mirek [at] me [dot] com>
                   http://github.com/mirek/CoreJSON
    
    Permission to use, copy, modify, and/or distribute this software for any
    purpose with or without fee is hereby granted, provided that the above
    copyright notice and this permission notice appear in all copies.

    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
