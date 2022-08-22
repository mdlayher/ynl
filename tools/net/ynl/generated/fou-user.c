// SPDX-License-Identifier: BSD-3-Clause
// Do not edit directly, auto-generated from:
//	Documentation/netlink/bindings/fou.yaml
// YNL-GEN user source

#include <linux/fou.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libmnl/libmnl.h>
#include <linux/genetlink.h>

#include "fou-user.h"
#include "ynl.h"

// Policies
extern struct ynl_policy_nest fou_nest;

struct ynl_policy_attr fou_policy[FOU_ATTR_MAX + 1] = {
	[FOU_ATTR_UNSPEC] = { .name = "unspec", .type = YNL_PT_REJECT, },
	[FOU_ATTR_PORT] = { .name = "port", .type = YNL_PT_U16, },
	[FOU_ATTR_AF] = { .name = "af", .type = YNL_PT_U8, },
	[FOU_ATTR_IPPROTO] = { .name = "ipproto", .type = YNL_PT_U8, },
	[FOU_ATTR_TYPE] = { .name = "type", .type = YNL_PT_U8, },
	[FOU_ATTR_REMCSUM_NOPARTIAL] = { .name = "remcsum_nopartial", .type = YNL_PT_FLAG, },
	[FOU_ATTR_LOCAL_V4] = { .name = "local_v4", .type = YNL_PT_U32, },
	[FOU_ATTR_LOCAL_V6] = { .name = "local_v6", .type = YNL_PT_NUL_STR, .len = 16, },
	[FOU_ATTR_PEER_V4] = { .name = "peer_v4", .type = YNL_PT_U32, },
	[FOU_ATTR_PEER_V6] = { .name = "peer_v6", .type = YNL_PT_NUL_STR, .len = 16, },
	[FOU_ATTR_PEER_PORT] = { .name = "peer_port", .type = YNL_PT_U16, },
	[FOU_ATTR_IFINDEX] = { .name = "ifindex", .type = YNL_PT_U32, },
};

struct ynl_policy_nest fou_nest = {
	.max_attr = FOU_ATTR_MAX,
	.table = fou_policy,
};

// Common nested types
/* ============== FOU_CMD_ADD ============== */
// FOU_CMD_ADD - do
int fou_add(struct ynl_sock *ys, struct fou_add_req *req)
{
	struct nlmsghdr *nlh;
	int len, err;

	nlh = ynl_gemsg_start_req(ys, ys->family_id, FOU_CMD_ADD, 1);
	ys->req_policy = &fou_nest;

	if (req->port_present)
		mnl_attr_put_u16(nlh, FOU_ATTR_PORT, req->port);
	if (req->ipproto_present)
		mnl_attr_put_u8(nlh, FOU_ATTR_IPPROTO, req->ipproto);
	if (req->type_present)
		mnl_attr_put_u8(nlh, FOU_ATTR_TYPE, req->type);
	if (req->remcsum_nopartial_present)
		mnl_attr_put(nlh, FOU_ATTR_REMCSUM_NOPARTIAL, 0, NULL);
	if (req->local_v4_present)
		mnl_attr_put_u32(nlh, FOU_ATTR_LOCAL_V4, req->local_v4);
	if (req->peer_v4_present)
		mnl_attr_put_u32(nlh, FOU_ATTR_PEER_V4, req->peer_v4);
	if (req->local_v6_present)
		mnl_attr_put(nlh, FOU_ATTR_LOCAL_V6, 16, req->local_v6);
	if (req->peer_v6_present)
		mnl_attr_put(nlh, FOU_ATTR_PEER_V6, 16, req->peer_v6);
	if (req->peer_port_present)
		mnl_attr_put_u16(nlh, FOU_ATTR_PEER_PORT, req->peer_port);
	if (req->ifindex_present)
		mnl_attr_put_u32(nlh, FOU_ATTR_IFINDEX, req->ifindex);

	err = mnl_socket_sendto(ys->sock, nlh, nlh->nlmsg_len);
	if (err < 0)
		return -1;

	len = mnl_socket_recvfrom(ys->sock, ys->rx_buf, MNL_SOCKET_BUFFER_SIZE);
	if (len < 0)
		return -1;

	err = ynl_recv_ack(ys, err);
	if (err)
		goto err_free;

	return 0;

err_free:
	return -1;
}

