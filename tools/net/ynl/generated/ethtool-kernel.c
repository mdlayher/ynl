// SPDX-License-Identifier: MIT
// Do not edit directly, auto-generated from:
//	../../../../Documentation/netlink/bindings/ethtool.yaml
// /home/kicinski/devel/linux/gen.py --mode kernel --source --spec ../../../../Documentation/netlink/bindings/ethtool.yaml

#include <net/netlink.h>

#include <linux/ethtool_netlink.h>

// ETHTOOL_MSG_CHANNELS_GET - do
const struct nla_policy ethtool_channels_get_policy[] = {
	[ETHTOOL_A_CHANNELS_HEADER] = { .type = NLA_NEST },
};

// ETHTOOL_MSG_CHANNELS_GET - dump
// ETHTOOL_MSG_CHANNELS_GET - notify
// ETHTOOL_MSG_CHANNELS_SET - do
const struct nla_policy ethtool_channels_set_policy[] = {
	[ETHTOOL_A_CHANNELS_HEADER] = { .type = NLA_NEST },
	[ETHTOOL_A_CHANNELS_RX_COUNT] = { .type = NLA_U32 },
	[ETHTOOL_A_CHANNELS_TX_COUNT] = { .type = NLA_U32 },
	[ETHTOOL_A_CHANNELS_OTHER_COUNT] = { .type = NLA_U32 },
	[ETHTOOL_A_CHANNELS_COMBINED_COUNT] = { .type = NLA_U32 },
};
