# m means module, y means built-in
obj-m += hello.o

KDIR = /lib/modules/$(shell uname -r)/build

all:
        make -C $(KDIR)  M=$(PWD) modules

clean:
        make -C $(KDIR) M=$(PWD) clean
