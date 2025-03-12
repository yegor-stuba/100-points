obj-m += netlink_module.o
USER_OBJ = netlink_user

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc -o $(USER_OBJ) $(USER_OBJ).c

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f $(USER_OBJ)
