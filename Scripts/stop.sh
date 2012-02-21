#!/bin/bash

LOG_LOCATION="/var/log/ifids";

if [ $EUID -ne 0 ]
then
	echo "Error: Must be run with root permissions";
	exit 1;
fi

if [ -a $LOG_LOCATION"/ifids_daemon.pid" ]
then
	echo -n "Stopping IFIDS daemon....................";
	pid=`cat $LOG_LOCATION"/ifids_daemon.pid"`;
	if ps $pid | grep ifids &> /dev/null
	then
		kill $pid;
		if [ $? == 0 ]
		then
			echo "done";
			rm $LOG_LOCATION"/ifids_daemon.pid"
		else
			echo "fail";
		fi
	else
		echo "\nPID File was found, but no service was running";
	fi
else
	echo "No PID file was found";
	possible_pid=`ps aux | grep -v grep | grep ifid | awk '{print $2}'`;
	if [ $possible_pid -ne 0 ]
	then
		echo -n "Found service, attempting to kill..................";
		kill $possible_pid;
		if [$? == 0 ]
		then
			echo "success";
		else
			echo "failed";
		fi
	else
		echo "No running service was found";
	fi
fi
