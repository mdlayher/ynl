// SPDX-License-Identifier: MIT
// Do not edit directly, auto-generated from:
//	../../../../Documentation/netlink/bindings/dpll.yaml
// /home/kicinski/devel/linux/gen.py --mode kernel --header --spec ../../../../Documentation/netlink/bindings/dpll.yaml

#ifndef _LINUX_DPLL_GEN_H
#define _LINUX_DPLL_GEN_H

#include <net/netlink.h>

#include <linux/dpll.h>

// DPLL_CMD_DEVICE_GET - do
extern const struct nla_policy dpll_device_get_policy[DPLLA_FLAGS + 1];

// DPLL_CMD_SET_SOURCE_TYPE - do
extern const struct nla_policy dpll_set_source_type_policy[DPLLA_SOURCE_TYPE + 1];

// DPLL_CMD_SET_OUTPUT_TYPE - do
extern const struct nla_policy dpll_set_output_type_policy[DPLLA_OUTPUT_TYPE + 1];

#endif /* _LINUX_DPLL_GEN_H */
