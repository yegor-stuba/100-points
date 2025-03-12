#!/bin/bash

set -e  # Exit on error

echo "Building module and user utility..."
make clean && make

echo "Loading kernel module..."
sudo insmod netlink_module.ko
sleep 1

echo "Checking kernel logs..."
dmesg | tail -n 10

echo "Listing network interfaces using user utility..."
./netlink_user

echo "Querying specific interface (eth0)..."
./netlink_user eth0 || echo "Interface eth0 not found"


echo "Unloading kernel module..."
sudo rmmod netlink_module
sleep 1

echo "Checking kernel logs after unloading..."
dmesg | tail -n 10

echo "Test completed!"
