// SPDX-License-Identifier: MIT
// Do not edit directly, auto-generated from:
//	Documentation/netlink/bindings/genetlink.yaml/
// /home/kicinski/devel/linux/gen.py --mode kernel --source --spec ../../../../Documentation/netlink/bindings/genetlink.yaml

#include <net/netlink.h>

#include <linux/genetlink.h>

// CTRL_CMD_GETFAMILY - do
const struct nla_policy nlctrl_getfamily_policy[CTRL_ATTR_FAMILY_NAME + 1] = {
	[CTRL_ATTR_FAMILY_ID] = { .type = NLA_U16 },
	[CTRL_ATTR_FAMILY_NAME] = { .type = NLA_NUL_STRING, .len = GENL_NAMSIZ - 1 },
};

// CTRL_CMD_GETFAMILY - dump
// CTRL_CMD_GETFAMILY - notify
// CTRL_CMD_GETPOLICY - dump
