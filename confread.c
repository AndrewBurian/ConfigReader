#include "confread.h"

int confread_open(struct confread_file* confFile);

struct confread_section* confread_find_section(struct confread_file* confFile, char* name){

  // loop counter
  int sectionCount = 0;

  // temp pointer to section
  struct confread_section* thisSection = 0;

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
  return thisSection;

}

struct confread_pair* confread_find_pair(struct confread_section* confSec, char* key){

  // loop counter
  int pairCount = 0;

  // temp pair pointer
  struct confread_pair* thisPair = 0;

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
  return thisPair;

}

char* confread_find_value(struct confread_section* confSec, char* name){

  return confread_find_pair(confSec, name)->value;

}

void confread_close(struct confread_file* confFile){

  // loop counters
  int sectionCount = 0;
  int pairCount = 0;

  // temp section pointer
  struct confread_section* thisSection = 0;

  // temp pair pointer
  struct confread_pair* thisPair = 0;

  // loop through all sections in the file
  for(sectionCount = 0; sectionCount < confFile->count; ++sectionCount){

    // get a pointer to the current section
    thisSection = confFile->sections[sectionCount];

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

  // lastly, free the file section data
  free(confFile->sections);
  confFile->sections = 0;

}
