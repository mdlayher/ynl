// SPDX-License-Identifier: MIT
// Do not edit directly, auto-generated from:
//	Documentation/netlink/bindings/dpll.yaml
// YNL-GEN kernel source

#include <net/netlink.h>

#include <linux/dpll.h>

// DPLL_CMD_DEVICE_GET - do
const struct nla_policy dpll_device_get_policy[DPLLA_FLAGS + 1] = {
	[DPLLA_DEVICE_ID] = { .type = NLA_U32 },
	[DPLLA_DEVICE_NAME] = { .type = NLA_NUL_STRING, .len = DPLL_NAME_LENGTH - 1 },
	[DPLLA_FLAGS] = NLA_POLICY_MASK(NLA_U32, 0x7),
};

// DPLL_CMD_SET_SOURCE_TYPE - do
const struct nla_policy dpll_set_source_type_policy[DPLLA_SOURCE_TYPE + 1] = {
	[DPLLA_DEVICE_ID] = { .type = NLA_U32 },
	[DPLLA_DEVICE_NAME] = { .type = NLA_NUL_STRING, .len = DPLL_NAME_LENGTH - 1 },
	[DPLLA_SOURCE_ID] = { .type = NLA_U32 },
	[DPLLA_SOURCE_TYPE] = NLA_POLICY_MAX(NLA_U32, 7),
};

// DPLL_CMD_SET_OUTPUT_TYPE - do
const struct nla_policy dpll_set_output_type_policy[DPLLA_OUTPUT_TYPE + 1] = {
	[DPLLA_DEVICE_ID] = { .type = NLA_U32 },
	[DPLLA_DEVICE_NAME] = { .type = NLA_NUL_STRING, .len = DPLL_NAME_LENGTH - 1 },
	[DPLLA_OUTPUT_ID] = { .type = NLA_U32 },
	[DPLLA_OUTPUT_TYPE] = NLA_POLICY_MAX(NLA_U32, 7),
};
