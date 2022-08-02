// SPDX-License-Identifier: MIT
// Do not edit directly, auto-generated from:
//	Documentation/netlink/bindings/fou.yaml
// ./gen.py --mode kernel --user-header fou-user.h user.h --source --spec Documentation/netlink/bindings/fou.yaml

#include <net/netlink.h>

#include <linux/fou.h>

/* ============== FOU_CMD_ADD ============== */
// FOU_CMD_ADD - do
void fou_add_req_parse(const struct nlattr **tb, struct fou_add_req *req)
{
	if (tb[FOU_ATTR_PORT]) {
		req->port_present = 1;
		req->port = nla_get_u16(tb[FOU_ATTR_PORT]);
	}
	if (tb[FOU_ATTR_IPPROTO]) {
		req->ipproto_present = 1;
		req->ipproto = nla_get_u8(tb[FOU_ATTR_IPPROTO]);
	}
	if (tb[FOU_ATTR_TYPE]) {
		req->type_present = 1;
		req->type = nla_get_u8(tb[FOU_ATTR_TYPE]);
	}
	if (tb[FOU_ATTR_REMCSUM_NOPARTIAL]) {
		req->remcsum_nopartial_present = 1;
	}
	if (tb[FOU_ATTR_LOCAL_V4]) {
		req->local_v4_present = 1;
		req->local_v4 = nla_get_u32(tb[FOU_ATTR_LOCAL_V4]);
	}
	if (tb[FOU_ATTR_PEER_V4]) {
		req->peer_v4_present = 1;
		req->peer_v4 = nla_get_u32(tb[FOU_ATTR_PEER_V4]);
	}
	if (tb[FOU_ATTR_LOCAL_V6]) {
		req->local_v6_present = 1;
	}
	if (tb[FOU_ATTR_PEER_V6]) {
		req->peer_v6_present = 1;
	}
	if (tb[FOU_ATTR_PEER_PORT]) {
		req->peer_port_present = 1;
		req->peer_port = nla_get_u16(tb[FOU_ATTR_PEER_PORT]);
	}
	if (tb[FOU_ATTR_IFINDEX]) {
		req->ifindex_present = 1;
		req->ifindex = nla_get_s32(tb[FOU_ATTR_IFINDEX]);
	}
}

const struct nla_policy fou_add_policy[] = {
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

/* ============== FOU_CMD_DEL ============== */
// FOU_CMD_DEL - do
void fou_del_req_parse(const struct nlattr **tb, struct fou_del_req *req)
{
	if (tb[FOU_ATTR_AF]) {
		req->af_present = 1;
		req->af = nla_get_u8(tb[FOU_ATTR_AF]);
	}
	if (tb[FOU_ATTR_IFINDEX]) {
		req->ifindex_present = 1;
		req->ifindex = nla_get_s32(tb[FOU_ATTR_IFINDEX]);
	}
	if (tb[FOU_ATTR_PORT]) {
		req->port_present = 1;
		req->port = nla_get_u16(tb[FOU_ATTR_PORT]);
	}
	if (tb[FOU_ATTR_PEER_PORT]) {
		req->peer_port_present = 1;
		req->peer_port = nla_get_u16(tb[FOU_ATTR_PEER_PORT]);
	}
	if (tb[FOU_ATTR_LOCAL_V4]) {
		req->local_v4_present = 1;
		req->local_v4 = nla_get_u32(tb[FOU_ATTR_LOCAL_V4]);
	}
	if (tb[FOU_ATTR_PEER_V4]) {
		req->peer_v4_present = 1;
		req->peer_v4 = nla_get_u32(tb[FOU_ATTR_PEER_V4]);
	}
	if (tb[FOU_ATTR_LOCAL_V6]) {
		req->local_v6_present = 1;
	}
	if (tb[FOU_ATTR_PEER_V6]) {
		req->peer_v6_present = 1;
	}
}

const struct nla_policy fou_del_policy[] = {
	[FOU_ATTR_AF] = { .type = NLA_U8 },
	[FOU_ATTR_IFINDEX] = { .type = NLA_S32 },
	[FOU_ATTR_PORT] = { .type = NLA_U16 },
	[FOU_ATTR_PEER_PORT] = { .type = NLA_U16 },
	[FOU_ATTR_LOCAL_V4] = { .type = NLA_U32 },
	[FOU_ATTR_PEER_V4] = { .type = NLA_U32 },
	[FOU_ATTR_LOCAL_V6] = { .type = NLA_BINARY, .len = 16 },
	[FOU_ATTR_PEER_V6] = { .type = NLA_BINARY, .len = 16 },
};

/* ============== FOU_CMD_GET ============== */
// FOU_CMD_GET - do
void fou_get_req_parse(const struct nlattr **tb, struct fou_get_req *req)
{
	if (tb[FOU_ATTR_AF]) {
		req->af_present = 1;
		req->af = nla_get_u8(tb[FOU_ATTR_AF]);
	}
	if (tb[FOU_ATTR_IFINDEX]) {
		req->ifindex_present = 1;
		req->ifindex = nla_get_s32(tb[FOU_ATTR_IFINDEX]);
	}
	if (tb[FOU_ATTR_PORT]) {
		req->port_present = 1;
		req->port = nla_get_u16(tb[FOU_ATTR_PORT]);
	}
	if (tb[FOU_ATTR_PEER_PORT]) {
		req->peer_port_present = 1;
		req->peer_port = nla_get_u16(tb[FOU_ATTR_PEER_PORT]);
	}
	if (tb[FOU_ATTR_LOCAL_V4]) {
		req->local_v4_present = 1;
		req->local_v4 = nla_get_u32(tb[FOU_ATTR_LOCAL_V4]);
	}
	if (tb[FOU_ATTR_PEER_V4]) {
		req->peer_v4_present = 1;
		req->peer_v4 = nla_get_u32(tb[FOU_ATTR_PEER_V4]);
	}
	if (tb[FOU_ATTR_LOCAL_V6]) {
		req->local_v6_present = 1;
	}
	if (tb[FOU_ATTR_PEER_V6]) {
		req->peer_v6_present = 1;
	}
}

const struct nla_policy fou_get_policy[] = {
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
