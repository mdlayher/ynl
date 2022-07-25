// SPDX-License-Identifier: MIT
// Do not edit directly, auto-generated from:
//	Documentation/netlink/bindings/genetlink.yaml

#include <linux/genetlink.h>

// CTRL_CMD_GETFAMILY
int genlctrl_getfamily(
	__u16 family_id,
	const char *family_name)
{
	return 0;
}

// CTRL_CMD_GETPOLICY
// Make it compile standalone
int main() { return 0; }
