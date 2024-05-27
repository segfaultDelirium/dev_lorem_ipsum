how to use this?


make
sudo insmod lorem.ko
sudo mknod /dev/lorem c $(cat /proc/devices | grep lorem | awk '{print $1}') 0
sudo chmod 666 /dev/lorem
dd if=/dev/lorem of=lorem.txt bs=1 count=1G
