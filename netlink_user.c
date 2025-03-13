#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <net/if.h>
#include <unistd.h>

#define NETLINK_USER 31
#define MSG_LIST_ALL 1
#define MSG_GET_ONE  2
#define MAX_PAYLOAD 1024

struct interface_info {
    char name[IFNAMSIZ];
    int index;
    unsigned char addr[6];
};

int main(int argc, char *argv[]) {
    struct sockaddr_nl src_addr, dest_addr;
    struct nlmsghdr *nlh = NULL;
    struct iovec iov;
    struct msghdr msg;
    int sock_fd;
    char *query = (argc > 1) ? argv[1] : "";

    sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_USER);
    if (sock_fd < 0) {
        perror("Socket creation failed");
        return -1;
    }

    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = getpid();
    bind(sock_fd, (struct sockaddr *)&src_addr, sizeof(src_addr));

    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0;

    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
    memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
    nlh->nlmsg_len = NLMSG_SPACE(strlen(query) + 1);
    nlh->nlmsg_pid = getpid();
    nlh->nlmsg_flags = 0;
    nlh->nlmsg_type = (argc > 1) ? MSG_GET_ONE : MSG_LIST_ALL;
    strcpy(NLMSG_DATA(nlh), query);

    iov.iov_base = (void *)nlh;
    iov.iov_len = nlh->nlmsg_len;
    msg.msg_name = (void *)&dest_addr;
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    if (sendmsg(sock_fd, &msg, 0) < 0) {
        perror("Send failed");
        close(sock_fd);
        free(nlh);
        return -1;
    }

    iov.iov_base = nlh;
    iov.iov_len = NLMSG_SPACE(MAX_PAYLOAD);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    printf("+------------------+----------------+\n");
    printf("| Interface Name   | MAC Address    |\n");
    printf("+------------------+----------------+\n");
    while (1) {
        ssize_t recv_len = recvmsg(sock_fd, &msg, 0);
        if (recv_len <= 0) {
            if (recv_len < 0) {
                perror("Receive failed");
            }
            break;
        }

        struct nlmsghdr *recv_nlh = (struct nlmsghdr *)nlh;
        if (NLMSG_OK(recv_nlh, recv_len)) {
            struct interface_info *info = (struct interface_info *)NLMSG_DATA(recv_nlh);
            printf("| %-16s | %02x:%02x:%02x:%02x:%02x:%02x |\n",
                   info->name, info->addr[0], info->addr[1], info->addr[2],
                   info->addr[3], info->addr[4], info->addr[5]);
        }
        if (recv_nlh->nlmsg_type == NLMSG_DONE) {
            break;
        }
    }
    printf("+------------------+----------------+\n");

    close(sock_fd);
    free(nlh);
    return 0;
}
