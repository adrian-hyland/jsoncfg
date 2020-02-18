# JSON Configuration File Utility

jsoncfg is a command line utility that can be used to make adjustments to a JSON configuration file.


## Building jsoncfg

To create a release version of the utility:

`make all`

To clean the release version of the utility:

`make clean`

To create a debug version of the utility:

`make all DEBUG=1`

To clean the debug version of the utility:

`make clean DEBUG=1`


## Using jsoncfg

The JSON content is provided using the input stream. The modified JSON content is provided using the output stream.

The 1st parameter provides the path of the key that you want to set \
The 2nd parameter provides the JSON value for the key


***Example 1***

The following will set the '`defines`' key value to '`["-g3","-DDEBUG"]`' in an object of the array '`configurations`' that has the '`name`' key set as '`Linux (Debug)`'

```bash
touch c_cpp_properties.json
cat c_cpp_properties.json | jsoncfg "/configurations[/name:\"Linux (Debug)\"]/defines" \
"[\"-g3\",\"-DDEBUG\"]"
```


***Example 2***

The following will replace an object in the array '`configurations`' that has the '`name`' key set as '`Linux (Release)`'

```bash
touch c_cpp_properties.json
cat c_cpp_properties.json | jsoncfg "/configurations[/name:\"Linux (Release)\"]" \
"{\"name\":\"Linux (Release)\",\"defines\":[\"-O3\",\"-DRELEASE\"]}"
```

### Known Limitations

- The JSON content should not contain any embedded comments as these are not currently supported by the parsing functionality
- The JSON content should not contain any escaped UTF-16 characters (any `\uXXXX` sequences) as these are also not yet supported by the parsing functionality



## Building the unit test harness

To create a release version of the unit test harness:

`make all TEST=1`

To clean the release version of the unit test harness:

`make clean TEST=1`

To create a debug version of the unit test harness:

`make all TEST=1 DEBUG=1`

To clean the debug version of the unit test harness:

`make clean TEST=1 DEBUG=1`
