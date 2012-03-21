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

#define MATCH(n) strcasecmp(name,n) == 0

#include "Config.h"

void list_insert(ip_config* ipc, configuration* mainConfig)
{
	if (!mainConfig->ip_list)
	{
		mainConfig->ip_list = (Node*)malloc(sizeof(Node));
		mainConfig->ip_list->data = ipc;
		mainConfig->ip_list->next = NULL;
		mainConfig->ip_list->prev = NULL;
	}
	else
	{
		Node* n = (Node*)malloc(sizeof(Node));
		n->data = ipc;
		n->next = mainConfig->ip_list;
		n->prev = NULL;
		mainConfig->ip_list->prev = n;
		mainConfig->ip_list = n;
	}
}

ip_config* list_select(char* search, configuration* mainConfig)
{
	Node* curNode = mainConfig->ip_list;
	while (curNode)
	{
		if (strcmp(curNode->data->ip_spread,search) == 0)
			return curNode->data;
		curNode = curNode->next;
	}
	return NULL;
}

void list_destroy(configuration* mainConfig)
{
	Node* curNode = mainConfig->ip_list;
	while (curNode)
	{
		if (curNode->data)
		{
			free(curNode->data->ip_spread);
#define load(a,b,c,d,e,f) f(curNode->data->b)
			IP_ATTRIBUTES(load)
#undef load
			free(curNode->data);
		}
		Node *tmp = curNode;
		curNode = curNode->next;
		free(tmp);
	}
	mainConfig->ip_list = NULL;
	return;
}
static int handler(void* user, const char* section, const char* name, const char* value)
{
	configuration* config = (configuration*)user;
	// GENERAL SECTION
	if (strcasecmp(section,"general") == 0)
	{
		// Keys of general section go here
#define load(a,b,c,d,e,f) if (MATCH(a)) config->b = d(value);
		GENERAL_ATTRIBUTES(load)
#undef load
		/*if (MATCH("allow_all_ip"))
			config->allow_all_ip = STR(value);
		else if (MATCH("allow_all_port"))
			config->allow_all_port = STR(value);*/
	}
	else // Must be IP range?
	{
		ip_config* configIP = NULL;
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

