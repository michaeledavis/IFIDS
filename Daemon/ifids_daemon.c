#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h> // Needed for STD[IN|OUT|ERR]_FILENO
#include <sys/stat.h>
#include <inttypes.h>

int main(void)
{
	if (getuid() > 0)
	{
		printf("Error: You must run this program with root permissions.\n");
		exit(EXIT_FAILURE);
	}
	pid_t pid, sid;
	FILE *logFile;
	pid = fork();
	if (pid < 0)
	{
		printf("There was an error forking the process\n");
		exit(EXIT_FAILURE);
	}
	if (pid > 0)
	{
		logFile = fopen("/var/log/ifids/ifids_daemon.pid","w");
		fprintf(logFile, "%jd",(intmax_t)pid);
		fclose(logFile);
		printf("Stopping parent process...\n");
		exit(EXIT_SUCCESS);
	}
	umask(0);
	openlog ("exampleprog", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
	syslog(LOG_NOTICE, "IFIDS_Daemon was started...\n");
	logFile = fopen("/var/log/ifids/ifids_daemon.log","a");
	if (logFile == NULL)
	{
		syslog(LOG_NOTICE, "IFIDS_Daemon was unable to open its own log file...exiting.\n");
		closelog();
		exit(EXIT_FAILURE);
	}
	closelog();
	sid = setsid();
	if (sid < 0)
	{
		fputs("There was an error setting the session id\n", logFile);
		exit(EXIT_FAILURE);
	}
	if (chdir("/") < 0)
	{
		fputs("Could not switch to root location\n",logFile);
		exit(EXIT_FAILURE);
	}
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	while (1)
	{
		sleep(60000);
		break;
	}
	fputs("Program completed successfully\n",logFile);
	fclose(logFile);
	exit(EXIT_SUCCESS);
}

