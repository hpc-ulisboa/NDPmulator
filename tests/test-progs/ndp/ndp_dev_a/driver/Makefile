TARGET_MOD:=ndp_dev_a
DEVICE_NUM:=`awk '/ndp_dev_a/{print $$1}' /proc/devices`

ifndef $(KERNEL_VER)
	KERNEL_VER:=`uname -r`
endif

# If we running by kernel building system
ifneq ($(KERNELRELEASE),)
	$(TARGET_MOD)-objs := main.o ndp_dev_a_file.o
	obj-m := $(TARGET_MOD).o

# If we are running without kernel build system
else
	BUILDSYSTEM_DIR:=/lib/modules/$(KERNEL_VER)/build
	PWD:=`pwd`


all:
# run kernel build system to make module
	$(MAKE) -C $(BUILDSYSTEM_DIR) M=$(PWD) modules

clean:
# run kernel build system to cleanup in current directory
	$(MAKE) -C $(BUILDSYSTEM_DIR) M=$(PWD) clean
	rm test_driver

load:
	insmod ./$(TARGET_MOD).ko
	mknod /dev/$(TARGET_MOD) c $(DEVICE_NUM) 0

unload:
	rmmod ./$(TARGET_MOD).ko
	rm /dev/$(TARGET_MOD)

test_driver:
	gcc -O3 test_driver.c -o test_driver

endif
