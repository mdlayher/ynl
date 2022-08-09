// SPDX-License-Identifier: MIT
// Do not edit directly, auto-generated from:
//	../../../../Documentation/netlink/bindings/genetlink.yaml
// /home/kicinski/devel/linux/gen.py --mode user --user-header genetlink-user.h ynl.h --source --spec ../../../../Documentation/netlink/bindings/genetlink.yaml

#include <linux/genetlink.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libmnl/libmnl.h>
#include <linux/genetlink.h>

#include "genetlink-user.h"
#include "ynl.h"

// Policies
extern struct ynl_policy_nest nlctrl_main_nest;
extern struct ynl_policy_nest nlctrl_operation_nest;
extern struct ynl_policy_nest nlctrl_mcast_group_nest;
extern struct ynl_policy_nest nlctrl_policy_nest;
extern struct ynl_policy_nest nlctrl_nl_policy_nest;

struct ynl_policy_attr nlctrl_main_policy[CTRL_ATTR_MAX + 1] = {
	[CTRL_ATTR_FAMILY_ID] = { .name = "family_id", .type = YNL_PT_U16, },
	[CTRL_ATTR_FAMILY_NAME] = { .name = "family_name", .type = YNL_PT_NUL_STR, .len = GENL_NAMSIZ, },
	[CTRL_ATTR_VERSION] = { .name = "version", .type = YNL_PT_U32, },
	[CTRL_ATTR_HDRSIZE] = { .name = "hdrsize", .type = YNL_PT_U32, },
	[CTRL_ATTR_MAXATTR] = { .name = "maxattr", .type = YNL_PT_U32, },
	[CTRL_ATTR_OPS] = { .name = "ops", .type = YNL_PT_NEST, .nest = &nlctrl_operation_nest, },
	[CTRL_ATTR_MCAST_GROUPS] = { .name = "mcast_groups", .type = YNL_PT_NEST, .nest = &nlctrl_mcast_group_nest, },
	[CTRL_ATTR_OP] = { .name = "op", .type = YNL_PT_U32, },
	[CTRL_ATTR_OP_POLICY] = { .name = "op_policy", .type = YNL_PT_NEST, .nest = &nlctrl_policy_nest, },
	[CTRL_ATTR_POLICY] = { .name = "policy", .type = YNL_PT_NEST, .nest = &nlctrl_nl_policy_nest, },
};

struct ynl_policy_nest nlctrl_main_nest = {
	.max_attr = CTRL_ATTR_MAX,
	.table = nlctrl_main_policy,
};

struct ynl_policy_attr nlctrl_operation_policy[CTRL_ATTR_OP_MAX + 1] = {
	[CTRL_ATTR_OP_ID] = { .name = "id", .type = YNL_PT_U32, },
	[CTRL_ATTR_OP_FLAGS] = { .name = "flags", .type = YNL_PT_U32, },
};

struct ynl_policy_nest nlctrl_operation_nest = {
	.max_attr = CTRL_ATTR_OP_MAX,
	.table = nlctrl_operation_policy,
};

struct ynl_policy_attr nlctrl_mcast_group_policy[CTRL_ATTR_MCAST_GRP_MAX + 1] = {
	[CTRL_ATTR_MCAST_GRP_ID] = { .name = "id", .type = YNL_PT_U32, },
	[CTRL_ATTR_MCAST_GRP_NAME] = { .name = "name", .type = YNL_PT_NUL_STR, .len = GENL_NAMSIZ, },
};

struct ynl_policy_nest nlctrl_mcast_group_nest = {
	.max_attr = CTRL_ATTR_MCAST_GRP_MAX,
	.table = nlctrl_mcast_group_policy,
};

struct ynl_policy_attr nlctrl_policy_policy[CTRL_ATTR_POLICY_MAX + 1] = {
	[CTRL_ATTR_POLICY_DO] = { .name = "do", .type = YNL_PT_U32, },
	[CTRL_ATTR_POLICY_DUMP] = { .name = "dump", .type = YNL_PT_U32, },
};

struct ynl_policy_nest nlctrl_policy_nest = {
	.max_attr = CTRL_ATTR_POLICY_MAX,
	.table = nlctrl_policy_policy,
};

