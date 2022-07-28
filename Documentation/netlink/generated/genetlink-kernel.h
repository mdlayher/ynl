// SPDX-License-Identifier: MIT
// Do not edit directly, auto-generated from:
//	Documentation/netlink/bindings/genetlink.yaml

#include <net/netlink.h>

#include <linux/genetlink.h>

// Common nested types
struct genlctrl_operation {
	u32 id_present:1;
	u32 flags_present:1;

	u32 id;
	u32 flags;
};

struct genlctrl_policy {
	u32 do_present:1;
	u32 dump_present:1;

	u32 do_;
	u32 dump;
};

struct genlctrl_nl_policy {
	u32 type_present:1;
	u32 min_value_u_present:1;
	u32 max_value_u_present:1;
	u32 min_value_s_present:1;
	u32 max_value_s_present:1;
	u32 mask_present:1;
	u32 min_length_present:1;
	u32 max_length_present:1;
	u32 policy_idx_present:1;
	u32 policy_maxtype_present:1;
	u32 bitfiled32_mask_present:1;

	u32 type;
	u64 min_value_u;
	u64 max_value_u;
	s64 min_value_s;
	s64 max_value_s;
	u64 mask;
	u32 min_length;
	u32 max_length;
	u32 policy_idx;
	u32 policy_maxtype;
	u32 bitfiled32_mask;
};

// CTRL_CMD_GETFAMILY
struct genlctrl_getfamily_req {
	u32 family_id_present:1;
	u32 family_name_present:1;

	u16 family_id;
	char family_name[GENL_NAMSIZ];
};

void genlctrl_getfamily_req_free(struct genlctrl_getfamily_req *req);

struct genlctrl_getfamily_rsp {
	u32 family_id_present:1;
	u32 family_name_present:1;
	u32 version_present:1;
	u32 hdrsize_present:1;
	u32 maxattr_present:1;

	u16 family_id;
	char family_name[GENL_NAMSIZ];
	u32 version;
	u32 hdrsize;
	u32 maxattr;
	unsigned int n_ops;
	struct genlctrl_operation *ops;
};

void genlctrl_getfamily_rsp_free(struct genlctrl_getfamily_rsp *req);

void genlctrl_getfamily_req_parse(const struct nlattr **tb, struct genlctrl_getfamily_req *req);
const struct nla_policy genlctrl_getfamily_policy[];

// CTRL_CMD_GETPOLICY
