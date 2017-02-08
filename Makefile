#makefile for mymod
obj-m += mymod.o
ccflags-y := -Wall -Werror -g
dev=/dev/kyouko3

default: mymod.ko user $(dev)

mymod.ko: mymod.c
	$(MAKE) -C /usr/src/linux M=$(PWD) modules

user:user.c
	gcc user.c -g -o user

$(dev):
	mknod $(dev) c 500 127

clean:
	rm *.ko *.o *.mod.c user modules.order Module.symvers

