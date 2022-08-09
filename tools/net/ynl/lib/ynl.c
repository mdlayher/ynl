#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <linux/types.h>

#include <libmnl/libmnl.h>
#include <linux/genetlink.h>

#include "ynl.h"

#define ARRAY_SIZE(arr)		(sizeof(arr) / sizeof(*arr))

#define __yerr_msg(yse, _msg...)					\
	({								\
		struct ynl_error *_yse = (yse);				\
									\
		if (_yse) {						\
			snprintf(_yse->msg, sizeof(_yse->msg) - 1,  _msg); \
			_yse->msg[sizeof(_yse->msg) - 1] = 0;		\
		}							\
	})

#define __yerr_code(yse, _code...)		\
	({					\
		struct ynl_error *_yse = (yse);	\
						\
		if (_yse) {			\
			_yse->code = _code;	\
		}				\
	})


#define __yerr(yse, _code, _msg...)		\
	({					\
		__yerr_msg(yse, _msg);		\
		__yerr_code(yse, _code);	\
	})

#define __perr(yse, _msg)		__yerr(yse, errno, _msg)

#define yerr_msg(_ys, _msg...)		__yerr_msg(&(_ys)->err, _msg)
#define yerr(_ys, _code, _msg...)	__yerr(&(_ys)->err, _code, _msg)
#define perr(_ys, _msg)			__yerr(&(_ys)->err, errno, _msg)

/* -- Netlink bolier plate */
static int
ynl_err_walk(struct ynl_sock *ys, void *start, void *end, unsigned int off,
	     struct ynl_policy_nest *policy, char *str, int str_sz)
{
	unsigned int astart_off, aend_off;
	const struct nlattr *attr;
	unsigned int data_len;
	unsigned int type;
	bool found = false;
	int n = 0;

	if (!policy) {
		if (n < str_sz)
			n += snprintf(str, str_sz, "!policy");
		return n;
	}

	data_len = end - start;

	mnl_attr_for_each_payload(start, data_len) {
		astart_off = (char *)attr - (char *)start;
		aend_off = astart_off + mnl_attr_get_payload_len(attr);
		if (aend_off <= off)
			continue;

		found = true;
		break;
	}
	if (!found)
		return 0;

	off -= astart_off;

	type = mnl_attr_get_type(attr);
	if (type > policy->max_attr) {
		if (n < str_sz)
			n += snprintf(str, str_sz, "!oob");
		return n;
	}

	if (!policy->table[type].name) {
		if (n < str_sz)
			n += snprintf(str, str_sz, "!name");
		return n;
	}

	if (n < str_sz)
		n += snprintf(str, str_sz - n, ".%s", policy->table[type].name);

	if (!off)
		return n;

	if (!policy->table[type].nest) {
		if (n < str_sz)
			n += snprintf(str, str_sz, "!nest");
		return n;
	}

	off -= sizeof(struct nlattr);
	start =  mnl_attr_get_payload(attr);
	end = start + mnl_attr_get_payload_len(attr);

	return n + ynl_err_walk(ys, start, end, off, policy->table[type].nest,
				&str[n], str_sz - n);
}

static int
ynl_ext_ack_check(struct ynl_sock *ys, const struct nlmsghdr *nlh,
		  unsigned int hlen)
{
	const struct nlattr *attr, *msg = NULL, *offs = NULL;
	char bad_attr[sizeof(ys->err.msg)];
	const char *str = "";

	if (!(nlh->nlmsg_flags & NLM_F_ACK_TLVS))
		return MNL_CB_OK;

	mnl_attr_for_each(attr, nlh, hlen) {
		if (mnl_attr_get_type(attr) == NLMSGERR_ATTR_MSG)
			msg = attr;
		if (mnl_attr_get_type(attr) == NLMSGERR_ATTR_OFFS)
			offs = attr;
	}
	if (!offs && !msg)
		return MNL_CB_OK;

	bad_attr[0] = '\0';

	if (offs) {
		unsigned int n, off;
		void *start, *end;

		if (mnl_attr_get_payload_len(offs) != sizeof(__u32))
			return MNL_CB_ERROR;

		ys->err.attr_offs = mnl_attr_get_u32(offs);

		n = snprintf(bad_attr, sizeof(bad_attr), " (bad attr: ");

		start = mnl_nlmsg_get_payload_offset(ys->nlh,
						     sizeof(struct genlmsghdr));
		end = mnl_nlmsg_get_payload_tail(ys->nlh);

		off = ys->err.attr_offs;
		off -= sizeof(struct nlmsghdr);
		off -= sizeof(struct genlmsghdr);

		n += ynl_err_walk(ys, start, end, off, ys->req_policy,
				  &bad_attr[n], sizeof(bad_attr) - n);

		if (n < sizeof(bad_attr))
			n += snprintf(&bad_attr[n], sizeof(bad_attr) - n, ")");
		if (n >= sizeof(bad_attr))
			n = sizeof(bad_attr) - 1;
		bad_attr[n] = '\0';
	}
	if (msg) {
		str = mnl_attr_get_payload(msg);

		if (str[mnl_attr_get_payload_len(msg) - 1])
			return MNL_CB_ERROR;
	}

	/* Implicitly depend on ys->err.code already set */
	yerr_msg(ys, "Kernel %s: '%s'%s",
		 ys->err.code ? "error" : "warning", str, bad_attr);

	return MNL_CB_OK;
}

