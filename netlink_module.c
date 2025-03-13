#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netlink.h>
#include <net/netlink.h>
#include <linux/if.h>
#include <linux/if_arp.h>
#include <net/sock.h>
#include <linux/netdevice.h>
#include <linux/string.h>  // Added for strscpy

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yegor Brusnyak");
MODULE_DESCRIPTION("Netlink module for network interface info");

#define NETLINK_USER 31  // Custom Netlink protocol number
#define MSG_LIST_ALL 1   // Message type: List all interfaces
#define MSG_GET_ONE  2   // Message type: Get one interface

static struct sock *nl_sk = NULL;

// Structure for interface data
struct interface_info {
    char name[IFNAMSIZ];  // Interface name (e.g., "eth0")
    int index;            // Interface index
    unsigned char addr[6]; // MAC address (L2)
};

// Send a single interface info to user space
static int send_interface(struct sk_buff *skb_out, int pid, struct net_device *dev, int seq) {
    struct nlmsghdr *nlh;
    struct interface_info info;
    int msg_size = sizeof(struct interface_info);
    int res;

    memset(&info, 0, sizeof(info));
    strscpy(info.name, dev->name, IFNAMSIZ);
    info.index = dev->ifindex;
    memcpy(info.addr, dev->dev_addr, 6);

    nlh = nlmsg_put(skb_out, 0, seq, NLMSG_DONE, msg_size, 0);
    if (!nlh) {
        printk(KERN_ERR "Failed to create nlmsg for %s\n", dev->name);
        return -ENOMEM;
    }
    memcpy(nlmsg_data(nlh), &info, msg_size);

    res = nlmsg_unicast(nl_sk, skb_out, pid);
    if (res < 0) {
        printk(KERN_ERR "Failed to send %s to user: %d\n", dev->name, res);
        return res;
    }
    printk(KERN_INFO "Sent %s to user\n", dev->name);
    return 0;
}

// Handle incoming messages from user space
static void nl_recv_msg(struct sk_buff *skb) {
    struct nlmsghdr *nlh = (struct nlmsghdr *)skb->data;
    int pid = nlh->nlmsg_pid;
    struct sk_buff *skb_out;
    struct net_device *dev;
    char *user_msg = (char *)nlmsg_data(nlh);
    int seq = 0;

    printk(KERN_INFO "Received from user: %s\n", user_msg);

    if (nlh->nlmsg_type == MSG_LIST_ALL) {
        for_each_netdev(&init_net, dev) {
            if (dev->type == ARPHRD_ETHER) {
                skb_out = nlmsg_new(sizeof(struct interface_info), 0);
                if (!skb_out) {
                    printk(KERN_ERR "Failed to allocate skb\n");
                    continue;
                }
                if (send_interface(skb_out, pid, dev, seq++) < 0) {
                    printk(KERN_ERR "Failed to send interface %s\n", dev->name);
                    kfree_skb(skb_out);
                }
            }
        }
    } else if (nlh->nlmsg_type == MSG_GET_ONE) {
        dev = dev_get_by_name(&init_net, user_msg);
        if (dev) {
            if (dev->type == ARPHRD_ETHER) {
                skb_out = nlmsg_new(sizeof(struct interface_info), 0);
                if (skb_out && send_interface(skb_out, pid, dev, seq) < 0) {
                    printk(KERN_ERR "Failed to send interface %s\n", dev->name);
                    kfree_skb(skb_out);
                }
            }
            dev_put(dev);
        } else {
            printk(KERN_INFO "Interface %s not found\n", user_msg);
            // Send an empty NLMSG_DONE to signal no data
            skb_out = nlmsg_new(0, 0);
            if (skb_out) {
                nlh = nlmsg_put(skb_out, 0, seq, NLMSG_DONE, 0, 0);
                if (nlh) {
                    nlmsg_unicast(nl_sk, skb_out, pid);
                } else {
                    kfree_skb(skb_out);
                }
            }
        }
    }
}

static int __init netlink_init(void) {
    struct netlink_kernel_cfg cfg = {
        .input = nl_recv_msg,
    };

    printk(KERN_INFO "Initializing Netlink module\n");
    nl_sk = netlink_kernel_create(&init_net, NETLINK_USER, &cfg);
    if (!nl_sk) {
        printk(KERN_ERR "Error creating Netlink socket\n");
        return -ENOMEM;
    }
    return 0;
}

static void __exit netlink_exit(void) {
    printk(KERN_INFO "Exiting Netlink module\n");
    if (nl_sk)
        netlink_kernel_release(nl_sk);
}

module_init(netlink_init);
module_exit(netlink_exit);
