#define MOD_NAME "ifids_module"

#include <stdio.h>
#include <stdlib.h>
#include <string.h> // Needed for strtok
#include <syslog.h> // Needed for syslog interaction
#include <unistd.h> // Needed for STD[IN|OUT|ERR]_FILENO
#include <sys/stat.h> // Needed for umask
#include <inttypes.h> // Needed for (intmax_t), which is used in one place...

int main(void)
{
	if (getuid() > 0) // Check to see if program is being run by root
	{
		printf("              Error: You must run this program with root permissions.\n");
		exit(EXIT_FAILURE);
	}
	// Check to see if module is already loaded:
	FILE *modList = fopen("/proc/modules","r");
	if (modList == NULL)
	{
		printf("              Error: Could not get list of active modules.\n");
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
		printf("              Module was not found.  Adding module...\n");
		int ret = system("insmod /usr/lib/ifids/ifids_module.ko");
		if (ret == 0)
		{
			printf("             Module was successfully loaded\n");
		}
		else
		{
			printf("             Module could not be successfully loaded.\n");
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		printf("              Module is already loaded\n");
	}
	// End check to see if module is already loaded.  It is now loaded.
	pid_t pid, sid;
	FILE *logFile; // Declare log file
	pid = fork(); // Fork child process
	if (pid < 0) // if fork didn't work
	{
		printf("There was an error forking the process\n");
		exit(EXIT_FAILURE);
	}
	if (pid > 0) // If process is parent
	{
		// Write pid file saying where the child is
		logFile = fopen("/var/log/ifids/ifids_daemon.pid","w");
		fprintf(logFile, "%jd",(intmax_t)pid);
		fclose(logFile);
		printf("Stopping parent process...\n");
		exit(EXIT_SUCCESS);
	}
	// Change permissions
	umask(0);
	// Open syslog in case we have trouble with our own log file
	openlog ("IFIDS_Daemon", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
	syslog(LOG_NOTICE, "IFIDS_Daemon was started...\n");
	logFile = fopen("/var/log/ifids/ifids_daemon.log","a");
	if (logFile == NULL) // Can't edit log file
	{
		syslog(LOG_NOTICE, "IFIDS_Daemon was unable to open its own log file...exiting.\n");
		closelog();
		exit(EXIT_FAILURE);
	}
	syslog(LOG_NOTICE, "IFIDS_Daemon started successfully. Switching to own log file\n");
	closelog(); // Close syslog because now we have our own log file
	fprintf(logFile, "IFIDS_Daemon was started...\n");
	sid = setsid(); // Get session id
	if (sid < 0) // If session id doesn't work
	{
		fprintf(logFile, "There was an error setting the session id\n");
		exit(EXIT_FAILURE);
	}
	if (chdir("/") < 0) // Change directory to /
	{
		fprintf(logFile, "Could not switch to root location\n");
		exit(EXIT_FAILURE);
	}
	// Because we are child process, close STD[IN|OUT|ERR]
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	// Is it flushing?
	fprintf(logFile, "IFIDS_Daemon entering serice loop successfully.\n");
	fflush(logFile);
	while (1)
	{
		// This is where all the code will go for our daemon
		sleep(60);
		break;
	}
	// Complete log file
	fprintf(logFile, "Program completed successfully\n");
	fclose(logFile);
	exit(EXIT_SUCCESS);
}
