/* SPDX-License-Identifier: GPL-2.0 */
/*
 *  Copyright (c) 2021 Meta Platforms, Inc. and affiliates
 */

int dpll_notify_device_create(int dpll_id, const char *name);
int dpll_notify_device_delete(int dpll_id);

int __init dpll_netlink_init(void);
void dpll_netlink_finish(void);
