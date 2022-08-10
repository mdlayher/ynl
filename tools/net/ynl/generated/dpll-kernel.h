// SPDX-License-Identifier: MIT
// Do not edit directly, auto-generated from:
//	../../../../Documentation/netlink/bindings/dpll.yaml
// /home/kicinski/devel/linux/gen.py --mode kernel --header --spec ../../../../Documentation/netlink/bindings/dpll.yaml

#ifndef _LINUX_DPLL_GEN_H
#define _LINUX_DPLL_GEN_H

#include <net/netlink.h>

#include <linux/dpll.h>

/* ============== DPLL_CMD_DEVICE_GET ============== */
// DPLL_CMD_DEVICE_GET - do
struct dpll_device_get_req {
	u32 device_id_present:1;
	u32 device_name_present:1;
	u32 flags_present:1;

	u32 device_id;
	char device_name[DPLL_NAME_LENGTH];
	u32 flags;
};

void dpll_device_get_req_free(struct dpll_device_get_req *req);

struct dpll_device_get_rsp {
	u32 device_id_present:1;
	u32 device_name_present:1;
	u32 status_present:1;
	u32 temp_present:1;
	u32 lock_status_present:1;

	u32 device_id;
	char device_name[DPLL_NAME_LENGTH];
	unsigned int n_source;
	struct dpll_source *source;
	unsigned int n_output;
	struct dpll_output *output;
	enum dpll_genl_status status;
	u32 temp;
	enum dpll_genl_lock_status lock_status;
};

void dpll_device_get_rsp_free(struct dpll_device_get_rsp *rsp);

void
dpll_device_get_req_parse(const struct nlattr **tb,
			  struct dpll_device_get_req *req);
const struct nla_policy dpll_device_get_policy[];

/* ============== DPLL_CMD_SET_SOURCE_TYPE ============== */
// DPLL_CMD_SET_SOURCE_TYPE - do
struct dpll_set_source_type_req {
	u32 device_id_present:1;
	u32 device_name_present:1;
	u32 source_id_present:1;
	u32 source_type_present:1;

	u32 device_id;
	char device_name[DPLL_NAME_LENGTH];
	u32 source_id;
	enum dpll_genl_signal_type source_type;
};

void dpll_set_source_type_req_free(struct dpll_set_source_type_req *req);

void
dpll_set_source_type_req_parse(const struct nlattr **tb,
			       struct dpll_set_source_type_req *req);
const struct nla_policy dpll_set_source_type_policy[];

/* ============== DPLL_CMD_SET_OUTPUT_TYPE ============== */
// DPLL_CMD_SET_OUTPUT_TYPE - do
struct dpll_set_output_type_req {
	u32 device_id_present:1;
	u32 device_name_present:1;
	u32 output_id_present:1;
	u32 output_type_present:1;

	u32 device_id;
	char device_name[DPLL_NAME_LENGTH];
	u32 output_id;
	enum dpll_genl_signal_type output_type;
};

void dpll_set_output_type_req_free(struct dpll_set_output_type_req *req);

void
dpll_set_output_type_req_parse(const struct nlattr **tb,
			       struct dpll_set_output_type_req *req);
const struct nla_policy dpll_set_output_type_policy[];

/* ============== DPLL_CMD_DEVICE-CREATE ============== */
/* ============== DPLL_CMD_DEVICE-DELETE ============== */
/* ============== DPLL_CMD_STATUS-LOCKED ============== */
/* ============== DPLL_CMD_STATUS-UNLOCKED ============== */
/* ============== DPLL_CMD_SOURCE-CHANGE ============== */
/* ============== DPLL_CMD_OUTPUT-CHANGE ============== */

#endif /* _LINUX_DPLL_GEN_H */
