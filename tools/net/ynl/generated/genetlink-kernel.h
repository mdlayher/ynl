// SPDX-License-Identifier: BSD-3-Clause
// Do not edit directly, auto-generated from:
//	Documentation/netlink/specs/genetlink.yaml
// YNL-GEN kernel header

#ifndef _LINUX_NLCTRL_GEN_H
#define _LINUX_NLCTRL_GEN_H

#include <net/netlink.h>

#include <linux/genetlink.h>

// CTRL_CMD_GETFAMILY - do
extern const struct nla_policy nlctrl_getfamily_policy[CTRL_ATTR_FAMILY_NAME + 1];

// Ops table for nlctrl
extern const struct genl_ops nlctrl_ops[2];

int nlctrl_getfamily_doit(struct sk_buff *skb, struct genl_info *info);
int nlctrl_getfamily_dumpit(struct sk_buff *skb, struct netlink_callback *cb);
int nlctrl_getpolicy_dumpit(struct sk_buff *skb, struct netlink_callback *cb);

#endif /* _LINUX_NLCTRL_GEN_H */
