// SPDX-License-Identifier: MIT
// Do not edit directly, auto-generated from:
//	Documentation/netlink/bindings/genetlink.yaml

#include <linux/genetlink.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libmnl/libmnl.h>

#include "genetlink-user.h"
#include "user.h"

// Common nested types
int genlctrl_nl_policy_parse(struct genlctrl_nl_policy *dst,
			     const struct nlattr *nested, __u32 attr_idx,
			     __u32 current_policy_idx)
{
	const struct nlattr *attr;

	dst->attr_idx = attr_idx;
	dst->current_policy_idx = current_policy_idx;

	mnl_attr_for_each_nested(attr, nested) {
		if (mnl_attr_get_type(attr) == NL_POLICY_TYPE_ATTR_TYPE) {
			dst->type_present = 1;
			dst->type = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == NL_POLICY_TYPE_ATTR_MIN_VALUE_U) {
			dst->min_value_u_present = 1;
			dst->min_value_u = mnl_attr_get_u64(attr);
		}
		if (mnl_attr_get_type(attr) == NL_POLICY_TYPE_ATTR_MAX_VALUE_U) {
			dst->max_value_u_present = 1;
			dst->max_value_u = mnl_attr_get_u64(attr);
		}
		if (mnl_attr_get_type(attr) == NL_POLICY_TYPE_ATTR_MIN_VALUE_S) {
			dst->min_value_s_present = 1;
			dst->min_value_s = mnl_attr_get_u64(attr);
		}
		if (mnl_attr_get_type(attr) == NL_POLICY_TYPE_ATTR_MAX_VALUE_S) {
			dst->max_value_s_present = 1;
			dst->max_value_s = mnl_attr_get_u64(attr);
		}
		if (mnl_attr_get_type(attr) == NL_POLICY_TYPE_ATTR_MASK) {
			dst->mask_present = 1;
			dst->mask = mnl_attr_get_u64(attr);
		}
		if (mnl_attr_get_type(attr) == NL_POLICY_TYPE_ATTR_MIN_LENGTH) {
			dst->min_length_present = 1;
			dst->min_length = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == NL_POLICY_TYPE_ATTR_MAX_LENGTH) {
			dst->max_length_present = 1;
			dst->max_length = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == NL_POLICY_TYPE_ATTR_POLICY_IDX) {
			dst->policy_idx_present = 1;
			dst->policy_idx = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == NL_POLICY_TYPE_ATTR_POLICY_MAXTYPE) {
			dst->policy_maxtype_present = 1;
			dst->policy_maxtype = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == NL_POLICY_TYPE_ATTR_BITFIELD32_MASK) {
			dst->bitfield32_mask_present = 1;
			dst->bitfield32_mask = mnl_attr_get_u32(attr);
		}
	}

	return 0;
}

int genlctrl_operation_parse(struct genlctrl_operation *dst,
			     const struct nlattr *nested, __u32 idx)
{
	const struct nlattr *attr;

	dst->idx = idx;

	mnl_attr_for_each_nested(attr, nested) {
		if (mnl_attr_get_type(attr) == CTRL_ATTR_OP_ID) {
			dst->id_present = 1;
			dst->id = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == CTRL_ATTR_OP_FLAGS) {
			dst->flags_present = 1;
			dst->flags = mnl_attr_get_u32(attr);
		}
	}

	return 0;
}

int genlctrl_policy_parse(struct genlctrl_policy *dst,
			  const struct nlattr *nested, __u32 cmd)
{
	const struct nlattr *attr;

	dst->cmd = cmd;

	mnl_attr_for_each_nested(attr, nested) {
		if (mnl_attr_get_type(attr) == CTRL_ATTR_POLICY_DO) {
			dst->do_present = 1;
			dst->do_ = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == CTRL_ATTR_POLICY_DUMP) {
			dst->dump_present = 1;
			dst->dump = mnl_attr_get_u32(attr);
		}
	}

	return 0;
}

// CTRL_CMD_GETFAMILY
int genlctrl_getfamily_rsp_parse(struct genlctrl_getfamily_rsp *dst,
				 const struct nlmsghdr *nlh)
{
	const struct nlattr *attr;
	const struct nlattr *attr_ops;
	int i;

	mnl_attr_for_each(attr, nlh, sizeof(struct genlmsghdr)) {
		if (mnl_attr_get_type(attr) == CTRL_ATTR_FAMILY_ID) {
			dst->family_id_present = 1;
			dst->family_id = mnl_attr_get_u16(attr);
		}
		if (mnl_attr_get_type(attr) == CTRL_ATTR_FAMILY_NAME) {
			dst->family_name_present = 1;
			strncpy(dst->family_name, mnl_attr_get_str(attr), GENL_NAMSIZ - 1);
			dst->family_name[GENL_NAMSIZ - 1] = 0;
		}
		if (mnl_attr_get_type(attr) == CTRL_ATTR_VERSION) {
			dst->version_present = 1;
			dst->version = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == CTRL_ATTR_HDRSIZE) {
			dst->hdrsize_present = 1;
			dst->hdrsize = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == CTRL_ATTR_MAXATTR) {
			dst->maxattr_present = 1;
			dst->maxattr = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == CTRL_ATTR_OPS) {
			const struct nlattr *attr2;

			attr_ops = attr;
			mnl_attr_for_each_nested(attr2, attr)
				dst->n_ops++;
		}
	}

	// ops
	dst->ops = calloc(dst->n_ops, sizeof(struct genlctrl_operation));
	i = 0;
	mnl_attr_for_each_nested(attr, attr_ops) {
		genlctrl_operation_parse(&dst->ops[i], attr, i);
		i++;
	}


	return 0;
}

struct genlctrl_getfamily_rsp *
genlctrl_getfamily(struct ynl_sock *ys, struct genlctrl_getfamily_req *req)
{
	struct genlctrl_getfamily_rsp *rsp;
	struct nlmsghdr *nlh;
	int len, err;

	nlh = ynl_gemsg_start_req(ys, GENL_ID_CTRL, CTRL_CMD_GETFAMILY, 1);

	if (req->family_id_present)
		mnl_attr_put_u16(nlh, CTRL_ATTR_FAMILY_ID, req->family_id);
	if (req->family_name_present)
		mnl_attr_put_strz(nlh, CTRL_ATTR_FAMILY_NAME, req->family_name);

	err = mnl_socket_sendto(ys->sock, nlh, nlh->nlmsg_len);
	if (err < 0)
		return NULL;

	len = mnl_socket_recvfrom(ys->sock, ys->buf, MNL_SOCKET_BUFFER_SIZE);
	if (len < 0)
		return NULL;

	nlh = (struct nlmsghdr *)ys->buf;
	if (!mnl_nlmsg_ok(nlh, len) || (unsigned int)len != nlh->nlmsg_len)
		return NULL;

	rsp = calloc(1, sizeof(*rsp));
	genlctrl_getfamily_rsp_parse(rsp, nlh);
	return rsp;
}

// CTRL_CMD_GETPOLICY
