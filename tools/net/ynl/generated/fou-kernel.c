// SPDX-License-Identifier: BSD-3-Clause
// Do not edit directly, auto-generated from:
//	Documentation/netlink/bindings/fou.yaml
// YNL-GEN kernel source

#include <net/netlink.h>

#include <linux/fou.h>

// FOU_CMD_ADD - do
const struct nla_policy fou_add_policy[FOU_ATTR_IFINDEX + 1] = {
	[FOU_ATTR_PORT] = { .type = NLA_U16 },
	[FOU_ATTR_IPPROTO] = { .type = NLA_U8 },
	[FOU_ATTR_TYPE] = { .type = NLA_U8 },
	[FOU_ATTR_REMCSUM_NOPARTIAL] = { .type = NLA_FLAG },
	[FOU_ATTR_LOCAL_V4] = { .type = NLA_U32 },
	[FOU_ATTR_PEER_V4] = { .type = NLA_U32 },
	[FOU_ATTR_LOCAL_V6] = { .type = NLA_BINARY, .len = 16 },
	[FOU_ATTR_PEER_V6] = { .type = NLA_BINARY, .len = 16 },
	[FOU_ATTR_PEER_PORT] = { .type = NLA_U16 },
	[FOU_ATTR_IFINDEX] = { .type = NLA_S32 },
};

// FOU_CMD_DEL - do
const struct nla_policy fou_del_policy[FOU_ATTR_IFINDEX + 1] = {
	[FOU_ATTR_AF] = { .type = NLA_U8 },
	[FOU_ATTR_IFINDEX] = { .type = NLA_S32 },
	[FOU_ATTR_PORT] = { .type = NLA_U16 },
	[FOU_ATTR_PEER_PORT] = { .type = NLA_U16 },
	[FOU_ATTR_LOCAL_V4] = { .type = NLA_U32 },
	[FOU_ATTR_PEER_V4] = { .type = NLA_U32 },
	[FOU_ATTR_LOCAL_V6] = { .type = NLA_BINARY, .len = 16 },
	[FOU_ATTR_PEER_V6] = { .type = NLA_BINARY, .len = 16 },
};

// FOU_CMD_GET - do
const struct nla_policy fou_get_policy[FOU_ATTR_IFINDEX + 1] = {
	[FOU_ATTR_AF] = { .type = NLA_U8 },
	[FOU_ATTR_IFINDEX] = { .type = NLA_S32 },
	[FOU_ATTR_PORT] = { .type = NLA_U16 },
	[FOU_ATTR_PEER_PORT] = { .type = NLA_U16 },
	[FOU_ATTR_LOCAL_V4] = { .type = NLA_U32 },
	[FOU_ATTR_PEER_V4] = { .type = NLA_U32 },
	[FOU_ATTR_LOCAL_V6] = { .type = NLA_BINARY, .len = 16 },
	[FOU_ATTR_PEER_V6] = { .type = NLA_BINARY, .len = 16 },
};

// FOU_CMD_GET - dump
