// SPDX-License-Identifier: MIT
// Do not edit directly, auto-generated from:
//	Documentation/netlink/bindings/genetlink.yaml

#include <linux/genetlink.h>

#include <stdlib.h>
#include "genetlink-user.h"
#include "user.h"

// CTRL_CMD_GETFAMILY
struct genlctrl_getfamily_rsp *
genlctrl_getfamily(struct ynl_sock *ys, struct genlctrl_getfamily_req *req)
{
	struct genlctrl_getfamily_rsp *rsp;

	rsp = calloc(1, sizeof(*rsp));

	return rsp;
}

// CTRL_CMD_GETPOLICY
