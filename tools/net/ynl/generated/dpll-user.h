// SPDX-License-Identifier: BSD-3-Clause
// Do not edit directly, auto-generated from:
//	Documentation/netlink/specs/dpll.yaml
// YNL-GEN user header

#ifndef _LINUX_DPLL_GEN_H
#define _LINUX_DPLL_GEN_H

#include <linux/dpll.h>

struct ynl_sock;

// Common nested types
struct dpll_output {
	__u32 source_id_present:1;
	__u32 source_type_present:1;

	__u32 source_id;
	enum dpll_genl_signal_type source_type;
	unsigned int n_source_supported;
	__u32 *source_supported;
};

struct dpll_source {
	__u32 output_id_present:1;
	__u32 output_type_present:1;

	__u32 output_id;
	enum dpll_genl_signal_type output_type;
	unsigned int n_output_supported;
	__u32 *output_supported;
};

/* ============== DPLL_CMD_DEVICE_GET ============== */
// DPLL_CMD_DEVICE_GET - do
struct dpll_device_get_req {
	__u32 device_id_present:1;
	__u32 device_name_present:1;
	__u32 flags_present:1;

	__u32 device_id;
	char device_name[DPLL_NAME_LENGTH];
	__u32 flags;
};

void dpll_device_get_req_free(struct dpll_device_get_req *req);
static inline void
dpll_device_get_req_set_device_id(struct dpll_device_get_req *req,
				  __u32 device_id)
{
	req->device_id_present = 1;
	req->device_id = device_id;
}
static inline void
dpll_device_get_req_set_device_name(struct dpll_device_get_req *req,
				    const char *device_name)
{
	req->device_name_present = 1;
	strncpy(req->device_name, device_name, sizeof(req->device_name));
	req->device_name[DPLL_NAME_LENGTH - 1] = 0;
}
static inline void
dpll_device_get_req_set_flags(struct dpll_device_get_req *req, __u32 flags)
{
	req->flags_present = 1;
	req->flags = flags;
}

struct dpll_device_get_rsp {
	__u32 device_id_present:1;
	__u32 device_name_present:1;
	__u32 status_present:1;
	__u32 temp_present:1;
	__u32 lock_status_present:1;

	__u32 device_id;
	char device_name[DPLL_NAME_LENGTH];
	unsigned int n_source;
	struct dpll_source *source;
	unsigned int n_output;
	struct dpll_output *output;
	enum dpll_genl_status status;
	__u32 temp;
	enum dpll_genl_lock_status lock_status;
};

void dpll_device_get_rsp_free(struct dpll_device_get_rsp *rsp);

/*
 * Get the device info. TODO..
 */
struct dpll_device_get_rsp *
dpll_device_get(struct ynl_sock *ys, struct dpll_device_get_req *req);

/* ============== DPLL_CMD_SET_SOURCE_TYPE ============== */
// DPLL_CMD_SET_SOURCE_TYPE - do
struct dpll_set_source_type_req {
	__u32 device_id_present:1;
	__u32 device_name_present:1;
	__u32 source_id_present:1;
	__u32 source_type_present:1;

	__u32 device_id;
	char device_name[DPLL_NAME_LENGTH];
	__u32 source_id;
	enum dpll_genl_signal_type source_type;
};

void dpll_set_source_type_req_free(struct dpll_set_source_type_req *req);
static inline void
dpll_set_source_type_req_set_device_id(struct dpll_set_source_type_req *req,
				       __u32 device_id)
{
	req->device_id_present = 1;
	req->device_id = device_id;
}
static inline void
dpll_set_source_type_req_set_device_name(struct dpll_set_source_type_req *req,
					 const char *device_name)
{
	req->device_name_present = 1;
	strncpy(req->device_name, device_name, sizeof(req->device_name));
	req->device_name[DPLL_NAME_LENGTH - 1] = 0;
}
static inline void
dpll_set_source_type_req_set_source_id(struct dpll_set_source_type_req *req,
				       __u32 source_id)
{
	req->source_id_present = 1;
	req->source_id = source_id;
}
static inline void
dpll_set_source_type_req_set_source_type(struct dpll_set_source_type_req *req,
					 enum dpll_genl_signal_type source_type)
{
	req->source_type_present = 1;
	req->source_type = source_type;
}

/*
 * Set the source type. TODO..
 */
int dpll_set_source_type(struct ynl_sock *ys,
			 struct dpll_set_source_type_req *req);

/* ============== DPLL_CMD_SET_OUTPUT_TYPE ============== */
// DPLL_CMD_SET_OUTPUT_TYPE - do
struct dpll_set_output_type_req {
	__u32 device_id_present:1;
	__u32 device_name_present:1;
	__u32 output_id_present:1;
	__u32 output_type_present:1;

	__u32 device_id;
	char device_name[DPLL_NAME_LENGTH];
	__u32 output_id;
	enum dpll_genl_signal_type output_type;
};

