obj-m := simple_block_1.o
KERNELDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	gcc -o simple_block_1_reader simple_block_1_reader.c
	gcc -o simple_block_1_writer simple_block_1_writer.c

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
	rm -f simple_block_1_reader simple_block_1_writer