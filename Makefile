all:
	@echo "Making IFIDS Components"
	@cd Daemon; make
	@cd Module; make
	@echo "IFIDS Components Built"
install:
	@echo "Installing IFIDS Components"
	@mkdir -p /usr/lib/ifids/
	@cp Daemon/ifids_daemon /usr/lib/ifids/ifids_daemon
	@cp Module/ifids_module.ko /usr/lib/ifids/ifids_module.ko
	@mkdir -p /var/log/ifids/
	@echo "IFIDS Installed."
uninstall:
	@echo "Uninstalling IFIDS Components"
	@./Scripts/ifids stop > /dev/null
	@rm -rf /usr/lib/ifids
	@rm -rf /var/log/ifids
	@echo "Uninstall complete"
clean:
	@echo "Cleaning Components"
	@cd Daemon; make clean
	@cd Module; make clean
	@echo "Clean complete"
