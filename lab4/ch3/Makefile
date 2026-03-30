obj-m := ch3_mod_202111360.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
	gcc -o ch3_reader ch3_reader.c
	gcc -o ch3_writer ch3_writer.c

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f ch3_reader ch3_writer