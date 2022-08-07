// SPDX-License-Identifier: MIT
// Do not edit directly, auto-generated from:
//	../../../../Documentation/netlink/bindings/genetlink.yaml
// /home/kicinski/devel/linux/gen.py --mode user --user-header ynl.h --header --spec ../../../../Documentation/netlink/bindings/genetlink.yaml

#ifndef _LINUX_NLCTRL_GEN_H
#define _LINUX_NLCTRL_GEN_H

#include <linux/genetlink.h>

struct ynl_sock;

// Common nested types
struct nlctrl_mcast_group {
	__u32 id_present:1;
	__u32 name_present:1;

	__u32 idx;
	__u32 id;
	char name[GENL_NAMSIZ];
};

struct nlctrl_nl_policy {
	__u32 type_present:1;
	__u32 min_value_u_present:1;
	__u32 max_value_u_present:1;
	__u32 min_value_s_present:1;
	__u32 max_value_s_present:1;
	__u32 mask_present:1;
	__u32 min_length_present:1;
	__u32 max_length_present:1;
	__u32 policy_idx_present:1;
	__u32 policy_maxtype_present:1;
	__u32 bitfield32_mask_present:1;

	__u32 attr_idx;
	__u32 current_policy_idx;
	__u32 type;
	__u64 min_value_u;
	__u64 max_value_u;
	__s64 min_value_s;
	__s64 max_value_s;
	__u64 mask;
	__u32 min_length;
	__u32 max_length;
	__u32 policy_idx;
	__u32 policy_maxtype;
	__u32 bitfield32_mask;
};

struct nlctrl_operation {
	__u32 id_present:1;
	__u32 flags_present:1;

	__u32 idx;
	__u32 id;
	__u32 flags;
};

struct nlctrl_policy {
	__u32 do_present:1;
	__u32 dump_present:1;

	__u32 cmd;
	__u32 do_;
	__u32 dump;
};

/* ============== CTRL_CMD_GETFAMILY ============== */
// CTRL_CMD_GETFAMILY - do
struct nlctrl_getfamily_req {
	__u32 family_id_present:1;
	__u32 family_name_present:1;

	__u16 family_id;
	char family_name[GENL_NAMSIZ];
};

void nlctrl_getfamily_req_free(struct nlctrl_getfamily_req *req);
static inline void
nlctrl_getfamily_req_set_family_id(struct nlctrl_getfamily_req *req,
				   __u16 family_id)
{
	req->family_id_present = 1;
	req->family_id = family_id;
}
static inline void
nlctrl_getfamily_req_set_family_name(struct nlctrl_getfamily_req *req,
				     const char *family_name)
{
	req->family_name_present = 1;
	strncpy(req->family_name, family_name, sizeof(req->family_name));
	req->family_name[GENL_NAMSIZ - 1] = 0;
}

struct nlctrl_getfamily_rsp {
	__u32 family_id_present:1;
	__u32 family_name_present:1;
	__u32 version_present:1;
	__u32 hdrsize_present:1;
	__u32 maxattr_present:1;

	__u16 family_id;
	char family_name[GENL_NAMSIZ];
	__u32 version;
	__u32 hdrsize;
	__u32 maxattr;
	unsigned int n_ops;
	struct nlctrl_operation *ops;
	unsigned int n_mcast_groups;
	struct nlctrl_mcast_group *mcast_groups;
};

void nlctrl_getfamily_rsp_free(struct nlctrl_getfamily_rsp *rsp);

struct nlctrl_getfamily_rsp *
nlctrl_getfamily(struct ynl_sock *ys, struct nlctrl_getfamily_req *req);

// CTRL_CMD_GETFAMILY - dump
struct nlctrl_getfamily_list {
	struct nlctrl_getfamily_list *next;
	struct nlctrl_getfamily_rsp obj;
};

void nlctrl_getfamily_list_free(struct nlctrl_getfamily_list *rsp);

struct nlctrl_getfamily_list *nlctrl_getfamily_dump(struct ynl_sock *ys);

// CTRL_CMD_GETFAMILY - notify
struct nlctrl_getfamily_ntf {
	__u16 family;
	__u8 cmd;
	struct nlctrl_getfamily_rsp obj;
};

void nlctrl_getfamily_ntf_free(struct nlctrl_getfamily_ntf *rsp);

/* ============== CTRL_CMD_GETPOLICY ============== */
// CTRL_CMD_GETPOLICY - dump
struct nlctrl_getpolicy_req_dump {
	__u32 family_id_present:1;
	__u32 family_name_present:1;
	__u32 op_present:1;

	__u16 family_id;
	char family_name[GENL_NAMSIZ];
	__u32 op;
};

void nlctrl_getpolicy_req_dump_free(struct nlctrl_getpolicy_req_dump *req);
static inline void
nlctrl_getpolicy_req_dump_set_family_id(struct nlctrl_getpolicy_req_dump *req,
					__u16 family_id)
{
	req->family_id_present = 1;
	req->family_id = family_id;
}
static inline void
nlctrl_getpolicy_req_dump_set_family_name(struct nlctrl_getpolicy_req_dump *req,
					  const char *family_name)
{
	req->family_name_present = 1;
	strncpy(req->family_name, family_name, sizeof(req->family_name));
	req->family_name[GENL_NAMSIZ - 1] = 0;
}
static inline void
nlctrl_getpolicy_req_dump_set_op(struct nlctrl_getpolicy_req_dump *req,
				 __u32 op)
{
	req->op_present = 1;
	req->op = op;
}

struct nlctrl_getpolicy_rsp_dump {
	__u32 family_id_present:1;
	__u32 op_policy_present:1;
	__u32 policy_present:1;

	__u16 family_id;
	struct nlctrl_policy op_policy;
	struct nlctrl_nl_policy policy;
};

struct nlctrl_getpolicy_rsp_list {
	struct nlctrl_getpolicy_rsp_list *next;
	struct nlctrl_getpolicy_rsp_dump obj;
};

void nlctrl_getpolicy_rsp_list_free(struct nlctrl_getpolicy_rsp_list *rsp);

struct nlctrl_getpolicy_rsp_list *
nlctrl_getpolicy_dump(struct ynl_sock *ys,
		      struct nlctrl_getpolicy_req_dump *req);

// --------------- Common notification parsing --------------- //
struct ynl_ntf_base_type *nlctrl_ntf_parse(struct ynl_sock *ys);

#endif /* _LINUX_NLCTRL_GEN_H */
