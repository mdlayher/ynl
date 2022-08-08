#ifndef __USER_MY_H
#define __USER_MY_H 1

#include <libmnl/libmnl.h>
#include <linux/types.h>

struct mnl_socket;
struct nlmsghdr;

/* Generic mnl helper code */

enum ynl_error_code {
	YNL_ERROR_NONE = 0,
	__YNL_ERRNO_END = 4096,
	YNL_ERROR_EXPECT_ACK,
	YNL_ERROR_EXPECT_MSG,
	YNL_ERROR_UNEXPECT_MSG,
	YNL_ERROR_ATTR_MISSING,
	YNL_ERROR_ATTR_INVALID,
};

struct ynl_error {
	enum ynl_error_code code;
	const char *msg;
};

struct ynl_sock {
	struct mnl_socket *sock;
	__u32 seq;
	__u32 portid;
	__u16 family_id;

	struct ynl_error err;

	struct nlmsghdr *nlh;
	unsigned char *tx_buf;
	unsigned char *rx_buf;
	unsigned char raw_buf[];
};

struct ynl_parse_arg {
	struct ynl_sock *ys;
	void *data;
};

extern mnl_cb_t ynl_cb_array[NLMSG_MIN_TYPE];

struct ynl_sock *ynl_sock_create(const char *family_name, struct ynl_error *e);
void ynl_sock_destroy(struct ynl_sock *ys);

struct nlmsghdr *
ynl_gemsg_start_req(struct ynl_sock *ys, __u32 id, __u8 cmd, __u8 version);
struct nlmsghdr *
ynl_gemsg_start_dump(struct ynl_sock *ys, __u32 id, __u8 cmd, __u8 version);

int ynl_recv_ack(struct ynl_sock *ys, int ret);
int ynl_cb_null(const struct nlmsghdr *nlh, void *data);

/* YNL specific helpers used by the auto-generated code */

struct ynl_ntf_base_type {
	__u16 family;
	__u8 cmd;
	unsigned char data[];
};

struct ynl_dump_list_type {
	struct ynl_dump_list_type *next;
	unsigned char data[];
};

struct ynl_dump_state {
	struct ynl_sock *ys;
	void *first;
	struct ynl_dump_list_type *last;
	size_t alloc_sz;
	mnl_cb_t cb;
};

int ynl_dump_trampoline(const struct nlmsghdr *nlh, void *data);

#endif
