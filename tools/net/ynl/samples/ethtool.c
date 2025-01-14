#include <stdio.h>
#include <string.h>

#include <ynl.h>

#include "ethtool-user.h"

static void print_device_config(struct ethtool_channels_get_rsp *rsp)
{
	if (rsp->header_present && rsp->header.dev_name_present)
		printf("%16s:", rsp->header.dev_name);
	if (rsp->rx_count_present && rsp->tx_count_present)
		printf("\trx: %u\ttx:%u", rsp->rx_count, rsp->tx_count);
	if (rsp->combined_count)
		printf("\tcombined: %u", rsp->combined_count);
	if (rsp->other_count)
		printf("\tother: %u", rsp->other_count);
	printf("\n");
}

int main(int argc, char **argv)
{
	struct ethtool_channels_get_rsp *rsp;
	struct ethtool_channels_get_req req;
	struct ynl_ntf_base_type *ntf;
	struct ynl_error yse;
	struct ynl_sock *ys;

	if (argc < 2) {
		fprintf(stderr, "Usage: %s <interface>\n", argv[0]);
		return 1;
	}

	ys = ynl_sock_create("ethtool", &yse);
	if (!ys) {
		fprintf(stderr, "YNL init: %s\n", yse.msg);
		return 1;
	}

	memset(&req, 0, sizeof(req));
	ethtool_channels_get_req_set_header_dev_index(&req, 21312);

	rsp = ethtool_channels_get(ys, &req);
	if (rsp)
		fprintf(stderr, "Unexpected success with bad ifindex\n");
	else
		fprintf(stderr, "ext ack test: %s\n", ys->err.msg);

	memset(&req, 0, sizeof(req));

	rsp = ethtool_channels_get(ys, &req);
	if (rsp)
		fprintf(stderr, "Unexpected success with no header\n");
	else
		fprintf(stderr, "ext ack test: %s\n", ys->err.msg);

	memset(&req, 0, sizeof(req));
	ethtool_channels_get_req_set_header_dev_name(&req, argv[1]);

	rsp = ethtool_channels_get(ys, &req);
	if (!rsp)
		goto out;

	print_device_config(rsp);
	ethtool_channels_get_rsp_free(rsp);

	if (argc == 2)
		goto done;

	if (ynl_subscribe(ys, ETHTOOL_MCGRP_MONITOR_NAME))
		goto out;

	ntf = ethtool_ntf_parse(ys);
	if (!ntf)
		goto out;

	if (ntf->cmd == ETHTOOL_MSG_CHANNELS_NTF) {
		rsp = (struct ethtool_channels_get_rsp *)&ntf->data;
		print_device_config(rsp);
	} else {
		fprintf(stderr, "Unknown msg type: %d\n", ntf->cmd);
	}
	ynl_ntf_free(ntf);

done:
	ynl_sock_destroy(ys);
	return 0;

out:
	fprintf(stderr, "Getting channels failed: %s\n", ys->err.msg);
	ynl_sock_destroy(ys);
	return 2;
}
