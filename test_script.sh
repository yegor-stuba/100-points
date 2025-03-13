#!/bin/bash

set -e  # Exit on error

echo "Building kernel module and user utility..."
make clean && make

echo "Loading kernel module..."
sudo insmod netlink_module.ko
sleep 1

echo "Checking kernel logs..."
sudo dmesg | tail -n 10

echo "Listing all L2 interfaces..."
./netlink_user

echo "Querying specific interface (enp0s1)..."
./netlink_user enp0s1

echo "Querying nonexistent interface (fakes1)..."
./netlink_user fakes1

echo "Unloading kernel module..."
sudo rmmod netlink_module
sleep 1

echo "Checking kernel logs after unloading..."
sudo dmesg | tail -n 10

echo "Test completed!"
