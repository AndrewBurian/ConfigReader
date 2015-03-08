/* ----------------------------------------------------------------------------
SOURCE FILE

Name:		confread.c

Program:	Configuration Reader

Developer:	Andrew Burian

Created On:	2015-03-07

Functions:
	struct confread_section* add_section(struct confread_file* file, char* name)
  struct confread_pair* add_pair(struct confread_section* section, char* key,
    char* value)
  struct confread_file* confread_open(char* path)
  struct confread_section* confread_find_section(struct confread_file* confFile,
    char* name)
  struct confread_pair* confread_find_pair(struct confread_section* confSec,
    char* key)
  char* confread_find_value(struct confread_section* confSec, char* name)
  void confread_close(struct confread_file** confFile)

Description:
	A config file reading library
  This tool is for easy loading and parsing of config files, and includes
  methods to easily search for sections and key-value pairs within the config
  file.
  All values are handled as strings, as it's up to the program to determine what
  to do with the given data

Revisions:
	(none)

---------------------------------------------------------------------------- */

#include "confread.h"

/* ----------------------------------------------------------------------------
FUNCTION

Name:		Add Section

Prototype:	struct confread_section* add_section(struct confread_file* file,
              char* name)

Developer:	Andrew Burian

Created On:	2015-03-07

Parameters:
	struct confread_file* file
    The confread file to add a section to
  char* name
    The name of the section to be added

Return Values:
	struct confread_section*
    a confread section which is either new, or a pointer to an existing one of
    the same name

Description:
	*** For internal use
  Given a section name, will either create and return a new empty section added
  to the file, or will return the existing section if one was found

Revisions:
	(none)

---------------------------------------------------------------------------- */
struct confread_section* add_section(struct confread_file* file, char* name){

  // temp section pointer
  struct confread_section* thisSection = 0;

  // ensure this section does not already exist
  if((thisSection = confread_find_section(file, name))){
    return thisSection;
  }

  // expand the number of sections in the file
  file->sections = realloc(file->sections, sizeof(struct confread_section*) * ++file->count);

  // add the new section
  thisSection = file->sections[file->count - 1] = malloc(sizeof(struct confread_section));

  // name the section
  thisSection->name = malloc(strlen(name) + 1);
  memcpy(thisSection->name, name, strlen(name) + 1);

  // set the initial values
  thisSection->count = 0;
  thisSection->pairs = 0;

  return thisSection;

}

/* ----------------------------------------------------------------------------
FUNCTION

Name:		Add Pair

Prototype:	struct confread_pair* add_pair(struct confread_section* section,
              char* key, char* value)

Developer:	Andrew Burian

Created On:	2015-03-07

Parameters:
	struct confread_section* section
    the section to add the key-value pair to
  char* key
    the key portion of the pair
  char* value
    the value portion of the pair

Return Values:
	struct confread_pair*
    pointer to the newly added pair

Description:
	*** For internal use
  Given a pair key, will either create and return a new pair added to the
  section with the given value, or an existing pair of the same key with the
  value overwritten.

Revisions:
	(none)

---------------------------------------------------------------------------- */
struct confread_pair* add_pair(struct confread_section* section, char* key, char* value){

  // temp pair pointer
  struct confread_pair* thisPair = 0;

  // ensure pair does not exist
  if((thisPair = confread_find_pair(section, key))){

    // overwrite the value
    thisPair->value = realloc(thisPair->value, strlen(value) + 1);
    memcpy(thisPair->value, value, strlen(value) + 1);
    return thisPair;
  }

  // expand the number of pairs in the section
  section->pairs = realloc(section->pairs, sizeof(struct confread_pair*) * ++section->count);

  // add the new pair
  thisPair = section->pairs[section->count - 1] = malloc(sizeof(struct confread_pair));

  // name the pair
  thisPair->key = malloc(strlen(key) + 1);
  memcpy(thisPair->key, key, strlen(key) + 1);

  // set the initial value
  thisPair->value = malloc(strlen(value) + 1);
  memcpy(thisPair->value, value, strlen(value) + 1);

  return thisPair;
}

/* ----------------------------------------------------------------------------
FUNCTION

Name:   Open

Prototype:  struct confread_file* confread_open(char* path)

Developer:	Andrew Burian

Created On:	2015-03-07

Parameters:
  char* path
    file path to the conf file to read

Return Values:
  struct confread_file*
    the newly creaded confread file
    null on failure

Description:
  Parses the given config file and loads it into the data structure for
  processing

Revisions:
  (none)

---------------------------------------------------------------------------- */
struct confread_file* confread_open(char* path){

