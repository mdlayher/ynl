// SPDX-License-Identifier: MIT
// Do not edit directly, auto-generated from:
//	../../../../Documentation/netlink/bindings/dpll.yaml
// /home/kicinski/devel/linux/gen.py --mode uapi --header --spec ../../../../Documentation/netlink/bindings/dpll.yaml

enum dpll_genl_attr {
	DPLLA_UNSPEC,
	DPLLA_DEVICE_ID,
	DPLLA_DEVICE_NAME,
	DPLLA_SOURCE,
	DPLLA_SOURCE_ID,
	DPLLA_SOURCE_TYPE,
	DPLLA_SOURCE_SUPPORTED,
	DPLLA_OUTPUT,
	DPLLA_OUTPUT_ID,
	DPLLA_OUTPUT_TYPE,
	DPLLA_OUTPUT_SUPPORTED,
	DPLLA_STATUS,
	DPLLA_TEMP,
	DPLLA_LOCK_STATUS,
	DPLLA_FLAGS,

	__DPLLA_MAX
};
#define DPLLA__MAX (__DPLLA_MAX - 1)

enum dpll_genl_cmd {
	DPLL_CMD_UNSPEC,
	DPLL_CMD_DEVICE_GET,
	DPLL_CMD_SET_SOURCE_TYPE,
	DPLL_CMD_SET_OUTPUT_TYPE,
};
