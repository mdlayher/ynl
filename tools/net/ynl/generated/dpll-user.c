// SPDX-License-Identifier: BSD-3-Clause
// Do not edit directly, auto-generated from:
//	Documentation/netlink/bindings/dpll.yaml
// YNL-GEN user source

#include <linux/dpll.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libmnl/libmnl.h>
#include <linux/genetlink.h>

#include "dpll-user.h"
#include "ynl.h"

// Policies
extern struct ynl_policy_nest dpll_main_nest;
extern struct ynl_policy_nest dpll_source_nest;
extern struct ynl_policy_nest dpll_output_nest;

struct ynl_policy_attr dpll_main_policy[DPLLA_MAX + 1] = {
	[DPLLA_UNSPEC] = { .name = "unspec", .type = YNL_PT_REJECT, },
	[DPLLA_DEVICE_ID] = { .name = "device_id", .type = YNL_PT_U32, },
	[DPLLA_DEVICE_NAME] = { .name = "device_name", .type = YNL_PT_NUL_STR, .len = DPLL_NAME_LENGTH, },
	[DPLLA_SOURCE] = { .name = "source", .type = YNL_PT_NEST, .nest = &dpll_source_nest, },
	[DPLLA_SOURCE_ID] = { .name = "source_id", .type = YNL_PT_U32, },
	[DPLLA_SOURCE_TYPE] = { .name = "source_type", .type = YNL_PT_U32, },
	[DPLLA_SOURCE_SUPPORTED] = { .name = "source_supported", .type = YNL_PT_U32, },
	[DPLLA_OUTPUT] = { .name = "output", .type = YNL_PT_NEST, .nest = &dpll_output_nest, },
	[DPLLA_OUTPUT_ID] = { .name = "output_id", .type = YNL_PT_U32, },
	[DPLLA_OUTPUT_TYPE] = { .name = "output_type", .type = YNL_PT_U32, },
	[DPLLA_OUTPUT_SUPPORTED] = { .name = "output_supported", .type = YNL_PT_U32, },
	[DPLLA_STATUS] = { .name = "status", .type = YNL_PT_U32, },
	[DPLLA_TEMP] = { .name = "temp", .type = YNL_PT_U32, },
	[DPLLA_LOCK_STATUS] = { .name = "lock_status", .type = YNL_PT_U32, },
	[DPLLA_FLAGS] = { .name = "flags", .type = YNL_PT_U32, },
};

struct ynl_policy_nest dpll_main_nest = {
	.max_attr = DPLLA_MAX,
	.table = dpll_main_policy,
};

struct ynl_policy_attr dpll_source_policy[DPLLA_MAX + 1] = {
	[DPLLA_OUTPUT_ID] = { .name = "output_id", .type = YNL_PT_U32, },
	[DPLLA_OUTPUT_TYPE] = { .name = "output_type", .type = YNL_PT_U32, },
	[DPLLA_OUTPUT_SUPPORTED] = { .name = "output_supported", .type = YNL_PT_U32, },
};

struct ynl_policy_nest dpll_source_nest = {
	.max_attr = DPLLA_MAX,
	.table = dpll_source_policy,
};

struct ynl_policy_attr dpll_output_policy[DPLLA_MAX + 1] = {
	[DPLLA_SOURCE_ID] = { .name = "source_id", .type = YNL_PT_U32, },
	[DPLLA_SOURCE_TYPE] = { .name = "source_type", .type = YNL_PT_U32, },
	[DPLLA_SOURCE_SUPPORTED] = { .name = "source_supported", .type = YNL_PT_U32, },
};

struct ynl_policy_nest dpll_output_nest = {
	.max_attr = DPLLA_MAX,
	.table = dpll_output_policy,
};

// Common nested types
void dpll_output_free(struct dpll_output *obj)
{
	free(obj->source_supported);
	free(obj);
}

