obj-m += netlink_module.o
USER_OBJ = netlink_user
KDIR = /lib/modules/$(shell uname -r)/build
PWD = $(shell pwd)

all: module user

module:
	make -C $(KDIR) M=$(PWD) modules

user:
	gcc -o $(USER_OBJ) $(USER_OBJ).c -Wall -Werror

clean:
	make -C $(KDIR) M=$(PWD) clean
	rm -f $(USER_OBJ)

test: all
	./test_script.sh

.PHONY: all module user clean test
