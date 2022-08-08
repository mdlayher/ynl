#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <linux/types.h>

#include <libmnl/libmnl.h>
#include <linux/genetlink.h>

#include "ynl.h"

#define ARRAY_SIZE(arr)		(sizeof(arr) / sizeof(*arr))

#define __err(yse, _code, _msg)			\
	({					\
		struct ynl_error *_yse = (yse);	\
						\
		if (_yse) {			\
			_yse->msg = _msg;	\
			_yse->code = _code;	\
		}				\
	})

#define __perr(yse, _msg)		__err(yse, errno, _msg)

#define err(_ys, _code, _msg)		__err(&(_ys)->err, _code, _msg)
#define perr(_ys, _msg)			__err(&(_ys)->err, errno, _msg)

/* -- Netlink bolier plate */
static int ynl_cb_noop(const struct nlmsghdr *nlh, void *data)
{
	return MNL_CB_OK;
}

static int ynl_cb_error(const struct nlmsghdr *nlh, void *data)
{
	const struct nlmsgerr *err = mnl_nlmsg_get_payload(nlh);
	struct ynl_parse_arg *yarg = data;
	int code;

	code = err->error >= 0 ? err->error : -err->error;
	yarg->ys->err.code = code;
	errno = code;

	return code ? MNL_CB_STOP : MNL_CB_ERROR;
}

static int ynl_cb_stop(const struct nlmsghdr *nlh, void *data)
{
	struct ynl_parse_arg *yarg = data;
	int err;

	err = *(int *)NLMSG_DATA(nlh);
	if (err < 0) {
		yarg->ys->err.code = -err;
		errno = -err;
		return MNL_CB_ERROR;
	}
	return MNL_CB_STOP;
}

mnl_cb_t ynl_cb_array[NLMSG_MIN_TYPE] = {
	[NLMSG_NOOP]	= ynl_cb_noop,
	[NLMSG_ERROR]	= ynl_cb_error,
	[NLMSG_DONE]	= ynl_cb_stop,
	[NLMSG_OVERRUN]	= ynl_cb_noop,
};

/* Generic code */

struct nlmsghdr *ynl_msg_start(struct ynl_sock *ys, __u32 id, __u16 flags)
{
	struct nlmsghdr *nlh;

	nlh = ys->nlh = mnl_nlmsg_put_header(ys->tx_buf);
	nlh->nlmsg_type	= id;
	nlh->nlmsg_flags = flags;
	nlh->nlmsg_seq = ++ys->seq;

	return nlh;
}

struct nlmsghdr *
ynl_gemsg_start(struct ynl_sock *ys, __u32 id, __u16 flags,
		__u8 cmd, __u8 version)
{
	struct genlmsghdr gehdr;
	struct nlmsghdr *nlh;
	void *data;

	nlh = ynl_msg_start(ys, id, flags);

	memset(&gehdr, 0, sizeof(gehdr));
	gehdr.cmd = cmd;
	gehdr.version = version;

	data = mnl_nlmsg_put_extra_header(nlh, sizeof(gehdr));
	memcpy(data, &gehdr, sizeof(gehdr));

	return nlh;
}

void ynl_msg_start_req(struct ynl_sock *ys, __u32 id)
{
	ynl_msg_start(ys, id, NLM_F_REQUEST | NLM_F_ACK);
}

void ynl_msg_start_dump(struct ynl_sock *ys, __u32 id)
{
	ynl_msg_start(ys, id, NLM_F_REQUEST | NLM_F_ACK | NLM_F_DUMP);
}

struct nlmsghdr *
ynl_gemsg_start_req(struct ynl_sock *ys, __u32 id, __u8 cmd, __u8 version)
{
	return ynl_gemsg_start(ys, id, NLM_F_REQUEST | NLM_F_ACK, cmd, version);
}

struct nlmsghdr *
ynl_gemsg_start_dump(struct ynl_sock *ys, __u32 id, __u8 cmd, __u8 version)
{
	return ynl_gemsg_start(ys, id, NLM_F_REQUEST | NLM_F_ACK | NLM_F_DUMP,
			       cmd, version);
}

int ynl_recv_ack(struct ynl_sock *ys, int ret)
{
	if (!ret) {
		err(ys, YNL_ERROR_EXPECT_ACK,
		    "Expecting an ACK but nothing received");
		return -1;
	}

	ret = mnl_socket_recvfrom(ys->sock, ys->rx_buf, MNL_SOCKET_BUFFER_SIZE);
	if (ret < 0) {
		perr(ys, "Socket receive failed");
		return ret;
	}
	return mnl_cb_run(ys->rx_buf, ret, ys->seq, ys->portid,
			  ynl_cb_null, ys);
}

