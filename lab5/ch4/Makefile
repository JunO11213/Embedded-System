obj-m := ch4_mod_202111360.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
	gcc -o ch4_reader ch4_reader.c
	gcc -o ch4_writer ch4_writer.c

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f ch4_reader ch4_writer