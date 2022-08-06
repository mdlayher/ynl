// SPDX-License-Identifier: MIT
// Do not edit directly, auto-generated from:
//	../../../../Documentation/netlink/bindings/fou.yaml
// /home/kicinski/devel/linux/gen.py --mode uapi --header --spec ../../../../Documentation/netlink/bindings/fou.yaml

enum {
	FOU_ATTR_UNSPEC,
	FOU_ATTR_PORT,
	FOU_ATTR_AF,
	FOU_ATTR_IPPROTO,
	FOU_ATTR_TYPE,
	FOU_ATTR_REMCSUM_NOPARTIAL,
	FOU_ATTR_LOCAL_V4,
	FOU_ATTR_LOCAL_V6,
	FOU_ATTR_PEER_V4,
	FOU_ATTR_PEER_V6,
	FOU_ATTR_PEER_PORT,
	FOU_ATTR_IFINDEX,

	__FOU_ATTR_MAX
};
#define FOU_ATTR__MAX (__FOU_ATTR_MAX - 1)

enum {
	FOU_CMD_UNSPEC,
	FOU_CMD_ADD,
	FOU_CMD_DEL,
	FOU_CMD_GET,
};
