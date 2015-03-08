# LibConfRead A configuration file reader in C

Introduction
---------------------
This project is an attempt to create a standard means to read and parse configuration files for any projects which may need them.

Feel free to make use of it, send me feedback, or contribute back to this project, all under the [license](LICENSE).

Installation and Compiling
--------------------------
Install with `sudo make install`

In source, include `#include <confread.h>`

Then compile with `-lconfread`

Config File Standard
---------------------
The format for the config files this tool will read is as follows:

```
key=value

[sectionHeader]

sectionkey=sectionvalue

# comment lines

```
Key-value pairs before any section header will automatically be placed under a section called `root`

```
[section header]
keyterm=valueterm
key2=value2
key3=value term
key 4=value term 4
key term = value term
```
Key-value pairs as well as section headers accept all whitespace, though leading or trailing whitespace characters are removed during parsing

Section headers as well as keys are case sensitive

Exception Case Handling
------------------------
Duplicate keys in the same section are NOT accepted! The latter instance of the key will overwrite the former

Duplicate section headers are accepted. They will be amalgamated into one section, with duplicate keys overwritten as mentioned above