int dpll_output_parse(struct ynl_parse_arg *yarg, const struct nlattr *nested)
{
	struct dpll_output *dst = yarg->data;
	const struct nlattr *attr;
	int i;

	mnl_attr_for_each_nested(attr, nested) {
		if (mnl_attr_get_type(attr) == DPLLA_SOURCE_ID) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
			dst->source_id_present = 1;
			dst->source_id = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == DPLLA_SOURCE_TYPE) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
			dst->source_type_present = 1;
			dst->source_type = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == DPLLA_SOURCE_SUPPORTED) {
			dst->n_source_supported++;
		}
	}

	if (dst->n_source_supported) {
		dst->source_supported = calloc(dst->n_source_supported, sizeof(*dst->source_supported));
		i = 0;
		mnl_attr_for_each_nested(attr, nested) {
			if (mnl_attr_get_type(attr) == DPLLA_SOURCE_SUPPORTED) {
				dst->source_supported[i] = mnl_attr_get_u32(attr);
				i++;
			}
		}
	}

	return 0;
}

void dpll_source_free(struct dpll_source *obj)
{
	free(obj->output_supported);
	free(obj);
}

int dpll_source_parse(struct ynl_parse_arg *yarg, const struct nlattr *nested)
{
	struct dpll_source *dst = yarg->data;
	const struct nlattr *attr;
	int i;

	mnl_attr_for_each_nested(attr, nested) {
		if (mnl_attr_get_type(attr) == DPLLA_OUTPUT_ID) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
			dst->output_id_present = 1;
			dst->output_id = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == DPLLA_OUTPUT_TYPE) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
			dst->output_type_present = 1;
			dst->output_type = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == DPLLA_OUTPUT_SUPPORTED) {
			dst->n_output_supported++;
		}
	}

	if (dst->n_output_supported) {
		dst->output_supported = calloc(dst->n_output_supported, sizeof(*dst->output_supported));
		i = 0;
		mnl_attr_for_each_nested(attr, nested) {
			if (mnl_attr_get_type(attr) == DPLLA_OUTPUT_SUPPORTED) {
				dst->output_supported[i] = mnl_attr_get_u32(attr);
				i++;
			}
		}
	}

	return 0;
}

/* ============== DPLL_CMD_DEVICE_GET ============== */
// DPLL_CMD_DEVICE_GET - do
void dpll_device_get_rsp_free(struct dpll_device_get_rsp *rsp)
{
	free(rsp->source);
	free(rsp->output);
	free(rsp);
}

int dpll_device_get_rsp_parse(const struct nlmsghdr *nlh, void *data)
{
	struct ynl_parse_arg *yarg = data;
	struct dpll_device_get_rsp *dst;
	const struct nlattr *attr;
	struct ynl_parse_arg parg;
	int i;

	dst = yarg->data;
	parg.ys = yarg->ys;

	mnl_attr_for_each(attr, nlh, sizeof(struct genlmsghdr)) {
		if (mnl_attr_get_type(attr) == DPLLA_DEVICE_ID) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
			dst->device_id_present = 1;
			dst->device_id = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == DPLLA_DEVICE_NAME) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
			dst->device_name_present = 1;
			strncpy(dst->device_name, mnl_attr_get_str(attr), DPLL_NAME_LENGTH - 1);
			dst->device_name[DPLL_NAME_LENGTH - 1] = 0;
		}
		if (mnl_attr_get_type(attr) == DPLLA_SOURCE) {
			dst->n_source++;
		}
		if (mnl_attr_get_type(attr) == DPLLA_OUTPUT) {
			dst->n_output++;
		}
		if (mnl_attr_get_type(attr) == DPLLA_STATUS) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
			dst->status_present = 1;
			dst->status = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == DPLLA_TEMP) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
			dst->temp_present = 1;
			dst->temp = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == DPLLA_LOCK_STATUS) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
			dst->lock_status_present = 1;
			dst->lock_status = mnl_attr_get_u32(attr);
		}
	}

	if (dst->n_output) {
		dst->output = calloc(dst->n_output, sizeof(*dst->output));
		i = 0;
		parg.rsp_policy = &dpll_output_nest;
		mnl_attr_for_each(attr, nlh, sizeof(struct genlmsghdr)) {
			if (mnl_attr_get_type(attr) == DPLLA_OUTPUT) {
				parg.data = &dst->output[i];
				dpll_output_parse(&parg, attr);
				i++;
			}
		}
	}
	if (dst->n_source) {
		dst->source = calloc(dst->n_source, sizeof(*dst->source));
		i = 0;
		parg.rsp_policy = &dpll_source_nest;
		mnl_attr_for_each(attr, nlh, sizeof(struct genlmsghdr)) {
			if (mnl_attr_get_type(attr) == DPLLA_SOURCE) {
				parg.data = &dst->source[i];
				dpll_source_parse(&parg, attr);
				i++;
			}
		}
	}

	return MNL_CB_OK;
}

