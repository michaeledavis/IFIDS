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

// MOD_NAME is the name of the module.  This should only change if the module's name changes
#define MOD_NAME "ifids_module"
// LOGFILE is the location of the log to write to.  This should correspond to the Makefile location
#define LOGFILE "/var/log/ifids/ifids_daemon.log"
// PIDFILE is the location of the PID file for the child process.  This should correspond to the Makefile location
#define PIDFILE "/var/run/ifids_daemon.pid"
// MODULES_LIST is the location of the modules list created by the Linux kernel (shouldn't change)
#define MODULES_LIST "/proc/modules"
// MODULE_LOCATION is the location of the module KO file once installed.  This should correspond to the Makefile's location
#define MODULE_LOCATION "/usr/sbin/ifids/ifids_module.ko"
// CONFIG_LOCATION is the location for the configuration file once installed.  This should correspond to the Makefile's location
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
FILE *logFile; // Pointer to the log file location
int keepGoing; // Acts as a boolean to see if the main service loop should keep executing ("infinite" loop).  This only changes in the sigHandler function

// This should be used to write logs.  If NO_DAEMON is defined, it prints to the console; otherwise to the log file
void writeLog(char*);

// This is used to handle the terminating signal (kill and ctrl-C) so the daemon stops correctly
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
	logFile = fopen(LOGFILE,"a"); // Attempt to open the log file location with appending rights
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
	char *mods = (char*)malloc(sizeof(char)*100); // Allocate a buffer to read the modules list
	int found = 0; // Boolean to see if our module was found
	while (fgets(mods,100,modList) != NULL) // Read through the modules list
	{
		char *pck = strtok(mods," "); // Split on space to get the module names
		if (strcmp(pck,MOD_NAME) == 0) // If the module is our module's name, then it is already loaded
		{
			found = 1; // Set to 1 to represent that our module is loaded
			break; // Break out of loop: stop searching the modules list
		}
	}
	fclose(modList); // Close the modules list
	free(mods); // Free the buffer
	if (found == 0) // If the module is NOT loaded
	{
		writeLog("Module was not found.  Adding module...\n");
		int ret = system("insmod " MODULE_LOCATION); // Attempt to load the module...hopefully "insmod" works
		if (ret == 0)
		{
			writeLog("Module was successfully loaded\n"); // Loaded successfully
		}
		else
		{
			writeLog("Module could not be successfully loaded.\n"); // Didn't load!  Maybe "insmod" doesn't work or maybe the module location wasn't found
			exit(EXIT_FAILURE);
		}
	}
	else // Module already loaded into the kernel
	{
		writeLog("Module is already loaded\n"); 
	}
	// End check to see if module is already loaded.  It is now loaded.
	writeLog("Parsing Configuration File:  ");
	int lineError = 0; // Used to hold the line number if there is an error.  lineError == 0 if no error, lineError > 0 if error in config file, lineError < 0 if the config file couldn't be opened
	configuration* config = parseConfigFile(CONFIG_LOCATION,&lineError); // Parse config file.  This function is in Config/Config.c
	if (config == NULL || lineError != 0) // If there was an error in the config file
	{
		writeLog("FAILED\n");
		char err[100]; // Statically allocate buffer to create string
		sprintf(err, "Error on line %d of configuration file (%s)\n\n",lineError, CONFIG_LOCATION);
		writeLog(err);
		exit(EXIT_FAILURE);
	}
	else // Config file worked!
		writeLog("SUCCESS\n");
	// Write the config settings that we got back into the log for debug support:
	writeLog("Configuration Settings:\n\n");
	char* buffer = (char*)malloc(sizeof(char)*100); // Allocate buffer to create string to write to the log
	// The following macro mess is due to the simplification of how we handle the configuration settings and
	// struct attributes.  This shouldn't be changed unless more macro types are added (like STR, BOOL).
#define BOOLA BOOL
#define STRA STR
#undef STR
#undef BOOL
#define STR "     %s: %s\n"
#define BOOL "     %s: %d\n"
#define load(A,B,C,D,E,F) sprintf(buffer, D,A, config->B); writeLog(buffer);
	writeLog("   [General]\n");
	// Prints the general attributes into the log file
	GENERAL_ATTRIBUTES(load)  // This loads the above macro for the GENERAL_ATTRIBUTES defined in Config/Config.h
