/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */
// Do not edit directly, auto-generated from:
//	Documentation/netlink/specs/ethtool.yaml
// YNL-GEN uapi header

#ifndef _UAPI_LINUX_ETHTOOL_H
#define _UAPI_LINUX_ETHTOOL_H

#define ETHTOOL_FAMILY_NAME	"ethtool"
#define ETHTOOL_FAMILY_VERSION	1

enum {
	ETHTOOL_A_HEADER_DEV_INDEX,
	ETHTOOL_A_HEADER_DEV_NAME,
	ETHTOOL_A_HEADER_FLAGS,

	__ETHTOOL_A_HEADER_MAX,
	ETHTOOL_A_HEADER_MAX = (__ETHTOOL_A_HEADER_MAX - 1)
};

enum {
	ETHTOOL_A_CHANNELS_HEADER,
	ETHTOOL_A_CHANNELS_RX_MAX,
	ETHTOOL_A_CHANNELS_TX_MAX,
	ETHTOOL_A_CHANNELS_OTHER_MAX,
	ETHTOOL_A_CHANNELS_COMBINED_MAX,
	ETHTOOL_A_CHANNELS_RX_COUNT,
	ETHTOOL_A_CHANNELS_TX_COUNT,
	ETHTOOL_A_CHANNELS_OTHER_COUNT,
	ETHTOOL_A_CHANNELS_COMBINED_COUNT,

	__ETHTOOL_A_CHANNELS_MAX,
	ETHTOOL_A_CHANNELS_MAX = (__ETHTOOL_A_CHANNELS_MAX - 1)
};

enum {
	ETHTOOL_MSG_CHANNELS_SET,

	__ETHTOOL_MSG_MAX,
	ETHTOOL_MSG_MAX = (__ETHTOOL_MSG_MAX - 1)
};

enum {
	ETHTOOL_MSG_CHANNELS_GET = 17,
	ETHTOOL_MSG_CHANNELS_NTF,
};

#define ETHTOOL_MCGRP_MONITOR_NAME	"monitor"

#endif /* _UAPI_LINUX_ETHTOOL_H */
