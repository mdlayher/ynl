// SPDX-License-Identifier: MIT
// Do not edit directly, auto-generated from:
//	Documentation/netlink/bindings/genetlink.yaml

#include <net/netlink.h>

#include <linux/genetlink.h>

// Codegen for CTRL_CMD_GETFAMILY
struct genlctrl_getfamily_req {
	u32 family_id_present:1;
	u32 family_name_present:1;

	u16 family_id;
	char family_name[GENL_NAMSIZ];
};
void genlctrl_getfamily_req_free(struct genlctrl_getfamily_rsp *getfamily);

void genlctrl_getfamily_req_parse(const struct nlattr **tb, struct genlctrl_getfamily_req *req)
{
	if (tb[CTRL_ATTR_FAMILY_ID]) {
		req->family_id_present = 1;
		req->family_id = nla_get_u16(tb[CTRL_ATTR_FAMILY_ID]);
	}
}

static const struct nla_policy genlctrl_getfamily_policy[] = {
	[CTRL_ATTR_FAMILY_ID] = { .type = NLA_U16 },
	[CTRL_ATTR_FAMILY_NAME] = { .type = NLA_NUL_STRING, .len = GENL_NAMSIZ - 1 },
};

// Codegen for CTRL_CMD_GETPOLICY
