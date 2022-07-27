// SPDX-License-Identifier: MIT
// Do not edit directly, auto-generated from:
//	Documentation/netlink/bindings/genetlink.yaml

#include <linux/genetlink.h>

#include <stdlib.h>
#include <string.h>
#include <libmnl/libmnl.h>

#include "genetlink-user.h"
#include "user.h"

// CTRL_CMD_GETFAMILY
struct genlctrl_getfamily_rsp *
genlctrl_getfamily(struct ynl_sock *ys, struct genlctrl_getfamily_req *req)
{
	struct genlctrl_getfamily_rsp *rsp;
	struct nlmsghdr *nlh;

	nlh = ynl_gemsg_start_req(ys, GENL_ID_CTRL, CTRL_CMD_GETFAMILY, 1);

	if (req->family_id_present)
		mnl_attr_put_u16(nlh, CTRL_ATTR_FAMILY_ID, req->family_id);
	if (req->family_name_present)
		mnl_attr_put_strz(nlh, CTRL_ATTR_FAMILY_NAME, req->family_name);

	rsp = calloc(1, sizeof(*rsp));

	return rsp;
}

// CTRL_CMD_GETPOLICY
