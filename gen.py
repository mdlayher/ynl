#!/usr/bin/env python

import argparse
import os
import yaml


class Family:
    def __init__(self, file_name):
        with open(file_name, "r") as stream:
            self.yaml = yaml.safe_load(stream)

        self.root_spaces = set()
        self.pure_nested_spaces = set()
        self.inherited_members = dict()

        self._compute_members()
        self._load_root_spaces()
        self._load_nested_spaces()

    def __getitem__(self, key):
        return self.yaml[key]

    def _compute_members(self):
        for _, space in self.yaml['attributes']['spaces'].items():
            for name, attr in space['list'].items():
                c_name = name
                if c_name in c_kw:
                    c_name += '_'
                attr['c_name'] = c_name

    def _load_root_spaces(self):
        for op_name, op in self.yaml['operations']['list'].items():
            self.root_spaces.add(op['attribute-space'])

    def _load_nested_spaces(self):
        for root_space in self.root_spaces:
            for attr, spec in self.yaml['attributes']['spaces'][root_space]['list'].items():
                if 'nested-attributes' in spec:
                    nested = spec['nested-attributes']
                    if nested not in self.root_spaces:
                        self.pure_nested_spaces.add(nested)
                    if 'type-value' in spec:
                        tv_set = set(spec['type-value'])
                        if nested in self.root_spaces:
                            raise Exception("Inheriting members to a space used as root not supported")
                        if nested in self.inherited_members and self.inherited_members[nested] != tv_set:
                            raise Exception("Inheriting different members not supported")
                        self.inherited_members[nested] = tv_set
                    elif spec['type'] == 'array-nest':
                        self.inherited_members[nested] = {'idx'}
                    else:
                        self.inherited_members[nested] = set()


class RenderInfo:
    def __init__(self, family, ku_space, op, op_name, op_mode, attr_space=None):
        self.family = family
        self.ku_space = ku_space
        self.op = op
        self.op_name = op_name
        self.op_mode = op_mode
        self.attr_space = attr_space
        if not self.attr_space:
            self.attr_space = op['attribute-space']

        if op:
            self.type_name = op_name
        else:
            self.type_name = attr_space.replace('-', '_')


scalars = {'u8', 'u16', 'u32', 'u64', 's64'}

direction_to_suffix = {
    'reply': '_rsp',
    'request': '_req',
    '': ''
}

c_kw = {
    'do'
}

# TODO: render interfaces


def rdir(direction):
    if direction == 'reply':
        return 'request'
    if direction == 'request':
        return 'reply'
    return direction


def op_enum_name(ri):
    return f"{ri.family['operations']['name-prefix']}{ri.op_name.upper()}"


def attr_enum_name(ri, attr):
    return f"{ri.family['attributes']['spaces'][ri.attr_space]['name-prefix']}{attr.upper()}"


def op_prefix(ri, direction):
    suffix = f'_{ri.type_name}{direction_to_suffix[direction]}'
    return f"{ri.family['name']}{suffix}"


def op_aspec(ri, attr):
    aspace = ri.family["attributes"]["spaces"][ri.op['attribute-space']]
    return aspace["list"][attr]


def type_name(ri, direction):
    return f"struct {op_prefix(ri, direction)}"


def nest_op_prefix(ri, attr_space):
    return f"{ri.family['name']}_{attr_space.replace('-', '_')}"


def nest_type_name(ri, attr_space):
    return f"struct {nest_op_prefix(ri, attr_space)}"


def attribute_policy(family, space, attr, prototype=True, suffix=""):
    aspace = family["attributes"]["spaces"][space]
    spec = aspace["list"][attr]
    policy = 'NLA_' + spec['type'].upper()
    policy = policy.replace('-', '_')

    mem = '{ .type = ' + policy
    if 'len' in spec:
        mem += ', .len = ' + spec['len']
    mem += ' }'

    print(f"\t[{aspace['name-prefix']}{attr.upper()}] = {mem},")


def _attribute_member_len(spec):
    if spec['len'][-4:] == " - 1":
        return f"{spec['len'][:-4]}"
    else:
        return f"{spec['len']} + 1"


def _attribute_member(ri, space, attr, prototype=True, suffix=""):
    spec = ri.family["attributes"]["spaces"][space]["list"][attr]

    t = spec['type']
    if t == "nul-string":
        if prototype:
            t = 'const char *'
        else:
            t = 'char '
            suffix = f'[{_attribute_member_len(spec)}]{suffix}'
    elif t == 'array-nest':
        t = f"struct {ri.family['name']}_{spec['nested-attributes']} *"
        if not prototype:
            print(f"\tunsigned int n_{attr};")
    elif t in scalars:
        pfx = '__' if ri.ku_space == 'user' else ''
        t = pfx + t + ' '

    return f"{t}{spec['c_name']}{suffix}"


