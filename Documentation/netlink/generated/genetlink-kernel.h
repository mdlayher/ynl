// SPDX-License-Identifier: MIT
// Do not edit directly, auto-generated from:
//	Documentation/netlink/bindings/genetlink.yaml
// ./gen.py --mode kernel --user-header genetlink-user.h user.h --header --spec Documentation/netlink/bindings/genetlink.yaml

#include <net/netlink.h>

#include <linux/genetlink.h>

// CTRL_CMD_GETFAMILY
struct nlctrl_getfamily_req {
	u32 family_id_present:1;
	u32 family_name_present:1;

	u16 family_id;
	char family_name[GENL_NAMSIZ];
};

void nlctrl_getfamily_req_free(struct nlctrl_getfamily_req *req);

struct nlctrl_getfamily_rsp {
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
	struct nlctrl_operation *ops;
};

void nlctrl_getfamily_rsp_free(struct nlctrl_getfamily_rsp *req);

void
nlctrl_getfamily_req_parse(const struct nlattr **tb,
			   struct nlctrl_getfamily_req *req);
const struct nla_policy nlctrl_getfamily_policy[];

// CTRL_CMD_GETPOLICY
