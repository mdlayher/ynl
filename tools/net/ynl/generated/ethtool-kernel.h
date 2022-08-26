// SPDX-License-Identifier: BSD-3-Clause
// Do not edit directly, auto-generated from:
//	Documentation/netlink/specs/ethtool.yaml
// YNL-GEN kernel header

#ifndef _LINUX_ETHTOOL_GEN_H
#define _LINUX_ETHTOOL_GEN_H

#include <net/netlink.h>

#include <linux/ethtool_netlink.h>
#include <linux/if.h>

// ETHTOOL_MSG_CHANNELS_GET - do
extern const struct nla_policy ethtool_channels_get_policy[ETHTOOL_A_CHANNELS_HEADER + 1];

// ETHTOOL_MSG_CHANNELS_SET - do
extern const struct nla_policy ethtool_channels_set_policy[ETHTOOL_A_CHANNELS_COMBINED_COUNT + 1];

// Ops table for ethtool
extern const struct genl_ops ethtool_ops[];
#endif /* _LINUX_ETHTOOL_GEN_H */