#undef load
	Node* curNode = config->ip_list; // Pointer to the beginning of the ip_list linked list
	while (curNode) // Loop through all the nodes in the linked list until its empty
	{
		sprintf(buffer, "\n   [%s]\n",curNode->data->ip_spread); // Print the IP section
		writeLog(buffer);
#define load(A,B,C,D,E,F) sprintf(buffer, D, A, curNode->data->B); writeLog(buffer);
		// Prints the IP_ATTRIBUTES for the current IP section
		IP_ATTRIBUTES(load) // Load the above macro for the IP_ATTRIBUTES defined in Config/Config.h
#undef load
		writeLog("\n\n");
		curNode = curNode->next; // Progress through the linked list
	}
	free(buffer); // Free the buffer that we allocated
	// Clean up the macro mess that we made
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
	pid_t sid; // Session ID
#ifndef NO_DAEMON
	pid_t pid; // PID of child process
	pid = fork(); // Fork child process
	// Two processes are now executing here: Parent and Child.  pid == 0 for child process, pid > 0 for parent process, pid < 0 on error
	if (pid < 0) // if fork didn't work
	{
		// Fork didn't work.  Maybe couldn't allocate the memory for it?  Resource limit reached? Something else?
		writeLog("There was an error forking the process\n");
		exit(EXIT_FAILURE);
	}
	if (pid > 0) // If process is parent
	{
		// Write pid file saying where the child is
		FILE* pidFile = fopen(PIDFILE,"w");
		fprintf(pidFile, "%jd",(intmax_t)pid); // Writing PID to the file so we know where the child process is
		fclose(pidFile);
		writeLog("Stopping parent process...\n");
		exit(EXIT_SUCCESS); // Stop the parent process
	}
	// At this point, only one process (the child process) is executing
#endif
	// sigHandler will execute when the signal SIGTERM (or SIGINT) is sent
	signal(SIGTERM, sigHandler); // SIGTERM is sent when we stop the service with the KILL command
#ifdef NO_DAEMON
	signal(SIGINT, sigHandler); // SIGINT is sent when not running as a daemon and we send a ctrl-C interrupt
#endif
	keepGoing = 1; // Boolean to tell the main service loop to keep executing.  This only changes in sigHandler
	// Change permissions
	umask(0); // Sets the correct permissions for the process
	sid = setsid(); // Set the session ID since the process is a child process
	if (sid < 0) // If session id doesn't work
	{
		writeLog("There was an error setting the session id\n");
		exit(EXIT_FAILURE);
	}
	if (chdir("/") < 0) // Change directory to /, which is the only location guarenteed to exist
	{
		writeLog("Could not switch to root location\n");
		exit(EXIT_FAILURE);
	}
#ifndef NO_DAEMON
	// Because we are child process, close STD[IN|OUT|ERR].  This will save us memory
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
#endif
	writeLog("IFIDS_Daemon entering serice loop successfully.\n");
	// MAIN SERVICE LOOP
	//    This loop keeps executing until the SIGTERM or SIGINT signals are sent
	while (keepGoing == 1)
	{
		// Future code location
		sleep(1); // Sleep a bit so we don't take up the CPU
	}
	freeConfig(config); // Free the configuration memory
	writeLog("Configuration was freed successfully\n");
	writeLog("Program completed successfully\n");
	fclose(logFile); // Close the log file
	exit(EXIT_SUCCESS); // Successfully exit the daemon
}



// writeLog writes the supplied c-string to either the log file or the screen, depending on
// if the process is running as a daemon.  The log file buffer is flushed every time it is
// written so if a power failure (or other failure) occurs, we will know when it happened
// because our log file will be up-to-date
void writeLog(char * str)
{
#ifdef NO_DAEMON
	puts(str); // Print to the screen because we aren't running as a daemon
#else
	if (logFile != NULL) // Make sure the log file is open for writing
	{
		fputs(str,logFile); // Write str to the log file
		fflush(logFile); // Flush the writing buffer
	}
#endif
}


// sigHandler is only called if SIGTERM or SIGINT signals are sent to the process.  It simply changes
// "keepGoing" to 0 so the main service loop exits safely
void sigHandler(int t)
{
	writeLog("Daemon has received terminating signal.  Getting out of main loop\n");
	keepGoing = 0;
}

