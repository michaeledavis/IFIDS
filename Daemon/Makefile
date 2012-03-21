CC=gcc
CFLAGS=-Wall -g -O0
EXECUTIBLE=ifids_daemon
SOURCES=ifids_daemon.c ../Config/*.c ../Config/*.h

all:
	@echo Building IFIDS Daemon
	@echo "   CC	$(SOURCES)"
	@$(CC) $(CFLAGS) -o $(EXECUTIBLE) $(SOURCES)
clean:
	@echo Cleaning IFIDS Daemon
	@echo "   RM	$(EXECUTIBLE)"
	@rm -rf $(EXECUTIBLE)