def attribute_member(ri, space, attr, prototype=True, suffix=""):
    print(f"\t{_attribute_member(ri, space, attr, prototype, suffix)}")


def attribute_pres_member(ri, space, attr, suffix=""):
    spec = ri.family["attributes"]["spaces"][space]["list"][attr]
    pfx = '__' if ri.ku_space == 'user' else ''

    if spec['type'] == 'array-nest':
        return False

    print(f"\t{pfx}u32 {attr}_present:1{suffix}")
    return True


def attribute_setter(ri, space, attr, direction):
    spec = ri.family["attributes"]["spaces"][space]["list"][attr]
    var = "req"

    if spec['type'] in scalars:
        pass
    elif spec['type'] == 'nul-string':
        pass
    else:
        return

    print('static inline void')
    print(f'{op_prefix(ri, direction)}_set_{attr}({type_name(ri, direction)} *{var}, ' +
          f'{_attribute_member(ri, space, attr, prototype=True)})')
    print('{')
    print(f'\t{var}->{attr}_present = 1;')
    if spec['type'] in scalars:
        print(f'\t{var}->{attr} = {attr};')
    elif spec['type'] == 'nul-string':
        print(f'\tstrncpy({var}->{attr}, {attr}, sizeof({var}->{attr}));')
        print(f'\t{var}->{attr}[{spec["len"]}] = 0;')
    print('}')


def attribute_put(ri, attr, var):
    spec = ri.family["attributes"]["spaces"][ri.attr_space]["list"][attr]

    if spec['type'] in scalars:
        t = spec['type']
        # mnl does not have a helper for signed types
        if t[0] == 's':
            t = 'u' + t[1:]
    elif spec['type'] == 'nul-string':
        t = 'strz'
    else:
        return

    print(f"\tif ({var}->{attr}_present)")
    print(f"\t\tmnl_attr_put_{t}(nlh, {attr_enum_name(ri, attr)}, {var}->{attr});")


def attribute_get(ri, attr, var):
    spec = ri.family["attributes"]["spaces"][ri.attr_space]["list"][attr]

    if spec['type'] in scalars:
        # mnl does not have a helper for signed types
        t = spec['type']
        if t[0] == 's':
            t = 'u' + t[1:]
        get_lines = [f"{var}->{spec['c_name']} = mnl_attr_get_{t}(attr);"]
    elif spec['type'] == 'nul-string':
        get_lines = [f"strncpy({var}->{spec['c_name']}, mnl_attr_get_str(attr), {spec['len']});",
                     f"{var}->{spec['c_name']}[{spec['len']}] = 0;"]
    elif spec['type'] == 'array-nest':
        get_lines = ['const struct nlattr *attr2;',
                     '',
                     f'attr_{attr} = attr;',
                     'mnl_attr_for_each_nested(attr2, attr)',
                     f'\t{var}->n_{attr}++;']
    else:
        return

    print(f"\t\tif (mnl_attr_get_type(attr) == {attr_enum_name(ri, attr)}) {'{'}")
    if spec['type'] != 'array-nest':
        print(f"\t\t\t{var}->{attr}_present = 1;")
    for l in get_lines:
        print('\t\t\t' + l if l else "")
    print('\t\t}')


def attribute_parse_kernel(family, space, attr, prototype=True, suffix=""):
    aspace = family["attributes"]["spaces"][space]
    spec = aspace["list"][attr]

    t = spec['type']
    print(f"\tif (tb[{aspace['name-prefix']}{attr.upper()}]) " + '{')
    print(f"\t\treq->{attr}_present = 1;")
    if t in scalars:
        print(f"\t\treq->{attr} = nla_get_{t}(tb[{aspace['name-prefix']}{attr.upper()}]);")
    elif t == 'nul-string':
        print(f"\t\tstrcpy(req->{attr}, nla_data(tb[{aspace['name-prefix']}{attr.upper()}]));")
    print('\t}')


def print_prototype(ri, direction, terminate=True):
    suffix = ');' if terminate else ')'

    print(f"{type_name(ri, rdir(direction))} *")
    print(f"{ri.family['name']}_{ri.op_name}(struct ynl_sock *ys, {type_name(ri, direction)} *" +
          f"{direction_to_suffix[direction][1:]}{suffix}")


def print_req_prototype(ri):
    print_prototype(ri, "request")


