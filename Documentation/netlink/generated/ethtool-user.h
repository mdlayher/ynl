// SPDX-License-Identifier: MIT
// Do not edit directly, auto-generated from:
//	Documentation/netlink/bindings/ethtool.yaml
// ./gen.py --mode user --user-header genetlink-user.h user.h --header --spec Documentation/netlink/bindings/ethtool.yaml

#include <linux/ethtool_netlink.h>

struct ynl_sock;

// Common nested types
struct ethtool_header {
	__u32 dev_index_present:1;
	__u32 dev_name_present:1;
	__u32 flags_present:1;

	__u32 dev_index;
	char dev_name[ALTIFNAMSIZ];
	__u32 flags;
};

/* ============== ETHTOOL_MSG_CHANNELS_GET ============== */
// ETHTOOL_MSG_CHANNELS_GET - do
struct ethtool_channels_get_req {
	__u32 header_present:1;
