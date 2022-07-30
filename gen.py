#!/usr/bin/env python

import argparse
import os
import yaml


class BaseNlLib:
    def __init__(self):
        pass

    def get_family_id(self):
        return 'ys->family_id'


class Family:
    def __init__(self, file_name):
        with open(file_name, "r") as stream:
            self.yaml = yaml.safe_load(stream)

        self.root_spaces = set()
        self.pure_nested_spaces = set()
        self.inherited_members = dict()
        self.all_notify = dict()

        self._compute_members()
        self._load_root_spaces()
        self._load_nested_spaces()
        self._load_all_notify()

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

    def _load_all_notify(self):
        for op_name, op in self.yaml['operations']['list'].items():
            if 'notify' in op:
                self.all_notify[op_name] = op['notify']['cmds']


class RenderInfo:
    def __init__(self, cw, family, ku_space, op, op_name, op_mode, attr_space=None):
        self.family = family
        self.nl = cw.nlib
        self.ku_space = ku_space
        self.op = op
        self.op_name = op_name
        self.op_mode = op_mode

        # 'do' and 'dump' response parsing is identical
        if op_mode != 'do' and 'do' in op and 'reply' in op['do'] and \
           op["do"]["reply"] == op["dump"]["reply"]:
            self.type_consistent = True
        else:
            self.type_consistent = False

        self.attr_space = attr_space
        if not self.attr_space:
            self.attr_space = op['attribute-space']

        if op:
            self.type_name = op_name
        else:
            self.type_name = attr_space.replace('-', '_')

        self.cw = cw


