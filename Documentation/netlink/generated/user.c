#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <linux/types.h>

#include <libmnl/libmnl.h>
#include <linux/genetlink.h>

#include "user.h"

/* -- Netlink bolier plate */

static int nl_dump_ext_ack_done(const struct nlmsghdr *nlh, int errfn)
{
	/* TODO */
	return 0;
}

static int nl_dump_ext_ack(const struct nlmsghdr *nlh, void *errfn)
{
	/* TODO */
	return 0;
}

static int get_family_id_attr_cb(const struct nlattr *attr, void *data)
{
	const struct nlattr **tb = data;
	int type = mnl_attr_get_type(attr);

	if (mnl_attr_type_valid(attr, CTRL_ATTR_MAX) < 0)
		return MNL_CB_ERROR;

	if (type == CTRL_ATTR_FAMILY_ID &&
	    mnl_attr_validate(attr, MNL_TYPE_U16) < 0)
		return MNL_CB_ERROR;

	tb[type] = attr;
	return MNL_CB_OK;
}

static int get_family_id_cb(const struct nlmsghdr *nlh, void *data)
{
	struct genlmsghdr *genl = mnl_nlmsg_get_payload(nlh);
	struct nlattr *tb[CTRL_ATTR_MAX + 1] = {};
	struct ynl_sock *ys = data;

	mnl_attr_parse(nlh, sizeof(*genl), get_family_id_attr_cb, tb);
	if (!tb[CTRL_ATTR_FAMILY_ID])
		return MNL_CB_ERROR;

	ys->family_id = mnl_attr_get_u16(tb[CTRL_ATTR_FAMILY_ID]);
	return MNL_CB_OK;
}

static int mnlg_cb_noop(const struct nlmsghdr *nlh, void *data)
{
	return MNL_CB_OK;
}

static int mnlg_cb_error(const struct nlmsghdr *nlh, void *data)
{
	const struct nlmsgerr *err = mnl_nlmsg_get_payload(nlh);

	/* Netlink subsystems returns the errno value with different signess */
	if (err->error < 0)
		errno = -err->error;
	else
		errno = err->error;

	if (nl_dump_ext_ack(nlh, NULL))
		return MNL_CB_ERROR;

	return err->error == 0 ? MNL_CB_STOP : MNL_CB_ERROR;
}

static int mnlg_cb_stop(const struct nlmsghdr *nlh, void *data)
{
	int len = *(int *)NLMSG_DATA(nlh);

	if (len < 0) {
		errno = -len;
		nl_dump_ext_ack_done(nlh, len);
		return MNL_CB_ERROR;
	}
	return MNL_CB_STOP;
}

static mnl_cb_t mnlg_cb_array[NLMSG_MIN_TYPE] = {
	[NLMSG_NOOP]	= mnlg_cb_noop,
	[NLMSG_ERROR]	= mnlg_cb_error,
	[NLMSG_DONE]	= mnlg_cb_stop,
	[NLMSG_OVERRUN]	= mnlg_cb_noop,
};

/* Generic code */

struct nlmsghdr *ynl_msg_start(struct ynl_sock *ys, __u32 id, __u16 flags)
{
	struct nlmsghdr *nlh;

	nlh = ys->nlh = mnl_nlmsg_put_header(ys->buf);
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

struct nlmsghdr *
ynl_gemsg_start_req(struct ynl_sock *ys, __u32 id, __u8 cmd, __u8 version)
{
	return ynl_gemsg_start(ys, id, NLM_F_REQUEST | NLM_F_ACK, cmd, version);
}

struct ynl_sock *ynl_sock_create(const char *family_name)
{
	struct nlmsghdr *nlh;
	struct ynl_sock *ys;
	int one = 1;
	int err;

	ys = malloc(sizeof(*ys) + MNL_SOCKET_BUFFER_SIZE);
	if (!ys)
		return NULL;
	memset(ys, 0, sizeof(*ys));

	ys->sock = mnl_socket_open(NETLINK_GENERIC);
	if (!ys->sock) {
		perr("failed to create a netlink socket");
		goto err_free_sock;
	}

	if (mnl_socket_setsockopt(ys->sock, NETLINK_CAP_ACK,
				  &one, sizeof(one))) {
		perr("failed to enable netlink ACK");
		goto err_close_sock;
	}
	if (mnl_socket_setsockopt(ys->sock, NETLINK_EXT_ACK,
				  &one, sizeof(one))) {
		perr("failed to enable netlink ext ACK");
		goto err_close_sock;
	}

	ys->seq = random();
	ys->portid = mnl_socket_get_portid(ys->sock);

	nlh = ynl_gemsg_start_req(ys, GENL_ID_CTRL, CTRL_CMD_GETFAMILY, 1);
	mnl_attr_put_strz(nlh, CTRL_ATTR_FAMILY_NAME, family_name);

	err = mnl_socket_sendto(ys->sock, nlh, nlh->nlmsg_len);
	if (err < 0) {
		perr("failed to request socket family info");
		goto err_close_sock;
	}

	do {
		err = mnl_socket_recvfrom(ys->sock, ys->buf,
					  MNL_SOCKET_BUFFER_SIZE);
		if (err <= 0) {
			perr("failed to receive the socket family info");
			goto err_close_sock;
		}
		err = mnl_cb_run2(ys->buf, err, ys->seq, ys->portid,
				  get_family_id_cb, ys,
				  mnlg_cb_array, ARRAY_SIZE(mnlg_cb_array));
		if (err < 0) {
			perr("failed to receive the socket family info 2");
			goto err_close_sock;
		}
	} while (err > 0);


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

/* -- sample main -- */

#include "genetlink-user.h"

int main(int argc, char **argv)
{
	struct genlctrl_getfamily_rsp *rsp;
	struct genlctrl_getfamily_req req;
	struct ynl_sock *ys;
	unsigned int i;

	if (argc < 2)
		return err_ret(1, "Usage: %s <family_name>\n", argv[0]);

	ys = ynl_sock_create(argv[1]);
	if (!ys)
		return 1;

	memset(&req, 0, sizeof(req));
	genlctrl_getfamily_req_set_family_name(&req, argv[1]);

	rsp = genlctrl_getfamily(ys, &req);
	if (rsp) {
		if (rsp->family_id_present && rsp->family_name_present) {
			printf("YS response family id %u name '%s' n_ops %d\n",
			       rsp->family_id, rsp->family_name, rsp->n_ops);
			for (i = 0; i < rsp->n_ops; i++)
				printf("\top[%d]: cmd:%d flags:%x\n",
				       rsp->ops[i].idx, rsp->ops[i].id,
				       rsp->ops[i].flags);
		}
		free(rsp);
	}

	ynl_sock_destroy(ys);
	return 0;
}
