// SPDX-License-Identifier: MIT
// Do not edit directly, auto-generated from:
//	Documentation/netlink/bindings/fou.yaml
// ./gen.py --mode user --user-header genetlink-user.h user.h --source --spec Documentation/netlink/bindings/fou.yaml

#include <linux/fou.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libmnl/libmnl.h>

#include "genetlink-user.h"
#include "user.h"

// Common nested types
/* ============== FOU_CMD_ADD ============== */
// FOU_CMD_ADD - do
int fou_add(struct ynl_sock *ys, struct fou_add_req *req)
{
	struct nlmsghdr *nlh;
	int len, err;

	nlh = ynl_gemsg_start_req(ys, ys->family_id, FOU_CMD_ADD, 1);

	if (req->port_present)
		mnl_attr_put_u16(nlh, FOU_ATTR_PORT, req->port);
	if (req->ipproto_present)
		mnl_attr_put_u8(nlh, FOU_ATTR_IPPROTO, req->ipproto);
	if (req->type_present)
		mnl_attr_put_u8(nlh, FOU_ATTR_TYPE, req->type);
