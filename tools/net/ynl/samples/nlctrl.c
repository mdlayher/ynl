#include "ynl.h"

#include "fou-user.h"
#include "genetlink-user.h"

int main(int argc, char **argv)
{
	struct nlctrl_getfamily_list *families, *f;
	struct nlctrl_getfamily_rsp *rsp;
	struct nlctrl_getfamily_req req;
	struct nlctrl_getpolicy_req_dump pdr;
	struct nlctrl_getpolicy_rsp_list *policies, *p;
	unsigned int i, fam_id;
	struct ynl_sock *ys;

	if (argc < 2)
		return err_ret(1, "Usage: %s <family_name>\n", argv[0]);

	ys = ynl_sock_create("nlctrl");
	if (!ys)
		return 1;

	memset(&req, 0, sizeof(req));
	nlctrl_getfamily_req_set_family_name(&req, argv[1]);

	rsp = nlctrl_getfamily(ys, &req);
	if (!rsp || !rsp->family_id_present)
		goto out;

	fam_id = rsp->family_id;

	if (rsp->family_id_present && rsp->family_name_present) {
		printf("YS response family id %u name '%s' n_ops %d n_mcast: %d\n",
		       rsp->family_id, rsp->family_name,
		       rsp->n_ops, rsp->n_mcast_groups);
		for (i = 0; i < rsp->n_ops; i++)
			printf("\top[%d]: cmd:%d flags:%x\n",
			       rsp->ops[i].idx, rsp->ops[i].id,
			       rsp->ops[i].flags);
		for (i = 0; i < rsp->n_mcast_groups; i++)
			printf("\tmcast_grp[%d]: id:%d name:%s\n",
			       rsp->mcast_groups[i].idx,
			       rsp->mcast_groups[i].id,
			       rsp->mcast_groups[i].name);
	}
	nlctrl_getfamily_rsp_free(rsp);

	families = nlctrl_getfamily_dump(ys);
	if (!families)
		goto out;

	printf("\nFAMILIES:\n");
	for (f = families; f; f = f->next) {
		rsp = &f->obj;

		if (rsp->family_id_present && rsp->family_name_present)
			printf("\t[%u] '%s' (%d)\n",
			       rsp->family_id, rsp->family_name, rsp->n_ops);
		else
			printf("\tSKIP\n");
	}
	nlctrl_getfamily_list_free(families);

	printf("\nPOLICY");
	memset(&pdr, 0, sizeof(pdr));

	nlctrl_getpolicy_req_dump_set_family_id(&pdr, fam_id);

	policies = nlctrl_getpolicy_dump(ys, &pdr);
	if (!policies)
		goto out;

	for (p = policies; p; p = p->next) {
		if (!p->obj.policy_present)
			continue;
		if (!p->obj.policy.type_present) {
			printf("\t[%d, %d] -- no type\n",
			       p->obj.policy.attr_idx,
			       p->obj.policy.current_policy_idx);
			continue;
		}
		printf("\t[%d, %d] %d\n", p->obj.policy.attr_idx,
		       p->obj.policy.current_policy_idx, p->obj.policy.type);
	}
	nlctrl_getpolicy_rsp_list_free(policies);
out:
	ynl_sock_destroy(ys);

	return 0;
}