static int ynl_cb_error(const struct nlmsghdr *nlh, void *data)
{
	const struct nlmsgerr *err = mnl_nlmsg_get_payload(nlh);
	struct ynl_parse_arg *yarg = data;
	unsigned int hlen;
	int code;

	code = err->error >= 0 ? err->error : -err->error;
	yarg->ys->err.code = code;
	errno = code;

	hlen = sizeof(*err);
	if (!(nlh->nlmsg_flags & NLM_F_CAPPED))
		hlen += mnl_nlmsg_get_payload_len(&err->msg);

	ynl_ext_ack_check(yarg->ys, nlh, hlen);

	return code ? MNL_CB_ERROR : MNL_CB_STOP;
}

static int ynl_cb_done(const struct nlmsghdr *nlh, void *data)
{
	struct ynl_parse_arg *yarg = data;
	int err;

	err = *(int *)NLMSG_DATA(nlh);
	if (err < 0) {
		yarg->ys->err.code = -err;
		errno = -err;

		ynl_ext_ack_check(yarg->ys, nlh, sizeof(int));

		return MNL_CB_ERROR;
	}
	return MNL_CB_STOP;
}

static int ynl_cb_noop(const struct nlmsghdr *nlh, void *data)
{
	return MNL_CB_OK;
}

mnl_cb_t ynl_cb_array[NLMSG_MIN_TYPE] = {
	[NLMSG_NOOP]	= ynl_cb_noop,
	[NLMSG_ERROR]	= ynl_cb_error,
	[NLMSG_DONE]	= ynl_cb_done,
	[NLMSG_OVERRUN]	= ynl_cb_noop,
};

/* Attribute validation */

int ynl_attr_validate(struct ynl_parse_arg *yarg, const struct nlattr *attr)
{
	struct ynl_policy_attr *policy;
	unsigned int type, len;
	unsigned char *data;

	data =mnl_attr_get_payload(attr);
	len = mnl_attr_get_payload_len(attr);
	type = mnl_attr_get_type(attr);
	if (type > yarg->rsp_policy->max_attr) {
		yerr(yarg->ys, YNL_ERROR_INTERNAL,
		     "Internal error, validating unknown attribute");
		return -1;
	}

	policy = &yarg->rsp_policy->table[type];

	switch (type) {
	case YNL_PT_REJECT:
		yerr(yarg->ys, YNL_ERROR_ATTR_INVALID,
		     "Rejected attribute (%s)", policy->name);
		return -1;
	case YNL_PT_U8:
		if (len == sizeof(__u8))
			break;
		yerr(yarg->ys, YNL_ERROR_ATTR_INVALID,
		     "Invalid attribute (u8 %s)", policy->name);
		return -1;
	case YNL_PT_U16:
		if (len == sizeof(__u16))
			break;
		yerr(yarg->ys, YNL_ERROR_ATTR_INVALID,
		     "Invalid attribute (u16 %s)", policy->name);
		return -1;
	case YNL_PT_U32:
		if (len == sizeof(__u32))
			break;
		yerr(yarg->ys, YNL_ERROR_ATTR_INVALID,
		     "Invalid attribute (u32 %s)", policy->name);
		return -1;
	case YNL_PT_U64:
		if (len == sizeof(__u64))
			break;
		yerr(yarg->ys, YNL_ERROR_ATTR_INVALID,
		     "Invalid attribute (u64 %s)", policy->name);
		return -1;
	case YNL_PT_FLAG:
		/* Let flags grow into real attrs, why not.. */
		break;
	case YNL_PT_NEST:
		if (!len || len >= sizeof(*attr))
			break;
		yerr(yarg->ys, YNL_ERROR_ATTR_INVALID,
		     "Invalid attribute (nest %s)", policy->name);
		return -1;
	case YNL_PT_BINARY:
		if (len == policy->len)
			break;
		yerr(yarg->ys, YNL_ERROR_ATTR_INVALID,
		     "Invalid attribute (binary %s)", policy->name);
		return -1;
	case YNL_PT_NUL_STR:
		if (len <= policy->len && !data[len])
			break;
		yerr(yarg->ys, YNL_ERROR_ATTR_INVALID,
		     "Invalid attribute (nul-string %s)", policy->name);
		return -1;
	default:
		yerr(yarg->ys, YNL_ERROR_ATTR_INVALID,
		     "Invalid attribute (unknown %s)", policy->name);
		return -1;
	}

	return 0;
}

/* Generic code */

static void ynl_err_reset(struct ynl_sock *ys)
{
	ys->err.code = 0;
	ys->err.attr_offs = 0;
	ys->err.msg[0] = 0;
}

struct nlmsghdr *ynl_msg_start(struct ynl_sock *ys, __u32 id, __u16 flags)
{
	struct nlmsghdr *nlh;

	ynl_err_reset(ys);

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
		yerr(ys, YNL_ERROR_EXPECT_ACK,
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

	yerr(yarg->ys, YNL_ERROR_UNEXPECT_MSG,
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
			yerr(ys, YNL_ERROR_ATTR_INVALID, "Invalid family ID");
			return MNL_CB_ERROR;
		}

		ys->family_id = mnl_attr_get_u16(attr);
		return MNL_CB_OK;
	}

	yerr(ys, YNL_ERROR_ATTR_MISSING, "Family ID missing");
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
