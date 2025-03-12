#!/bin/bash

set -e  # Exit on error

echo "Building kernel module..."
make clean && make

echo "Loading kernel module..."
sudo insmod netlink_module.ko
sleep 1

echo "Checking kernel logs..."
sudo dmesg | tail -n 10

echo "Unloading kernel module..."
sudo rmmod netlink_module
sleep 1

echo "Checking kernel logs after unloading..."
sudo dmesg | tail -n 10

echo "Test completed!"
