// SPDX-License-Identifier: BSD-3-Clause
// Do not edit directly, auto-generated from:
//	Documentation/netlink/specs/fou.yaml
// YNL-GEN kernel source

#include <net/netlink.h>

#include <linux/fou.h>

// Global operation policy for fou
const struct nla_policy fou_policy[FOU_ATTR_IFINDEX + 1] = {
	[FOU_ATTR_UNSPEC] = { .type = NLA_UNUSED },
	[FOU_ATTR_PORT] = { .type = NLA_U16 },
	[FOU_ATTR_AF] = { .type = NLA_U8 },
	[FOU_ATTR_IPPROTO] = { .type = NLA_U8 },
	[FOU_ATTR_TYPE] = { .type = NLA_U8 },
	[FOU_ATTR_REMCSUM_NOPARTIAL] = { .type = NLA_FLAG },
	[FOU_ATTR_LOCAL_V4] = { .type = NLA_U32 },
	[FOU_ATTR_LOCAL_V6] = { .type = NLA_BINARY, .len = 16 },
	[FOU_ATTR_PEER_V4] = { .type = NLA_U32 },
	[FOU_ATTR_PEER_V6] = { .type = NLA_BINARY, .len = 16 },
	[FOU_ATTR_PEER_PORT] = { .type = NLA_U16 },
	[FOU_ATTR_IFINDEX] = { .type = NLA_S32 },
};
