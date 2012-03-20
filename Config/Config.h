#ifndef LINKLIST_H
#define LINKLIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ini.h"

typedef struct Node Node;
typedef struct ip_config ip_config;
typedef struct configuration configuration;

struct Node
{
	Node* prev;
	Node* next;
	ip_config* data;
};

// Change the general values the configurations can have here
struct configuration
{
	Node* ip_list;
	char* allow_all_ip;
	char* allow_all_port;
};
// END general changes

// Change the values that IP ranges can have here
struct ip_config
{
	char* ip_spread;
	char* allowip;
};
// END IP changes


void list_insert(ip_config*, configuration*);
ip_config* list_select(char*,configuration*);
void list_destroy(configuration*);
configuration* parseConfigFile(char*,int*);

#endif
