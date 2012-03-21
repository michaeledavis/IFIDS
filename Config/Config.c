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

#include "Config.h"

// list_insert puts the ip_config struct (ipc) into the linked list that is located
// in mainConfig->ip_list (configuration struct).  All IPs are inserted into the
// front of the list, so later configuration settings take precedence over former.
void list_insert(ip_config* ipc, configuration* mainConfig)
{
	if (!mainConfig->ip_list) // If the linked list is non-existant (first IP inserted)
	{
		mainConfig->ip_list = (Node*)malloc(sizeof(Node)); // Allocate memory for the Node
		mainConfig->ip_list->data = ipc; // Set the data of the Node to the ip_config struct
		mainConfig->ip_list->next = NULL; // Set the two location pointers to null
		mainConfig->ip_list->prev = NULL;
	}
	else // The linked list already has information in it, so add to the head of the list
	{
		Node* n = (Node*)malloc(sizeof(Node)); // Allocate memory for the new Node
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
	while (curNode)
	{
		if (curNode->data) // If the data exists (which it should)
		{
			free(curNode->data->ip_spread); // Free the IP
#define load(a,b,c,d,e,f) f(curNode->data->b)
			IP_ATTRIBUTES(load) // Free all the IP attributes
#undef load
			free(curNode->data); // Free the entire ip_config data
		}
		Node *tmp = curNode; // Temporarily save Node location
		curNode = curNode->next; // Progress curNode to the next location
		free(tmp); // Free last Node's memory
	}
	mainConfig->ip_list = NULL; // Set the linked list HEAD pointer to NULL
	return;
}

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
			if (strcasecmp(section, config->ip_list->data->ip_spread) == 0)
			{
				configIP = config->ip_list->data;
			}
		}
		if (!configIP)
		{
			configIP = (ip_config*)malloc(sizeof(ip_config));
			configIP->ip_spread = strdup(section);
#define load(a,b,c,d,e,f) configIP->b = c;
			IP_ATTRIBUTES(load)
#undef load
			list_insert(configIP,config);
		}
		// Set IP range options here
#define load(a,b,c,d,e,f) if (MATCH(a)) configIP->b = d(value);
		IP_ATTRIBUTES(load)
#undef load
		/*if (MATCH("allow_ip"))
		{
			configIP->allowip = STR(value);
		}*/
	}
	return 1;
}
configuration* parseConfigFile(char* fileLoc,int* errorCode)
{
	configuration* mainConfig = (configuration*)malloc(sizeof(configuration));
	mainConfig->ip_list = NULL;
	// Set defaults here
#define load(a,b,c,d,e,f) mainConfig->b = c;
	//mainConfig->allow_all_ip = STR("yes");
	//mainConfig->allow_all_port = STR("yes");
	GENERAL_ATTRIBUTES(load)
#undef load
	// End defaults
	*errorCode = ini_parse(fileLoc, handler, mainConfig);
	if (*errorCode != 0)
		return NULL;
	return mainConfig;
}

void freeConfig(configuration* conf)
{
	list_destroy(conf);
#define load(a,b,c,d,e,f) f(conf->b);
	GENERAL_ATTRIBUTES(load)
#undef load
	free(conf);
}

