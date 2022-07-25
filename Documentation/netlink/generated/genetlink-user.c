// SPDX-License-Identifier: MIT
// Do not edit directly, auto-generated from:
//	Documentation/netlink/bindings/genetlink.yaml

#include <linux/genetlink.h>

// Header content

// Common nested types
struct genlctrl_operation {
	__u32 op_id_present:1;
	__u32 op_flags_present:1;

	__u32 op_id;
	__u32 op_flags;
};
void genlctrl_operation_free(struct genlctrl_operation *);

// CTRL_CMD_GETFAMILY
int genlctrl_getfamily(
	__u16 family_id,
	const char *family_name
);

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
void genlctrl_getfamily_rsp_free(struct genlctrl_getfamily_rsp *getfamily);

// CTRL_CMD_GETPOLICY
// Source content

// CTRL_CMD_GETFAMILY

// CTRL_CMD_GETPOLICY