/* ============== FOU_CMD_DEL ============== */
// FOU_CMD_DEL - do
int fou_del(struct ynl_sock *ys, struct fou_del_req *req)
{
	struct nlmsghdr *nlh;
	int len, err;

	nlh = ynl_gemsg_start_req(ys, ys->family_id, FOU_CMD_DEL, 1);
	ys->req_policy = &fou_nest;

	if (req->af_present)
		mnl_attr_put_u8(nlh, FOU_ATTR_AF, req->af);
	if (req->ifindex_present)
		mnl_attr_put_u32(nlh, FOU_ATTR_IFINDEX, req->ifindex);
	if (req->port_present)
		mnl_attr_put_u16(nlh, FOU_ATTR_PORT, req->port);
	if (req->peer_port_present)
		mnl_attr_put_u16(nlh, FOU_ATTR_PEER_PORT, req->peer_port);
	if (req->local_v4_present)
		mnl_attr_put_u32(nlh, FOU_ATTR_LOCAL_V4, req->local_v4);
	if (req->peer_v4_present)
		mnl_attr_put_u32(nlh, FOU_ATTR_PEER_V4, req->peer_v4);
	if (req->local_v6_present)
		mnl_attr_put(nlh, FOU_ATTR_LOCAL_V6, 16, req->local_v6);
	if (req->peer_v6_present)
		mnl_attr_put(nlh, FOU_ATTR_PEER_V6, 16, req->peer_v6);

	err = mnl_socket_sendto(ys->sock, nlh, nlh->nlmsg_len);
	if (err < 0)
		return -1;

	len = mnl_socket_recvfrom(ys->sock, ys->rx_buf, MNL_SOCKET_BUFFER_SIZE);
	if (len < 0)
		return -1;

	err = ynl_recv_ack(ys, err);
	if (err)
		goto err_free;

	return 0;

err_free:
	return -1;
}

/* ============== FOU_CMD_GET ============== */
// FOU_CMD_GET - do
void fou_get_rsp_free(struct fou_get_rsp *rsp)
{
	free(rsp);
}

int fou_get_rsp_parse(const struct nlmsghdr *nlh, void *data)
{
	struct ynl_parse_arg *yarg = data;
	const struct nlattr *attr;
	struct fou_get_rsp *dst;

	dst = yarg->data;

	mnl_attr_for_each(attr, nlh, sizeof(struct genlmsghdr)) {
		if (mnl_attr_get_type(attr) == FOU_ATTR_PORT) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
			dst->port_present = 1;
			dst->port = mnl_attr_get_u16(attr);
		}
		if (mnl_attr_get_type(attr) == FOU_ATTR_IPPROTO) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
			dst->ipproto_present = 1;
			dst->ipproto = mnl_attr_get_u8(attr);
		}
		if (mnl_attr_get_type(attr) == FOU_ATTR_TYPE) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
			dst->type_present = 1;
			dst->type = mnl_attr_get_u8(attr);
		}
		if (mnl_attr_get_type(attr) == FOU_ATTR_REMCSUM_NOPARTIAL) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
			dst->remcsum_nopartial_present = 1;
		}
		if (mnl_attr_get_type(attr) == FOU_ATTR_LOCAL_V4) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
			dst->local_v4_present = 1;
			dst->local_v4 = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == FOU_ATTR_PEER_V4) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
			dst->peer_v4_present = 1;
			dst->peer_v4 = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == FOU_ATTR_LOCAL_V6) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
			dst->local_v6_present = 1;
			memcpy(dst->local_v6, mnl_attr_get_payload(attr), 16);
		}
		if (mnl_attr_get_type(attr) == FOU_ATTR_PEER_V6) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
			dst->peer_v6_present = 1;
			memcpy(dst->peer_v6, mnl_attr_get_payload(attr), 16);
		}
		if (mnl_attr_get_type(attr) == FOU_ATTR_PEER_PORT) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
			dst->peer_port_present = 1;
			dst->peer_port = mnl_attr_get_u16(attr);
		}
		if (mnl_attr_get_type(attr) == FOU_ATTR_IFINDEX) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
			dst->ifindex_present = 1;
			dst->ifindex = mnl_attr_get_u32(attr);
		}
	}

	return MNL_CB_OK;
}

