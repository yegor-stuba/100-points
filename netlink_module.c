#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netlink.h>
#include <net/netlink.h>
#include <linux/if.h>
#include <linux/if_arp.h>  // Added for ARPHRD_ETHER
#include <net/sock.h>
#include <linux/netdevice.h>

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
static int send_interface(struct sk_buff *skb_out, int pid, struct net_device *dev) {
    struct nlmsghdr *nlh;
    struct interface_info info;
    int msg_size = sizeof(struct interface_info);

    memset(&info, 0, sizeof(info));
    strncpy(info.name, dev->name, IFNAMSIZ);
    info.index = dev->ifindex;
    memcpy(info.addr, dev->dev_addr, 6);  // Copy MAC address

    nlh = nlmsg_put(skb_out, 0, 0, NLMSG_DONE, msg_size, 0);
    if (!nlh) {
        printk(KERN_ERR "Failed to create nlmsg\n");
        return -ENOMEM;
    }
    memcpy(nlmsg_data(nlh), &info, msg_size);

    return nlmsg_unicast(nl_sk, skb_out, pid);
}

// Handle incoming messages from user space
static void nl_recv_msg(struct sk_buff *skb) {
    struct nlmsghdr *nlh = (struct nlmsghdr *)skb->data;
    int pid = nlh->nlmsg_pid;  // Sender's PID
    struct sk_buff *skb_out;
    struct net_device *dev;
    char *user_msg = (char *)nlmsg_data(nlh);

    printk(KERN_INFO "Received from user: %s\n", user_msg);

    if (nlh->nlmsg_type == MSG_LIST_ALL) {
        // List all L2 interfaces
        for_each_netdev(&init_net, dev) {
            if (dev->type == ARPHRD_ETHER) {  // Only Ethernet (L2) interfaces
                skb_out = nlmsg_new(sizeof(struct interface_info), 0);
                if (!skb_out) {
                    printk(KERN_ERR "Failed to allocate skb\n");
                    continue;
                }
                if (send_interface(skb_out, pid, dev) < 0) {
                    printk(KERN_ERR "Failed to send interface %s\n", dev->name);
                    kfree_skb(skb_out);
                }
            }
        }
    } else if (nlh->nlmsg_type == MSG_GET_ONE) {
        // Get specific interface by name
        dev = dev_get_by_name(&init_net, user_msg);
        if (dev) {
            if (dev->type == ARPHRD_ETHER) {
                skb_out = nlmsg_new(sizeof(struct interface_info), 0);
                if (skb_out && send_interface(skb_out, pid, dev) < 0) {
                    printk(KERN_ERR "Failed to send interface %s\n", dev->name);
                    kfree_skb(skb_out);
                }
            }
            dev_put(dev);  // Release device reference
        } else {
            printk(KERN_INFO "Interface %s not found\n", user_msg);
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
