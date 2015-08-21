# LibConfRead A configuration file reader

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

Data Structures
-------------------------
Confreader creates and uses these data structures to store config files

```c
// The struct for the entire configuration file
struct confread_file {
  char*  name;
  int    count;
  struct confread_section** sections;
};

// The struct for each config section
struct confread_section{
  char*  name;
  int    count;
  struct confread_pair** pairs;
};

// The key-value pair
struct confread_pair{
  char* key;
  char* value;
};
```

Available Functions
------------------------
These functions are used to easily create and access the data parsed by confreader

```c
struct confread_file* confread_open(char* path);
struct confread_section* confread_find_section(struct confread_file* confFile, char* name);
struct confread_pair* confread_find_pair(struct confread_section* confSec, char* key);
char* confread_find_value(struct confread_section* confSec, char* key);
void confread_close(struct confread_file** confFile);
int confread_check_pair(struct confread_section *section, char *key, char *value);
```

`confread_open` loads the entire config file, making the data accessible and not vulnerable to changes once the program is running

the `confread_find` functions are then used to quickly find sections, and key-pairs in sections after it has been opened

`confread_close` should then be called to properly free up all memory used by confreader.
