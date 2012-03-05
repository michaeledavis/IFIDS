CC=gcc
CFLAGS=-Wall
EXECUTIBLE=ifids_daemon
SOURCES=ifids_daemon.c 

all:
	@echo Building IFIDS Daemon
	@echo "   CC	$(SOURCES)"
	@$(CC) $(CFLAGS) -o $(EXECUTIBLE) $(SOURCES)
clean:
	@echo Cleaning IFIDS Daemon
	@echo "   RM	$(EXECUTIBLE)"
	@rm -rf $(EXECUTIBLE)
