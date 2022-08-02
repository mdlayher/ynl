// SPDX-License-Identifier: MIT
// Do not edit directly, auto-generated from:
//	Documentation/netlink/bindings/fou.yaml
// ./gen.py --mode kernel --user-header genetlink-user.h user.h --header --spec Documentation/netlink/bindings/fou.yaml

#include <net/netlink.h>

#include <linux/fou.h>

/* ============== FOU_CMD_ADD ============== */
// FOU_CMD_ADD - do
struct fou_add_req {
	u32 port_present:1;
	u32 ipproto_present:1;
	u32 type_present:1;
	u32 remcsum_nopartial_present:1;
	u32 local_v4_present:1;
	u32 peer_v4_present:1;
	u32 local_v6_present:1;
	u32 peer_v6_present:1;
	u32 peer_port_present:1;
	u32 ifindex_present:1;

	u16 port;
	u8 ipproto;
	u8 type;
