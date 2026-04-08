obj-m := simple_block_2.o
KERNELDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	gcc -o simple_block_2_reader simple_block_2_reader.c
	gcc -o simple_block_2_writer simple_block_2_writer.c

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
	rm -f simple_block_2_reader simple_block_2_writer