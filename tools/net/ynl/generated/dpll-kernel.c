// SPDX-License-Identifier: MIT
// Do not edit directly, auto-generated from:
//	../../../../Documentation/netlink/bindings/dpll.yaml
// /home/kicinski/devel/linux/gen.py --mode kernel --source --spec ../../../../Documentation/netlink/bindings/dpll.yaml

#include <net/netlink.h>

#include <linux/dpll.h>

/* ============== DPLL_CMD_DEVICE_GET ============== */
// DPLL_CMD_DEVICE_GET - do
void
dpll_device_get_req_parse(const struct nlattr **tb,
			  struct dpll_device_get_req *req)
{
	if (tb[DPLLA_DEVICE_ID]) {
		req->device_id_present = 1;
		req->device_id = nla_get_u32(tb[DPLLA_DEVICE_ID]);
	}
	if (tb[DPLLA_DEVICE_NAME]) {
		req->device_name_present = 1;
		strcpy(req->device_name, nla_data(tb[DPLLA_DEVICE_NAME]));
	}
	if (tb[DPLLA_FLAGS]) {
		req->flags_present = 1;
		req->flags = nla_get_u32(tb[DPLLA_FLAGS]);
	}
}

const struct nla_policy dpll_device_get_policy[] = {
	[DPLLA_DEVICE_ID] = { .type = NLA_U32 },
	[DPLLA_DEVICE_NAME] = { .type = NLA_NUL_STRING, .len = DPLL_NAME_LENGTH - 1 },
	[DPLLA_FLAGS] = NLA_POLICY_MASK(NLA_U32, 0x7),
};

/* ============== DPLL_CMD_SET_SOURCE_TYPE ============== */
// DPLL_CMD_SET_SOURCE_TYPE - do
void
dpll_set_source_type_req_parse(const struct nlattr **tb,
			       struct dpll_set_source_type_req *req)
{
	if (tb[DPLLA_DEVICE_ID]) {
		req->device_id_present = 1;
		req->device_id = nla_get_u32(tb[DPLLA_DEVICE_ID]);
	}
	if (tb[DPLLA_DEVICE_NAME]) {
		req->device_name_present = 1;
		strcpy(req->device_name, nla_data(tb[DPLLA_DEVICE_NAME]));
	}
	if (tb[DPLLA_SOURCE_ID]) {
		req->source_id_present = 1;
		req->source_id = nla_get_u32(tb[DPLLA_SOURCE_ID]);
	}
	if (tb[DPLLA_SOURCE_TYPE]) {
		req->source_type_present = 1;
		req->source_type = nla_get_u32(tb[DPLLA_SOURCE_TYPE]);
	}
}

const struct nla_policy dpll_set_source_type_policy[] = {
	[DPLLA_DEVICE_ID] = { .type = NLA_U32 },
	[DPLLA_DEVICE_NAME] = { .type = NLA_NUL_STRING, .len = DPLL_NAME_LENGTH - 1 },
	[DPLLA_SOURCE_ID] = { .type = NLA_U32 },
	[DPLLA_SOURCE_TYPE] = { .type = NLA_U32 },
};

/* ============== DPLL_CMD_SET_OUTPUT_TYPE ============== */
// DPLL_CMD_SET_OUTPUT_TYPE - do
void
dpll_set_output_type_req_parse(const struct nlattr **tb,
			       struct dpll_set_output_type_req *req)
{
	if (tb[DPLLA_DEVICE_ID]) {
		req->device_id_present = 1;
		req->device_id = nla_get_u32(tb[DPLLA_DEVICE_ID]);
	}
	if (tb[DPLLA_DEVICE_NAME]) {
		req->device_name_present = 1;
		strcpy(req->device_name, nla_data(tb[DPLLA_DEVICE_NAME]));
	}
	if (tb[DPLLA_OUTPUT_ID]) {
		req->output_id_present = 1;
		req->output_id = nla_get_u32(tb[DPLLA_OUTPUT_ID]);
	}
	if (tb[DPLLA_OUTPUT_TYPE]) {
		req->output_type_present = 1;
		req->output_type = nla_get_u32(tb[DPLLA_OUTPUT_TYPE]);
	}
}

const struct nla_policy dpll_set_output_type_policy[] = {
	[DPLLA_DEVICE_ID] = { .type = NLA_U32 },
	[DPLLA_DEVICE_NAME] = { .type = NLA_NUL_STRING, .len = DPLL_NAME_LENGTH - 1 },
	[DPLLA_OUTPUT_ID] = { .type = NLA_U32 },
	[DPLLA_OUTPUT_TYPE] = { .type = NLA_U32 },
};
