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

	struct ethtool_header header;
};

void ethtool_channels_get_req_free(struct ethtool_channels_get_req *req);

struct ethtool_channels_get_rsp {
	__u32 header_present:1;
	__u32 rx_max_present:1;
	__u32 tx_max_present:1;
	__u32 other_max_present:1;
	__u32 combined_max_present:1;
	__u32 rx_present:1;
	__u32 tx_present:1;
	__u32 other_present:1;
	__u32 combined_present:1;

	struct ethtool_header header;
	__u32 rx_max;
	__u32 tx_max;
	__u32 other_max;
	__u32 combined_max;
	__u32 rx;
	__u32 tx;
	__u32 other;
	__u32 combined;
};

void ethtool_channels_get_rsp_free(struct ethtool_channels_get_rsp *rsp);

struct ethtool_channels_get_rsp *
ethtool_channels_get(struct ynl_sock *ys, struct ethtool_channels_get_req *req);

// ETHTOOL_MSG_CHANNELS_GET - dump
struct ethtool_channels_get_list {
	struct ethtool_channels_get_list *next;
	struct ethtool_channels_get_rsp obj;
};

void ethtool_channels_get_list_free(struct ethtool_channels_get_list *rsp);

struct ethtool_channels_get_list *
ethtool_channels_get_dump(struct ynl_sock *ys);

// ETHTOOL_MSG_CHANNELS_GET - notify
struct ethtool_channels_get_ntf {
	__u16 family;
	__u8 cmd;
	struct ethtool_channels_get_rsp obj;
};

void ethtool_channels_get_ntf_free(struct ethtool_channels_get_ntf *rsp);

/* ============== ETHTOOL_MSG_CHANNELS_SET ============== */
// ETHTOOL_MSG_CHANNELS_SET - do
struct ethtool_channels_set_req {
	__u32 header_present:1;
	__u32 rx_present:1;
	__u32 tx_present:1;
	__u32 other_present:1;
	__u32 combined_present:1;

	struct ethtool_header header;
	__u32 rx;
	__u32 tx;
	__u32 other;
	__u32 combined;
};

void ethtool_channels_set_req_free(struct ethtool_channels_set_req *req);
static inline void
ethtool_channels_set_req_set_rx(struct ethtool_channels_set_req *req, __u32 rx)
{
	req->rx_present = 1;
	req->rx = rx;
}
static inline void
ethtool_channels_set_req_set_tx(struct ethtool_channels_set_req *req, __u32 tx)
{
	req->tx_present = 1;
	req->tx = tx;
}
static inline void
ethtool_channels_set_req_set_other(struct ethtool_channels_set_req *req,
				   __u32 other)
{
	req->other_present = 1;
	req->other = other;
}
static inline void
ethtool_channels_set_req_set_combined(struct ethtool_channels_set_req *req,
				      __u32 combined)
{
	req->combined_present = 1;
	req->combined = combined;
}

int ethtool_channels_set(struct ynl_sock *ys,
			 struct ethtool_channels_set_req *req);

// --------------- Common notification parsing --------------- //
struct ynl_ntf_base_type *ethtool_ntf_parse(struct ynl_sock *ys);
