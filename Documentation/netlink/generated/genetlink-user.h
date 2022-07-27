// SPDX-License-Identifier: MIT
// Do not edit directly, auto-generated from:
//	Documentation/netlink/bindings/genetlink.yaml

#include <linux/genetlink.h>

struct ynl_sock;

// Common nested types
struct genlctrl_operation {
	__u32 op_id_present:1;
	__u32 op_flags_present:1;

	__u32 op_id;
	__u32 op_flags;
};

// CTRL_CMD_GETFAMILY
struct genlctrl_getfamily_req {
	__u32 family_id_present:1;
	__u32 family_name_present:1;

	__u16 family_id;
	char family_name[GENL_NAMSIZ];
};

static inline void
genlctrl_getfamily_req_set_family_id(struct genlctrl_getfamily_req *req, __u16 family_id)
{
	req->family_id_present = 1;
	req->family_id = family_id;
}
static inline void
genlctrl_getfamily_req_set_family_name(struct genlctrl_getfamily_req *req, const char *family_name)
{
	req->family_name_present = 1;
	strncpy(req->family_name, family_name, sizeof(req->family_name));
	req->family_name[GENL_NAMSIZ - 1] = 0;
}

void genlctrl_getfamily_req_free(struct genlctrl_getfamily_req *req);

struct genlctrl_getfamily_rsp {
	__u32 family_id_present:1;
	__u32 family_name_present:1;
	__u32 version_present:1;
	__u32 hdrsize_present:1;
	__u32 maxattr_present:1;
	__u32 ops_present:1;

	__u16 family_id;
	char family_name[GENL_NAMSIZ];
	__u32 version;
	__u32 hdrsize;
	__u32 maxattr;
	unsigned int n_ops;
	struct genlctrl_operation *ops;
};
void genlctrl_getfamily_rsp_free(struct genlctrl_getfamily_rsp *req);

struct genlctrl_getfamily_rsp *
genlctrl_getfamily(struct ynl_sock *ys, struct genlctrl_getfamily_req *req);

// CTRL_CMD_GETPOLICY
