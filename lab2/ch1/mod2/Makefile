obj-m := ch1_mod2_202111360.o

# mod1의 Module.symvers 파일 경로를 지정합니다.
# 현재 디렉토리(mod2)에서 한 단계 올라가서 mod1 폴더로 들어가는 경로입니다.
KBUILD_EXTRA_SYMBOLS := $(PWD)/../mod1/Module.symvers

KERNELDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean