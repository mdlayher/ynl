// SPDX-License-Identifier: MIT
// Do not edit directly, auto-generated from:
//	Documentation/netlink/bindings/fou.yaml
// ./gen.py --mode user --user-header genetlink-user.h user.h --header --spec Documentation/netlink/bindings/fou.yaml

#include <linux/fou.h>

struct ynl_sock;

// Common nested types
/* ============== FOU_CMD_ADD ============== */
// FOU_CMD_ADD - do
struct fou_add_req {
	__u32 port_present:1;
	__u32 ipproto_present:1;
	__u32 type_present:1;
	__u32 remcsum_nopartial_present:1;
	__u32 local_v4_present:1;
	__u32 peer_v4_present:1;
	__u32 local_v6_present:1;
	__u32 peer_v6_present:1;
	__u32 peer_port_present:1;
	__u32 ifindex_present:1;

	__u16 port;
	__u8 ipproto;
	__u8 type;