  // the actual configuration file
  FILE* confDataFile = 0;

  // temp pointers
  struct confread_file* confFile = 0;
  struct confread_section* thisSection = 0;
  struct confread_pair* thisPair = 0;

  // the line from the conf file
  char* line = 0;
  size_t lineLen = 0;

  // other char pointers for manipulation of the line
  char* lineStart = 0;
  char* lineEnd = 0;
  char* keyStart = 0;
  char* keyEnd = 0;

  // create the conf file
  confFile = malloc(sizeof(struct confread_file));
  confFile->count = 0;
  confFile->name = malloc(strlen(path) + 1);
  memcpy(confFile->name, path, strlen(path) + 1);

  // open the data file
  if(!(confDataFile = fopen(confFile->name, "r"))){
    return 0;
  }

  // add the 'root section'
  thisSection = add_section(confFile, "root");

  // Read the file line by line until EOF
  while(getline(&line, &lineLen, confDataFile) != -1){

    // seek forward to the first non-space character
    for(lineStart = line; *lineStart != 0; ++lineStart){
      if(!isspace(*lineStart)){
        break;
      }
    }

    // omit if empty
    if(*lineStart == 0){
      continue;
    }

    // omit if comment '#'
    if(*lineStart == '#'){
      continue;
    }

    // see if this is a section header
    if(*lineStart == '['){

      // move lineStart off the header start
      ++lineStart;


      // seek forward to the end of the header ']'
      for(lineEnd = lineStart; *lineEnd != 0; ++lineEnd){
        if(*lineEnd == ']'){
          break;
        }
      }

      // if it's anything other than the end of header, invalid line
      if(*lineEnd != ']'){
        continue;
      }

      // seek linestart forward to trim whitespace
      for(lineStart = lineStart; lineStart != lineEnd; ++lineStart){
        if(!isspace(*lineStart)){
          break;
        }
      }

      // omit zero size headers
      if(lineStart == lineEnd){
        continue;
      }

      // seek line end back to trim whitespace
      for(lineEnd = lineEnd - 1; lineEnd != lineStart; --lineEnd){
        if(!isspace(*lineEnd)){
          break;
        }
      }

      // set next char as null
      lineEnd[1] = 0;

      // create the new section
      thisSection = add_section(confFile, lineStart);

      continue;
    }

    // otherwise, key-value
    else{

      // seek line end forward to the separator '='
      for(lineEnd = lineStart; *lineEnd != 0; ++lineEnd){
        if(*lineEnd == '='){
          break;
        }
      }

      // discard if invalid
      if(*lineEnd != '='){
        continue;
      }

      // if key is zero length, discard
      if(lineEnd == lineStart){
        continue;
      }

      // set the key start
      keyStart = lineEnd + 1;

      // seek the line end backwards to trim whitespace
      for(lineEnd = lineEnd - 1; lineEnd != lineStart; --lineEnd){
        if(!isspace(*lineEnd)){
          break;
        }
      }

      // set the following char to null
      lineEnd[1] = 0;

      // seek the key end forward to the end of the line
      for(keyEnd = keyStart; *keyEnd != 0; ++keyEnd){
        if(*keyEnd == '\n'){
          break;
        }
      }

      // if the key is zero length, discard
      if(keyEnd == keyStart){
        continue;
      }

      // seek key end backwards to trim trailing whitespace
      for(keyEnd = keyEnd; keyEnd != keyStart; --keyEnd){
        if(!isspace(*keyEnd)){
          break;
        }
      }

      // seek the key start forward to trim leading whitespace
      for(keyStart = keyStart; keyStart != keyEnd; ++keyStart){
        if(!isspace(*keyStart)){
          break;
        }
      }

      // set the new line char to null
      keyEnd[1] = 0;

      // add the key-value pair
      add_pair(thisSection, lineStart, keyStart);

    }
  }

  return confFile;

}

