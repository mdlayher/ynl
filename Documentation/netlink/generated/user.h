#ifndef __USER_MY_H
#define __USER_MY_H 1

#define ARRAY_SIZE(arr)		(sizeof(arr) / sizeof(*arr))

#define err(msg...)		fprintf(stderr, msg)
#define err_ret(ret, msg...)	({ err(msg); ret; })
#define perr(msg)		fprintf(stderr, "%s: %s\n", (msg), strerror(errno))
#define perr_ret(ret, msg)	({ perr(msg); ret; })

struct mnl_socket;
struct nlmsghdr;

struct ynl_sock {
	struct mnl_socket *sock;
	__u32 seq;
	__u32 portid;
	__u16 family_id;
	struct nlmsghdr *nlh;
	unsigned char buf[];
};

struct nlmsghdr *
ynl_gemsg_start_req(struct ynl_sock *ys, __u32 id, __u8 cmd, __u8 version);

#endif
