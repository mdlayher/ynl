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
struct nlmsghdr *
ynl_gemsg_start_dump(struct ynl_sock *ys, __u32 id, __u8 cmd, __u8 version);


struct ynl_dump_list_type {
	struct ynl_dump_list_type *next;
	unsigned char data[];
};

struct ynl_dump_state {
	void *first;
	struct ynl_dump_list_type *last;
	size_t alloc_sz;
	mnl_cb_t cb;
};

int ynl_recv_ack(struct ynl_sock *ys, int ret);
int ynl_cb_null(const struct nlmsghdr *nlh, void *data);
int ynl_dump_trampoline(const struct nlmsghdr *nlh, void *data);

#endif