/* ----------------------------------------------------------------------------
FUNCTION

Name:   Find Section

Prototype:  struct confread_section* confread_find_section(
              struct confread_file* confFile, char* name)

Developer:	Andrew Burian

Created On:	2015-03-07

Parameters:
  struct confread_file* confFile
    the file to search for the section in
  char* name
    the name of the section to search for

Return Values:
  struct confread_section*
    the section matching the provided name
    null on not found

Description:
  Searches for a section within a file and returns it if found

Revisions:
  (none)

---------------------------------------------------------------------------- */
struct confread_section* confread_find_section(struct confread_file* confFile, char* name){

  // loop counter
  int sectionCount = 0;

  // temp pointer to section
  struct confread_section* thisSection = 0;

  // null check
  if(!confFile || !name){
    return 0;
  }

  // loop through all sections
  for(sectionCount = 0; sectionCount < confFile->count; ++sectionCount){

    // get pointer to section
    thisSection = confFile->sections[sectionCount];

    // check for match
    if(!strcmp(thisSection->name, name)){
      break;
    }

  }

  // return found section or null
  return (sectionCount == confFile->count ? 0 : thisSection);

}

/* ----------------------------------------------------------------------------
FUNCTION

Name:   Find Pair

Prototype:  struct confread_pair* confread_find_pair(
              struct confread_section* confSec, char* key)

Developer:	Andrew Burian

Created On:	2015-03-07

Parameters:
  struct confread_section* confSec
    the section to search in
  char* key
    the key to search for

Return Values:
  struct confread_pair*
    the pair matching the provided value
    null on not found

Description:
  Finds a key pair in the section and returns it if found

Revisions:
  (none)

---------------------------------------------------------------------------- */
struct confread_pair* confread_find_pair(struct confread_section* confSec, char* key){

  // loop counter
  int pairCount = 0;

  // temp pair pointer
  struct confread_pair* thisPair = 0;

  // null check
  if(!confSec || !key){
    return 0;
  }

  // iterate through all pairs in section
  for(pairCount = 0; pairCount < confSec->count; ++pairCount){

    // get the pair pointer
    thisPair = confSec->pairs[pairCount];

    // check if the key matches target key
    if(!strcmp(thisPair->key, key)){
      break;
    }

  }

  // return found pair or null
  return (pairCount == confSec->count ? 0 : thisPair);

}

/* ----------------------------------------------------------------------------
FUNCTION

Name:   Find Value

Prototype:  struct confread_pair* confread_find_pair(
              struct confread_section* confSec, char* key)

Developer:	Andrew Burian

Created On:	2015-03-07

Parameters:
  struct confread_section* confSec
    the section to search in
  char* key
    the key to search for

Return Values:
  char*
    the value matching the key in the provided section
    null on not found

Description:
  Finds a key pair in the section and returns only it's value if found

Revisions:
  (none)

---------------------------------------------------------------------------- */
char* confread_find_value(struct confread_section* confSec, char* key){

  struct confread_pair* thisPair = 0;

  // get the pair
  if(!(thisPair = confread_find_pair(confSec, key))){
    return 0;
  }

  return thisPair->value;

}

/* ----------------------------------------------------------------------------
FUNCTION

Name:   Close

Prototype:  void confread_close(struct confread_file** confFile)

Developer:	Andrew Burian

Created On:	2015-03-07

Parameters:
  struct confread_file** confFile
    a pointer to the address of the confread file to be freed

Description:
  Properly frees and closes all resources used by the confread file, and sets
  the pointer to null

Revisions:
  (none)

---------------------------------------------------------------------------- */
void confread_close(struct confread_file** confFile){

  // loop counters
  int sectionCount = 0;
  int pairCount = 0;

  // temp section pointer
  struct confread_section* thisSection = 0;

  // temp pair pointer
  struct confread_pair* thisPair = 0;

  // loop through all sections in the file
  for(sectionCount = 0; sectionCount < (*confFile)->count; ++sectionCount){

    // get a pointer to the current section
    thisSection = (*confFile)->sections[sectionCount];

    // loop through each key pair in the section
    for(pairCount = 0; pairCount < thisSection->count; ++pairCount){

      // get a pointer to the current pair
      thisPair = thisSection->pairs[pairCount];

      // free the two data elements in the pair
      free(thisPair->key);
      free(thisPair->value);

      // free the pair itself
      free(thisPair);

    }

    // all pairs have been freed, now free the section data
    free(thisSection->pairs);

    // free the section itself
    free(thisSection);

  }

  // free the file section data
  free((*confFile)->sections);
  (*confFile)->sections = 0;

  // free the conf file itself
  free(*confFile);
  confFile = 0;

}