void dpll_set_output_type_req_free(struct dpll_set_output_type_req *req);
static inline void
dpll_set_output_type_req_set_device_id(struct dpll_set_output_type_req *req,
				       __u32 device_id)
{
	req->device_id_present = 1;
	req->device_id = device_id;
}
static inline void
dpll_set_output_type_req_set_device_name(struct dpll_set_output_type_req *req,
					 const char *device_name)
{
	req->device_name_present = 1;
	strncpy(req->device_name, device_name, sizeof(req->device_name));
	req->device_name[DPLL_NAME_LENGTH - 1] = 0;
}
static inline void
dpll_set_output_type_req_set_output_id(struct dpll_set_output_type_req *req,
				       __u32 output_id)
{
	req->output_id_present = 1;
	req->output_id = output_id;
}
static inline void
dpll_set_output_type_req_set_output_type(struct dpll_set_output_type_req *req,
					 enum dpll_genl_signal_type output_type)
{
	req->output_type_present = 1;
	req->output_type = output_type;
}

/*
 * Set the output type. TODO..
 */
int dpll_set_output_type(struct ynl_sock *ys,
			 struct dpll_set_output_type_req *req);

/* ============== DPLL_EVENT_DEVICE_CREATE ============== */
// DPLL_EVENT_DEVICE_CREATE - event
struct dpll_device_create_rsp {
	__u32 device_id_present:1;
	__u32 device_name_present:1;

	__u32 device_id;
	char device_name[DPLL_NAME_LENGTH];
};

struct dpll_device_create {
	__u16 family;
	__u8 cmd;
	void (*free)(struct dpll_device_create *ntf);
	struct dpll_device_create_rsp obj __attribute__ ((aligned (8)));
};

void dpll_device_create_free(struct dpll_device_create *rsp);

/* ============== DPLL_EVENT_DEVICE_DELETE ============== */
// DPLL_EVENT_DEVICE_DELETE - event
struct dpll_device_delete_rsp {
	__u32 device_id_present:1;
	__u32 device_name_present:1;

	__u32 device_id;
	char device_name[DPLL_NAME_LENGTH];
};

struct dpll_device_delete {
	__u16 family;
	__u8 cmd;
	void (*free)(struct dpll_device_delete *ntf);
	struct dpll_device_delete_rsp obj __attribute__ ((aligned (8)));
};

void dpll_device_delete_free(struct dpll_device_delete *rsp);

/* ============== DPLL_EVENT_STATUS_LOCKED ============== */
// DPLL_EVENT_STATUS_LOCKED - event
struct dpll_status_locked_rsp {
	__u32 device_id_present:1;
	__u32 lock_status_present:1;

	__u32 device_id;
	enum dpll_genl_lock_status lock_status;
};

struct dpll_status_locked {
	__u16 family;
	__u8 cmd;
	void (*free)(struct dpll_status_locked *ntf);
	struct dpll_status_locked_rsp obj __attribute__ ((aligned (8)));
};

void dpll_status_locked_free(struct dpll_status_locked *rsp);

/* ============== DPLL_EVENT_STATUS_UNLOCKED ============== */
// DPLL_EVENT_STATUS_UNLOCKED - event
struct dpll_status_unlocked_rsp {
	__u32 device_id_present:1;
	__u32 lock_status_present:1;

	__u32 device_id;
	enum dpll_genl_lock_status lock_status;
};

struct dpll_status_unlocked {
	__u16 family;
	__u8 cmd;
	void (*free)(struct dpll_status_unlocked *ntf);
	struct dpll_status_unlocked_rsp obj __attribute__ ((aligned (8)));
};

void dpll_status_unlocked_free(struct dpll_status_unlocked *rsp);

/* ============== DPLL_EVENT_SOURCE_CHANGE ============== */
// DPLL_EVENT_SOURCE_CHANGE - event
struct dpll_source_change_rsp {
	__u32 device_id_present:1;
	__u32 source_id_present:1;
	__u32 source_type_present:1;

	__u32 device_id;
	__u32 source_id;
	enum dpll_genl_signal_type source_type;
};

struct dpll_source_change {
	__u16 family;
	__u8 cmd;
	void (*free)(struct dpll_source_change *ntf);
	struct dpll_source_change_rsp obj __attribute__ ((aligned (8)));
};

void dpll_source_change_free(struct dpll_source_change *rsp);

/* ============== DPLL_EVENT_OUTPUT_CHANGE ============== */
// DPLL_EVENT_OUTPUT_CHANGE - event
struct dpll_output_change_rsp {
	__u32 device_id_present:1;
	__u32 output_id_present:1;
	__u32 output_type_present:1;

	__u32 device_id;
	__u32 output_id;
	enum dpll_genl_signal_type output_type;
};

struct dpll_output_change {
	__u16 family;
	__u8 cmd;
	void (*free)(struct dpll_output_change *ntf);
	struct dpll_output_change_rsp obj __attribute__ ((aligned (8)));
};

void dpll_output_change_free(struct dpll_output_change *rsp);

#endif /* _LINUX_DPLL_GEN_H */
