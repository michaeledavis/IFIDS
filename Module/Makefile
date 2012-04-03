obj-m += ifids_module.o
ifids_module-objs := ../Config/Config.o
all:
	@echo "Building IFIDS Module"
	@make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	@echo "Cleaning IFIDS Module"
	@make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
