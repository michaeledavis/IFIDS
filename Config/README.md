

Intranet Firewall and Intrusion Detection System
================================================


About
-----

### Source Control:
- GitHub: https://github.com/Raikia/IFIDS

### Contributers:
- Chris King
- Michael Davis
- Sam Doerr

### Abstract:

The goal of IFIDS is to provide a simple, scalable, and highly customizable
solution for a firewall and intrusion detection system inside a network.  In
many networks, once the outer firewall is surpassed, a malicious user has 
free reign of the internal network with the ability to access all ports on
all servers.  One solution is to install a firewall on each client and server
on a network, but that quickly becomes difficult to maintain.  IFIDS is an
alternative that is quick to install, quick and intuitive to customize, and
still provides the same security.

### Function:

IFIDS is designed to run on a system that separates users, subnets, switches,
or routers.  The server running IFIDS must be in a position that requires two
interfaces to be bridged.  IFIDS acts as a friendly man-in-the-middle, detecting
irregular activity, denying/allowing access to predetermined locations, and
alerting administrators of potential compromises.  Its focus is to maintain the
functionality of firewall software (such as IPTables), while simplifying setup
and enhancing scalability.

### Future:

Currently, IFIDS runs as a standalone server.  This is not a plausible solution
if it is deployed across a large network (each IFIDS instance would require
configuration).  In the future, we plan to implement a client-server architecture,
allowing each IFIDS instance to query an IFIDS server for its configuration.
This will simplify installation and significantly boost scalability.

### License:

Intranet Firwall and Intrusion Detection System (IFIDS)    
Copyright (C) 2012  Christopher King, Michael Davis, Sam Doerr

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see http://www.gnu.org/licenses/

For more information, contact Christopher King at ccking@smu.edu


Installation:
-------------

This compiles and installs the program onto your system

	make
	sudo make install

Usage:
------

Depending on the Linux distribution, one of the following commands should work:

	sudo service ifids start

	sudo /etc/init.d/ifids start

	sudo /etc/rc.d/ifids start

If none of the above work, execute:

	sudo ./Scripts/ifids start

Uninstall:
----------

This removes all traces of IFIDS from your system

	sudo make uninstall

Clean:
------

This removes the compiled binaries from their built locations, but does not
uninstall anything from your system

	make clean


