#include "confread.h"

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

      // replace header end with null
      *lineEnd = 0;

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

char* confread_find_value(struct confread_section* confSec, char* name){

  struct confread_pair* thisPair = 0;

  // get the pair
  if(!(thisPair = confread_find_pair(confSec, name))){
    return 0;
  }

  return thisPair->value;

}

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
