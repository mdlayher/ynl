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
	u32 local_v4;
	u32 peer_v4;
	unsigned char local_v6[16];
	unsigned char peer_v6[16];
	u16 peer_port;
	s32 ifindex;
};

void fou_add_req_free(struct fou_add_req *req);

void fou_add_req_parse(const struct nlattr **tb, struct fou_add_req *req);
const struct nla_policy fou_add_policy[];

/* ============== FOU_CMD_DEL ============== */
// FOU_CMD_DEL - do
struct fou_del_req {
	u32 af_present:1;
	u32 ifindex_present:1;
	u32 port_present:1;
	u32 peer_port_present:1;
	u32 local_v4_present:1;
	u32 peer_v4_present:1;
	u32 local_v6_present:1;
	u32 peer_v6_present:1;

	u8 af;
	s32 ifindex;
	u16 port;
	u16 peer_port;
	u32 local_v4;
	u32 peer_v4;
	unsigned char local_v6[16];
	unsigned char peer_v6[16];
};

void fou_del_req_free(struct fou_del_req *req);

void fou_del_req_parse(const struct nlattr **tb, struct fou_del_req *req);
const struct nla_policy fou_del_policy[];

/* ============== FOU_CMD_GET ============== */
// FOU_CMD_GET - do
struct fou_get_req {
	u32 af_present:1;
	u32 ifindex_present:1;
	u32 port_present:1;
	u32 peer_port_present:1;
	u32 local_v4_present:1;
	u32 peer_v4_present:1;
	u32 local_v6_present:1;
	u32 peer_v6_present:1;

	u8 af;
	s32 ifindex;
	u16 port;
	u16 peer_port;
	u32 local_v4;
	u32 peer_v4;
	unsigned char local_v6[16];
	unsigned char peer_v6[16];
};

void fou_get_req_free(struct fou_get_req *req);

struct fou_get_rsp {
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
	u32 local_v4;
	u32 peer_v4;
	unsigned char local_v6[16];
	unsigned char peer_v6[16];
	u16 peer_port;
	s32 ifindex;
};

void fou_get_rsp_free(struct fou_get_rsp *rsp);

void fou_get_req_parse(const struct nlattr **tb, struct fou_get_req *req);
const struct nla_policy fou_get_policy[];

// FOU_CMD_GET - dump