int ynl_cb_null(const struct nlmsghdr *nlh, void *data)
{
	struct ynl_parse_arg *yarg = data;

	err(yarg->ys, YNL_ERROR_UNEXPECT_MSG,
	    "Received a message when none were expected");

	return MNL_CB_ERROR;
}

/* Init/fini and genetlink boiler plate */

static int ynl_get_family_id_cb(const struct nlmsghdr *nlh, void *data)
{
	struct ynl_parse_arg *yarg = data;
	struct ynl_sock *ys = yarg->ys;
	const struct nlattr *attr;

	mnl_attr_for_each(attr, nlh, sizeof(struct genlmsghdr)) {
		if (mnl_attr_get_type(attr) != CTRL_ATTR_FAMILY_ID)
			continue;

		if (mnl_attr_get_payload_len(attr) != sizeof(__u16)) {
			err(ys, YNL_ERROR_ATTR_INVALID, "Invalid family ID");
			return MNL_CB_ERROR;
		}

		ys->family_id = mnl_attr_get_u16(attr);
		return MNL_CB_OK;
	}

	err(ys, YNL_ERROR_ATTR_MISSING, "Family ID missing");
	return MNL_CB_ERROR;
}

static int ynl_sock_read_family(struct ynl_sock *ys, const char *family_name)
{
	struct ynl_parse_arg yarg = { .ys = ys, };
	struct nlmsghdr *nlh;
	int err;

	nlh = ynl_gemsg_start_req(ys, GENL_ID_CTRL, CTRL_CMD_GETFAMILY, 1);
	mnl_attr_put_strz(nlh, CTRL_ATTR_FAMILY_NAME, family_name);

	err = mnl_socket_sendto(ys->sock, nlh, nlh->nlmsg_len);
	if (err < 0) {
		perr(ys, "failed to request socket family info");
		return err;
	}

	err = mnl_socket_recvfrom(ys->sock, ys->rx_buf, MNL_SOCKET_BUFFER_SIZE);
	if (err <= 0) {
		perr(ys, "failed to receive the socket family info");
		return err;
	}
	err = mnl_cb_run2(ys->rx_buf, err, ys->seq, ys->portid,
			  ynl_get_family_id_cb, &yarg,
			  ynl_cb_array, ARRAY_SIZE(ynl_cb_array));
	if (err < 0) {
		perr(ys, "failed to receive the socket family info 2");
		return err;
	}

	return ynl_recv_ack(ys, err);
}

struct ynl_sock *ynl_sock_create(const char *family_name, struct ynl_error *yse)
{
	struct ynl_sock *ys;
	int one = 1;

	ys = malloc(sizeof(*ys) + 2 * MNL_SOCKET_BUFFER_SIZE);
	if (!ys)
		return NULL;
	memset(ys, 0, sizeof(*ys));

	ys->tx_buf = &ys->raw_buf[0];
	ys->rx_buf = &ys->raw_buf[MNL_SOCKET_BUFFER_SIZE];

	ys->sock = mnl_socket_open(NETLINK_GENERIC);
	if (!ys->sock) {
		__perr(yse, "failed to create a netlink socket");
		goto err_free_sock;
	}

	if (mnl_socket_setsockopt(ys->sock, NETLINK_CAP_ACK,
				  &one, sizeof(one))) {
		__perr(yse, "failed to enable netlink ACK");
		goto err_close_sock;
	}
	if (mnl_socket_setsockopt(ys->sock, NETLINK_EXT_ACK,
				  &one, sizeof(one))) {
		__perr(yse, "failed to enable netlink ext ACK");
		goto err_close_sock;
	}

	ys->seq = random();
	ys->portid = mnl_socket_get_portid(ys->sock);

	if (ynl_sock_read_family(ys, family_name)) {
		if (yse)
			memcpy(yse, &ys->err, sizeof(*yse));
		goto err_close_sock;
	}

	return ys;

err_close_sock:
	mnl_socket_close(ys->sock);
err_free_sock:
	free(ys);
	return NULL;
}

void ynl_sock_destroy(struct ynl_sock *ys)
{
	mnl_socket_close(ys->sock);
	free(ys);
}

/* YNL specific helpers used by the auto-generated code */

int ynl_dump_trampoline(const struct nlmsghdr *nlh, void *data)
{
	struct ynl_dump_state *ds = data;
	struct ynl_dump_list_type *obj;
	struct ynl_parse_arg yarg = {};

	obj = calloc(1, ds->alloc_sz);
	if (!obj)
		return MNL_CB_ERROR;

	if (!ds->first)
		ds->first = obj;
	if (ds->last)
		ds->last->next = obj;
	ds->last = obj;

	yarg.ys = ds->ys;
	yarg.data = &obj->data;

	return ds->cb(nlh, &yarg);
}
