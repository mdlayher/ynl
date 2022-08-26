// SPDX-License-Identifier: BSD-3-Clause
// Do not edit directly, auto-generated from:
//	Documentation/netlink/specs/genetlink.yaml
// YNL-GEN kernel source

#include <net/netlink.h>

#include <linux/genetlink.h>

// CTRL_CMD_GETFAMILY - do
const struct nla_policy nlctrl_getfamily_policy[CTRL_ATTR_FAMILY_NAME + 1] = {
	[CTRL_ATTR_FAMILY_ID] = { .type = NLA_U16 },
	[CTRL_ATTR_FAMILY_NAME] = { .type = NLA_NUL_STRING, .len = GENL_NAMSIZ - 1 },
};

// CTRL_CMD_GETPOLICY - dump
const struct nla_policy nlctrl_getpolicy_policy[CTRL_ATTR_OP + 1] = {
	[CTRL_ATTR_FAMILY_ID] = { .type = NLA_U16 },
	[CTRL_ATTR_FAMILY_NAME] = { .type = NLA_NUL_STRING, .len = GENL_NAMSIZ - 1 },
	[CTRL_ATTR_OP] = { .type = NLA_U32 },
};

// Ops table for nlctrl
const struct genl_ops nlctrl_ops[2] = {
	{
		.cmd = CTRL_CMD_GETFAMILY,
		.validate = GENL_DONT_VALIDATE_STRICT | GENL_DONT_VALIDATE_DUMP,
		.doit = nlctrl_getfamily_doit,
		.dumpit = nlctrl_getfamily_dumpit,
		.policy = nlctrl_getfamily_policy,
	},
	{
		.cmd = CTRL_CMD_GETPOLICY,
		.dumpit = nlctrl_getpolicy_dumpit,
		.policy = nlctrl_getpolicy_policy,
	},
};
