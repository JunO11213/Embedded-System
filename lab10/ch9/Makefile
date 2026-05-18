# 주의: '학번' 부분을 실제 본인의 학번으로 변경하세요. (예: ch9_mod_20260001.o)
obj-m := ch9_mod_202111360.o

KDIR := ~/linux-rpi/
ARM := ARCH=arm CROSS_COMPILE=/usr/bin/arm-linux-gnueabi-
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) $(ARM) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) $(ARM) clean

scp:
	# 라즈베리파이 IP 주소가 10.10.10.12가 맞는지 확인 후 사용하세요.
	scp $(obj-m:.o=.ko) pi@10.10.10.12:~/