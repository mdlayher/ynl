// SPDX-License-Identifier: MIT
// Do not edit directly, auto-generated from:
//	Documentation/netlink/bindings/ethtool.yaml
// ./gen.py --mode kernel --user-header ethtool-user.h genetlink-user.h user.h linux/if.h --header --spec Documentation/netlink/bindings/ethtool.yaml

#include <net/netlink.h>

#include <linux/ethtool_netlink.h>

/* ============== ETHTOOL_MSG_CHANNELS_GET ============== */
// ETHTOOL_MSG_CHANNELS_GET - do
struct ethtool_channels_get_req {
	u32 header_present:1;

	struct ethtool_header header;
};

void ethtool_channels_get_req_free(struct ethtool_channels_get_req *req);

struct ethtool_channels_get_rsp {
	u32 header_present:1;
	u32 rx_max_present:1;
	u32 tx_max_present:1;
	u32 other_max_present:1;
	u32 combined_max_present:1;
	u32 rx_count_present:1;
	u32 tx_count_present:1;
	u32 other_count_present:1;
	u32 combined_count_present:1;

	struct ethtool_header header;
	u32 rx_max;
	u32 tx_max;
	u32 other_max;
	u32 combined_max;
	u32 rx_count;
	u32 tx_count;
	u32 other_count;
	u32 combined_count;
};

void ethtool_channels_get_rsp_free(struct ethtool_channels_get_rsp *rsp);

void
ethtool_channels_get_req_parse(const struct nlattr **tb,
			       struct ethtool_channels_get_req *req);
const struct nla_policy ethtool_channels_get_policy[];

// ETHTOOL_MSG_CHANNELS_GET - dump
// ETHTOOL_MSG_CHANNELS_GET - notify
/* ============== ETHTOOL_MSG_CHANNELS_SET ============== */
// ETHTOOL_MSG_CHANNELS_SET - do
struct ethtool_channels_set_req {
	u32 header_present:1;
	u32 rx_count_present:1;
	u32 tx_count_present:1;
	u32 other_count_present:1;
	u32 combined_count_present:1;

	struct ethtool_header header;
	u32 rx_count;
	u32 tx_count;
	u32 other_count;
	u32 combined_count;
};

void ethtool_channels_set_req_free(struct ethtool_channels_set_req *req);

void
ethtool_channels_set_req_parse(const struct nlattr **tb,
			       struct ethtool_channels_set_req *req);
const struct nla_policy ethtool_channels_set_policy[];
