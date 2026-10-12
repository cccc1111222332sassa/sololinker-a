#usb_SEL短接3v3

echo 4 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio4/direction
echo 1 > /sys/class/gpio/gpio4/value
echo host > /sys/devices/platform/ff3e0000.usb2-phy/otg_mode



