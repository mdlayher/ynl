// SPDX-License-Identifier: MIT
// Do not edit directly, auto-generated from:
//	Documentation/netlink/bindings/genetlink.yaml

#include <linux/genetlink.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libmnl/libmnl.h>

#include "genetlink-user.h"
#include "user.h"

// CTRL_CMD_GETFAMILY
struct genlctrl_getfamily_rsp *
genlctrl_getfamily(struct ynl_sock *ys, struct genlctrl_getfamily_req *req)
{
	struct genlctrl_getfamily_rsp *rsp;
	const struct nlattr *attr;
	struct nlmsghdr *nlh;
	int len, err;

	nlh = ynl_gemsg_start_req(ys, GENL_ID_CTRL, CTRL_CMD_GETFAMILY, 1);

	if (req->family_id_present)
		mnl_attr_put_u16(nlh, CTRL_ATTR_FAMILY_ID, req->family_id);
	if (req->family_name_present)
		mnl_attr_put_strz(nlh, CTRL_ATTR_FAMILY_NAME, req->family_name);

	err = mnl_socket_sendto(ys->sock, nlh, nlh->nlmsg_len);
	if (err < 0)
		return NULL;

	len = mnl_socket_recvfrom(ys->sock, ys->buf, MNL_SOCKET_BUFFER_SIZE);
	if (len < 0)
		return NULL;

	nlh = (struct nlmsghdr *)ys->buf;
	if (!mnl_nlmsg_ok(nlh, len) || (unsigned int)len != nlh->nlmsg_len)
		return NULL;

	rsp = calloc(1, sizeof(*rsp));

	mnl_attr_for_each(attr, nlh, sizeof(struct genlmsghdr)) {
		if (mnl_attr_get_type(attr) == CTRL_ATTR_FAMILY_ID) {
			rsp->family_id_present = 1;
			rsp->family_id = mnl_attr_get_u16(attr);
		}
		if (mnl_attr_get_type(attr) == CTRL_ATTR_FAMILY_NAME) {
			rsp->family_name_present = 1;
			strncpy(rsp->family_name, mnl_attr_get_str(attr), GENL_NAMSIZ - 1);
			rsp->family_name[GENL_NAMSIZ - 1] = 0;
		}
		if (mnl_attr_get_type(attr) == CTRL_ATTR_VERSION) {
			rsp->version_present = 1;
			rsp->version = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == CTRL_ATTR_HDRSIZE) {
			rsp->hdrsize_present = 1;
			rsp->hdrsize = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == CTRL_ATTR_MAXATTR) {
			rsp->maxattr_present = 1;
			rsp->maxattr = mnl_attr_get_u32(attr);
		}
	}
	return rsp;
}

// CTRL_CMD_GETPOLICY
