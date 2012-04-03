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

// The following macro is used to compare the strings without case sensitivity
#define MATCH(n) strcasecmp(name,n) == 0

#ifdef __KERNEL__
	#define ALLOC(n) vmalloc(n)
#else
	#define ALLOC(n) malloc(n)
#endif

#include "Config.h"

// list_insert puts the ip_config struct (ipc) into the linked list that is located
// in mainConfig->ip_list (configuration struct).  All IPs are inserted into the
// front of the list, so later configuration settings take precedence over former.
void list_insert(ip_config* ipc, configuration* mainConfig)
{
	if (!mainConfig->ip_list) // If the linked list is non-existant (first IP inserted)
	{
		mainConfig->ip_list = (Node*)ALLOC(sizeof(Node)); // Allocate memory for the Node
		mainConfig->ip_list->data = ipc; // Set the data of the Node to the ip_config struct
		mainConfig->ip_list->next = NULL; // Set the two location pointers to null
		mainConfig->ip_list->prev = NULL;
	}
	else // The linked list already has information in it, so add to the head of the list
	{
		Node* n = (Node*)ALLOC(sizeof(Node)); // Allocate memory for the new Node
		n->data = ipc; // Set the data to the ip_config struct
		n->next = mainConfig->ip_list; // Set the "next" location to the head of the current list
		n->prev = NULL; // Set the previous location to null, since its going to be at the head of the list
		mainConfig->ip_list->prev = n; // Set the previous of the current head in the list to our new Node
		mainConfig->ip_list = n; // Set our new Node to the head of the list
	}
}

// list_select searches for, and returns, the ip_config struct for the corresponding IP address that is sent.
// Currently, the IP is represented by a c-string, which will probably change due to efficiency costs
ip_config* list_select(char* search, configuration* mainConfig)
{
	Node* curNode = mainConfig->ip_list; // Set Node pointer to the head of the list so we can loop through the IPs
	while (curNode)
	{
		// If the current Node's ip_spread is equal to what we are searching for, return the ip_config struct!
		if (strcmp(curNode->data->ip_spread,search) == 0)
			return curNode->data;
		curNode = curNode->next; // Progress through linked list
	}
	return NULL; // Didn't find the IP, return NULL
}

// list_destroy frees up the memory for all the allocations that were made in the linked list.  This is only called
// by freeConfig, and shouldn't be used otherwise
void list_destroy(configuration* mainConfig)
{
	Node* curNode = mainConfig->ip_list; // Node pointer to loop through linked lit
	Node* tmp = NULL;
	while (curNode)
	{
		if (curNode->data) // If the data exists (which it should)
		{
			FREE(curNode->data->ip_spread); // Free the IP
#define load(a,b,c,d,e,f) f(curNode->data->b)
			IP_ATTRIBUTES(load) // Free all the IP attributes
#undef load
			FREE(curNode->data); // Free the entire ip_config data
		}
		tmp = curNode; // Temporarily save Node location
		curNode = curNode->next; // Progress curNode to the next location
		FREE(tmp); // Free last Node's memory
	}
	mainConfig->ip_list = NULL; // Set the linked list HEAD pointer to NULL
	return;
}

#ifndef __KERNEL__
// handler is called for EVERY key-value pair found in the configuration file.  Look at inih documentation
// for more details
static int handler(void* user, const char* section, const char* name, const char* value)
{
	configuration* config = (configuration*)user;
	if (strcasecmp(section,"general") == 0) // If the current section is "General", these are the general settings
	{
		// Keys of general section go here
#define load(a,b,c,d,e,f) if (MATCH(a)) config->b = d(value);
		GENERAL_ATTRIBUTES(load) // Search for all the possible general attribute matches.  GENERAL_ATTRIBUTES is defined in Config.h
#undef load
	}
	else // If its not in the general section, then it's in an IP section (assume the user isn't stupid)
	{
		ip_config* configIP = NULL;
		// If the configuration exists, if the linked list exists, and if the data exists in the
		// first Node of the linked list, then we want to see if this key-value pair is apart of the
		// current HEAD of the list.
		if (config && config->ip_list && config->ip_list->data && config->ip_list->data->ip_spread)
		{
			// If the HEAD of the list is the IP section we are currently in, we want to edit its attribute
			if (strcasecmp(section, config->ip_list->data->ip_spread) == 0)
			{
				configIP = config->ip_list->data;
			}
		}
		// If the IP section isn't at the HEAD of the linked list, insert it!
		if (!configIP)
		{
			configIP = (ip_config*)malloc(sizeof(ip_config)); // Allocate the new ip_config struct
			configIP->ip_spread = strdup(section); // Set the IP attribute of the struct
#define load(a,b,c,d,e,f) configIP->b = c;
			IP_ATTRIBUTES(load) // Initialize all the IP_ATTRIBUTES of the struct to their defaults
#undef load
			list_insert(configIP,config); // Add the struct to the HEAD of the list
		}
		// Set IP range options here
#define load(a,b,c,d,e,f) if (MATCH(a)) configIP->b = d(value);
		IP_ATTRIBUTES(load) // Set the attribute that corresponds to the key-value pair in the config file
#undef load
	}
	return 1; // Return no error
}
// parseConfigFile takes the file location and an errorCode int pointer, and returns a "configuration" struct pointer
configuration* parseConfigFile(char* fileLoc,int* errorCode)
{
	// Allocate the configuration struct that we will populate with all the config settings
	configuration* mainConfig = (configuration*)malloc(sizeof(configuration));
	mainConfig->ip_list = NULL; // Pre-set the ip_list pointer to NULL
#define load(a,b,c,d,e,f) mainConfig->b = c;
	GENERAL_ATTRIBUTES(load) // Load the general attributes and initialize them to their defaults
#undef load
	*errorCode = ini_parse(fileLoc, handler, mainConfig); // Call inih library to parse the configuration file
	if (*errorCode != 0) // If there was any error, we want to return NULL and die since the inih library does not
		return NULL;
	return mainConfig;
}
#endif
// freeConfig acts like the destructor for configuration structs.  It frees up all the allocated memory
void freeConfig(configuration* conf)
{
	list_destroy(conf); // Free the ip linked list memory
#define load(a,b,c,d,e,f) f(conf->b);
	GENERAL_ATTRIBUTES(load) // Free the general attributes of the struct based on FREE or NOFREE macros
#undef load
	FREE(conf); // Free the configuration struct itself
}

