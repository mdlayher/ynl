// SPDX-License-Identifier: MIT
// Do not edit directly, auto-generated from:
//	../../../../Documentation/netlink/bindings/ethtool.yaml
// /home/kicinski/devel/linux/gen.py --mode kernel --header --spec ../../../../Documentation/netlink/bindings/ethtool.yaml

#ifndef _LINUX_ETHTOOL_GEN_H
#define _LINUX_ETHTOOL_GEN_H

#include <net/netlink.h>

#include <linux/ethtool_netlink.h>

// ETHTOOL_MSG_CHANNELS_GET - do
extern const struct nla_policy ethtool_channels_get_policy[ETHTOOL_A_CHANNELS_HEADER + 1];

// ETHTOOL_MSG_CHANNELS_GET - dump
// ETHTOOL_MSG_CHANNELS_GET - notify
// ETHTOOL_MSG_CHANNELS_SET - do
extern const struct nla_policy ethtool_channels_set_policy[ETHTOOL_A_CHANNELS_COMBINED_COUNT + 1];

#endif /* _LINUX_ETHTOOL_GEN_H */
