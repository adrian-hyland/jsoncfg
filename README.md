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
make all
```

To clean the release version of the utility:
```bash
make clean
```

To create a debug version of the utility:
```bash
make all DEBUG=1
```

To clean the debug version of the utility:
```bash
make clean DEBUG=1
```



## Using jsoncfg

The JSON content is provided using the input stream. The modified JSON content is provided using the output stream.

The 1st parameter provides the path of the key that you want to set \
The 2nd parameter provides the JSON content value for the key

**Path Format**
```abnf
quotation-mark = %x22 ; "
key-start = %x2F      ; /
value-start = %x3A    ; :
array-start = %x5B    ; [
array-end = %x5D      ; ]
escape = %x5C         ; \
char = unescaped /
       escape (
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

***Example 1***

The following will set the `defines` key value to `["-g3","-DDEBUG"]` in an object of the array `configurations` that has the `name` key set as `Linux (Debug)`
```bash
touch c_cpp_properties.json
cat c_cpp_properties.json | jsoncfg "/configurations[/name:\"Linux (Debug)\"]/defines" \
"[\"-g3\",\"-DDEBUG\"]"
```


***Example 2***

The following will replace an object in the array `configurations` that has the `name` key set as `Linux (Release)`
```bash
touch c_cpp_properties.json
cat c_cpp_properties.json | jsoncfg "/configurations[/name:\"Linux (Release)\"]" \
"{\"name\":\"Linux (Release)\",\"defines\":[\"-O3\",\"-DRELEASE\"]}"
```

### Known Limitations

- The JSON content should not contain any embedded comments as these are not currently supported by the parsing functionality
- The JSON content should not contain any escaped UTF-16 characters (any `\uXXXX` sequences) as these are also not yet supported by the parsing functionality
- The parsing functionality does not check the validity of any literals that are contained in the JSON content - it will accept any literal value that that is composed of alphanumeric, '+', '-' and '.' characters.



## Adding build configurations to the VS Code project

To add a 'Relase' build configuration to the VS Code project
```bash
make vscode
```

To add a 'Debug' build configuration to the VS Code project
```bash
make vscode DEBUG=1
```



## Building the unit test harness

To create a release version of the unit test harness:
```bash
make all TEST=1
```

To clean the release version of the unit test harness:
```bash
make clean TEST=1
```

To create a debug version of the unit test harness:
```bash
make all TEST=1 DEBUG=1
```

To clean the debug version of the unit test harness:
```bash
make clean TEST=1 DEBUG=1
```
