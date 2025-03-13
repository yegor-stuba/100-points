#!/bin/bash

set -e  # Exit on error

echo "Building kernel module and user utility..."
make clean && make

echo "Unloading kernel module if already loaded..."
sudo rmmod netlink_module 2>/dev/null || true

echo "Loading kernel module..."
sudo insmod netlink_module.ko
sleep 1

echo "Checking kernel logs..."
sudo dmesg | tail -n 10

echo "Listing all L2 interfaces with netlink_user..."
./netlink_user

echo "Querying specific interface (enp0s1) with netlink_user..."
./netlink_user enp0s1

echo "Querying nonexistent interface (fakes1) with netlink_user..."
./netlink_user fakes1

echo "Comparing with ip link output..."
ip link | grep -E "link/ether" | while read -r line; do
    iface=$(echo "$line" | awk '{print $2}' | sed 's/:$//')
    mac=$(echo "$line" | awk '{print $3}')
    echo "ip link: $iface - $mac"
done

echo "Unloading kernel module..."
sudo rmmod netlink_module
sleep 1

echo "Checking kernel logs after unloading..."
sudo dmesg | tail -n 10

echo "Test completed!"