def print_rsp_nested(ri, attr_space):
    struct_type = nest_type_name(ri, attr_space)

    extra_args = ''
    for arg in ri.family.inherited_members[attr_space]:
        extra_args += f", __u32 {arg}"

    print(f"int {nest_op_prefix(ri, attr_space)}_parse({struct_type} *dst,")
    print(f"\tconst struct nlattr *nested{extra_args})")
    print('{')
    print('\tconst struct nlattr *attr;')
    print()

    for arg in ri.family.inherited_members[attr_space]:
        print(f'\tdst->{arg} = {arg};')
    if ri.family.inherited_members[attr_space]:
        print()

    print("\tmnl_attr_for_each_nested(attr, nested) {")

    for arg in ri.family['attributes']['spaces'][attr_space]['list']:
        attribute_get(ri, arg, "dst")

    print('\t}')
    print()
    print('\treturn 0;')
    print('}')
    print()


def print_req(ri):
    direction = "request"
    print_prototype(ri, direction, terminate=False)
    print('{')
    local_vars = [f'{type_name(ri, rdir(direction))} *rsp;',
                  'const struct nlattr *attr;',
                  'struct nlmsghdr *nlh;',
                  'int len, err;']

    array_nests = set()
    for arg in ri.op[ri.op_mode]["reply"]['attributes']:
        aspec = op_aspec(ri, arg)
        if aspec['type'] == 'array-nest':
            local_vars.append(f'const struct nlattr *attr_{arg};')
            array_nests.add(arg)
    if array_nests:
        local_vars.append('int i;')

    for var in local_vars:
        print(f'\t{var}')
    if local_vars:
        print()

    print(f"\tnlh = ynl_gemsg_start_req(ys, GENL_ID_CTRL, {op_enum_name(ri)}, 1);")
    print()

    for arg in ri.op[ri.op_mode]["request"]['attributes']:
        attribute_put(ri, arg, "req")
    print("""
	err = mnl_socket_sendto(ys->sock, nlh, nlh->nlmsg_len);
	if (err < 0)
		return NULL;

	len = mnl_socket_recvfrom(ys->sock, ys->buf, MNL_SOCKET_BUFFER_SIZE);
	if (len < 0)
		return NULL;

	nlh = (struct nlmsghdr *)ys->buf;
	if (!mnl_nlmsg_ok(nlh, len) || (unsigned int)len != nlh->nlmsg_len)
		return NULL;

	rsp = calloc(1, sizeof(*rsp));

	mnl_attr_for_each(attr, nlh, sizeof(struct genlmsghdr)) {""")

    for arg in ri.op[ri.op_mode]["reply"]['attributes']:
        attribute_get(ri, arg, "rsp")

    print("\t}")

    if array_nests:
        print()
        for anest in sorted(array_nests):
            aspec = op_aspec(ri, anest)
            print(f'\t// {anest}')
            print(f"\trsp->{anest} = calloc(rsp->n_{anest}, " +
                  f"sizeof(struct {ri.family['name']}_{aspec['nested-attributes']}));")
            print('\ti = 0;')
            print(f"\tmnl_attr_for_each_nested(attr, attr_{anest})" + ' {')
            print(f"\t\t{ri.family['name']}_{aspec['nested-attributes']}_parse(&rsp->ops[i], attr, i);")
            print('\t\ti++;')
            print('\t}')
        print()

    print('\treturn rsp;')
    print('}')


def _print_type(ri, direction, type_list, inherited_list={}):
    suffix = f'_{ri.type_name}{direction_to_suffix[direction]}'

    print(f"struct {ri.family['name']}{suffix} " + '{')
    any_presence = False
    for arg in type_list:
        any_presence |= \
            attribute_pres_member(ri, ri.attr_space, arg, suffix=';')
    if any_presence:
        print()

    for arg in sorted(inherited_list):
        print(f"\t__u32 {arg};")

    for arg in type_list:
        attribute_member(ri, ri.attr_space, arg, prototype=False, suffix=';')
    print("};")
    print()


def print_type(ri, direction):
    return _print_type(ri, direction, ri.op[ri.op_mode][direction]['attributes'])


def print_type_full(ri, attr_space):
    types = ri.family['attributes']['spaces'][attr_space]['list']
    return _print_type(ri, "", types, ri.family.inherited_members[attr_space])


