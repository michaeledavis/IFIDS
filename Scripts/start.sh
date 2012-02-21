#!/bin/bash

EXECUTIBLE="/usr/lib/ifids/ifids_daemon";
LOG_LOCATION="/var/log/ifids";


if [ $EUID -ne 0 ]
then
	echo "Error: Must be run as root.";
	exit 1;
fi

if [ -a $LOG_LOCATION"/ifids_daemon.pid" ]
then
	echo "Error: Daemon is currently running.  Stop it before starting a new one";
	exit 1;
fi


if [ -a $EXECUTIBLE ]
then
	echo -n "Starting IFIDS Daemon..........";
	$EXECUTIBLE &> /dev/null;
	#$EXECUTIBLE;
	if [ $? == 0 ]
	then
		echo "done";
	else
		echo "fail";
	fi
else
	echo "No IFIDS Daemon found.  Is it installed?";
fi