struct dpll_device_get_rsp *
dpll_device_get(struct ynl_sock *ys, struct dpll_device_get_req *req)
{
	struct ynl_parse_arg yarg = { .ys = ys, };
	struct dpll_device_get_rsp *rsp;
	struct nlmsghdr *nlh;
	int len, err;

	nlh = ynl_gemsg_start_req(ys, ys->family_id, DPLL_CMD_DEVICE_GET, 1);
	ys->req_policy = &dpll_main_nest;

	if (req->device_id_present)
		mnl_attr_put_u32(nlh, DPLLA_DEVICE_ID, req->device_id);
	if (req->device_name_present)
		mnl_attr_put_strz(nlh, DPLLA_DEVICE_NAME, req->device_name);
	if (req->flags_present)
		mnl_attr_put_u32(nlh, DPLLA_FLAGS, req->flags);

	err = mnl_socket_sendto(ys->sock, nlh, nlh->nlmsg_len);
	if (err < 0)
		return NULL;

	len = mnl_socket_recvfrom(ys->sock, ys->rx_buf, MNL_SOCKET_BUFFER_SIZE);
	if (len < 0)
		return NULL;

	rsp = calloc(1, sizeof(*rsp));
	yarg.data = rsp;

	err = mnl_cb_run2(ys->rx_buf, len, ys->seq, ys->portid,
			 dpll_device_get_rsp_parse, &yarg,
			 ynl_cb_array, NLMSG_MIN_TYPE);
	if (err < 0)
		goto err_free;

	err = ynl_recv_ack(ys, err);
	if (err)
		goto err_free;

	return rsp;

err_free:
	dpll_device_get_rsp_free(rsp);
	return NULL;
}

/* ============== DPLL_CMD_SET_SOURCE_TYPE ============== */
// DPLL_CMD_SET_SOURCE_TYPE - do
int dpll_set_source_type(struct ynl_sock *ys,
			 struct dpll_set_source_type_req *req)
{
	struct nlmsghdr *nlh;
	int len, err;

	nlh = ynl_gemsg_start_req(ys, ys->family_id, DPLL_CMD_SET_SOURCE_TYPE, 1);
	ys->req_policy = &dpll_main_nest;

	if (req->device_id_present)
		mnl_attr_put_u32(nlh, DPLLA_DEVICE_ID, req->device_id);
	if (req->device_name_present)
		mnl_attr_put_strz(nlh, DPLLA_DEVICE_NAME, req->device_name);
	if (req->source_id_present)
		mnl_attr_put_u32(nlh, DPLLA_SOURCE_ID, req->source_id);
	if (req->source_type_present)
		mnl_attr_put_u32(nlh, DPLLA_SOURCE_TYPE, req->source_type);

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

/* ============== DPLL_CMD_SET_OUTPUT_TYPE ============== */
// DPLL_CMD_SET_OUTPUT_TYPE - do
int dpll_set_output_type(struct ynl_sock *ys,
			 struct dpll_set_output_type_req *req)
{
	struct nlmsghdr *nlh;
	int len, err;

	nlh = ynl_gemsg_start_req(ys, ys->family_id, DPLL_CMD_SET_OUTPUT_TYPE, 1);
	ys->req_policy = &dpll_main_nest;

	if (req->device_id_present)
		mnl_attr_put_u32(nlh, DPLLA_DEVICE_ID, req->device_id);
	if (req->device_name_present)
		mnl_attr_put_strz(nlh, DPLLA_DEVICE_NAME, req->device_name);
	if (req->output_id_present)
		mnl_attr_put_u32(nlh, DPLLA_OUTPUT_ID, req->output_id);
	if (req->output_type_present)
		mnl_attr_put_u32(nlh, DPLLA_OUTPUT_TYPE, req->output_type);

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

/* ============== DPLL_EVENT_DEVICE_CREATE ============== */
/* ============== DPLL_EVENT_DEVICE_DELETE ============== */
/* ============== DPLL_EVENT_STATUS_LOCKED ============== */
/* ============== DPLL_EVENT_STATUS_UNLOCKED ============== */
/* ============== DPLL_EVENT_SOURCE_CHANGE ============== */
/* ============== DPLL_EVENT_OUTPUT_CHANGE ============== */
