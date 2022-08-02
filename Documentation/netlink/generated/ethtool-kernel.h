// SPDX-License-Identifier: MIT
// Do not edit directly, auto-generated from:
//	Documentation/netlink/bindings/ethtool.yaml
// ./gen.py --mode kernel --user-header genetlink-user.h user.h --header --spec Documentation/netlink/bindings/ethtool.yaml

#include <net/netlink.h>

#include <linux/ethtool_netlink.h>

/* ============== ETHTOOL_MSG_CHANNELS_GET ============== */
// ETHTOOL_MSG_CHANNELS_GET - do
struct ethtool_channels_get_req {
	u32 header_present:1;