struct ynl_policy_attr nlctrl_nl_policy_policy[NL_POLICY_TYPE_ATTR_MAX + 1] = {
	[NL_POLICY_TYPE_ATTR_TYPE] = { .name = "type", .type = YNL_PT_U32, },
	[NL_POLICY_TYPE_ATTR_MIN_VALUE_U] = { .name = "min_value_u", .type = YNL_PT_U64, },
	[NL_POLICY_TYPE_ATTR_MAX_VALUE_U] = { .name = "max_value_u", .type = YNL_PT_U64, },
	[NL_POLICY_TYPE_ATTR_MIN_VALUE_S] = { .name = "min_value_s", .type = YNL_PT_U64, },
	[NL_POLICY_TYPE_ATTR_MAX_VALUE_S] = { .name = "max_value_s", .type = YNL_PT_U64, },
	[NL_POLICY_TYPE_ATTR_MASK] = { .name = "mask", .type = YNL_PT_U64, },
	[NL_POLICY_TYPE_ATTR_MIN_LENGTH] = { .name = "min_length", .type = YNL_PT_U32, },
	[NL_POLICY_TYPE_ATTR_MAX_LENGTH] = { .name = "max_length", .type = YNL_PT_U32, },
	[NL_POLICY_TYPE_ATTR_POLICY_IDX] = { .name = "policy_idx", .type = YNL_PT_U32, },
	[NL_POLICY_TYPE_ATTR_POLICY_MAXTYPE] = { .name = "policy_maxtype", .type = YNL_PT_U32, },
	[NL_POLICY_TYPE_ATTR_BITFIELD32_MASK] = { .name = "bitfield32_mask", .type = YNL_PT_U32, },
};

struct ynl_policy_nest nlctrl_nl_policy_nest = {
	.max_attr = NL_POLICY_TYPE_ATTR_MAX,
	.table = nlctrl_nl_policy_policy,
};

// Common nested types
void nlctrl_mcast_group_free(struct nlctrl_mcast_group *obj)
{
	free(obj);
}

int nlctrl_mcast_group_parse(struct ynl_parse_arg *yarg,
			     const struct nlattr *nested, __u32 idx)
{
	struct nlctrl_mcast_group *dst = yarg->data;
	const struct nlattr *attr;

	dst->idx = idx;

	mnl_attr_for_each_nested(attr, nested) {
		if (mnl_attr_get_type(attr) == CTRL_ATTR_MCAST_GRP_ID) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
			dst->id_present = 1;
			dst->id = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == CTRL_ATTR_MCAST_GRP_NAME) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
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

int nlctrl_nl_policy_parse(struct ynl_parse_arg *yarg,
			   const struct nlattr *nested, __u32 attr_idx,
			   __u32 current_policy_idx)
{
	struct nlctrl_nl_policy *dst = yarg->data;
	const struct nlattr *attr;

	dst->attr_idx = attr_idx;
	dst->current_policy_idx = current_policy_idx;

	mnl_attr_for_each_nested(attr, nested) {
		if (mnl_attr_get_type(attr) == NL_POLICY_TYPE_ATTR_TYPE) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
			dst->type_present = 1;
			dst->type = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == NL_POLICY_TYPE_ATTR_MIN_VALUE_U) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
			dst->min_value_u_present = 1;
			dst->min_value_u = mnl_attr_get_u64(attr);
		}
		if (mnl_attr_get_type(attr) == NL_POLICY_TYPE_ATTR_MAX_VALUE_U) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
			dst->max_value_u_present = 1;
			dst->max_value_u = mnl_attr_get_u64(attr);
		}
		if (mnl_attr_get_type(attr) == NL_POLICY_TYPE_ATTR_MIN_VALUE_S) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
			dst->min_value_s_present = 1;
			dst->min_value_s = mnl_attr_get_u64(attr);
		}
		if (mnl_attr_get_type(attr) == NL_POLICY_TYPE_ATTR_MAX_VALUE_S) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
			dst->max_value_s_present = 1;
			dst->max_value_s = mnl_attr_get_u64(attr);
		}
		if (mnl_attr_get_type(attr) == NL_POLICY_TYPE_ATTR_MASK) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
			dst->mask_present = 1;
			dst->mask = mnl_attr_get_u64(attr);
		}
		if (mnl_attr_get_type(attr) == NL_POLICY_TYPE_ATTR_MIN_LENGTH) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
			dst->min_length_present = 1;
			dst->min_length = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == NL_POLICY_TYPE_ATTR_MAX_LENGTH) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
			dst->max_length_present = 1;
			dst->max_length = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == NL_POLICY_TYPE_ATTR_POLICY_IDX) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
			dst->policy_idx_present = 1;
			dst->policy_idx = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == NL_POLICY_TYPE_ATTR_POLICY_MAXTYPE) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
			dst->policy_maxtype_present = 1;
			dst->policy_maxtype = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == NL_POLICY_TYPE_ATTR_BITFIELD32_MASK) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
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

