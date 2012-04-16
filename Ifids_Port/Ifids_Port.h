#ifndef IFIDS_PORT_H
#define IFIDS_PORT_H

#define STR(n) strdup(n)
	#define YES 1
	#define NO 0
#define BOOL(n) (strcasecmp(n,"yes")==0 || strcasecmp(n,"true")==0)?YES:NO


//Typedefs are so we don't have to repeatedly type "struct Port" instead of just "Port"
typedef struct ifids_port ifids_port;
//typedef struct ifids_ip_sect `ddifids_ip_sect;

struct ifids_port
{
	int high;
	int low;
};

ifids_port* parsePort(char* str);

int portIsInRange(char* str, ifids_port* portRange);

int portIfidsAreEqual(ifids_port* ip1, ifids_port* ip2);

int portEqualsRange(char* str, ifids_port* portRange);

#endif




