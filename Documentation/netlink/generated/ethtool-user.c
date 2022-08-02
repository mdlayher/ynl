// SPDX-License-Identifier: MIT
// Do not edit directly, auto-generated from:
//	Documentation/netlink/bindings/ethtool.yaml
// ./gen.py --mode user --user-header genetlink-user.h user.h --source --spec Documentation/netlink/bindings/ethtool.yaml

#include <linux/ethtool_netlink.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libmnl/libmnl.h>

#include "genetlink-user.h"
#include "user.h"

// Common nested types
void ethtool_header_free(struct ethtool_header *obj)
{
	free(obj);
}

int ethtool_header_parse(struct ethtool_header *dst,
			 const struct nlattr *nested)
{
	const struct nlattr *attr;

	mnl_attr_for_each_nested(attr, nested) {
		if (mnl_attr_get_type(attr) == ETHTOOL_A_HEADER_DEV_INDEX) {
			dst->dev_index_present = 1;
			dst->dev_index = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == ETHTOOL_A_HEADER_DEV_NAME) {
			dst->dev_name_present = 1;
			strncpy(dst->dev_name, mnl_attr_get_str(attr), ALTIFNAMSIZ - 1);
			dst->dev_name[ALTIFNAMSIZ - 1] = 0;
		}
		if (mnl_attr_get_type(attr) == ETHTOOL_A_HEADER_FLAGS) {
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
	struct ethtool_channels_get_rsp *dst = data;
	const struct nlattr *attr;

	mnl_attr_for_each(attr, nlh, sizeof(struct genlmsghdr)) {
		if (mnl_attr_get_type(attr) == ETHTOOL_A_CHANNELS_HEADER) {
			dst->header_present = 1;
			ethtool_header_parse(&dst->header, attr);
		}
		if (mnl_attr_get_type(attr) == ETHTOOL_A_CHANNELS_RX_MAX) {
			dst->rx_max_present = 1;
			dst->rx_max = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == ETHTOOL_A_CHANNELS_TX_MAX) {
			dst->tx_max_present = 1;
			dst->tx_max = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == ETHTOOL_A_CHANNELS_OTHER_MAX) {
			dst->other_max_present = 1;
			dst->other_max = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == ETHTOOL_A_CHANNELS_COMBINED_MAX) {
			dst->combined_max_present = 1;
			dst->combined_max = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == ETHTOOL_A_CHANNELS_RX) {
			dst->rx_present = 1;
			dst->rx = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == ETHTOOL_A_CHANNELS_TX) {
			dst->tx_present = 1;
			dst->tx = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == ETHTOOL_A_CHANNELS_OTHER) {
			dst->other_present = 1;
			dst->other = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == ETHTOOL_A_CHANNELS_COMBINED) {
			dst->combined_present = 1;
			dst->combined = mnl_attr_get_u32(attr);
		}
	}

	return MNL_CB_OK;
}

struct ethtool_channels_get_rsp *
ethtool_channels_get(struct ynl_sock *ys, struct ethtool_channels_get_req *req)
{
	struct ethtool_channels_get_rsp *rsp;
	struct nlmsghdr *nlh;
	int len, err;

	nlh = ynl_gemsg_start_req(ys, ys->family_id, ETHTOOL_MSG_CHANNELS_GET, 1);

	if (req->header_present)
		ethtool_header_put(nlh, ETHTOOL_A_CHANNELS_HEADER, &req->header);

	err = mnl_socket_sendto(ys->sock, nlh, nlh->nlmsg_len);
	if (err < 0)
		return NULL;

	len = mnl_socket_recvfrom(ys->sock, ys->buf, MNL_SOCKET_BUFFER_SIZE);
	if (len < 0)
		return NULL;

	rsp = calloc(1, sizeof(*rsp));

	err = mnl_cb_run(ys->buf, len, ys->seq, ys->portid,
			 ethtool_channels_get_rsp_parse, rsp);
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

	yds.alloc_sz = sizeof(*rsp);
	yds.cb = ethtool_channels_get_rsp_parse;

	nlh = ynl_gemsg_start_dump(ys, ys->family_id, ETHTOOL_MSG_CHANNELS_GET, 1);

	err = mnl_socket_sendto(ys->sock, nlh, nlh->nlmsg_len);
	if (err < 0)
		return NULL;

	do {
		len = mnl_socket_recvfrom(ys->sock, ys->buf, MNL_SOCKET_BUFFER_SIZE);
		if (len < 0)
			goto free_list;

		err = mnl_cb_run(ys->buf, len, ys->seq, ys->portid,
				 ynl_dump_trampoline, &yds);
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

	if (req->header_present)
		ethtool_header_put(nlh, ETHTOOL_A_CHANNELS_HEADER, &req->header);
	if (req->rx_present)
		mnl_attr_put_u32(nlh, ETHTOOL_A_CHANNELS_RX, req->rx);
	if (req->tx_present)
		mnl_attr_put_u32(nlh, ETHTOOL_A_CHANNELS_TX, req->tx);
	if (req->other_present)
		mnl_attr_put_u32(nlh, ETHTOOL_A_CHANNELS_OTHER, req->other);
	if (req->combined_present)
		mnl_attr_put_u32(nlh, ETHTOOL_A_CHANNELS_COMBINED, req->combined);

	err = mnl_socket_sendto(ys->sock, nlh, nlh->nlmsg_len);
	if (err < 0)
		return -1;

	len = mnl_socket_recvfrom(ys->sock, ys->buf, MNL_SOCKET_BUFFER_SIZE);
	if (len < 0)
		return -1;

	err = ynl_recv_ack(ys, err);
	if (err)
		goto err_free;

	return 0;

err_free:
	ethtool_channels_set_rsp_free(rsp);
	return -1;
}

// --------------- Common notification parsing --------------- //
struct ynl_ntf_base_type *ethtool_ntf_parse(struct ynl_sock *ys)
{
	struct ynl_ntf_base_type *rsp;
	struct genlmsghdr *genlh;
	struct nlmsghdr *nlh;
	mnl_cb_t parse;
	int len, err;

	len = mnl_socket_recvfrom(ys->sock, ys->buf, MNL_SOCKET_BUFFER_SIZE);
	if (len < (ssize_t)(sizeof(*nlh) + sizeof(*genlh)))
		return NULL;

	nlh = (void *)ys->buf;
	genlh = mnl_nlmsg_get_payload(nlh);

	switch (genlh->cmd) {
	case ETHTOOL_MSG_CHANNELS_NTF:
		rsp = calloc(1, sizeof(struct ethtool_channels_get_ntf));
		parse = ethtool_channels_get_rsp_parse;
		break;
	default:
		return NULL;
	}

	err = mnl_cb_run(ys->buf, len, 0, 0, parse, rsp);
	if (err)
		goto err_free;

	rsp->family = nlh->nlmsg_type;
	rsp->cmd = genlh->cmd;
	return rsp;

err_free:
	free(rsp);
	return NULL;
}
