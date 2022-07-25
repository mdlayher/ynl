// SPDX-License-Identifier: MIT
// Do not edit directly, auto-generated from:
//	Documentation/netlink/bindings/genetlink.yaml

#include <net/netlink.h>

#include <linux/genetlink.h>

// Header content

// Common nested types
struct genlctrl_operation {
	u32 op_id_present:1;
	u32 op_flags_present:1;

	u32 op_id;
	u32 op_flags;
};
void genlctrl_operation_free(struct genlctrl_operation *);

// CTRL_CMD_GETFAMILY
struct genlctrl_getfamily_req {
	u32 family_id_present:1;
	u32 family_name_present:1;

	u16 family_id;
	char family_name[GENL_NAMSIZ];
};
void genlctrl_getfamily_req_free(struct genlctrl_getfamily_req *getfamily);
void genlctrl_getfamily_req_parse(const struct nlattr **tb, struct genlctrl_getfamily_req *req);
const struct nla_policy genlctrl_getfamily_policy[];

// CTRL_CMD_GETPOLICY
// Source content

// CTRL_CMD_GETFAMILY
void genlctrl_getfamily_req_parse(const struct nlattr **tb, struct genlctrl_getfamily_req *req)
{
	if (tb[CTRL_ATTR_FAMILY_ID]) {
		req->family_id_present = 1;
		req->family_id = nla_get_u16(tb[CTRL_ATTR_FAMILY_ID]);
	}
	if (tb[CTRL_ATTR_FAMILY_NAME]) {
		req->family_name_present = 1;
		strcpy(req->family_name, nla_data(tb[CTRL_ATTR_FAMILY_NAME]));
	}
}

const struct nla_policy genlctrl_getfamily_policy[] = {
	[CTRL_ATTR_FAMILY_ID] = { .type = NLA_U16 },
	[CTRL_ATTR_FAMILY_NAME] = { .type = NLA_NUL_STRING, .len = GENL_NAMSIZ - 1 },
};

// CTRL_CMD_GETPOLICY
