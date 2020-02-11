#ifndef FILEREADER_H
#define FLIEREADER_H

#include <stdbool.h> 

#define MAX_STRING_LEN 50
#define MAX_NUMBER_FILES 50

/**
* This file handles reading directories and files
*/

/**
*	Parameters: Path to directory
				Array to hold file names found
				SizeOfArray sent
	Returns -1 if Failed
	Returns the number of files found excluding files starting with '.' and '..'
	Stores found files inside the fileArray parameter passed in
*/
int listFiles(const char *path, char (*fileArray)[MAX_STRING_LEN], bool verbose);

#endif
