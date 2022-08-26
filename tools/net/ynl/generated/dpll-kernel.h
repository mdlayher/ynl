// SPDX-License-Identifier: BSD-3-Clause
// Do not edit directly, auto-generated from:
//	Documentation/netlink/specs/dpll.yaml
// YNL-GEN kernel header

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

// Ops table for dpll
extern const struct genl_ops dpll_ops[];
#endif /* _LINUX_DPLL_GEN_H */
