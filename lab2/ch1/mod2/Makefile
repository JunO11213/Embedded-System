obj-m := ch1_mod2_202111360.o

KBUILD_EXTRA_SYMBOLS := $(PWD)/../mod1/Module.symvers

KERNELDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean