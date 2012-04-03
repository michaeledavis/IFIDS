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

// Define the "type" macros for later use
#define STR(n) strdup(n)
	#define YES 1
	#define NO 0
#define BOOL(n) (strcasecmp(n,"yes")==0 || strcasecmp(n,"true")==0)?YES:NO

// Define whether the attribute should be freed or not
#ifdef __KERNEL__
	#define FREE(n) vfree(n);
#else
	#define FREE(n) free(n);
#endif

#define NOFREE(n) 


// The following preprocessor macros have this format:
//        F(name of settings command, name of struct attribute, default setting, Preprocessor function to interpret data, type of data, FREE or NOFREE)

// Attributes for each IP section inside the config file
#define IP_ATTRIBUTES(F) \
	F("allowip",allowip, NULL, STR, char*, FREE)

// Attributes for the general section inside the config file
#define GENERAL_ATTRIBUTES(F) \
	F("allow_all_ip",allow_all_ip,YES, BOOL, int, NOFREE) \
	F("allow_all_port", allow_all_port,NO, BOOL, int, NOFREE)


#ifndef __KERNEL__
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include "ini.h"
#else
	#include <linux/vmalloc.h>
#endif

// Typedefs are so we don't have to repeatedly type "struct Node" instead of just "Node"
typedef struct Node Node;
typedef struct ip_config ip_config;
typedef struct configuration configuration;

// Node struct represents each link inside the linked list that holds the IP data
struct Node
{
	Node* prev;
	Node* next;
	ip_config* data;
};

// configuration struct represents the general configuration structure that contains
// both general attributes as well as a pointer to the HEAD of the IP linked list
struct configuration
{
	Node* ip_list;
#define load(a,b,c,d,e,f) e b;
	GENERAL_ATTRIBUTES(load) // Add the general attributes to the configuration struct
#undef load
};

// ip_config struct represents each IP section that is encountered, and contains all the
// configuration settings for each section
struct ip_config
{
	char* ip_spread; // IP address representation
#define load(a,b,c,d,e,f) e b;
	IP_ATTRIBUTES(load) // Add the IP attributes to the ip_config struct
#undef load
};

// Function declarations!

// list_insert inserts a Node struct (with Node->data pointer to the supplied ip_config struct) and
// inserts the new Node to the HEAD of the linked list in the supplied configuration struct
void list_insert(ip_config*, configuration*);

// list_select returns the entire ip_config struct representing the IP that is searched for
ip_config* list_select(char*,configuration*);

// list_destroy should ONLY be used in freeConfig to free the linked list from memory.  This might be
// taken out of the header file in the future, since there is no reason to have it in here
void list_destroy(configuration*);
#ifndef __KERNEL__
// parseConfigFile takes the file location as a c-string, and an errorcode integer pointer and parses
// the supplied configuration file, returning a pointer to a configuration struct and setting the 
// errorcode integer to the correct corresponding value (0 if no error, !=0 if error)
configuration* parseConfigFile(char*,int*);
#endif
// freeConfig frees all the memory for the entire configuration struct, including the ip linked list
void freeConfig(configuration*);

#endif

