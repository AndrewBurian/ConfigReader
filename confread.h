/* ----------------------------------------------------------------------------
HEADER FILE

Name:		confread.h

Program:	Configuration Reader

Developer:	Andrew Burian

Created On:	2015-03-07

Description:
	A config file reading library for C

Revisions:
	(none)

---------------------------------------------------------------------------- */

#ifndef CONFREAD_H
#define CONFREAD_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

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
struct confread_file* confread_open(char* path);
struct confread_section* confread_find_section(struct confread_file* confFile, char* name);
struct confread_pair* confread_find_pair(struct confread_section* confSec, char* key);
char* confread_find_value(struct confread_section* confSec, char* key);
void confread_close(struct confread_file** confFile);

#endif
