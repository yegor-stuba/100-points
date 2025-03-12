#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

#ifdef CONFIG_UNWINDER_ORC
#include <asm/orc_header.h>
ORC_HEADER;
#endif

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif



static const char ____versions[]
__used __section("__versions") =
	"\x10\x00\x00\x00\x7e\x3a\x2c\x12"
	"_printk\0"
	"\x14\x00\x00\x00\x50\x52\xe0\xcb"
	"init_net\0\0\0\0"
	"\x20\x00\x00\x00\x9f\xae\xd1\x46"
	"__netlink_kernel_create\0"
	"\x1c\x00\x00\x00\xcb\xf6\xfd\xf0"
	"__stack_chk_fail\0\0\0\0"
	"\x20\x00\x00\x00\xd1\xae\x44\xd6"
	"netlink_kernel_release\0\0"
	"\x10\x00\x00\x00\x94\xb6\x16\xa9"
	"strnlen\0"
	"\x10\x00\x00\x00\x39\xe6\x64\xdd"
	"strscpy\0"
	"\x14\x00\x00\x00\x61\xea\xcd\xbd"
	"__nlmsg_put\0"
	"\x18\x00\x00\x00\x8a\x91\xf5\xa9"
	"netlink_unicast\0"
	"\x18\x00\x00\x00\x8c\x89\xd4\xcb"
	"fortify_panic\0\0\0"
	"\x14\x00\x00\x00\x8f\x06\xe3\xdc"
	"__alloc_skb\0"
	"\x1c\x00\x00\x00\x0f\x82\x16\xfa"
	"kfree_skb_reason\0\0\0\0"
	"\x18\x00\x00\x00\x8d\xa3\x30\xcd"
	"dev_get_by_name\0"
	"\x2c\x00\x00\x00\xee\x52\x0e\x26"
	"dynamic_preempt_schedule_notrace\0\0\0\0"
	"\x18\x00\x00\x00\x2d\xb8\x9d\x1d"
	"module_layout\0\0\0"
	"\x00\x00\x00\x00\x00\x00\x00\x00";

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "9A1E4BF909E4C9725589C67");