class CodeWriter:
    def __init__(self, nlib):
        self.nlib = nlib

        self._nl = False
        self._ind = 0

    def p(self, line):
        if self._nl:
            print()
            self._nl = False
        ind = self._ind
        if line[-1] == ':':
            ind -= 1
        print('\t' * ind + line)

    def nl(self):
        self._nl = True

    def block_start(self, line=''):
        if line:
            line = line + ' '
        self.p(line + '{')
        self._ind += 1

    def block_end(self, line=''):
        self._ind -= 1
        self.p('}' + line)

    def write_func_prot(self, qual_ret, name, args=None, suffix=''):
        if not args:
            args = ['void']

        oneline = qual_ret
        if qual_ret[-1] != '*':
            oneline += ' '
        oneline += f"{name}({', '.join(args)}){suffix}"

        if len(oneline) < 80:
            self.p(oneline)
            return

        v = qual_ret
        if len(v) > 3:
            self.p(v)
            v = ''
        elif qual_ret[-1] != '*':
            v += ' '
        v += name + '('
        ind = '\t' * (len(v) // 8) + ' ' * (len(v) % 8)
        delta_ind = len(v) - len(ind)
        v += args[0]
        i = 1
        while i < len(args):
            next_len = len(v) + len(args[i])
            if v[0] == '\t':
                next_len += delta_ind
            if next_len > 76:
                self.p(v + ',')
                v = ind
            else:
                v += ', '
            v += args[i]
            i += 1
        self.p(v + ')' + suffix)

    def write_func_lvar(self, local_vars):
        if not local_vars:
            return

        if type(local_vars) is str:
            local_vars = [local_vars]

        local_vars.sort(key=len, reverse=True)
        for var in local_vars:
            self.p(var)
        self.nl()


scalars = {'u8', 'u16', 'u32', 'u64', 's64'}

direction_to_suffix = {
    'reply': '_rsp',
    'request': '_req',
    '': ''
}

op_mode_to_wrapper = {
    'do': '',
    'dump': '_list',
    'notify': '_ntf',
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


def op_prefix(ri, direction, deref=False):
    suffix = f"_{ri.type_name.replace('-', '_')}"

    if not ri.op_mode or ri.op_mode == 'do':
        suffix += f"{direction_to_suffix[direction]}"
    else:
        if direction == 'request':
            suffix += '_req_dump'
        else:
            if ri.type_consistent:
                if deref:
                    suffix += f"{direction_to_suffix[direction]}"
                else:
                    suffix += op_mode_to_wrapper[ri.op_mode]
            else:
                suffix += '_rsp'
                suffix += '_dump' if deref else '_list'

    return f"{ri.family['name']}{suffix}"


def op_aspec(ri, attr):
    aspace = ri.family["attributes"]["spaces"][ri.op['attribute-space']]
    return aspace["list"][attr]


def type_name(ri, direction, deref=False):
    return f"struct {op_prefix(ri, direction, deref=deref)}"


def nest_op_prefix(ri, attr_space):
    return f"{ri.family['name']}_{attr_space.replace('-', '_')}"


def nest_type_name(ri, attr_space):
    return f"struct {nest_op_prefix(ri, attr_space)}"


def attribute_policy(ri, attr, prototype=True, suffix=""):
    aspace = ri.family["attributes"]["spaces"][ri.attr_space]
    spec = aspace["list"][attr]
    policy = 'NLA_' + spec['type'].upper()
    policy = policy.replace('-', '_')

    mem = '{ .type = ' + policy
    if 'len' in spec:
        mem += ', .len = ' + spec['len']
    mem += ' }'

    ri.cw.p(f"\t[{aspace['name-prefix']}{attr.upper()}] = {mem},")


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
        t = f"struct {nest_op_prefix(ri, spec['nested-attributes'])} *"
        if not prototype:
            ri.cw.p(f"unsigned int n_{attr};")
    elif t == 'nest-type-value':
        t = f"struct {nest_op_prefix(ri, spec['nested-attributes'])} "
    elif t in scalars:
        pfx = '__' if ri.ku_space == 'user' else ''
        t = pfx + t + ' '
    else:
        raise Exception(f'Type {t} not supported yet')

    return [f"{t}{spec['c_name']}{suffix}"]


def attribute_member(ri, space, attr, prototype=True, suffix=""):
    members = _attribute_member(ri, space, attr, prototype, suffix)
    for line in members:
        ri.cw.p(line)


def attribute_pres_member(ri, space, attr, suffix=""):
    spec = ri.family["attributes"]["spaces"][space]["list"][attr]
    pfx = '__' if ri.ku_space == 'user' else ''

    if spec['type'] == 'array-nest':
        return False

    ri.cw.p(f"{pfx}u32 {attr}_present:1{suffix}")
    return True


def attribute_setter(ri, space, attr, direction, deref=False):
    spec = ri.family["attributes"]["spaces"][space]["list"][attr]
    var = "req"

    if spec['type'] in scalars:
        pass
    elif spec['type'] == 'nul-string':
        pass
    else:
        return

    ri.cw.write_func_prot('static inline void',
                          f'{op_prefix(ri, direction, deref=deref)}_set_{attr}',
                          [f'{type_name(ri, direction, deref=deref)} *{var}'] +
                          _attribute_member(ri, space, attr, prototype=True))
    ri.cw.block_start()
    ri.cw.p(f'{var}->{attr}_present = 1;')
    if spec['type'] in scalars:
        ri.cw.p(f'{var}->{attr} = {attr};')
    elif spec['type'] == 'nul-string':
        ri.cw.p(f'strncpy({var}->{attr}, {attr}, sizeof({var}->{attr}));')
        ri.cw.p(f'{var}->{attr}[{spec["len"]}] = 0;')
    ri.cw.block_end()


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
        raise Exception(f"Type {spec['type']} not supported yet")

    ri.cw.p(f"if ({var}->{attr}_present)")
    ri.cw.p(f"\tmnl_attr_put_{t}(nlh, {attr_enum_name(ri, attr)}, {var}->{attr});")


def attribute_get(ri, attr, var):
    spec = ri.family["attributes"]["spaces"][ri.attr_space]["list"][attr]
    local_vars = []
    get_lines = []

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
        local_vars += ['const struct nlattr *attr2;']
        get_lines += [f'attr_{attr} = attr;',
                      'mnl_attr_for_each_nested(attr2, attr)',
                      f'\t{var}->n_{attr}++;']
    elif spec['type'] == 'nest-type-value':
        prev = 'attr'
        if 'type-value' in spec:
            local_vars += [f'const struct nlattr *attr_{", *attr_".join(spec["type-value"])};']
            local_vars += [f'__u32 {", ".join(spec["type-value"])};']
            for level in spec["type-value"]:
                get_lines += [f'attr_{level} = mnl_attr_get_payload({prev});']
                get_lines += [f'{level} = mnl_attr_get_type(attr_{level});']
                prev = 'attr_' + level

        get_lines += [f"{nest_op_prefix(ri, spec['nested-attributes'])}_parse(&{var}->{attr}, " +
                      f"{prev}, {', '.join(spec['type-value'])});"]
    else:
        raise Exception(f"Type {spec['type']} not supported yet")

    ri.cw.block_start(line=f"if (mnl_attr_get_type(attr) == {attr_enum_name(ri, attr)})")
    if local_vars:
        for l in local_vars:
            ri.cw.p(l)
        ri.cw.nl()

    if spec['type'] != 'array-nest':
        ri.cw.p(f"{var}->{attr}_present = 1;")

    for l in get_lines:
        ri.cw.p(l)
    ri.cw.block_end()


def attribute_parse_kernel(ri, attr, prototype=True, suffix=""):
    aspace = ri.family["attributes"]["spaces"][ri.attr_space]
    spec = aspace["list"][attr]

    t = spec['type']
    ri.cw.block_start(line=f"if (tb[{aspace['name-prefix']}{attr.upper()}])")
    ri.cw.p(f"req->{attr}_present = 1;")
    if t in scalars:
        ri.cw.p(f"req->{attr} = nla_get_{t}(tb[{aspace['name-prefix']}{attr.upper()}]);")
    elif t == 'nul-string':
        ri.cw.p(f"strcpy(req->{attr}, nla_data(tb[{aspace['name-prefix']}{attr.upper()}]));")
    ri.cw.block_end()


def print_prototype(ri, direction, terminate=True):
    suffix = ';' if terminate else ''

    fname = f"{ri.family['name']}_{ri.op_name}"
    if ri.op_mode == 'dump':
        fname += '_dump'

    args = ['struct ynl_sock *ys']
    if 'request' in ri.op[ri.op_mode]:
        args.append(f"{type_name(ri, direction)} *" + f"{direction_to_suffix[direction][1:]}")

    ri.cw.write_func_prot(f"{type_name(ri, rdir(direction))} *", fname, args, suffix)


def print_req_prototype(ri):
    print_prototype(ri, "request")


def print_dump_prototype(ri):
    print_prototype(ri, "request")


def parse_rsp_nested(ri, attr_space):
    struct_type = nest_type_name(ri, attr_space)

    func_args = [f'{struct_type} *dst',
                 'const struct nlattr *nested']
    for arg in sorted(ri.family.inherited_members[attr_space]):
        func_args.append('__u32 ' + arg)

    ri.cw.write_func_prot('int', f'{nest_op_prefix(ri, attr_space)}_parse', func_args)
    ri.cw.block_start()
    ri.cw.write_func_lvar('const struct nlattr *attr;')

    for arg in sorted(ri.family.inherited_members[attr_space]):
        ri.cw.p(f'dst->{arg} = {arg};')
    if ri.family.inherited_members[attr_space]:
        ri.cw.nl()

    ri.cw.block_start(line="mnl_attr_for_each_nested(attr, nested)")

    for arg in ri.family['attributes']['spaces'][attr_space]['list']:
        attribute_get(ri, arg, "dst")

    ri.cw.block_end()
    ri.cw.nl()
    ri.cw.p('return 0;')
    ri.cw.block_end()
    ri.cw.nl()


def parse_rsp_msg(ri, deref=False):
    struct_type = type_name(ri, "reply", deref=deref)

    func_args = ['const struct nlmsghdr *nlh',
                 'void *data']

    local_vars = [f'{struct_type} *dst = data;',
                  'const struct nlattr *attr;']

    array_nests = set()
    for arg in ri.op[ri.op_mode]["reply"]['attributes']:
        aspec = op_aspec(ri, arg)
        if aspec['type'] == 'array-nest':
            local_vars.append(f'const struct nlattr *attr_{arg};')
            array_nests.add(arg)
    if array_nests:
        local_vars.append('int i;')

    ri.cw.write_func_prot('int', f'{op_prefix(ri, "reply", deref=deref)}_parse', func_args)
    ri.cw.block_start()
    ri.cw.write_func_lvar(local_vars)

    ri.cw.block_start(line="mnl_attr_for_each(attr, nlh, sizeof(struct genlmsghdr))")

    for arg in ri.op[ri.op_mode]["reply"]['attributes']:
        attribute_get(ri, arg, "dst")

    ri.cw.block_end()

    ri.cw.nl()
    for anest in sorted(array_nests):
        aspec = op_aspec(ri, anest)
        ri.cw.block_start(line=f"if (dst->n_{anest})")
        ri.cw.p(f"dst->{anest} = calloc(dst->n_{anest}, sizeof(*dst->{anest}));")
        ri.cw.p('i = 0;')
        ri.cw.block_start(line=f"mnl_attr_for_each_nested(attr, attr_{anest})")
        ri.cw.p(f"{nest_op_prefix(ri, aspec['nested-attributes'])}_parse(&dst->{anest}[i], attr, " +
                "mnl_attr_get_type(attr));")
        ri.cw.p('i++;')
        ri.cw.block_end()
        ri.cw.block_end()

    ri.cw.nl()
    ri.cw.p('return MNL_CB_OK;')
    ri.cw.block_end()
    ri.cw.nl()


def print_req(ri):
    direction = "request"
    local_vars = [f'{type_name(ri, rdir(direction))} *rsp;',
                  'struct nlmsghdr *nlh;',
                  'int len, err;']

    print_prototype(ri, direction, terminate=False)
    ri.cw.block_start()
    ri.cw.write_func_lvar(local_vars)

    ri.cw.p(f"nlh = ynl_gemsg_start_req(ys, {ri.nl.get_family_id()}, {op_enum_name(ri)}, 1);")
    ri.cw.nl()

    for arg in ri.op[ri.op_mode]["request"]['attributes']:
        attribute_put(ri, arg, "req")
    ri.cw.nl()

    ri.cw.p(f"""err = mnl_socket_sendto(ys->sock, nlh, nlh->nlmsg_len);
	if (err < 0)
		return NULL;

	len = mnl_socket_recvfrom(ys->sock, ys->buf, MNL_SOCKET_BUFFER_SIZE);
	if (len < 0)
		return NULL;

	rsp = calloc(1, sizeof(*rsp));

	err = mnl_cb_run(ys->buf, len, ys->seq, ys->portid,
			 {op_prefix(ri, "reply")}_parse, rsp);
	if (err < 0)
		goto err_free;

	err = ynl_recv_ack(ys, err);
	if (err)
		goto err_free;

	return rsp;

err_free:
	{call_free(ri, rdir(direction), 'rsp')}
	return NULL;""")
    ri.cw.block_end()


def print_dump(ri):
    direction = "request"
    print_prototype(ri, direction, terminate=False)
    ri.cw.block_start()
    local_vars = [f'{type_name(ri, rdir(direction))} *rsp, *cur;',
                  'struct ynl_dump_state yds = {};',
                  'struct nlmsghdr *nlh;',
                  'int len, err;']

    for var in local_vars:
        ri.cw.p(f'{var}')
    ri.cw.nl()

    ri.cw.p('yds.alloc_sz = sizeof(*rsp);')
    ri.cw.p(f"yds.cb = {op_prefix(ri, 'reply', deref=True)}_parse;")
    ri.cw.nl()
    ri.cw.p(f"nlh = ynl_gemsg_start_dump(ys, {ri.nl.get_family_id()}, {op_enum_name(ri)}, 1);")
    ri.cw.nl()

    if "request" in ri.op[ri.op_mode]:
        for arg in ri.op[ri.op_mode]["request"]['attributes']:
            attribute_put(ri, arg, "req")
        ri.cw.nl()

    ri.cw.p(f"""err = mnl_socket_sendto(ys->sock, nlh, nlh->nlmsg_len);
	if (err < 0)
		return NULL;

	do {'{'}
		len = mnl_socket_recvfrom(ys->sock, ys->buf, MNL_SOCKET_BUFFER_SIZE);
		if (len < 0)
			goto free_list;

		err = mnl_cb_run(ys->buf, len, ys->seq, ys->portid,
				 ynl_dump_trampoline, &yds);
		if (err < 0)
			goto free_list;
	{'}'} while (err > 0);

	return yds.first;

free_list:
	rsp = yds.first;
	while (rsp) {'{'}
		cur = rsp;
		rsp = rsp->next;
		{call_free(ri, rdir(direction), 'cur')}
	{'}'}
	return NULL;""")
    ri.cw.block_end()


def call_free(ri, direction, var):
    return f"{op_prefix(ri, direction)}_free({var});"


def free_arg_name(ri, direction):
    if direction:
        return direction_to_suffix[direction][1:]
    return 'obj'


def print_free_prototype(ri, direction, suffix=';'):
    name = op_prefix(ri, direction)
    arg = free_arg_name(ri, direction)
    ri.cw.write_func_prot('void', f"{name}_free", [f"struct {name} *{arg}"], suffix=suffix)


def _print_type(ri, direction, type_list, inherited_list={}):
    suffix = f'_{ri.type_name}{direction_to_suffix[direction]}'

    if ri.op_mode == 'dump':
        suffix += '_dump'

    ri.cw.block_start(line=f"struct {ri.family['name']}{suffix}")
    for arg in type_list:
        attribute_pres_member(ri, ri.attr_space, arg, suffix=';')
    ri.cw.nl()

    for arg in sorted(inherited_list):
        ri.cw.p(f"__u32 {arg};")

    for arg in type_list:
        attribute_member(ri, ri.attr_space, arg, prototype=False, suffix=';')

    ri.cw.block_end(line=';')
    ri.cw.nl()


def print_type(ri, direction):
    _print_type(ri, direction, ri.op[ri.op_mode][direction]['attributes'])


def print_type_full(ri, attr_space):
    types = ri.family['attributes']['spaces'][attr_space]['list']
    _print_type(ri, "", types, ri.family.inherited_members[attr_space])


def print_type_helpers(ri, direction, deref=False):
    print_free_prototype(ri, direction)

    type_list = ri.op[ri.op_mode][direction]['attributes']

    if ri.ku_space == 'user' and direction == 'request':
        for arg in type_list:
            attribute_setter(ri, ri.attr_space, arg, direction, deref=deref)
    ri.cw.nl()


def print_req_type_helpers(ri):
    print_type_helpers(ri, "request")


def print_rsp_type_helpers(ri):
    print_type_helpers(ri, "reply")


def print_parse_prototype(ri, direction, terminate=True):
    suffix = "_rsp" if direction == "reply" else "_req"
    term = ';' if terminate else ''

    ri.cw.write_func_prot('void', f"{ri.family['name']}_{ri.op_name}{suffix}_parse",
                          ['const struct nlattr **tb',
                           f"struct {ri.family['name']}_{ri.op_name}{suffix} *req"],
                          suffix=term)


def print_parse_kernel(ri, direction):
    print_parse_prototype(ri, direction, terminate=False)
    ri.cw.block_start()
    for arg in ri.op[ri.op_mode][direction]['attributes']:
        attribute_parse_kernel(ri, arg, prototype=False, suffix=';')
    ri.cw.block_end()


def print_req_type(ri):
    print_type(ri, "request")


def print_rsp_type(ri):
    print_type(ri, "reply")


def print_wrapped_type(ri):
    ri.cw.block_start(line=f"{type_name(ri, 'reply')}")
    if ri.op_mode == 'dump':
        ri.cw.p(f"{type_name(ri, 'reply')} *next;")
    elif ri.op_mode == 'notify':
        ri.cw.p('__u16 family;')
        ri.cw.p('__u8 cmd;')
    ri.cw.p(f"{type_name(ri, 'reply', deref=True)} obj;")
    ri.cw.block_end(line=';')
    ri.cw.nl()
    print_free_prototype(ri, 'reply')
    ri.cw.nl()


def _free_type_members(ri, var, type_list, ref=''):
    for arg in type_list:
        spec = ri.family["attributes"]["spaces"][ri.attr_space]["list"][arg]
        if spec['type'] == 'array-nest':
            ri.cw.p(f'free({var}->{ref}{arg});')
    ri.cw.p(f'free({var});')


def _free_type(ri, direction, type_list):
    var = free_arg_name(ri, direction)

    print_free_prototype(ri, direction, suffix='')
    ri.cw.block_start()
    _free_type_members(ri, var, type_list)
    ri.cw.block_end()
    ri.cw.nl()


def free_rsp_nested(ri, attr_space):
    types = ri.family['attributes']['spaces'][attr_space]['list']
    _free_type(ri, "", types)


def print_rsp_free(ri):
    _free_type(ri, 'reply', ri.op[ri.op_mode]['reply']['attributes'])


def print_dump_type_free(ri):
    sub_type = type_name(ri, 'reply')

    print_free_prototype(ri, 'reply', suffix='')
    ri.cw.block_start()
    ri.cw.p(f"{sub_type} *next = rsp;")
    ri.cw.nl()
    ri.cw.block_start(line='while (next)')
    ri.cw.p('rsp = next;')
    ri.cw.p('next = rsp->next;')
    ri.cw.nl()

    _free_type_members(ri, 'rsp', ri.op[ri.op_mode]['reply']['attributes'], ref='obj.')
    ri.cw.block_end()
    ri.cw.block_end()
    ri.cw.nl()


def print_ntf_type_free(ri):
    print_free_prototype(ri, 'reply', suffix='')
    ri.cw.block_start()
    _free_type_members(ri, 'rsp', ri.op[ri.op_mode]['reply']['attributes'], ref='obj.')
    ri.cw.block_end()
    ri.cw.nl()


def print_ntf_parse_prototype(family, cw, suffix=';'):
    cw.write_func_prot('struct ynl_ntf_base_type *', f"{family['name']}_ntf_parse",
                       ['struct ynl_sock *ys'], suffix=suffix)


def print_ntf_type_parse(family, cw, ku_mode):
    print_ntf_parse_prototype(family, cw, suffix='')
    cw.block_start()
    cw.write_func_lvar(['struct genlmsghdr *genlh;',
                        'struct nlmsghdr *nlh;',
                        'struct ynl_ntf_base_type *rsp;',
                        'int len, err;',
                        'mnl_cb_t parse;'])
    cw.p('len = mnl_socket_recvfrom(ys->sock, ys->buf, MNL_SOCKET_BUFFER_SIZE);')
    cw.p('if (len < (ssize_t)(sizeof(*nlh) + sizeof(*genlh)))')
    cw.p('\treturn NULL;')
    cw.nl()
    cw.p('nlh = (void *)ys->buf;')
    cw.p('genlh = mnl_nlmsg_get_payload(nlh);')
    cw.nl()
    cw.block_start(line='switch (genlh->cmd)')
    for ntf_op in sorted(family.all_notify.keys()):
        op = family['operations']['list'][ntf_op]
        ri = RenderInfo(cw, family, ku_mode, op, ntf_op, "notify")
        for ntf in op['notify']['cmds']:
            cw.p(f"case {family['operations']['name-prefix']}{ntf.upper()}:")
        cw.p(f"rsp = calloc(1, sizeof({type_name(ri, 'notify')}));")
        cw.p(f"parse = {op_prefix(ri, 'reply', deref=True)}_parse;")
        cw.p('break;')
    cw.p('default:')
    cw.p('return NULL;')
    cw.block_end()
    cw.nl()
    print("""	err = mnl_cb_run(ys->buf, len, 0, 0, parse, rsp);
	if (err)
		goto err_free;""")
    cw.nl()
    cw.p('rsp->family = nlh->nlmsg_type;')
    cw.p('rsp->cmd = genlh->cmd;')
    cw.p('return rsp;')
    cw.nl()
    cw.p('err_free:')
    cw.p('free(rsp);')
    cw.p('return NULL;')
    cw.block_end()
    cw.nl()


def print_req_policy_fwd(ri, terminate=True):
    suffix = ';' if terminate else ' = {'
    ri.cw.p(f"const struct nla_policy {ri.family['name']}_{ri.op_name}_policy[]{suffix}")


def print_req_policy(ri):
    print_req_policy_fwd(ri, terminate=False)
    for arg in ri.op[ri.op_mode]["request"]['attributes']:
        attribute_policy(ri, arg)
    ri.cw.p("};")


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

    cw = CodeWriter(BaseNlLib())

    cw.p(f"// SPDX-License-Identifier: MIT")
    cw.p("// Do not edit directly, auto-generated from:")
    cw.p(f"//\t{args.spec}")
    cw.p(f"// {' '.join(os.sys.argv)}")
    cw.nl()
    if args.mode == 'kernel':
        cw.p(f'#include <net/netlink.h>')
        cw.nl()
    cw.p(f"#include <{parsed['headers'][args.mode]}>\n")

    if args.mode == "user":
        if not args.header:
            cw.p("#include <stdlib.h>")
            cw.p("#include <stdio.h>")
            cw.p("#include <string.h>")
            cw.p("#include <libmnl/libmnl.h>")
            cw.nl()
            for h in args.user_header:
                cw.p(f'#include "{h}"')
        else:
            cw.p('struct ynl_sock;')
        cw.nl()

    has_ntf = False
    if args.header:
        if args.mode == "user":
            cw.p('// Common nested types')
            for attr_space in sorted(parsed.pure_nested_spaces):
                ri = RenderInfo(cw, parsed, args.mode, "", "", "", attr_space)
                print_type_full(ri, attr_space)

        for op_name, op in parsed['operations']['list'].items():
            cw.p(f"// {parsed['operations']['name-prefix']}{op_name.upper()}")

            if 'do' in op:
                ri = RenderInfo(cw, parsed, args.mode, op, op_name, "do")

                print_req_type(ri)
                print_req_type_helpers(ri)
                cw.nl()
                print_rsp_type(ri)
                print_rsp_type_helpers(ri)
                cw.nl()

                if args.mode == "user":
                    print_req_prototype(ri)
                elif args.mode == "kernel":
                    print_parse_prototype(ri, "request")
                    print_req_policy_fwd(ri)
                cw.nl()

            if 'dump' in op:
                ri = RenderInfo(cw, parsed, args.mode, op, op_name, 'dump')
                if args.mode == "user":
                    if 'request' in op['dump']:
                        print_req_type(ri)
                        print_req_type_helpers(ri)
                    if not ri.type_consistent:
                        print_rsp_type(ri)
                    print_wrapped_type(ri)
                    print_dump_prototype(ri)
                    cw.nl()

            if 'notify' in op:
                ri = RenderInfo(cw, parsed, args.mode, op, op_name, 'notify')
                if args.mode == "user":
                    has_ntf = True
                    if not ri.type_consistent:
                        raise Exception('Only notifications with consistent types supported')
                    print_wrapped_type(ri)

        if has_ntf:
            print_ntf_parse_prototype(parsed, cw)
    else:
        if args.mode == "user":
            cw.p('// Common nested types')
            for attr_space in sorted(parsed.pure_nested_spaces):
                ri = RenderInfo(cw, parsed, args.mode, "", "", "", attr_space)
                free_rsp_nested(ri, attr_space)
                parse_rsp_nested(ri, attr_space)

        for op_name, op in parsed['operations']['list'].items():
            cw.p(f"// {parsed['operations']['name-prefix']}{op_name.upper()}")

            if 'do' in op:
                ri = RenderInfo(cw, parsed, args.mode, op, op_name, "do")
                if args.mode == "user":
                    print_rsp_free(ri)
                    parse_rsp_msg(ri)
                    print_req(ri)
                elif args.mode == "kernel":
                    print_parse_kernel(ri, "request")
                    cw.nl()
                    print_req_policy(ri)
                cw.nl()

            if 'dump' in op:
                ri = RenderInfo(cw, parsed, args.mode, op, op_name, "dump")
                if args.mode == "user":
                    if not ri.type_consistent:
                        parse_rsp_msg(ri, deref=True)
                    print_dump_type_free(ri)
                    print_dump(ri)
                    cw.nl()

            if 'notify' in op:
                ri = RenderInfo(cw, parsed, args.mode, op, op_name, 'notify')
                if args.mode == "user":
                    has_ntf = True
                    if not ri.type_consistent:
                        raise Exception('Only notifications with consistent types supported')
                    print_ntf_type_free(ri)

        if has_ntf:
            print_ntf_type_parse(parsed, cw, args.mode)

if __name__ == "__main__":
    main()
