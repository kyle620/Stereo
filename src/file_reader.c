#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

#include "file_reader.h"

int listFiles(const char *path, char (*fileArray)[MAX_STRING_LEN], bool verbose)
{
	struct dirent *dp;
	int count = 0;
	char fileName[255];
    DIR *dir = opendir(path);

    // Unable to open directory stream
    if (!dir)
	{
        return -1; 
	}
	
	if(verbose)
	{
		printf("Contents Inside %s\n",path);
	}
	
    while ((dp = readdir(dir)) != NULL)
    {
		strncpy(fileName,dp->d_name, 254);
		
		if(strcmp(fileName,".") == 0 || strcmp(fileName,"..") == 0)
			continue;
		
		if(count < MAX_NUMBER_FILES)
		{
			if(verbose)
			{
			printf("\t--> %s\n", fileName);
			}
			strcpy(fileArray[count],fileName);
			count++;
		}
		else		// array is full we need to exit
			break;
    }
	
	if(verbose)
	{
		printf("Total Found: %d\n",count);
	}
	
    // Close directory stream
    closedir(dir);
	
	return count;
}
