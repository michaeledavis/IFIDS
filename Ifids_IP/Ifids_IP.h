#ifndef IFIDS_IP_H
#define IFIDS_IP_H

#define STR(n) strdup(n)
	#define YES 1
	#define NO 0
#define BOOL(n) (strcasecmp(n,"yes")==0 || strcasecmp(n,"true")==0)?YES:NO


//Typedefs are so we don't have to repeatedly type "struct IP" instead of just "IP"
typedef struct ifids_ip ifids_ip;
typedef struct ifids_ip_sect ifids_ip_sect;

struct ifids_ip_sect
{
	int high;
	int low;
};

struct ifids_ip
{
	ifids_ip_sect* first;
	ifids_ip_sect* second;
	ifids_ip_sect* third;
	ifids_ip_sect* fourth;
};

ifids_ip* parseIP(char* str);

int ipIsInRange(char* str, ifids_ip* ipRange);

int ifidsAreEqual(ifids_ip* ip1, ifids_ip* ip2);

int ipEqualsRange(char* str, ifids_ip* ipRange);

#endif




