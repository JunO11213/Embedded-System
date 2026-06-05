# Makefile for Pi2
# - pi2_led_driver.c -> pi2_led_driver.ko
# - server_pi_app.c  -> server_pi_app
# 커널 모듈은 VM에서 크로스 컴파일
# 응용 프로그램은 Pi에서 직접 컴파일하는 것을 권장

obj-m := pi2_led_driver.o

KDIR := ~/linux-rpi/
ARM := ARCH=arm CROSS_COMPILE=/usr/bin/arm-linux-gnueabi-
PWD := $(shell pwd)

APP := server_pi_app
APP_SRC := server_pi_app.c

PI_USER := pi
PI_IP := 10.10.10.12
PI_DIR := ~/

default: module

module:
	$(MAKE) -C $(KDIR) M=$(PWD) $(ARM) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) $(ARM) clean
	rm -f $(APP)

scp: module
	scp pi2_led_driver.ko server_pi_app.c mknod.sh $(PI_USER)@$(PI_IP):$(PI_DIR)

# 이 target은 라즈베리파이 안에서 실행할 때 사용
app:
	gcc -o $(APP) $(APP_SRC) -lmosquitto -lpthread

run:
	sudo systemctl start mosquitto
	sudo insmod pi2_led_driver.ko || true
	sudo sh mknod.sh
	sudo ./$(APP)

remove:
	sudo rm -f /dev/pi2_led_dev
	sudo rmmod pi2_led_driver || true