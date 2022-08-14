// SPDX-License-Identifier: BSD-3-Clause
// Do not edit directly, auto-generated from:
//	Documentation/netlink/bindings/ethtool.yaml
// YNL-GEN user source

#include <linux/ethtool_netlink.h>
#include <linux/if.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libmnl/libmnl.h>
#include <linux/genetlink.h>

#include "ethtool-user.h"
#include "ynl.h"

// Policies
extern struct ynl_policy_nest ethtool_header_nest;
extern struct ynl_policy_nest ethtool_channels_nest;

struct ynl_policy_attr ethtool_header_policy[ETHTOOL_A_HEADER_MAX + 1] = {
	[ETHTOOL_A_HEADER_DEV_INDEX] = { .name = "dev_index", .type = YNL_PT_U32, },
	[ETHTOOL_A_HEADER_DEV_NAME] = { .name = "dev_name", .type = YNL_PT_NUL_STR, .len = ALTIFNAMSIZ, },
	[ETHTOOL_A_HEADER_FLAGS] = { .name = "flags", .type = YNL_PT_U32, },
};

struct ynl_policy_nest ethtool_header_nest = {
	.max_attr = ETHTOOL_A_HEADER_MAX,
	.table = ethtool_header_policy,
};

struct ynl_policy_attr ethtool_channels_policy[ETHTOOL_A_CHANNELS_MAX + 1] = {
	[ETHTOOL_A_CHANNELS_HEADER] = { .name = "header", .type = YNL_PT_NEST, .nest = &ethtool_header_nest, },
	[ETHTOOL_A_CHANNELS_RX_MAX] = { .name = "rx_max", .type = YNL_PT_U32, },
	[ETHTOOL_A_CHANNELS_TX_MAX] = { .name = "tx_max", .type = YNL_PT_U32, },
	[ETHTOOL_A_CHANNELS_OTHER_MAX] = { .name = "other_max", .type = YNL_PT_U32, },
	[ETHTOOL_A_CHANNELS_COMBINED_MAX] = { .name = "combined_max", .type = YNL_PT_U32, },
	[ETHTOOL_A_CHANNELS_RX_COUNT] = { .name = "rx_count", .type = YNL_PT_U32, },
	[ETHTOOL_A_CHANNELS_TX_COUNT] = { .name = "tx_count", .type = YNL_PT_U32, },
	[ETHTOOL_A_CHANNELS_OTHER_COUNT] = { .name = "other_count", .type = YNL_PT_U32, },
	[ETHTOOL_A_CHANNELS_COMBINED_COUNT] = { .name = "combined_count", .type = YNL_PT_U32, },
};

struct ynl_policy_nest ethtool_channels_nest = {
	.max_attr = ETHTOOL_A_CHANNELS_MAX,
	.table = ethtool_channels_policy,
};

// Common nested types
void ethtool_header_free(struct ethtool_header *obj)
{
	free(obj);
}

int ethtool_header_put(struct nlmsghdr *nlh, unsigned int attr_type,
		       struct ethtool_header *obj)
{
	struct nlattr *nest;

	nest = mnl_attr_nest_start(nlh, attr_type);
	if (obj->dev_index_present)
		mnl_attr_put_u32(nlh, ETHTOOL_A_HEADER_DEV_INDEX, obj->dev_index);
	if (obj->dev_name_present)
		mnl_attr_put_strz(nlh, ETHTOOL_A_HEADER_DEV_NAME, obj->dev_name);
	if (obj->flags_present)
		mnl_attr_put_u32(nlh, ETHTOOL_A_HEADER_FLAGS, obj->flags);
	mnl_attr_nest_end(nlh, nest);

	return 0;
}

