// SPDX-License-Identifier: MIT
// Do not edit directly, auto-generated from:
//	../../../../Documentation/netlink/bindings/dpll.yaml
// /home/kicinski/devel/linux/gen.py --mode user --user-header dpll-user.h ynl.h --source --spec ../../../../Documentation/netlink/bindings/dpll.yaml

#include <linux/dpll.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libmnl/libmnl.h>
#include <linux/genetlink.h>

#include "dpll-user.h"
#include "ynl.h"

// Common nested types
void dpll_output_free(struct dpll_output *obj)
{
	free(obj);
}

int dpll_output_parse(struct dpll_output *dst, const struct nlattr *nested)
{
	const struct nlattr *attr;
	int i;

	mnl_attr_for_each_nested(attr, nested) {
		if (mnl_attr_get_type(attr) == DPLLA_SOURCE_ID) {
			dst->source_id_present = 1;
			dst->source_id = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == DPLLA_SOURCE_TYPE) {
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
	free(obj);
}

int dpll_source_parse(struct dpll_source *dst, const struct nlattr *nested)
{
	const struct nlattr *attr;
	int i;

	mnl_attr_for_each_nested(attr, nested) {
		if (mnl_attr_get_type(attr) == DPLLA_OUTPUT_ID) {
			dst->output_id_present = 1;
			dst->output_id = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == DPLLA_OUTPUT_TYPE) {
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
	free(rsp);
}

int dpll_device_get_rsp_parse(const struct nlmsghdr *nlh, void *data)
{
	struct dpll_device_get_rsp *dst = data;
	const struct nlattr *attr;
	int i;

	mnl_attr_for_each(attr, nlh, sizeof(struct genlmsghdr)) {
		if (mnl_attr_get_type(attr) == DPLLA_DEVICE_ID) {
			dst->device_id_present = 1;
			dst->device_id = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == DPLLA_DEVICE_NAME) {
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
			dst->status_present = 1;
			dst->status = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == DPLLA_TEMP) {
			dst->temp_present = 1;
			dst->temp = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == DPLLA_LOCK_STATUS) {
			dst->lock_status_present = 1;
			dst->lock_status = mnl_attr_get_u32(attr);
		}
	}

	if (dst->n_output) {
		dst->output = calloc(dst->n_output, sizeof(*dst->output));
		i = 0;
		mnl_attr_for_each(attr, nlh, sizeof(struct genlmsghdr)) {
			if (mnl_attr_get_type(attr) == DPLLA_OUTPUT) {
				dpll_output_parse(&dst->output[i], attr);
				i++;
			}
		}
	}
	if (dst->n_source) {
		dst->source = calloc(dst->n_source, sizeof(*dst->source));
		i = 0;
		mnl_attr_for_each(attr, nlh, sizeof(struct genlmsghdr)) {
			if (mnl_attr_get_type(attr) == DPLLA_SOURCE) {
				dpll_source_parse(&dst->source[i], attr);
				i++;
			}
		}
	}

	return MNL_CB_OK;
}

struct dpll_device_get_rsp *
dpll_device_get(struct ynl_sock *ys, struct dpll_device_get_req *req)
{
	struct dpll_device_get_rsp *rsp;
	struct nlmsghdr *nlh;
	int len, err;

	nlh = ynl_gemsg_start_req(ys, ys->family_id, DPLL_CMD_DEVICE_GET, 1);

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

	err = mnl_cb_run(ys->rx_buf, len, ys->seq, ys->portid,
			 dpll_device_get_rsp_parse, rsp);
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
