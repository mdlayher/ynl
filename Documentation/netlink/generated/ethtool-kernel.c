// SPDX-License-Identifier: MIT
// Do not edit directly, auto-generated from:
//	Documentation/netlink/bindings/ethtool.yaml
// ./gen.py --mode kernel --user-header genetlink-user.h user.h --source --spec Documentation/netlink/bindings/ethtool.yaml

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
	if (tb[ETHTOOL_A_CHANNELS_RX]) {
		req->rx_present = 1;
		req->rx = nla_get_u32(tb[ETHTOOL_A_CHANNELS_RX]);
	}
	if (tb[ETHTOOL_A_CHANNELS_TX]) {
		req->tx_present = 1;
		req->tx = nla_get_u32(tb[ETHTOOL_A_CHANNELS_TX]);
	}
	if (tb[ETHTOOL_A_CHANNELS_OTHER]) {
		req->other_present = 1;
		req->other = nla_get_u32(tb[ETHTOOL_A_CHANNELS_OTHER]);
	}
	if (tb[ETHTOOL_A_CHANNELS_COMBINED]) {
		req->combined_present = 1;
		req->combined = nla_get_u32(tb[ETHTOOL_A_CHANNELS_COMBINED]);
	}
}

const struct nla_policy ethtool_channels_set_policy[] = {
	[ETHTOOL_A_CHANNELS_HEADER] = { .type = NLA_NEST },
	[ETHTOOL_A_CHANNELS_RX] = { .type = NLA_U32 },
	[ETHTOOL_A_CHANNELS_TX] = { .type = NLA_U32 },
	[ETHTOOL_A_CHANNELS_OTHER] = { .type = NLA_U32 },
	[ETHTOOL_A_CHANNELS_COMBINED] = { .type = NLA_U32 },
};
