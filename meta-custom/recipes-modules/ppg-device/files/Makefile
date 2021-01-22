obj-m := ppg-device.o

SRC := $(shell pwd)

all:
	@echo '************  TEST VERSION ************'
	@echo '************  $(SRC) ************'
	$(MAKE) -I $(SRC) -C $(KERNEL_SRC) M=$(SRC)

modules_install:
	$(MAKE) INSTALL_MOD_DIR=kernel/drivers/misc -C $(KERNEL_SRC) M=$(SRC) modules_install

clean:
	rm -f *.o *~ core .depend .*.cmd *.ko *.mod.c
	rm -f Module.markers Module.symvers modules.order
	rm -rf .tmp_versions Modules.symvers
