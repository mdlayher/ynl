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

#ifdef KERNEL
struct genlctrl_getfamily_req {
	__u16 family_id;
	char family_name[GENL_NAMSIZ];
};
void genlctrl_getfamily_rsp_free(struct genlctrl_getfamily_rsp *getfamily);

int genlctrl_getfamily_req_parse(.. req) 
{
	if (tb[CTRL_ATTR_FAMILY_ID]) {
		req->family_id_present = 1;
		req->family_id = nla_get_u16(tb[CTRL_ATTR_FAMILY_ID]);
	}
}

static const struct nla_policy genlctrl_getfamily_policy[] = {
	[CTRL_ATTR_FAMILY_ID] = { .type = NLA_U16 },
	[CTRL_ATTR_FAMILY_NAME] = { .type = NLA_NUL_STRING, .len = GENL_NAMSIZ - 1 },
};
#endif

// Codegen for CTRL_CMD_GETPOLICY
