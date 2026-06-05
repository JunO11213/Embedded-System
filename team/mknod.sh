#!/bin/sh
MODULE="pi2_led_driver"
DEVFILE="/dev/pi2_led_dev"

rm -f $DEVFILE

major=$(awk "\$2==\"$MODULE\" {print \$1}" /proc/devices)

if [ -z "$major" ]; then
    echo "Error: cannot find major number for $MODULE"
    echo "Run: sudo insmod pi2_led_driver.ko"
    exit 1
fi

mknod $DEVFILE c $major 0
chmod 666 $DEVFILE

echo "Created $DEVFILE with major $major minor 0"
