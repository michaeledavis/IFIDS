SHELL := /bin/bash

# Names of binaries/scripts before install
SCRIPT := ifids
DAEMON := ifids_daemon
MODULE := ifids_module.ko

# Names of binaries/scripts after install
PUT_SCRIPT = $(SCRIPT)
PUT_DAEMON = $(DAEMON)
PUT_MODULE = $(MODULE)

# Binary locations
BIN_DAEMON_LOCATION := ./Daemon/
BIN_MODULE_LOCATION := ./Module/
BIN_SCRIPT_LOCATION := ./Scripts/

# Install locations
DAEMON_LOCATION := /usr/lib/ifids/
MODULE_LOCATION := /usr/lib/ifids/
# Keep the following blank!
SCRIPT_LOCATION := 

# Log location
LOG_LOCATION := /var/log/ifids/

# Determines correct path for SCRIPT_LOCATION
ifeq ($(shell test -d /etc/init.d/ && echo true),true)
	SCRIPT_LOCATION := /etc/init.d/
endif
ifeq ($(shell test -d /etc/rc.d/ && echo true),true)
	SCRIPT_LOCATION := /etc/rc.d/
endif



all:
	@echo "Making IFIDS Components"
	@cd $(BIN_DAEMON_LOCATION); make
	@cd $(BIN_MODULE_LOCATION); make
	@echo "IFIDS Components Built"
install:
	@echo "Installing IFIDS Components"
	@if [ ! -d "$(MODULE_LOCATION)" ]; then \
		echo "   MK	$(MODULE_LOCATION)"; \
		mkdir -p $(MODULE_LOCATION); \
	fi
	@if [ ! -d "$(DAEMON_LOCATION)" ]; then \
		echo "   MK	$(MODULE_LOCATION)"; \
		mkdir -p $(DAEMON_LOCATION); \
	fi
	@cp $(BIN_DAEMON_LOCATION)$(DAEMON) $(DAEMON_LOCATION)$(PUT_DAEMON)
	@echo "   CP	$(BIN_DAEMON_LOCATION)$(DAEMON) $(DAEMON_LOCATION)$(PUT_DAEMON)"
	@cp $(BIN_MODULE_LOCATION)$(MODULE) $(MODULE_LOCATION)$(PUT_MODULE)
	@echo "   CP	$(BIN_MODULE_LOCATION)$(MODULE) $(MODULE_LOCATION)$(PUT_MODULE)"
	@if [ "$(SCRIPT_LOCATION)" != "" ]; then \
		cp $(BIN_SCRIPT_LOCATION)$(SCRIPT) $(SCRIPT_LOCATION)$(PUT_SCRIPT); \
		echo "   CP	$(BIN_SCRIPT_LOCATION)$(SCRIPT) $(SCRIPT_LOCATION)$(PUT_SCRIPT)"; \
	else \
		echo "   Warning: Couldn't install script.  Manually install $(BIN_SCRIPT_LOCATION)$(SCRIPT)"; \
	fi
	@mkdir -p $(LOG_LOCATION)
	@echo "   MK	$(LOG_LOCATION)"
	@echo "IFIDS Installed."
uninstall:
	@echo "Uninstalling IFIDS Components"
	@echo "   STOP	$(BIN_SCRIPT_LOCATION)$(SCRIPT)"
	@$(BIN_SCRIPT_LOCATION)$(SCRIPT) stop > /dev/null
	@echo "   RM	$(DAEMON_LOCATION)$(PUT_DAEMON)"
	@rm -f $(DAEMON_LOCATION)$(PUT_DAEMON)
	@echo "   RM	$(MODULE_LOCATION)$(PUT_MODULE)"
	@rm -f $(MODULE_LOCATION)$(PUT_MODULE)
	@echo "   RM	$(LOG_LOCATION)"
	@rm -rf $(LOG_LOCATION)
	@if rmdir $(DAEMON_LOCATION) &> /dev/null; \
	then \
		echo "   RM	$(DAEMON_LOCATION)"; \
	fi
	@if [ -d $(MODULE_LOCATION) ]; then \
		if rmdir $(MODULE_LOCATION) &> /dev/null; \
		then \
			echo "   RM	$(MODULE_LOCATION)"; \
		fi \
	fi
	@if [ "$(SCRIPT_LOCATION)" != "" ]; then \
		echo "   RM	$(SCRIPT_LOCATION)$(PUT_SCRIPT)"; \
		rm -f $(SCRIPT_LOCATION)$(PUT_SCRIPT); \
	else \
		echo "   Warning: Couldn't remove service script.  Manually remove \"$(PUT_SCRIPT)\""; \
	fi
	@echo "Uninstall complete"
clean:
	@echo "Cleaning Components"
	@cd $(BIN_DAEMON_LOCATION); make clean
	@cd $(BIN_MODULE_LOCATION); make clean
	@echo "Clean complete"
