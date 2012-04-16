#define MATCH(n) strcasecmp(name,n) == 0

#include "Ifids_Port.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

ifids_port* parsePort(char* str)
{
	char input[40];
	strcpy(input,str);

	char* buffer;
	char *saveptr1;

	ifids_port* ips = (ifids_port*)malloc(sizeof(ifids_port));

	buffer = strtok_r(input, "-", &saveptr1);
	if(buffer != NULL)
	{

		ips->low = atoi(buffer);
		buffer = strtok_r(NULL, "-", &saveptr1);
		if(buffer != NULL){
			ips->high = atoi(buffer);
		}
	}
	return ips;
}

int portIsInRange(char* str, ifids_port* portRange)
{
	if(str == NULL)
		return 0;	

	char input[40];
	strcpy(input, str);

	int intBuffer;

	intBuffer = atoi(input);

	if(intBuffer > portRange->low)
		if(intBuffer < portRange->high)
		{
			return 1;
		}

	return 0;
}

int portIfidsAreEqual(ifids_port* port1, ifids_port* port2)
{
	if(port1->high == port2->high)
	{
		if(port1->low == port1->low)
		{
			return 1;
		}
	}

    return 0;
}

int portEqualsRange(char* str, ifids_port* portRange)
{
	int result;
	
	result = portIfidsAreEqual(portRange,parsePort(str));

	return result;
}

void freeIP(ifids_port* portToFree){
	if(portToFree != NULL)
	{
		free(portToFree);
	}
}
