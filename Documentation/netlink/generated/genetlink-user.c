#include <linux/genetlink.h>

// Codegen for CTRL_CMD_GETFAMILY
int genlctrl_getfamily(
	__u16 family_id,
	const char *family_name
);

struct genlctrl_getfamily_rsp {
	__u16 family_id;
	char family_name[GENL_NAMSIZ];
	__u32 version;
	__u32 hdrsize;
	__u32 maxattr;
	unsigned int n_ops;
	struct genlctrl_operation *ops;
};
void genlctrl_getfamily_rsp_free(struct genlctrl_getfamily_rsp *getfamily);

// Codegen for CTRL_CMD_GETPOLICY
