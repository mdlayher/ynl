#include <stdio.h>
#include <string.h>

#include <ynl.h>

#include "ethtool-user.h"

int main(int argc, char **argv)
{
	struct ethtool_channels_get_rsp *rsp;
	struct ethtool_channels_get_req req;
	struct ynl_sock *ys;

	if (argc < 2) {
		fprintf(stderr, "Usage: %s <interface>\n", argv[0]);
		return 1;
	}

	ys = ynl_sock_create("ethtool");
	if (!ys)
		return 1;

	memset(&req, 0, sizeof(req));
	req.header_present = 1;
	req.header.dev_name_present = 1;
	strcpy(req.header.dev_name, argv[1]);

	rsp = ethtool_channels_get(ys, &req);
	if (!rsp)
		goto out;

	printf("%s:", argv[1]);
	if (rsp->rx_count_present && rsp->tx_count_present)
		printf("\trx: %u\ttx:%u", rsp->rx_count, rsp->tx_count);
	if (rsp->combined_count)
		printf("\tcombined: %u", rsp->combined_count);
	if (rsp->other_count)
		printf("\tother: %u", rsp->other_count);
	printf("\n");

	ethtool_channels_get_rsp_free(rsp);
	ynl_sock_destroy(ys);
	return 0;

out:
	perror("can't get the channels");
	ynl_sock_destroy(ys);
	return 2;
}
