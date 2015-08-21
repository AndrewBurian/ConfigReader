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
	Andrew Burian
	2015-08-21
	Revised sections and pair to be a linked list rather than
	reallocating continuous memory sections.

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
		a confread section which is either new, or a pointer to an existing
		one of the same name

Description:
	*** For internal use
	Given a section name, will either create and return a new empty section added
	to the file, or will return the existing section if one was found

Revisions:
	Andrew Burian
	2015-08-21
	Revised sections and pair to be a linked list rather than
	reallocating continuous memory sections.

---------------------------------------------------------------------------- */
struct confread_section *add_section(struct confread_file *file, char *name)
{

	// temp section pointer
	struct confread_section *thisSection = 0;
	struct confread_section *prevSection = 0;

	// ensure this section does not already exist
	if ((thisSection = confread_find_section(file, name))) {
		return thisSection;
	}
	// add the new section
	thisSection = malloc(sizeof(struct confread_section));

	// name the section
	thisSection->name = malloc(strlen(name) + 1);
	memcpy(thisSection->name, name, strlen(name) + 1);

	// set the initial values
	thisSection->next = 0;
	thisSection->pairs = 0;

	// get the first section in the list
	prevSection = file->sections;

	// special handling for first item
	if (!prevSection) {
		file->sections = thisSection;
		return thisSection;
	}
	// find the end of the linked list of sections
	while (prevSection && prevSection->next) {
		prevSection = prevSection->next;
	}

	// append to list
	prevSection->next = thisSection;

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
	Andrew Burian
	2015-08-21
	Revised sections and pair to be a linked list rather than
	reallocating continuous memory sections.

---------------------------------------------------------------------------- */
struct confread_pair *add_pair(struct confread_section *section, char *key,
			       char *value)
{

	// temp pair pointer
	struct confread_pair *thisPair = 0;
	struct confread_pair *prevPair = 0;

	// ensure pair does not exist
	if ((thisPair = confread_find_pair(section, key))) {

		// overwrite the value
		thisPair->value = realloc(thisPair->value, strlen(value) + 1);
		memcpy(thisPair->value, value, strlen(value) + 1);
		return thisPair;
	}
	// add the new pair
	thisPair = malloc(sizeof(struct confread_pair));

	// name the pair
	thisPair->key = malloc(strlen(key) + 1);
	memcpy(thisPair->key, key, strlen(key) + 1);

	// set the initial value
	thisPair->value = malloc(strlen(value) + 1);
	memcpy(thisPair->value, value, strlen(value) + 1);

	// set as end of list
	thisPair->next = 0;

	// get the first pair in the section
	prevPair = section->pairs;

	// special handling for first
	if (!prevPair) {
		section->pairs = thisPair;
		return thisPair;
	}
	// find the end of the linked list of pairs
	while (prevPair && prevPair->next) {
		prevPair = prevPair->next;
	}

	// add to list
	prevPair->next = thisPair;

	return thisPair;
}

/* ----------------------------------------------------------------------------
FUNCTION

Name:		Open

Prototype:	struct confread_file* confread_open(char* path)

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
	Andrew Burian
	2015-08-21
	Revised sections and pair to be a linked list rather than
	reallocating continuous memory sections.

---------------------------------------------------------------------------- */
struct confread_file *confread_open(char *path)
{

	// the actual configuration file
	FILE *confDataFile = 0;

	// temp pointers
	struct confread_file *confFile = 0;
	struct confread_section *thisSection = 0;

	// the line from the conf file
	char *line = 0;
	size_t lineLen = 0;

	// other char pointers for manipulation of the line
	char *lineStart = 0;
	char *lineEnd = 0;
	char *keyStart = 0;
	char *keyEnd = 0;

	// open the data file
	if (!(confDataFile = fopen(path, "r"))) {
		return 0;
	}
	// create the conf file
	confFile = malloc(sizeof(struct confread_file));
	confFile->name = malloc(strlen(path) + 1);
	confFile->sections = 0;
	memcpy(confFile->name, path, strlen(path) + 1);