int ethtool_header_parse(struct ynl_parse_arg *yarg,
			 const struct nlattr *nested)
{
	struct ethtool_header *dst = yarg->data;
	const struct nlattr *attr;

	mnl_attr_for_each_nested(attr, nested) {
		if (mnl_attr_get_type(attr) == ETHTOOL_A_HEADER_DEV_INDEX) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
			dst->dev_index_present = 1;
			dst->dev_index = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == ETHTOOL_A_HEADER_DEV_NAME) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
			dst->dev_name_present = 1;
			strncpy(dst->dev_name, mnl_attr_get_str(attr), ALTIFNAMSIZ - 1);
			dst->dev_name[ALTIFNAMSIZ - 1] = 0;
		}
		if (mnl_attr_get_type(attr) == ETHTOOL_A_HEADER_FLAGS) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
			dst->flags_present = 1;
			dst->flags = mnl_attr_get_u32(attr);
		}
	}

	return 0;
}

/* ============== ETHTOOL_MSG_CHANNELS_GET ============== */
// ETHTOOL_MSG_CHANNELS_GET - do
void ethtool_channels_get_rsp_free(struct ethtool_channels_get_rsp *rsp)
{
	free(rsp);
}

int ethtool_channels_get_rsp_parse(const struct nlmsghdr *nlh, void *data)
{
	struct ethtool_channels_get_rsp *dst;
	struct ynl_parse_arg *yarg = data;
	const struct nlattr *attr;
	struct ynl_parse_arg parg;

	dst = yarg->data;
	parg.ys = yarg->ys;

	mnl_attr_for_each(attr, nlh, sizeof(struct genlmsghdr)) {
		if (mnl_attr_get_type(attr) == ETHTOOL_A_CHANNELS_HEADER) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
			dst->header_present = 1;

			parg.rsp_policy = &ethtool_header_nest;
			parg.data = &dst->header;
			ethtool_header_parse(&parg, attr);
		}
		if (mnl_attr_get_type(attr) == ETHTOOL_A_CHANNELS_RX_MAX) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
			dst->rx_max_present = 1;
			dst->rx_max = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == ETHTOOL_A_CHANNELS_TX_MAX) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
			dst->tx_max_present = 1;
			dst->tx_max = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == ETHTOOL_A_CHANNELS_OTHER_MAX) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
			dst->other_max_present = 1;
			dst->other_max = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == ETHTOOL_A_CHANNELS_COMBINED_MAX) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
			dst->combined_max_present = 1;
			dst->combined_max = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == ETHTOOL_A_CHANNELS_RX_COUNT) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
			dst->rx_count_present = 1;
			dst->rx_count = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == ETHTOOL_A_CHANNELS_TX_COUNT) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
			dst->tx_count_present = 1;
			dst->tx_count = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == ETHTOOL_A_CHANNELS_OTHER_COUNT) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
			dst->other_count_present = 1;
			dst->other_count = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == ETHTOOL_A_CHANNELS_COMBINED_COUNT) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
			dst->combined_count_present = 1;
			dst->combined_count = mnl_attr_get_u32(attr);
		}
	}

	return MNL_CB_OK;
}

struct ethtool_channels_get_rsp *
ethtool_channels_get(struct ynl_sock *ys, struct ethtool_channels_get_req *req)
{
	struct ynl_parse_arg yarg = { .ys = ys, };
	struct ethtool_channels_get_rsp *rsp;
	struct nlmsghdr *nlh;
	int len, err;

	nlh = ynl_gemsg_start_req(ys, ys->family_id, ETHTOOL_MSG_CHANNELS_GET, 1);
	ys->req_policy = &ethtool_channels_nest;
	yarg.rsp_policy = &ethtool_channels_nest;

	if (req->header_present)
		ethtool_header_put(nlh, ETHTOOL_A_CHANNELS_HEADER, &req->header);

	err = mnl_socket_sendto(ys->sock, nlh, nlh->nlmsg_len);
	if (err < 0)
		return NULL;

	len = mnl_socket_recvfrom(ys->sock, ys->rx_buf, MNL_SOCKET_BUFFER_SIZE);
	if (len < 0)
		return NULL;

	rsp = calloc(1, sizeof(*rsp));
	yarg.data = rsp;

	err = mnl_cb_run2(ys->rx_buf, len, ys->seq, ys->portid,
			 ethtool_channels_get_rsp_parse, &yarg,
			 ynl_cb_array, NLMSG_MIN_TYPE);
	if (err < 0)
		goto err_free;

	err = ynl_recv_ack(ys, err);
	if (err)
		goto err_free;

	return rsp;

err_free:
	ethtool_channels_get_rsp_free(rsp);
	return NULL;
}

