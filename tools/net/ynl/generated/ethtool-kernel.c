// SPDX-License-Identifier: BSD-3-Clause
// Do not edit directly, auto-generated from:
//	Documentation/netlink/specs/ethtool.yaml
// YNL-GEN kernel source

#include <net/netlink.h>

#include <linux/ethtool_netlink.h>
#include <linux/if.h>

// ETHTOOL_MSG_CHANNELS_GET - do
const struct nla_policy ethtool_channels_get_policy[ETHTOOL_A_CHANNELS_HEADER + 1] = {
	[ETHTOOL_A_CHANNELS_HEADER] = { .type = NLA_NEST },
};

// ETHTOOL_MSG_CHANNELS_SET - do
const struct nla_policy ethtool_channels_set_policy[ETHTOOL_A_CHANNELS_COMBINED_COUNT + 1] = {
	[ETHTOOL_A_CHANNELS_HEADER] = { .type = NLA_NEST },
	[ETHTOOL_A_CHANNELS_RX_COUNT] = { .type = NLA_U32 },
	[ETHTOOL_A_CHANNELS_TX_COUNT] = { .type = NLA_U32 },
	[ETHTOOL_A_CHANNELS_OTHER_COUNT] = { .type = NLA_U32 },
	[ETHTOOL_A_CHANNELS_COMBINED_COUNT] = { .type = NLA_U32 },
};

// Ops table for ethtool
const struct genl_ops ethtool_ops[] = {
	{
		.cmd = ETHTOOL_MSG_CHANNELS_GET,
	},
	{
		.cmd = ETHTOOL_MSG_CHANNELS_SET,
	},
};
