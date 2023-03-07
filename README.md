# JSON Configuration File Utility

**jsoncfg** is a command line utility that can be used to make adjustments to a JSON configuration file.

***Design Goals***
- The utility should be small and lightweight.
- It should be able to *stream* content through the JSON parsing and formatting routines (to avoid having to allocate memory for the whole of the content before it can be parsed or formatted).

***Use Cases***
- The main motivation behind the utility was to be able to configure the `c_cpp_properties.json` file from a make file so that 'VS Code' could pick up the correct compilation directives (include directories, pre-processor defines etc) based on a particular build type and/or target.

## Building jsoncfg

To create a release version of the utility:

```bash
$ make all
```

To clean the release version of the utility:
```bash
$ make clean
```

To create a debug version of the utility:
```bash
$ make all DEBUG=1
```

To clean the debug version of the utility:
```bash
$ make clean DEBUG=1
```

## Using jsoncfg

The JSON content is provided using the input stream. The modified JSON content is provided using the output stream.

***Parameters***
```
[-c{n|l|b}] [-i{0-9}] [<key> <value>]*
```

***Description***
- `-c{n|l|b}`\
Set the type of formatting for comments in the JSON content.\
`-cn` will strip comments from the JSON content.\
`-cl` will use line comments in the JSON content.\
`-cb` will use block comments in the JSON content.\
This parameter is optional - if it is not given then line comments will be used in the JSON content.
- `-i{0-9}`\
Set the indentation size (to a value between 0 and 9).\
A value of zero will turn off indentation - the JSON content will be formatted using just spaces (without any comments).\
This parameter is optional - if it is not given then an indentation size of 3 will be used.
- `<key>`\
Provides the path of the key that you want to set (see below for a description of the path format)
- `<value>`\
Provides the JSON content value for the key

***Notes***
- There can be any number of `<key>` `<value>` pairs provided in the parameters

***Key Path Format***\
The path used to identify the key in the JSON content should have the following format (note that this format, although similar, is **not** intended to conform to RFC6901)
```abnf
quotation-mark = %x22     ; "
key-start = %x2F          ; /
value-start = %x3A        ; :
array-start = %x5B        ; [
array-end = %x5D          ; ]
escape = %x5C             ; \
hex = %x31-39 /           ; 1-9
      %x41-46 /           ; A-F
      %x61-66             ; a-f
unicode = %x75 4hex       ; uXXXX
char = unescaped /
       escape (
         unicode        /
         quotation-mark /
         key-start      /
         value-start    /
         array-start    /
         array-end      /
         escape )
unescaped = %x20-21 / %x23-2E / %x30-39 / %x3B-5A / %x5E-10FFFF

key = key-start [quotation-mark] *char [quotation-mark] ; Key names do not need to be enclosed in quotation marks
string = quotation-mark *char quotation-mark            ; String values must be enclosed in quotation marks
literal = *char                                         ; Literal values must not be enclosed in quotation marks
value = value-start (string / literal)
array = array-start [path] value array-end              ; The '[path] value' identifies which element to find in the array
path = key [array] [path]
```

***Example 1***\
The following will set the `defines` key value to `["-g3","-DDEBUG"]` in an object of the array `configurations` that has the `name` key set as `Linux (Debug)` (in the `c_cpp_properties.json` file)
```bash
$ touch c_cpp_properties.json
$ cat c_cpp_properties.json | jsoncfg "/configurations[/name:\"Linux (Debug)\"]/defines" \
"[\"-g3\",\"-DDEBUG\"]"
```

***Example 2***\
The following will replace an object in the array `configurations` that has the `name` key set as `Linux (Release)` (in the `c_cpp_properties.json` file)
```bash
$ touch c_cpp_properties.json
$ cat c_cpp_properties.json | jsoncfg "/configurations[/name:\"Linux (Release)\"]" \
"{\"name\":\"Linux (Release)\",\"defines\":[\"-O3\",\"-DRELEASE\"]}"
```

***Example 3***\
The following will reformat the `c_cpp_properties.json` file content so that it does not have any comments and uses an indentation size of 4
```bash
$ touch c_cpp_properties.json
$ cat c_cpp_properties.json | jsoncfg -cn -i4
```

### Known Limitations and Exceptions

- The input JSON content should be encoded using UTF-8 (without a byte order mark).
- The input JSON content may contain any line or block comments even though, technically, comments should not be used for JSON. Support for handling comments was added as they can be used quite a lot in JSON configuration files.
- The parsing functionality does not check the validity of any literals that are contained in the input JSON content - it will accept any literal value that that is composed of alphanumeric, '+', '-' and '.' characters.
- The parsing functionality will not accept any NUL characters in the input JSON content, even if they have been escaped as \u0000.
- The output JSON content is encoded using UTF-8 (without a byte order mark)
- The formatting functionality will escape any control characters (U+00000..U+001F) in the output JSON content.

## Adding build configurations to the VS Code project

To add a 'Release' build configuration to the VS Code project
```bash
$ make vscode
```

To add a 'Debug' build configuration to the VS Code project
```bash
$ make vscode DEBUG=1
```

## Building the unit test harness

To create a release version of the unit test harness:
```bash
$ make all TEST=1
```

To clean the release version of the unit test harness:
```bash
$ make clean TEST=1
```

To create a debug version of the unit test harness:
```bash
$ make all TEST=1 DEBUG=1
```

To clean the debug version of the unit test harness:
```bash
$ make clean TEST=1 DEBUG=1
```

***Notes***\
The debug version of the unit test harness has malloc tracing enabled. To produce the malloc trace and check for any memory issues during testing, you can do the following:
```bash
$ export MALLOC_TRACE=./malloc-trace
$ ./bin/debug/jsoncfg-test
$ mtrace ./bin/debug/jsoncfg-test ./malloc-trace
```