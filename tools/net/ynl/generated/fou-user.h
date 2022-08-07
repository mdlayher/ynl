// SPDX-License-Identifier: MIT
// Do not edit directly, auto-generated from:
//	../../../../Documentation/netlink/bindings/fou.yaml
// /home/kicinski/devel/linux/gen.py --mode user --user-header ynl.h --header --spec ../../../../Documentation/netlink/bindings/fou.yaml

#ifndef _LINUX_FOU_GEN_H
#define _LINUX_FOU_GEN_H

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
	__u32 local_v4;
	__u32 peer_v4;
	unsigned char local_v6[16];
	unsigned char peer_v6[16];
	__u16 peer_port;
	__s32 ifindex;
};

void fou_add_req_free(struct fou_add_req *req);
static inline void fou_add_req_set_port(struct fou_add_req *req, __u16 port)
{
	req->port_present = 1;
	req->port = port;
}
static inline void
fou_add_req_set_ipproto(struct fou_add_req *req, __u8 ipproto)
{
	req->ipproto_present = 1;
	req->ipproto = ipproto;
}
static inline void fou_add_req_set_type(struct fou_add_req *req, __u8 type)
{
	req->type_present = 1;
	req->type = type;
}
static inline void fou_add_req_set_remcsum_nopartial(struct fou_add_req *req)
{
	req->remcsum_nopartial_present = 1;
}
static inline void
fou_add_req_set_local_v4(struct fou_add_req *req, __u32 local_v4)
{
	req->local_v4_present = 1;
	req->local_v4 = local_v4;
}
static inline void
fou_add_req_set_peer_v4(struct fou_add_req *req, __u32 peer_v4)
{
	req->peer_v4_present = 1;
	req->peer_v4 = peer_v4;
}
static inline void
fou_add_req_set_local_v6(struct fou_add_req *req, const void *local_v6)
{
	req->local_v6_present = 1;
	memcpy(req->local_v6, local_v6, 16);
}
static inline void
fou_add_req_set_peer_v6(struct fou_add_req *req, const void *peer_v6)
{
	req->peer_v6_present = 1;
	memcpy(req->peer_v6, peer_v6, 16);
}
static inline void
fou_add_req_set_peer_port(struct fou_add_req *req, __u16 peer_port)
{
	req->peer_port_present = 1;
	req->peer_port = peer_port;
}
static inline void
fou_add_req_set_ifindex(struct fou_add_req *req, __s32 ifindex)
{
	req->ifindex_present = 1;
	req->ifindex = ifindex;
}

int fou_add(struct ynl_sock *ys, struct fou_add_req *req);

/* ============== FOU_CMD_DEL ============== */
// FOU_CMD_DEL - do
struct fou_del_req {
	__u32 af_present:1;
	__u32 ifindex_present:1;
	__u32 port_present:1;
	__u32 peer_port_present:1;
	__u32 local_v4_present:1;
	__u32 peer_v4_present:1;
	__u32 local_v6_present:1;
	__u32 peer_v6_present:1;

	__u8 af;
	__s32 ifindex;
	__u16 port;
	__u16 peer_port;
	__u32 local_v4;
	__u32 peer_v4;
	unsigned char local_v6[16];
	unsigned char peer_v6[16];
};

void fou_del_req_free(struct fou_del_req *req);
static inline void fou_del_req_set_af(struct fou_del_req *req, __u8 af)
{
	req->af_present = 1;
	req->af = af;
}
static inline void
fou_del_req_set_ifindex(struct fou_del_req *req, __s32 ifindex)
{
	req->ifindex_present = 1;
	req->ifindex = ifindex;
}
static inline void fou_del_req_set_port(struct fou_del_req *req, __u16 port)
{
	req->port_present = 1;
	req->port = port;
}
static inline void
fou_del_req_set_peer_port(struct fou_del_req *req, __u16 peer_port)
{
	req->peer_port_present = 1;
	req->peer_port = peer_port;
}
static inline void
fou_del_req_set_local_v4(struct fou_del_req *req, __u32 local_v4)
{
	req->local_v4_present = 1;
	req->local_v4 = local_v4;
}
static inline void
fou_del_req_set_peer_v4(struct fou_del_req *req, __u32 peer_v4)
{
	req->peer_v4_present = 1;
	req->peer_v4 = peer_v4;
}
static inline void
fou_del_req_set_local_v6(struct fou_del_req *req, const void *local_v6)
{
	req->local_v6_present = 1;
	memcpy(req->local_v6, local_v6, 16);
}
static inline void
fou_del_req_set_peer_v6(struct fou_del_req *req, const void *peer_v6)
{
	req->peer_v6_present = 1;
	memcpy(req->peer_v6, peer_v6, 16);
}

int fou_del(struct ynl_sock *ys, struct fou_del_req *req);

/* ============== FOU_CMD_GET ============== */
// FOU_CMD_GET - do
struct fou_get_req {
	__u32 af_present:1;
	__u32 ifindex_present:1;
	__u32 port_present:1;
	__u32 peer_port_present:1;
	__u32 local_v4_present:1;
	__u32 peer_v4_present:1;
	__u32 local_v6_present:1;
	__u32 peer_v6_present:1;

	__u8 af;
	__s32 ifindex;
	__u16 port;
	__u16 peer_port;
	__u32 local_v4;
	__u32 peer_v4;
	unsigned char local_v6[16];
	unsigned char peer_v6[16];
};

void fou_get_req_free(struct fou_get_req *req);
static inline void fou_get_req_set_af(struct fou_get_req *req, __u8 af)
{
	req->af_present = 1;
	req->af = af;
}
static inline void
fou_get_req_set_ifindex(struct fou_get_req *req, __s32 ifindex)
{
	req->ifindex_present = 1;
	req->ifindex = ifindex;
}
static inline void fou_get_req_set_port(struct fou_get_req *req, __u16 port)
{
	req->port_present = 1;
	req->port = port;
}
static inline void
fou_get_req_set_peer_port(struct fou_get_req *req, __u16 peer_port)
{
	req->peer_port_present = 1;
	req->peer_port = peer_port;
}
static inline void
fou_get_req_set_local_v4(struct fou_get_req *req, __u32 local_v4)
{
	req->local_v4_present = 1;
	req->local_v4 = local_v4;
}
static inline void
fou_get_req_set_peer_v4(struct fou_get_req *req, __u32 peer_v4)
{
	req->peer_v4_present = 1;
	req->peer_v4 = peer_v4;
}
static inline void
fou_get_req_set_local_v6(struct fou_get_req *req, const void *local_v6)
{
	req->local_v6_present = 1;
	memcpy(req->local_v6, local_v6, 16);
}
static inline void
fou_get_req_set_peer_v6(struct fou_get_req *req, const void *peer_v6)
{
	req->peer_v6_present = 1;
	memcpy(req->peer_v6, peer_v6, 16);
}

struct fou_get_rsp {
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
	__u32 local_v4;
	__u32 peer_v4;
	unsigned char local_v6[16];
	unsigned char peer_v6[16];
	__u16 peer_port;
	__s32 ifindex;
};

void fou_get_rsp_free(struct fou_get_rsp *rsp);

struct fou_get_rsp *fou_get(struct ynl_sock *ys, struct fou_get_req *req);

// FOU_CMD_GET - dump
struct fou_get_list {
	struct fou_get_list *next;
	struct fou_get_rsp obj;
};

void fou_get_list_free(struct fou_get_list *rsp);

struct fou_get_list *fou_get_dump(struct ynl_sock *ys);

#endif /* _LINUX_FOU_GEN_H */
