/*
 * Intranet Firwall and Intrusion Detection System (IFIDS)    
 * Copyright (C) 2012  Christopher King, Michael Davis, Sam Doerr
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/
 *
 * For more information, contact Christopher King at ccking@smu.edu
 *
 */

#ifndef CONFIG_H
#define CONFIG_H

#define STR(n) strdup(n)
	#define YES 1
	#define NO 0
#define BOOL(n) (strcasecmp(n,"yes")==0 || strcasecmp(n,"true")==0)?YES:NO


#define FREE(n) free(n);
#define NOFREE(n) 


// The following preprocessor commands have this format:
//        F("name of settings command", name of struct attribute, "default setting", Preprocessor function to interpret data, type of data, FREE or NOFREE)

#define IP_ATTRIBUTES(F) \
	F("allow_ip",allowip, NULL, STR, char*, FREE)

#define GENERAL_ATTRIBUTES(F) \
	F("allow_all_ip",allow_all_ip,YES, BOOL, int, NOFREE) \
	F("allow_all_port", allow_all_port,NO, BOOL, int, NOFREE)



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
#define load(a,b,c,d,e,f) e b;\

	GENERAL_ATTRIBUTES(load)
#undef load
	//char* allow_all_ip;
	//char* allow_all_port;
};
// END general changes

// Change the values that IP ranges can have here
struct ip_config
{
	char* ip_spread;
#define load(a,b,c,d,e,f) e b;\

	IP_ATTRIBUTES(load)
#undef load
	//char* allowip;
};
// END IP changes


void list_insert(ip_config*, configuration*);
ip_config* list_select(char*,configuration*);
void list_destroy(configuration*);
configuration* parseConfigFile(char*,int*);
void freeConfig(configuration*);
#endif
