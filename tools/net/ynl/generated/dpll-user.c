// SPDX-License-Identifier: MIT
// Do not edit directly, auto-generated from:
//	../../../../Documentation/netlink/bindings/dpll.yaml
// /home/kicinski/devel/linux/gen.py --mode user --user-header dpll-user.h ynl.h --source --spec ../../../../Documentation/netlink/bindings/dpll.yaml

#include <linux/dpll.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libmnl/libmnl.h>
#include <linux/genetlink.h>

#include "dpll-user.h"
#include "ynl.h"

// Common nested types
void dpll_source_free(struct dpll_source *obj)
{
	free(obj);
}

int dpll_source_parse(struct dpll_source *dst, const struct nlattr *nested)
{
	const struct nlattr *attr;

	mnl_attr_for_each_nested(attr, nested) {
		if (mnl_attr_get_type(attr) == DPLLA_OUTPUT_ID) {
			dst->output_id_present = 1;
			dst->output_id = mnl_attr_get_u32(attr);
		}
		if (mnl_attr_get_type(attr) == DPLLA_OUTPUT_TYPE) {
			dst->output_type_present = 1;
			dst->output_type = mnl_attr_get_u32(attr);
		}
