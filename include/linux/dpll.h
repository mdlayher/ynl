/* SPDX-License-Identifier: GPL-2.0 */
/*
 *  Copyright (c) 2021 Meta Platforms, Inc. and affiliates
 */

#ifndef __DPLL_H__
#define __DPLL_H__

struct dpll_device;

struct dpll_device_ops {
	int (*get_status)(struct dpll_device *dpll);
	int (*get_temp)(struct dpll_device *dpll);
	int (*get_lock_status)(struct dpll_device *dpll);
	int (*get_source_type)(struct dpll_device *dpll, int id);
	int (*get_source_supported)(struct dpll_device *dpll, int id, int type);
	int (*get_output_type)(struct dpll_device *dpll, int id);
	int (*get_output_supported)(struct dpll_device *dpll, int id, int type);
	int (*set_source_type)(struct dpll_device *dpll, int id, int val);
	int (*set_output_type)(struct dpll_device *dpll, int id, int val);
};

struct dpll_device *dpll_device_alloc(struct dpll_device_ops *ops, int sources_count,
				      const char *name, int outputs_count, void *priv);
void dpll_device_register(struct dpll_device *dpll);
void dpll_device_unregister(struct dpll_device *dpll);
void dpll_device_free(struct dpll_device *dpll);
void *dpll_priv(struct dpll_device *dpll);

int dpll_notify_status_locked(int dpll_id);
int dpll_notify_status_unlocked(int dpll_id);
int dpll_notify_source_change(int dpll_id, int source_id, int source_type);
int dpll_notify_output_change(int dpll_id, int output_id, int output_type);
#endif
