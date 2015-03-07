#ifndef CONFREAD_H
#define CONFREAD_H

#include <stdlib.h>

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


// functions
int confread_open(struct confread_file* confFile);
void confread_close(struct confread_file* confFile);
struct confread_section* confread_find_section(struct confread_file* confFile, char* name);
struct confread_pair* confread_find_key(struct confread_section* confSec, char* name);
char* confread_find_value(struct confread_section* confSec, char* name);

#endif