int nlctrl_operation_parse(struct ynl_parse_arg *yarg,
			   const struct nlattr *nested, __u32 idx)
{
	struct nlctrl_operation *dst = yarg->data;
	const struct nlattr *attr;

	dst->idx = idx;

	mnl_attr_for_each_nested(attr, nested) {
		if (mnl_attr_get_type(attr) == CTRL_ATTR_OP_ID) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
			dst->id_present = 1;
			dst->id = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == CTRL_ATTR_OP_FLAGS) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
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

int nlctrl_policy_parse(struct ynl_parse_arg *yarg,
			const struct nlattr *nested, __u32 cmd)
{
	struct nlctrl_policy *dst = yarg->data;
	const struct nlattr *attr;

	dst->cmd = cmd;

	mnl_attr_for_each_nested(attr, nested) {
		if (mnl_attr_get_type(attr) == CTRL_ATTR_POLICY_DO) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
			dst->do_present = 1;
			dst->do_ = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == CTRL_ATTR_POLICY_DUMP) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
			dst->dump_present = 1;
			dst->dump = mnl_attr_get_u32(attr);
		}
	}

	return 0;
}

/* ============== CTRL_CMD_GETFAMILY ============== */
// CTRL_CMD_GETFAMILY - do
void nlctrl_getfamily_rsp_free(struct nlctrl_getfamily_rsp *rsp)
{
	free(rsp->ops);
	free(rsp->mcast_groups);
	free(rsp);
}

int nlctrl_getfamily_rsp_parse(const struct nlmsghdr *nlh, void *data)
{
	const struct nlattr *attr_mcast_groups;
	struct ynl_parse_arg *yarg = data;
	struct nlctrl_getfamily_rsp *dst;
	const struct nlattr *attr_ops;
	const struct nlattr *attr;
	struct ynl_parse_arg parg;
	int i;

	dst = yarg->data;
	parg.ys = yarg->ys;

	mnl_attr_for_each(attr, nlh, sizeof(struct genlmsghdr)) {
		if (mnl_attr_get_type(attr) == CTRL_ATTR_FAMILY_ID) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
			dst->family_id_present = 1;
			dst->family_id = mnl_attr_get_u16(attr);
		}
		if (mnl_attr_get_type(attr) == CTRL_ATTR_FAMILY_NAME) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
			dst->family_name_present = 1;
			strncpy(dst->family_name, mnl_attr_get_str(attr), GENL_NAMSIZ - 1);
			dst->family_name[GENL_NAMSIZ - 1] = 0;
		}
		if (mnl_attr_get_type(attr) == CTRL_ATTR_VERSION) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
			dst->version_present = 1;
			dst->version = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == CTRL_ATTR_HDRSIZE) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
			dst->hdrsize_present = 1;
			dst->hdrsize = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == CTRL_ATTR_MAXATTR) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
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
		parg.rsp_policy = &nlctrl_mcast_group_nest;
		mnl_attr_for_each_nested(attr, attr_mcast_groups) {
			parg.data = &dst->mcast_groups[i];
			nlctrl_mcast_group_parse(&parg, attr, mnl_attr_get_type(attr));
			i++;
		}
	}
	if (dst->n_ops) {
		dst->ops = calloc(dst->n_ops, sizeof(*dst->ops));
		i = 0;
		parg.rsp_policy = &nlctrl_operation_nest;
		mnl_attr_for_each_nested(attr, attr_ops) {
			parg.data = &dst->ops[i];
			nlctrl_operation_parse(&parg, attr, mnl_attr_get_type(attr));
			i++;
		}
	}

	return MNL_CB_OK;
}