def print_type_helpers(ri, direction):
    type_list = ri.op[ri.op_mode][direction]['attributes']
    suffix = f'_{ri.type_name}{direction_to_suffix[direction]}'

    if ri.ku_space == 'user' and direction == 'request':
        for arg in type_list:
            attribute_setter(ri, ri.attr_space, arg, direction)
        if type_list:
            print()

    print(f"void {ri.family['name']}{suffix}_free(" +
          f"struct {ri.family['name']}{suffix} *req);")


def print_req_type_helpers(ri):
    print_type_helpers(ri, "request")


def print_rsp_type_helpers(ri):
    print_type_helpers(ri, "reply")


def print_parse_prototype(ri, direction, terminate=True):
    suffix = "_rsp" if direction == "reply" else "_req"
    term = ';' if terminate else ''

    print(f"void {ri.family['name']}_{ri.op_name}{suffix}_parse(const struct nlattr **tb," +
          f" struct {ri.family['name']}_{ri.op_name}{suffix} *req){term}")


def print_parse_kernel(ri, direction):
    print_parse_prototype(ri, direction, terminate=False)
    print('{')
    for arg in ri.op[ri.op_mode][direction]['attributes']:
        attribute_parse_kernel(ri.family, ri.attr_space, arg, prototype=False, suffix=';')
    print("}")


def print_req_type(ri):
    print_type(ri, "request")


def print_rsp_type(ri):
    print_type(ri, "reply")


def print_req_policy_fwd(ri, terminate=True):
    suffix = ';' if terminate else ' = {'
    print(f"const struct nla_policy {ri.family['name']}_{ri.op_name}_policy[]{suffix}")


def print_req_policy(ri):
    print_req_policy_fwd(ri, terminate=False)
    for arg in ri.op[ri.op_mode]["request"]['attributes']:
        attribute_policy(ri.family, ri.attr_space, arg)
    print("};")


def main():
    parser = argparse.ArgumentParser(description='Netlink simple parsing generator')
    parser.add_argument('--mode', dest='mode', type=str, required=True)
    parser.add_argument('--spec', dest='spec', type=str, required=True)
    parser.add_argument('--header', dest='header', action='store_true', default=None)
    parser.add_argument('--source', dest='header', action='store_false')
    parser.add_argument('--user-header', nargs='+', default=[])
    args = parser.parse_args()

    if args.header is None:
        parser.error("--header or --source is required")

    try:
        parsed = Family(args.spec)
    except yaml.YAMLError as exc:
        print(exc)
        os.sys.exit(1)
        return

    print(f"// SPDX-License-Identifier: MIT")
    print("// Do not edit directly, auto-generated from:")
    print(f"//\t{args.spec}")
    print()
    if args.mode == 'kernel':
        print(f'#include <net/netlink.h>')
        print()
    print(f"#include <{parsed['headers'][args.mode]}>\n")

    if args.mode == "user":
        if not args.header:
            print("#include <stdlib.h>")
            print("#include <stdio.h>")
            print("#include <string.h>")
            print("#include <libmnl/libmnl.h>")
            print()
            for h in args.user_header:
                print(f'#include "{h}"')
        else:
            print('struct ynl_sock;')
        print()

    if args.header:
        if args.mode == "user":
            print('// Common nested types')
            for attr_space in sorted(parsed.pure_nested_spaces):
                ri = RenderInfo(parsed, args.mode, "", "", "", attr_space)
                print_type_full(ri, attr_space)

        for op_name, op in parsed['operations']['list'].items():
            print(f"// {parsed['operations']['name-prefix']}{op_name.upper()}")

            if op and "do" in op:
                ri = RenderInfo(parsed, args.mode, op, op_name, "do")

                print_req_type(ri)
                print_req_type_helpers(ri)
                print()
                print_rsp_type(ri)
                print_rsp_type_helpers(ri)
                print()

                if args.mode == "user":
                    print_req_prototype(ri)
                elif args.mode == "kernel":
                    print_parse_prototype(ri, "request")
                    print_req_policy_fwd(ri)
                print()
    else:
        if args.mode == "user":
            print('// Common nested types')
            for attr_space in sorted(parsed.pure_nested_spaces):
                ri = RenderInfo(parsed, args.mode, "", "", "", attr_space)
                print_rsp_nested(ri, attr_space)

        for op_name, op in parsed['operations']['list'].items():
            print(f"// {parsed['operations']['name-prefix']}{op_name.upper()}")

            if op and "do" in op:
                ri = RenderInfo(parsed, args.mode, op, op_name, "do")
                if args.mode == "user":
                    print_req(ri)
                elif args.mode == "kernel":
                    print_parse_kernel(ri, "request")
                    print()
                    print_req_policy(ri)
                print()


if __name__ == "__main__":
    main()
