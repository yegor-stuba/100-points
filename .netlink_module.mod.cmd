savedcmd_/home/traveler/kernel_dev/netlink_module.mod := printf '%s\n'   netlink_module.o | awk '!x[$$0]++ { print("/home/traveler/kernel_dev/"$$0) }' > /home/traveler/kernel_dev/netlink_module.mod
