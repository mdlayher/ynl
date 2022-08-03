// SPDX-License-Identifier: MIT
// Do not edit directly, auto-generated from:
//	../../../../Documentation/netlink/bindings/dpll.yaml
// /home/kicinski/devel/linux/gen.py --mode user --user-header ynl.h --header --spec ../../../../Documentation/netlink/bindings/dpll.yaml

#include <linux/dpll.h>

struct ynl_sock;

// Common nested types
struct dpll_output {
	__u32 source_id_present:1;
	__u32 source_type_present:1;

	__u32 source_id;
	__u32 source_type;
	unsigned int n_source_supported;
	__u32 *source_supported;
};

struct dpll_source {
	__u32 output_id_present:1;
	__u32 output_type_present:1;

	__u32 output_id;
	__u32 output_type;
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
	__u32 status;
	__u32 temp;
	__u32 lock_status;
};

void dpll_device_get_rsp_free(struct dpll_device_get_rsp *rsp);

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
	__u32 source_type;
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
					 __u32 source_type)
{
	req->source_type_present = 1;
	req->source_type = source_type;
}

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
	__u32 output_type;
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
					 __u32 output_type)
{
	req->output_type_present = 1;
	req->output_type = output_type;
}

int dpll_set_output_type(struct ynl_sock *ys,
			 struct dpll_set_output_type_req *req);