struct fou_get_rsp *fou_get(struct ynl_sock *ys, struct fou_get_req *req)
{
	struct ynl_parse_arg yarg = { .ys = ys, };
	struct fou_get_rsp *rsp;
	struct nlmsghdr *nlh;
	int len, err;

	nlh = ynl_gemsg_start_req(ys, ys->family_id, FOU_CMD_GET, 1);
	ys->req_policy = &fou_nest;
	yarg.rsp_policy = &fou_nest;

	if (req->af_present)
		mnl_attr_put_u8(nlh, FOU_ATTR_AF, req->af);
	if (req->ifindex_present)
		mnl_attr_put_u32(nlh, FOU_ATTR_IFINDEX, req->ifindex);
	if (req->port_present)
		mnl_attr_put_u16(nlh, FOU_ATTR_PORT, req->port);
	if (req->peer_port_present)
		mnl_attr_put_u16(nlh, FOU_ATTR_PEER_PORT, req->peer_port);
	if (req->local_v4_present)
		mnl_attr_put_u32(nlh, FOU_ATTR_LOCAL_V4, req->local_v4);
	if (req->peer_v4_present)
		mnl_attr_put_u32(nlh, FOU_ATTR_PEER_V4, req->peer_v4);
	if (req->local_v6_present)
		mnl_attr_put(nlh, FOU_ATTR_LOCAL_V6, 16, req->local_v6);
	if (req->peer_v6_present)
		mnl_attr_put(nlh, FOU_ATTR_PEER_V6, 16, req->peer_v6);

	err = mnl_socket_sendto(ys->sock, nlh, nlh->nlmsg_len);
	if (err < 0)
		return NULL;

	len = mnl_socket_recvfrom(ys->sock, ys->rx_buf, MNL_SOCKET_BUFFER_SIZE);
	if (len < 0)
		return NULL;

	rsp = calloc(1, sizeof(*rsp));
	yarg.data = rsp;

	err = mnl_cb_run2(ys->rx_buf, len, ys->seq, ys->portid,
			 fou_get_rsp_parse, &yarg,
			 ynl_cb_array, NLMSG_MIN_TYPE);
	if (err < 0)
		goto err_free;

	err = ynl_recv_ack(ys, err);
	if (err)
		goto err_free;

	return rsp;

err_free:
	fou_get_rsp_free(rsp);
	return NULL;
}

// FOU_CMD_GET - dump
void fou_get_list_free(struct fou_get_list *rsp)
{
	struct fou_get_list *next = rsp;

	while (next) {
		rsp = next;
		next = rsp->next;

		free(rsp);
	}
}

struct fou_get_list *fou_get_dump(struct ynl_sock *ys)
{
	struct ynl_dump_state yds = {};
	struct nlmsghdr *nlh;
	int len, err;

	yds.ys = ys;
	yds.alloc_sz = sizeof(struct fou_get_list);
	yds.cb = fou_get_rsp_parse;
	yds.rsp_policy = &fou_nest;

	nlh = ynl_gemsg_start_dump(ys, ys->family_id, FOU_CMD_GET, 1);

	err = mnl_socket_sendto(ys->sock, nlh, nlh->nlmsg_len);
	if (err < 0)
		return NULL;

	do {
		len = mnl_socket_recvfrom(ys->sock, ys->rx_buf, MNL_SOCKET_BUFFER_SIZE);
		if (len < 0)
			goto free_list;

		err = mnl_cb_run2(ys->rx_buf, len, ys->seq, ys->portid,
				 ynl_dump_trampoline, &yds,
				 ynl_cb_array, NLMSG_MIN_TYPE);
		if (err < 0)
			goto free_list;
	} while (err > 0);

	return yds.first;

free_list:
	fou_get_list_free(yds.first);
	return NULL;
}
