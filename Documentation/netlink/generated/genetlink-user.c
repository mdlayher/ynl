// SPDX-License-Identifier: MIT
// Do not edit directly, auto-generated from:
//	Documentation/netlink/bindings/genetlink.yaml
// ./gen.py --mode user --user-header genetlink-user.h user.h --source --spec Documentation/netlink/bindings/genetlink.yaml

#include <linux/genetlink.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libmnl/libmnl.h>

#include "genetlink-user.h"
#include "user.h"

// Common nested types
void nlctrl_mcast_group_free(struct nlctrl_mcast_group *obj)
{
	free(obj);
}

int nlctrl_mcast_group_parse(struct nlctrl_mcast_group *dst,
			     const struct nlattr *nested, __u32 idx)
{
	const struct nlattr *attr;

	dst->idx = idx;

	mnl_attr_for_each_nested(attr, nested) {
		if (mnl_attr_get_type(attr) == CTRL_ATTR_MCAST_GRP_ID) {
			dst->id_present = 1;
			dst->id = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == CTRL_ATTR_MCAST_GRP_NAME) {
			dst->name_present = 1;
			strncpy(dst->name, mnl_attr_get_str(attr), GENL_NAMSIZ - 1);
			dst->name[GENL_NAMSIZ - 1] = 0;
		}
	}

	return 0;
}

void nlctrl_nl_policy_free(struct nlctrl_nl_policy *obj)
{
	free(obj);
}

int nlctrl_nl_policy_parse(struct nlctrl_nl_policy *dst,
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

void nlctrl_operation_free(struct nlctrl_operation *obj)
{
	free(obj);
}

int nlctrl_operation_parse(struct nlctrl_operation *dst,
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

void nlctrl_policy_free(struct nlctrl_policy *obj)
{
	free(obj);
}

int nlctrl_policy_parse(struct nlctrl_policy *dst, const struct nlattr *nested,
			__u32 cmd)
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
void nlctrl_getfamily_rsp_free(struct nlctrl_getfamily_rsp *rsp)
{
	free(rsp->ops);
	free(rsp->mcast_groups);
	free(rsp);
}

int nlctrl_getfamily_rsp_parse(const struct nlmsghdr *nlh, void *data)
{
	struct nlctrl_getfamily_rsp *dst = data;
	const struct nlattr *attr_mcast_groups;
	const struct nlattr *attr_ops;
	const struct nlattr *attr;
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
		if (mnl_attr_get_type(attr) == CTRL_ATTR_MCAST_GROUPS) {
			const struct nlattr *attr2;

			attr_mcast_groups = attr;
			mnl_attr_for_each_nested(attr2, attr)
				dst->n_mcast_groups++;
		}
	}

	if (dst->n_mcast_groups) {
		dst->mcast_groups = calloc(dst->n_mcast_groups, sizeof(*dst->mcast_groups));
		i = 0;
		mnl_attr_for_each_nested(attr, attr_mcast_groups) {
			nlctrl_mcast_group_parse(&dst->mcast_groups[i], attr, mnl_attr_get_type(attr));
			i++;
		}
	}
	if (dst->n_ops) {
		dst->ops = calloc(dst->n_ops, sizeof(*dst->ops));
		i = 0;
		mnl_attr_for_each_nested(attr, attr_ops) {
			nlctrl_operation_parse(&dst->ops[i], attr, mnl_attr_get_type(attr));
			i++;
		}
	}

	return MNL_CB_OK;
}

struct nlctrl_getfamily_rsp *
nlctrl_getfamily(struct ynl_sock *ys, struct nlctrl_getfamily_req *req)
{
	struct nlctrl_getfamily_rsp *rsp;
	struct nlmsghdr *nlh;
	int len, err;

	nlh = ynl_gemsg_start_req(ys, ys->family_id, CTRL_CMD_GETFAMILY, 1);

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

	rsp = calloc(1, sizeof(*rsp));

	err = mnl_cb_run(ys->buf, len, ys->seq, ys->portid,
			 nlctrl_getfamily_rsp_parse, rsp);
	if (err < 0)
		goto err_free;

	err = ynl_recv_ack(ys, err);
	if (err)
		goto err_free;

	return rsp;

err_free:
	nlctrl_getfamily_rsp_free(rsp);
	return NULL;
}

void nlctrl_getfamily_list_free(struct nlctrl_getfamily_list *rsp)
{
	struct nlctrl_getfamily_list *next = rsp;

	while (next) {
		rsp = next;
		next = rsp->next;

		free(rsp->obj.ops);
		free(rsp->obj.mcast_groups);
		free(rsp);
	}
}

struct nlctrl_getfamily_list *nlctrl_getfamily_dump(struct ynl_sock *ys)
{
	struct nlctrl_getfamily_list *rsp, *cur;
	struct ynl_dump_state yds = {};
	struct nlmsghdr *nlh;
	int len, err;

	yds.alloc_sz = sizeof(*rsp);
	yds.cb = nlctrl_getfamily_rsp_parse;

	nlh = ynl_gemsg_start_dump(ys, ys->family_id, CTRL_CMD_GETFAMILY, 1);

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
		nlctrl_getfamily_list_free(cur);
	}
	return NULL;
}

