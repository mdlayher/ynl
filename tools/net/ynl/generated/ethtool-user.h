// SPDX-License-Identifier: BSD-3-Clause
// Do not edit directly, auto-generated from:
//	Documentation/netlink/bindings/ethtool.yaml
// YNL-GEN user header

#ifndef _LINUX_ETHTOOL_GEN_H
#define _LINUX_ETHTOOL_GEN_H

#include <linux/ethtool_netlink.h>
#include <linux/if.h>

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
static inline void
ethtool_channels_get_req_set_header_dev_index(struct ethtool_channels_get_req *req,
					      __u32 dev_index)
{
	req->header_present = 1;
	req->header.dev_index_present = 1;
	req->header.dev_index = dev_index;
}
static inline void
ethtool_channels_get_req_set_header_dev_name(struct ethtool_channels_get_req *req,
					     const char *dev_name)
{
	req->header_present = 1;
	req->header.dev_name_present = 1;
	strncpy(req->header.dev_name, dev_name, sizeof(req->header.dev_name));
	req->header.dev_name[ALTIFNAMSIZ - 1] = 0;
}
static inline void
ethtool_channels_get_req_set_header_flags(struct ethtool_channels_get_req *req,
					  __u32 flags)
{
	req->header_present = 1;
	req->header.flags_present = 1;
	req->header.flags = flags;
}

struct ethtool_channels_get_rsp {
	__u32 header_present:1;
	__u32 rx_max_present:1;
	__u32 tx_max_present:1;
	__u32 other_max_present:1;
	__u32 combined_max_present:1;
	__u32 rx_count_present:1;
	__u32 tx_count_present:1;
	__u32 other_count_present:1;
	__u32 combined_count_present:1;

	struct ethtool_header header;
	__u32 rx_max;
	__u32 tx_max;
	__u32 other_max;
	__u32 combined_max;
	__u32 rx_count;
	__u32 tx_count;
	__u32 other_count;
	__u32 combined_count;
};

void ethtool_channels_get_rsp_free(struct ethtool_channels_get_rsp *rsp);

/*
 * Get current and max supported number of channels.
 */
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
	__u32 rx_count_present:1;
	__u32 tx_count_present:1;
	__u32 other_count_present:1;
	__u32 combined_count_present:1;

	struct ethtool_header header;
	__u32 rx_count;
	__u32 tx_count;
	__u32 other_count;
	__u32 combined_count;
};

void ethtool_channels_set_req_free(struct ethtool_channels_set_req *req);
static inline void
ethtool_channels_set_req_set_header_dev_index(struct ethtool_channels_set_req *req,
					      __u32 dev_index)
{
	req->header_present = 1;
	req->header.dev_index_present = 1;
	req->header.dev_index = dev_index;
}
static inline void
ethtool_channels_set_req_set_header_dev_name(struct ethtool_channels_set_req *req,
					     const char *dev_name)
{
	req->header_present = 1;
	req->header.dev_name_present = 1;
	strncpy(req->header.dev_name, dev_name, sizeof(req->header.dev_name));
	req->header.dev_name[ALTIFNAMSIZ - 1] = 0;
}
static inline void
ethtool_channels_set_req_set_header_flags(struct ethtool_channels_set_req *req,
					  __u32 flags)
{
	req->header_present = 1;
	req->header.flags_present = 1;
	req->header.flags = flags;
}
static inline void
ethtool_channels_set_req_set_rx_count(struct ethtool_channels_set_req *req,
				      __u32 rx_count)
{
	req->rx_count_present = 1;
	req->rx_count = rx_count;
}
static inline void
ethtool_channels_set_req_set_tx_count(struct ethtool_channels_set_req *req,
				      __u32 tx_count)
{
	req->tx_count_present = 1;
	req->tx_count = tx_count;
}
static inline void
ethtool_channels_set_req_set_other_count(struct ethtool_channels_set_req *req,
					 __u32 other_count)
{
	req->other_count_present = 1;
	req->other_count = other_count;
}
static inline void
ethtool_channels_set_req_set_combined_count(struct ethtool_channels_set_req *req,
					    __u32 combined_count)
{
	req->combined_count_present = 1;
	req->combined_count = combined_count;
}

/*
 * Set number of channels.
 */
int ethtool_channels_set(struct ynl_sock *ys,
			 struct ethtool_channels_set_req *req);

// --------------- Common notification parsing --------------- //
struct ynl_ntf_base_type *ethtool_ntf_parse(struct ynl_sock *ys);

#endif /* _LINUX_ETHTOOL_GEN_H */
