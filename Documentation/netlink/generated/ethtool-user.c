// SPDX-License-Identifier: MIT
// Do not edit directly, auto-generated from:
//	Documentation/netlink/bindings/ethtool.yaml
// ./gen.py --mode user --user-header genetlink-user.h user.h --source --spec Documentation/netlink/bindings/ethtool.yaml

#include <linux/ethtool_netlink.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libmnl/libmnl.h>

#include "genetlink-user.h"
#include "user.h"

// Common nested types
void ethtool_header_free(struct ethtool_header *obj)
{
	free(obj);
}

int ethtool_header_parse(struct ethtool_header *dst,
			 const struct nlattr *nested)
{
	const struct nlattr *attr;

	mnl_attr_for_each_nested(attr, nested) {
		if (mnl_attr_get_type(attr) == ETHTOOL_A_HEADER_DEV_INDEX) {
			dst->dev_index_present = 1;
			dst->dev_index = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == ETHTOOL_A_HEADER_DEV_NAME) {
			dst->dev_name_present = 1;
			strncpy(dst->dev_name, mnl_attr_get_str(attr), ALTIFNAMSIZ - 1);
			dst->dev_name[ALTIFNAMSIZ - 1] = 0;
		}
		if (mnl_attr_get_type(attr) == ETHTOOL_A_HEADER_FLAGS) {
			dst->flags_present = 1;
			dst->flags = mnl_attr_get_u32(attr);
		}
	}

	return 0;
}

/* ============== ETHTOOL_MSG_CHANNELS_GET ============== */
// ETHTOOL_MSG_CHANNELS_GET - do
void ethtool_channels_get_rsp_free(struct ethtool_channels_get_rsp *rsp)
{
	free(rsp);
}

int ethtool_channels_get_rsp_parse(const struct nlmsghdr *nlh, void *data)
{
	struct ethtool_channels_get_rsp *dst = data;
	const struct nlattr *attr;

	mnl_attr_for_each(attr, nlh, sizeof(struct genlmsghdr)) {