void nlctrl_getfamily_ntf_free(struct nlctrl_getfamily_ntf *rsp)
{
	free(rsp->obj.ops);
	free(rsp->obj.mcast_groups);
	free(rsp);
}

// CTRL_CMD_GETPOLICY
int nlctrl_getpolicy_rsp_dump_parse(const struct nlmsghdr *nlh, void *data)
{
	struct nlctrl_getpolicy_rsp_dump *dst = data;
	const struct nlattr *attr;

	mnl_attr_for_each(attr, nlh, sizeof(struct genlmsghdr)) {
		if (mnl_attr_get_type(attr) == CTRL_ATTR_FAMILY_ID) {
			dst->family_id_present = 1;
			dst->family_id = mnl_attr_get_u16(attr);
		}
		if (mnl_attr_get_type(attr) == CTRL_ATTR_OP_POLICY) {
			const struct nlattr *attr_cmd;
			__u32 cmd;

			dst->op_policy_present = 1;
			attr_cmd = mnl_attr_get_payload(attr);
			cmd = mnl_attr_get_type(attr_cmd);
			nlctrl_policy_parse(&dst->op_policy, attr_cmd, cmd);
		}
		if (mnl_attr_get_type(attr) == CTRL_ATTR_POLICY) {
			const struct nlattr *attr_current_policy_idx, *attr_attr_idx;
			__u32 current_policy_idx, attr_idx;

			dst->policy_present = 1;
			attr_current_policy_idx = mnl_attr_get_payload(attr);
			current_policy_idx = mnl_attr_get_type(attr_current_policy_idx);
			attr_attr_idx = mnl_attr_get_payload(attr_current_policy_idx);
			attr_idx = mnl_attr_get_type(attr_attr_idx);
			nlctrl_nl_policy_parse(&dst->policy, attr_attr_idx, current_policy_idx, attr_idx);
		}
	}

	return MNL_CB_OK;
}

void nlctrl_getpolicy_rsp_list_free(struct nlctrl_getpolicy_rsp_list *rsp)
{
	struct nlctrl_getpolicy_rsp_list *next = rsp;

	while (next) {
		rsp = next;
		next = rsp->next;

		free(rsp);
	}
}

struct nlctrl_getpolicy_rsp_list *
nlctrl_getpolicy_dump(struct ynl_sock *ys,
		      struct nlctrl_getpolicy_req_dump *req)
{
	struct nlctrl_getpolicy_rsp_list *rsp, *cur;
	struct ynl_dump_state yds = {};
	struct nlmsghdr *nlh;
	int len, err;

	yds.alloc_sz = sizeof(*rsp);
	yds.cb = nlctrl_getpolicy_rsp_dump_parse;

	nlh = ynl_gemsg_start_dump(ys, ys->family_id, CTRL_CMD_GETPOLICY, 1);

	if (req->family_id_present)
		mnl_attr_put_u16(nlh, CTRL_ATTR_FAMILY_ID, req->family_id);
	if (req->family_name_present)
		mnl_attr_put_strz(nlh, CTRL_ATTR_FAMILY_NAME, req->family_name);
	if (req->op_present)
		mnl_attr_put_u32(nlh, CTRL_ATTR_OP, req->op);

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
		nlctrl_getpolicy_rsp_list_free(cur);
	}
	return NULL;
}