// ETHTOOL_MSG_CHANNELS_GET - dump
void ethtool_channels_get_list_free(struct ethtool_channels_get_list *rsp)
{
	struct ethtool_channels_get_list *next = rsp;

	while (next) {
		rsp = next;
		next = rsp->next;

		free(rsp);
	}
}

struct ethtool_channels_get_list *
ethtool_channels_get_dump(struct ynl_sock *ys)
{
	struct ethtool_channels_get_list *rsp, *cur;
	struct ynl_dump_state yds = {};
	struct nlmsghdr *nlh;
	int len, err;

	yds.ys = ys;
	yds.alloc_sz = sizeof(*rsp);
	yds.cb = ethtool_channels_get_rsp_parse;

	nlh = ynl_gemsg_start_dump(ys, ys->family_id, ETHTOOL_MSG_CHANNELS_GET, 1);

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
	rsp = yds.first;
	while (rsp) {
		cur = rsp;
		rsp = rsp->next;
		ethtool_channels_get_list_free(cur);
	}
	return NULL;
}

// ETHTOOL_MSG_CHANNELS_GET - notify
void ethtool_channels_get_ntf_free(struct ethtool_channels_get_ntf *rsp)
{
	free(rsp);
}

/* ============== ETHTOOL_MSG_CHANNELS_SET ============== */
// ETHTOOL_MSG_CHANNELS_SET - do
int ethtool_channels_set(struct ynl_sock *ys,
			 struct ethtool_channels_set_req *req)
{
	struct nlmsghdr *nlh;
	int len, err;

	nlh = ynl_gemsg_start_req(ys, ys->family_id, ETHTOOL_MSG_CHANNELS_SET, 1);
	ys->req_policy = &ethtool_channels_nest;

	if (req->header_present)
		ethtool_header_put(nlh, ETHTOOL_A_CHANNELS_HEADER, &req->header);
	if (req->rx_count_present)
		mnl_attr_put_u32(nlh, ETHTOOL_A_CHANNELS_RX_COUNT, req->rx_count);
	if (req->tx_count_present)
		mnl_attr_put_u32(nlh, ETHTOOL_A_CHANNELS_TX_COUNT, req->tx_count);
	if (req->other_count_present)
		mnl_attr_put_u32(nlh, ETHTOOL_A_CHANNELS_OTHER_COUNT, req->other_count);
	if (req->combined_count_present)
		mnl_attr_put_u32(nlh, ETHTOOL_A_CHANNELS_COMBINED_COUNT, req->combined_count);

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

// --------------- Common notification parsing --------------- //
struct ynl_ntf_base_type *ethtool_ntf_parse(struct ynl_sock *ys)
{
	struct ynl_parse_arg yarg = { .ys = ys, };
	struct ynl_ntf_base_type *rsp;
	struct genlmsghdr *genlh;
	struct nlmsghdr *nlh;
	mnl_cb_t parse;
	int len, err;

	len = mnl_socket_recvfrom(ys->sock, ys->rx_buf, MNL_SOCKET_BUFFER_SIZE);
	if (len < (ssize_t)(sizeof(*nlh) + sizeof(*genlh)))
		return NULL;

	nlh = (void *)ys->rx_buf;
	genlh = mnl_nlmsg_get_payload(nlh);

	switch (genlh->cmd) {
	case ETHTOOL_MSG_CHANNELS_NTF:
		rsp = calloc(1, sizeof(struct ethtool_channels_get_ntf));
		parse = ethtool_channels_get_rsp_parse;
		break;
	default:
		return NULL;
	}

	yarg.data = rsp->data;

	err = mnl_cb_run2(ys->rx_buf, len, 0, 0, parse, &yarg,
			 ynl_cb_array, NLMSG_MIN_TYPE);
	if (err)
		goto err_free;

	rsp->family = nlh->nlmsg_type;
	rsp->cmd = genlh->cmd;
	return rsp;

err_free:
	free(rsp);
	return NULL;
}
