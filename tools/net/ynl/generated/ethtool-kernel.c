// SPDX-License-Identifier: MIT
// Do not edit directly, auto-generated from:
//	../../../../Documentation/netlink/bindings/ethtool.yaml
// /home/kicinski/devel/linux/gen.py --mode kernel --source --spec ../../../../Documentation/netlink/bindings/ethtool.yaml

#include <net/netlink.h>

#include <linux/ethtool_netlink.h>

/* ============== ETHTOOL_MSG_CHANNELS_GET ============== */
// ETHTOOL_MSG_CHANNELS_GET - do
void
ethtool_channels_get_req_parse(const struct nlattr **tb,
			       struct ethtool_channels_get_req *req)
{
	if (tb[ETHTOOL_A_CHANNELS_HEADER]) {
		req->header_present = 1;
	}
}

const struct nla_policy ethtool_channels_get_policy[] = {
	[ETHTOOL_A_CHANNELS_HEADER] = { .type = NLA_NEST },
};

// ETHTOOL_MSG_CHANNELS_GET - dump
// ETHTOOL_MSG_CHANNELS_GET - notify
/* ============== ETHTOOL_MSG_CHANNELS_SET ============== */
// ETHTOOL_MSG_CHANNELS_SET - do
void
ethtool_channels_set_req_parse(const struct nlattr **tb,
			       struct ethtool_channels_set_req *req)
{
	if (tb[ETHTOOL_A_CHANNELS_HEADER]) {
		req->header_present = 1;
	}
	if (tb[ETHTOOL_A_CHANNELS_RX_COUNT]) {
		req->rx_count_present = 1;
		req->rx_count = nla_get_u32(tb[ETHTOOL_A_CHANNELS_RX_COUNT]);
	}
	if (tb[ETHTOOL_A_CHANNELS_TX_COUNT]) {
		req->tx_count_present = 1;
		req->tx_count = nla_get_u32(tb[ETHTOOL_A_CHANNELS_TX_COUNT]);
	}
	if (tb[ETHTOOL_A_CHANNELS_OTHER_COUNT]) {
		req->other_count_present = 1;
		req->other_count = nla_get_u32(tb[ETHTOOL_A_CHANNELS_OTHER_COUNT]);
	}
	if (tb[ETHTOOL_A_CHANNELS_COMBINED_COUNT]) {
		req->combined_count_present = 1;
		req->combined_count = nla_get_u32(tb[ETHTOOL_A_CHANNELS_COMBINED_COUNT]);
	}
}

const struct nla_policy ethtool_channels_set_policy[] = {
	[ETHTOOL_A_CHANNELS_HEADER] = { .type = NLA_NEST },
	[ETHTOOL_A_CHANNELS_RX_COUNT] = { .type = NLA_U32 },
	[ETHTOOL_A_CHANNELS_TX_COUNT] = { .type = NLA_U32 },
	[ETHTOOL_A_CHANNELS_OTHER_COUNT] = { .type = NLA_U32 },
	[ETHTOOL_A_CHANNELS_COMBINED_COUNT] = { .type = NLA_U32 },
};
