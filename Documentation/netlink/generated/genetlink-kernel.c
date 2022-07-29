// SPDX-License-Identifier: MIT
// Do not edit directly, auto-generated from:
//	Documentation/netlink/bindings/genetlink.yaml
// ./gen.py --mode kernel --user-header genetlink-user.h user.h --source --spec Documentation/netlink/bindings/genetlink.yaml

#include <net/netlink.h>

#include <linux/genetlink.h>

// CTRL_CMD_GETFAMILY
void
genlctrl_getfamily_req_parse(const struct nlattr **tb,
			     struct genlctrl_getfamily_req *req)
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
