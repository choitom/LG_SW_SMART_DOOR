obj-m := $(notdir $(PWD)).o

ifeq ($(shell uname -m), "armv7l")
  KDIR := /lib/modules/$(shell uname -r)/build  
else
  KDIR := $(HOME)/linux
endif

default:
	make -C $(KDIR) M=$(PWD) modules	
	
clean :	
	rm -rf *.ko
	rm -rf *.mod.*
	rm -rf .*.cmd
	rm -rf *.o
	rm -rf *.symvers
	rm -rf *.order
	rm -rf .tmp_versions


