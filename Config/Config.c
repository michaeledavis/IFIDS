#define MATCH(n) strcasecmp(name,n) == 0
#define STR(n) strdup(n)



#include "Config.h"

void list_insert(ip_config* ipc, configuration* mainConfig)
{
	if (!mainConfig->ip_list)
	{
		mainConfig->ip_list = (Node*)malloc(sizeof(Node));
		mainConfig->ip_list->data = ipc;
	}
	else
	{
		Node* n = (Node*)malloc(sizeof(Node));
		n->data = ipc;
		n->next = mainConfig->ip_list;
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
			free(curNode->data);
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
		if (MATCH("allow_all_ip"))
			config->allow_all_ip = STR(value);
		else if (MATCH("allow_all_port"))
			config->allow_all_port = STR(value);
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
			list_insert(configIP,config);
		}
		// Set IP range options here
		if (MATCH("allow_ip"))
		{
			configIP->allowip = STR(value);
		}

	}
	return 1;
}
configuration* parseConfigFile(char* fileLoc,int* errorCode)
{
	configuration* mainConfig = (configuration*)malloc(sizeof(configuration));
	// Set defaults here
	mainConfig->allow_all_ip = STR("yes");
	mainConfig->allow_all_port = STR("yes");
	// End defaults
	*errorCode = ini_parse(fileLoc, handler, mainConfig);
	if (*errorCode != 0)
		return NULL;
	return mainConfig;
}

