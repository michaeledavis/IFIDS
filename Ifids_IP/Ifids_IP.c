#define MATCH(n) strcasecmp(name,n) == 0

#include "Ifids_IP.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

ifids_ip* parseIP(char* str)
{
	char input[40];
	strcpy(input,str);

	char* buffer;
	char* part_buffer;
	char *saveptr1, *saveptr2;

	ifids_ip* ips = (ifids_ip*)malloc(sizeof(ifids_ip));
	ifids_ip_sect* se;

	int x = 0;
	buffer = strtok_r(input, ".", &saveptr1);
	while(buffer != NULL && x < 4)
	{
		se = (ifids_ip_sect*)malloc(sizeof(ifids_ip_sect));
		part_buffer = strtok_r(buffer, "-", &saveptr2);
		if(part_buffer != NULL)
		{
			se->low = atoi(part_buffer);
			part_buffer = strtok_r(NULL, "-", &saveptr2);
			if(part_buffer != NULL)
				se->high = atoi(part_buffer);
			else
				se->high = se->low;
		}
		switch(x)
		{
			case 0:
				ips->first = se;
				break;
			case 1: 
				ips->second = se;
				break;
			case 2:
				ips->third = se;
				break;
			case 3:
				ips->fourth = se;
				break;
		}
		++x;
		buffer = strtok_r(NULL, ".", &saveptr1);

	}
	return ips;
}

int ipIsInRange(char* str, ifids_ip* ipRange)
{
	if(str == NULL)
		return 0;	

	char input[40];
	strcpy(input, str);

	char* part_buffer;
	int intBuffer;
	int willContinue = 1;

	part_buffer = strtok(input, ".");
	int x = 0;
	while(part_buffer != NULL && x < 4 && willContinue)
	{	
		intBuffer = atoi(part_buffer);
		switch(x)
		{
			case 0:
				if(intBuffer >= ipRange->first->low)
					if(intBuffer <= ipRange->first->high)
					{	
						willContinue = 1;
						break;
					}
				willContinue = 0;
				break;
			case 1:
				if(intBuffer >= ipRange->second->low)
					if(intBuffer <= ipRange->second->high)
					{
						willContinue = 1;
						break;
					}
				willContinue = 0;
				break;	
			case 2:
				if(intBuffer >= ipRange->third->low)
					if(intBuffer <= ipRange->third->high)
					{
						willContinue = 1;
						break;
					}
				willContinue = 0;
				break;
			case 3:
				if(intBuffer >= ipRange->fourth->low)
					if(intBuffer <= ipRange->fourth->high)
					{
						willContinue = 1;
						break;
					}
				willContinue = 0;
				break;
			default: 
				willContinue = 0;
				break;
		}
		part_buffer = strtok(NULL, ".");	
		++x;
	}
	return willContinue;
}

int ifidsAreEqual(ifids_ip* ip1, ifids_ip* ip2)
{
	if(ip1->first->high == ip2->first->high)
	{
	  if(ip1->first->low == ip2->first->low)
	  {
	    if(ip1->second->high == ip2->second->high )
	    {
	      if(ip1->second->low == ip2->second->low)
	      {
	        if(ip1->third->high == ip2->third->high)
		{
		  if(ip1->third->low == ip2->third->low)
		  {
		    if(ip1->fourth->high == ip2->fourth->high)
		    {
		      if(ip1->fourth->low == ip2->fourth->low)
		      {
			return 1;
		      }
		    }	
		  }
		}
	      }
	   }
	}
    }
    return 0;
    
}

int ipEqualsRange(char* str, ifids_ip* ipRange)
{
	int result;
	
	result = ifidsAreEqual(ipRange,parseIP(str));
	return result;
}

void freeIP(ifids_ip* ipToFree){
	if(ipToFree != NULL)
	{
		if(ipToFree->first != NULL)
		{
			free(ipToFree->first);
		}
		if(ipToFree->first != NULL)
		{
			free(ipToFree->second);
		}
		if(ipToFree->third != NULL)
		{
			free(ipToFree->third);
		}
		if(ipToFree->fourth != NULL)
		{
			free(ipToFree->fourth);
		}
		free(ipToFree);
	}
}