struct nlctrl_getfamily_rsp *
nlctrl_getfamily(struct ynl_sock *ys, struct nlctrl_getfamily_req *req)
{
	struct ynl_parse_arg yarg = { .ys = ys, };
	struct nlctrl_getfamily_rsp *rsp;
	struct nlmsghdr *nlh;
	int len, err;

	nlh = ynl_gemsg_start_req(ys, ys->family_id, CTRL_CMD_GETFAMILY, 1);
	ys->req_policy = &nlctrl_main_nest;

	if (req->family_id_present)
		mnl_attr_put_u16(nlh, CTRL_ATTR_FAMILY_ID, req->family_id);
	if (req->family_name_present)
		mnl_attr_put_strz(nlh, CTRL_ATTR_FAMILY_NAME, req->family_name);

	err = mnl_socket_sendto(ys->sock, nlh, nlh->nlmsg_len);
	if (err < 0)
		return NULL;

	len = mnl_socket_recvfrom(ys->sock, ys->rx_buf, MNL_SOCKET_BUFFER_SIZE);
	if (len < 0)
		return NULL;

	rsp = calloc(1, sizeof(*rsp));
	yarg.data = rsp;

	err = mnl_cb_run2(ys->rx_buf, len, ys->seq, ys->portid,
			 nlctrl_getfamily_rsp_parse, &yarg,
			 ynl_cb_array, NLMSG_MIN_TYPE);
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

// CTRL_CMD_GETFAMILY - dump
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

	yds.ys = ys;
	yds.alloc_sz = sizeof(*rsp);
	yds.cb = nlctrl_getfamily_rsp_parse;

	nlh = ynl_gemsg_start_dump(ys, ys->family_id, CTRL_CMD_GETFAMILY, 1);

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
		nlctrl_getfamily_list_free(cur);
	}
	return NULL;
}

// CTRL_CMD_GETFAMILY - notify
void nlctrl_getfamily_ntf_free(struct nlctrl_getfamily_ntf *rsp)
{
	free(rsp->obj.ops);
	free(rsp->obj.mcast_groups);
	free(rsp);
}

/* ============== CTRL_CMD_GETPOLICY ============== */
// CTRL_CMD_GETPOLICY - dump
int nlctrl_getpolicy_rsp_dump_parse(const struct nlmsghdr *nlh, void *data)
{
	struct nlctrl_getpolicy_rsp_dump *dst;
	struct ynl_parse_arg *yarg = data;
	const struct nlattr *attr;
	struct ynl_parse_arg parg;

	dst = yarg->data;
	parg.ys = yarg->ys;

	mnl_attr_for_each(attr, nlh, sizeof(struct genlmsghdr)) {
		if (mnl_attr_get_type(attr) == CTRL_ATTR_FAMILY_ID) {
			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
			dst->family_id_present = 1;
			dst->family_id = mnl_attr_get_u16(attr);
		}
		if (mnl_attr_get_type(attr) == CTRL_ATTR_OP_POLICY) {
			const struct nlattr *attr_cmd;
			__u32 cmd;

			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
			dst->op_policy_present = 1;

			parg.rsp_policy = &nlctrl_policy_nest;
			parg.data = &dst->op_policy;
			attr_cmd = mnl_attr_get_payload(attr);
			cmd = mnl_attr_get_type(attr_cmd);
			nlctrl_policy_parse(&parg, attr_cmd, cmd);
		}
		if (mnl_attr_get_type(attr) == CTRL_ATTR_POLICY) {
			const struct nlattr *attr_current_policy_idx, *attr_attr_idx;
			__u32 current_policy_idx, attr_idx;

			if (ynl_attr_validate(yarg, attr))
				return MNL_CB_ERROR;
			dst->policy_present = 1;

			parg.rsp_policy = &nlctrl_nl_policy_nest;
			parg.data = &dst->policy;
			attr_current_policy_idx = mnl_attr_get_payload(attr);
			current_policy_idx = mnl_attr_get_type(attr_current_policy_idx);
			attr_attr_idx = mnl_attr_get_payload(attr_current_policy_idx);
			attr_idx = mnl_attr_get_type(attr_attr_idx);
			nlctrl_nl_policy_parse(&parg, attr_attr_idx, current_policy_idx, attr_idx);
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

	yds.ys = ys;
	yds.alloc_sz = sizeof(*rsp);
	yds.cb = nlctrl_getpolicy_rsp_dump_parse;

	nlh = ynl_gemsg_start_dump(ys, ys->family_id, CTRL_CMD_GETPOLICY, 1);
	ys->req_policy = &nlctrl_main_nest;

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
		nlctrl_getpolicy_rsp_list_free(cur);
	}
	return NULL;
}

// --------------- Common notification parsing --------------- //
struct ynl_ntf_base_type *nlctrl_ntf_parse(struct ynl_sock *ys)
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
	case CTRL_CMD_NEWFAMILY:
	case CTRL_CMD_DELFAMILY:
	case CTRL_CMD_NEWMCAST_GRP:
	case CTRL_CMD_DELMCAST_GRP:
		rsp = calloc(1, sizeof(struct nlctrl_getfamily_ntf));
		parse = nlctrl_getfamily_rsp_parse;
		break;
	default:
		return NULL;
	}

	yarg.data = rsp;

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