	// add the 'root section'
	thisSection = add_section(confFile, "root");

	// Read the file line by line until EOF
	while (getline(&line, &lineLen, confDataFile) != -1) {

		// seek forward to the first non-space character
		for (lineStart = line; *lineStart != 0; ++lineStart) {
			if (!isspace(*lineStart)) {
				break;
			}
		}

		// omit if empty
		if (*lineStart == 0) {
			continue;
		}
		// omit if comment '#'
		if (*lineStart == '#') {
			continue;
		}
		// see if this is a section header
		if (*lineStart == '[') {

			// move lineStart off the header start
			++lineStart;

			// seek forward to the end of the header ']'
			for (lineEnd = lineStart; *lineEnd != 0; ++lineEnd) {
				if (*lineEnd == ']') {
					break;
				}
			}

			// if it's anything other than the end of header, invalid line
			if (*lineEnd != ']') {
				continue;
			}
			// seek linestart forward to trim whitespace
			for (lineStart = lineStart; lineStart != lineEnd;
			     ++lineStart) {
				if (!isspace(*lineStart)) {
					break;
				}
			}

			// omit zero size headers
			if (lineStart == lineEnd) {
				continue;
			}
			// seek line end back to trim whitespace
			for (lineEnd = lineEnd - 1; lineEnd != lineStart;
			     --lineEnd) {
				if (!isspace(*lineEnd)) {
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
		else {

			// seek line end forward to the separator '='
			for (lineEnd = lineStart; *lineEnd != 0; ++lineEnd) {
				if (*lineEnd == '=') {
					break;
				}
			}

			// discard if invalid
			if (*lineEnd != '=') {
				continue;
			}
			// if key is zero length, discard
			if (lineEnd == lineStart) {
				continue;
			}
			// set the key start
			keyStart = lineEnd + 1;

			// seek the line end backwards to trim whitespace
			for (lineEnd = lineEnd - 1; lineEnd != lineStart;
			     --lineEnd) {
				if (!isspace(*lineEnd)) {
					break;
				}
			}

			// set the following char to null
			lineEnd[1] = 0;

			// seek the key end forward to the end of the line
			for (keyEnd = keyStart; *keyEnd != 0; ++keyEnd) {
				if (*keyEnd == '\n') {
					break;
				}
			}

			// if the key is zero length, discard
			if (keyEnd == keyStart) {
				continue;
			}
			// seek key end backwards to trim trailing whitespace
			for (keyEnd = keyEnd; keyEnd != keyStart; --keyEnd) {
				if (!isspace(*keyEnd)) {
					break;
				}
			}

			// seek the key start forward to trim leading whitespace
			for (keyStart = keyStart; keyStart != keyEnd;
			     ++keyStart) {
				if (!isspace(*keyStart)) {
					break;
				}
			}

			// set the new line char to null
			keyEnd[1] = 0;

			// add the key-value pair
			add_pair(thisSection, lineStart, keyStart);

		}
	}

	free(line);
	fclose(confDataFile);

	return confFile;

}

/* ----------------------------------------------------------------------------
FUNCTION

Name:		Find Section

Prototype:	struct confread_section* confread_find_section(
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
	Andrew Burian
	2015-08-21
	Revised sections and pair to be a linked list rather than
	reallocating continuous memory sections.

---------------------------------------------------------------------------- */
struct confread_section *confread_find_section(struct confread_file *confFile,
					       char *name)
{

	// temp pointer to section
	struct confread_section *thisSection = 0;

	// null check
	if (!confFile || !name) {
		return 0;
	}
	// set as first section
	thisSection = confFile->sections;

	// loop through all sections
	while (thisSection) {

		// check for match
		if (!strcmp(thisSection->name, name)) {
			break;
		}
		// next section
		thisSection = thisSection->next;

	}

	// return found section or null
	return thisSection;

}

/* ----------------------------------------------------------------------------
FUNCTION

Name:		Find Pair

Prototype:	struct confread_pair* confread_find_pair(
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
	Andrew Burian
	2015-08-21
	Revised sections and pair to be a linked list rather than
	reallocating continuous memory sections.

---------------------------------------------------------------------------- */
struct confread_pair *confread_find_pair(struct confread_section *confSec,
					 char *key)
{

	// temp pair pointer
	struct confread_pair *thisPair = 0;

	// null check
	if (!confSec || !key) {
		return 0;
	}
	// set to start of pairs
	thisPair = confSec->pairs;

	// iterate through all pairs in section
	while (thisPair) {

		// check if the key matches target key
		if (!strcmp(thisPair->key, key)) {
			break;
		}
		// get next pair
		thisPair = thisPair->next;

	}

	// return found pair or null
	return thisPair;

}

/* ----------------------------------------------------------------------------
FUNCTION

Name:		Find Value

Prototype:	struct confread_pair* confread_find_pair(
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
char *confread_find_value(struct confread_section *confSec, char *key)
{

	struct confread_pair *thisPair = 0;

	// get the pair
	if (!(thisPair = confread_find_pair(confSec, key))) {
		return 0;
	}

	return thisPair->value;

}

/* ----------------------------------------------------------------------------
FUNCTION

Name:		Close

Prototype:	void confread_close(struct confread_file** confFile)

Developer:	Andrew Burian

Created On:	2015-03-07

Parameters:
	struct confread_file** confFile
		a pointer to the address of the confread file to be freed

Description:
	Properly frees and closes all resources used by the confread file, and sets
	the pointer to null

Revisions:
	Andrew Burian
	2015-08-21
	Revised sections and pair to be a linked list rather than
	reallocating continuous memory sections.

---------------------------------------------------------------------------- */
void confread_close(struct confread_file **confFile)
{

	// temp section pointer
	struct confread_section *thisSection = 0;
	struct confread_section *nextSection = 0;

	// temp pair pointer
	struct confread_pair *thisPair = 0;
	struct confread_pair *nextPair = 0;

	// null check
	if (!confFile || !(*confFile)) {
		return;
	}
	// get the first section
	thisSection = (*confFile)->sections;

	// loop through all sections in the file
	while (thisSection) {

		// get the first pair in this section
		thisPair = thisSection->pairs;

		// loop through each key pair in the section
		while (thisPair) {

			// get the next pointer before freeing anything
			nextPair = thisPair->next;

			// free the two data elements in the pair
			free(thisPair->key);
			free(thisPair->value);

			// free the pair itself
			free(thisPair);

			// move to the next one
			thisPair = nextPair;

		}

		// get the next pointer before freeing anything
		nextSection = thisSection->next;

		// all pairs have been freed, now free the section data
		free(thisSection->name);

		// free the section itself
		free(thisSection);

		// move to the next one
		thisSection = nextSection;

	}

	// free the file section data
	free((*confFile)->name);

	// free the conf file itself
	free(*confFile);
	*confFile = 0;

}

/* ----------------------------------------------------------------------------
FUNCTION

Name:		Check Value

Prototype:	int confread_check_pair(struct confread_section *section,
			char* key, char *value);

Developer:	Andrew Burian

Created On:	2015-08-21

Parameters:
	struct confread_section *secion
		the section to search for the key-value pair in
	char *key
		the key of the pair to search for
	char *value
		the value to check if the key is found

Return Values:
	int
		boolean result
		1 if the key is present and the value matches
		0 otherwise

Description:
	Searches for the given key in the given section, and if it exists
	checks to see if the provided value matches the one in the pair.

Revisions:
	(none)

---------------------------------------------------------------------------- */
int confread_check_pair(struct confread_section *section, char *key, char *value){

	char *thisValue = 0;

	// sanity checks
	if(!section || !key || !value){
		return 0;
	}

	// attempt to find the value
	thisValue = confread_find_value(section, key);

	if(!thisValue){
		return 0;
	}

	return (!strcmp(value, thisValue));
}
