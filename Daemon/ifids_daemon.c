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


// #define NO_DAEMON

#define MOD_NAME "ifids_module"
#define LOGFILE "/var/log/ifids/ifids_daemon.log"
#define PIDFILE "/var/run/ifids_daemon.pid"
#define MODULES_LIST "/proc/modules"
#define MODULE_LOCATION "/usr/sbin/ifids/ifids_module.ko"
#define CONFIG_LOCATION "/etc/ifids.conf"


#include <stdio.h>
#include <stdlib.h>
#include <signal.h> // Needed to handle signal (SIGTERM)
#include <string.h> // Needed for strtok
#include <syslog.h> // Needed for syslog interaction
#include <unistd.h> // Needed for STD[IN|OUT|ERR]_FILENO
#include <sys/stat.h> // Needed for umask
#include <inttypes.h> // Needed for (intmax_t), which is used in one place...
#include "../Config/Config.h"
FILE *logFile;
int keepGoing;

// This should be used to write logs.  If NO_DAEMON is defined, it prints to the console; otherwise to the log file
void writeLog(char*);

// This is used to handle the terminating signal
void sigHandler(int);

int main(void)
{
	if (getuid() > 0) // Check to see if program is being run by root
	{
		printf("              Error: You must run this program with root permissions.\n");
		exit(EXIT_FAILURE);
	}
	// Open syslog in case we have trouble with our own log file
	openlog ("IFIDS_Daemon", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
	syslog(LOG_NOTICE, "IFIDS_Daemon was started...\n");
	logFile = fopen(LOGFILE,"a");
	if (logFile == NULL) // Can't edit log file
	{
		syslog(LOG_NOTICE, "IFIDS_Daemon was unable to open its own log file...exiting.\n");
		closelog();
		exit(EXIT_FAILURE);
	}
	syslog(LOG_NOTICE, "IFIDS_Daemon started successfully. Switching to own log file\n");
	closelog(); // Close syslog because now we have our own log file
	// Check to see if module is already loaded:
	FILE *modList = fopen(MODULES_LIST,"r");
	if (modList == NULL)
	{
		writeLog("Error: Could not get list of active modules.\n");
		exit(EXIT_FAILURE);
	}
	char *mods = (char*)malloc(sizeof(char)*100);
	int found = 0;
	while (found == 0 && fgets(mods,100,modList) != NULL)
	{
		char *pck = strtok(mods," ");
		if (strcmp(pck,MOD_NAME) == 0)
		{
			found = 1;
			break;
		}
	}
	fclose(modList);
	free(mods);
	if (found == 0)
	{
		writeLog("Module was not found.  Adding module...\n");
		int ret = system("insmod " MODULE_LOCATION);
		if (ret == 0)
		{
			writeLog("Module was successfully loaded\n");
		}
		else
		{
			writeLog("Module could not be successfully loaded.\n");
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		writeLog("Module is already loaded\n");
	}
	// End check to see if module is already loaded.  It is now loaded.
	writeLog("Parsing Configuration File:  ");
	int lineError = 0;
	configuration* config = parseConfigFile(CONFIG_LOCATION,&lineError);
	if (config == NULL || lineError != 0)
	{
		writeLog("FAILED\n");
		//writeLog("Error on line %d of configuration file (%s)\n\n",lineError,CONFIG_LOCATION);
		char err[100];
		sprintf(err, "Error on line %d of configuration file (%s)\n\n",lineError, CONFIG_LOCATION);
		writeLog(err);
		exit(EXIT_FAILURE);
	}
	else
		writeLog("SUCCESS\n");
	// Write read configurations into the log files:
	writeLog("Configuration Settings:\n\n");
	char* buffer = (char*)malloc(sizeof(char)*100);
#define BOOLA BOOL
#define STRA STR
#undef STR
#undef BOOL
#define STR "     %s: %s\n"
#define BOOL "     %s: %d\n"
#define load(A,B,C,D,E,F) sprintf(buffer, D,A, config->B); writeLog(buffer);
	writeLog("   [General]\n");
	GENERAL_ATTRIBUTES(load)
#undef load
	Node* curNode = config->ip_list;
	while (curNode)
	{
		sprintf(buffer, "\n   [%s]\n",curNode->data->ip_spread);
		writeLog(buffer);
#define load(A,B,C,D,E,F) sprintf(buffer, D, A, curNode->data->B); writeLog(buffer);
		IP_ATTRIBUTES(load)
#undef load
		writeLog("\n\n");
		curNode = curNode->next;
	}
	free(buffer);
#define load(A,B,C,D,E,F)
#undef load
#undef STR
#undef BOOL
#define STR STRA
#define BOOL BOOLA
#undef STRA
#undef BOOLA
	writeLog("End configuration\n");
	// End config log write
	pid_t sid;
#ifndef NO_DAEMON
	pid_t pid;
	pid = fork(); // Fork child process
	if (pid < 0) // if fork didn't work
	{
		writeLog("There was an error forking the process\n");
		exit(EXIT_FAILURE);
	}
	if (pid > 0) // If process is parent
	{
		// Write pid file saying where the child is
		logFile = fopen(PIDFILE,"w");
		fprintf(logFile, "%jd",(intmax_t)pid);
		fclose(logFile);
		writeLog("Stopping parent process...\n");
		exit(EXIT_SUCCESS);
	}
#endif
	// Set signal handler for when Daemon is supposed to stop
	signal(SIGTERM, sigHandler);
#ifdef NO_DAEMON
	signal(SIGINT, sigHandler);
#endif
	keepGoing = 1;
	// Change permissions
	umask(0);
	sid = setsid(); // Get session id
	if (sid < 0) // If session id doesn't work
	{
		writeLog("There was an error setting the session id\n");
		exit(EXIT_FAILURE);
	}
	if (chdir("/") < 0) // Change directory to /
	{
		writeLog("Could not switch to root location\n");
		exit(EXIT_FAILURE);
	}
#ifndef NO_DAEMON
	// Because we are child process, close STD[IN|OUT|ERR]
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
#endif
	writeLog("IFIDS_Daemon entering serice loop successfully.\n");
	while (keepGoing == 1)
	{
		// This is where all our code will go
		sleep(1);
	}
	// Free configuration memory
	freeConfig(config);
	writeLog("Configuration was freed successfully\n");
	// Complete log file
	writeLog("Program completed successfully\n");
	fclose(logFile);
	exit(EXIT_SUCCESS);
}




void writeLog(char * str)
{
#ifdef NO_DAEMON
	puts(str);
#else
	if (logFile != NULL)
	{
		fputs(str,logFile);
		fflush(logFile);
	}
#endif
}

void sigHandler(int t)
{
	writeLog("Daemon has received terminating signal.  Getting out of main loop\n");
	keepGoing = 0;
}
