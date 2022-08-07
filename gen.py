#!/usr/bin/env python

import argparse
import jsonschema
import os
import yaml


class BaseNlLib:
    def __init__(self):
        pass

    def get_family_id(self):
        return 'ys->family_id'


class Type:
    def __init__(self, attr):
        self.attr = attr
        self.name = attr['name']
        self.c_name = attr['c_name']
        self.type = attr['type']
        if 'len' in attr:
            self.len = attr['len']
        if 'nested-attributes' in attr:
            self.nested_attrs = attr['nested-attributes']

    def is_multi_val(self):
        return None

    def is_scalar(self):
        return self.type in {'u8', 'u16', 'u32', 'u64', 's32', 's64'}

    def _attr_put_line(self, ri, var, line):
        ri.cw.p(f"if ({var}->{self.name}_present)")
        ri.cw.p(f"{line};", add_ind=1)

    def _attr_put_simple(self, ri, var, put_type):
        line = f"mnl_attr_put_{put_type}(nlh, {attr_enum_name(ri, self.name)}, {var}->{self.name})"
        self._attr_put_line(ri, var, line)

    def attr_put(self, ri, var):
        raise Exception(f"Put not implemented for class type {self.type}")

    def _attr_get(self, ri, var, lines, local_vars=None):
        if type(lines) is str:
            lines = [lines]

        ri.cw.block_start(line=f"if (mnl_attr_get_type(attr) == {attr_enum_name(ri, self.name)})")
        if local_vars:
            for local in local_vars:
                ri.cw.p(local)
            ri.cw.nl()

        if not self.is_multi_val():
            ri.cw.p(f"{var}->{self.name}_present = 1;")

        for line in lines:
            ri.cw.p(line)
        ri.cw.block_end()

    def attr_get(self, ri, var):
        raise Exception(f"Put not implemented for class type {self.type}")


class TypeUnused(Type):
    pass


class TypeScalar(Type):
    def _mnl_type(self):
        t = self.type
        # mnl does not have a helper for signed types
        if t[0] == 's':
            t = 'u' + t[1:]
        return t

    def attr_put(self, ri, var):
        self._attr_put_simple(ri, var, self._mnl_type())

    def attr_get(self, ri, var):
        self._attr_get(ri, var, f"{var}->{self.c_name} = mnl_attr_get_{self._mnl_type()}(attr);")


class TypeFlag(Type):
    def attr_put(self, ri, var):
        self._attr_put_line(ri, var, f"mnl_attr_put(nlh, {attr_enum_name(ri, self.name)}, 0, NULL)")

    def attr_get(self, ri, var):
        self._attr_get(ri, var, [])


class TypeNulString(Type):
    def attr_put(self, ri, var):
        self._attr_put_simple(ri, var, 'strz')

    def attr_get(self, ri, var):
        self._attr_get(ri, var,
                       [f"strncpy({var}->{self.c_name}, mnl_attr_get_str(attr), {self.len});",
                        f"{var}->{self.c_name}[{self.len}] = 0;"])


class TypeBinary(Type):
    def attr_put(self, ri, var):
        self._attr_put_line(ri, var, f"mnl_attr_put(nlh, {attr_enum_name(ri, self.name)}, " +
                            f"{self.len}, {var}->{self.name})")

    def attr_get(self, ri, var):
        self._attr_get(ri, var, f"memcpy({var}->{self.name}, mnl_attr_get_payload(attr), {self.len});")


class TypeNest(Type):
    def attr_put(self, ri, var):
        self._attr_put_line(ri, var, f"{nest_op_prefix(ri, self.nested_attrs)}_put(nlh, " +
                            f"{attr_enum_name(ri, self.name)}, &{var}->{self.name})")

    def attr_get(self, ri, var):
        self._attr_get(ri, var,
                       f"{nest_op_prefix(ri, self.nested_attrs)}_parse(&{var}->{self.name}, attr);")


class TypeMultiAttr(Type):
    def is_multi_val(self):
        return True

    def attr_get(self, ri, var):
        self._attr_get(ri, var, f'{var}->n_{self.name}++;')


class TypeArrayNest(Type):
    def is_multi_val(self):
        return True

    def attr_get(self, ri, var):
        local_vars = ['const struct nlattr *attr2;']
        get_lines = [f'attr_{self.name} = attr;',
                     'mnl_attr_for_each_nested(attr2, attr)',
                     f'\t{var}->n_{self.name}++;']
        self._attr_get(ri, var, get_lines, local_vars)


class TypeNestTypeValue(Type):
    def attr_get(self, ri, var):
        prev = 'attr'
        tv_args = ''
        get_lines = []
        local_vars = []
        if 'type-value' in self.attr:
            local_vars += [f'const struct nlattr *attr_{", *attr_".join(self.attr["type-value"])};']
            local_vars += [f'__u32 {", ".join(self.attr["type-value"])};']
            for level in self.attr["type-value"]:
                get_lines += [f'attr_{level} = mnl_attr_get_payload({prev});']
                get_lines += [f'{level} = mnl_attr_get_type(attr_{level});']
                prev = 'attr_' + level

            tv_args = f", {', '.join(self.attr['type-value'])}"

        get_lines += [f"{nest_op_prefix(ri, self.nested_attrs)}_parse(&{var}->{self.name}, " +
                      f"{prev}{tv_args});"]
        self._attr_get(ri, var, get_lines, local_vars)


class Attribute:
    def __init__(self, yaml):
        self.yaml = yaml

        c_name = yaml['name']
        if c_name in c_kw:
            c_name += '_'
        self.yaml['c_name'] = c_name

        if yaml['type'] in scalars:
            self.typed = TypeScalar(self)
        elif yaml['type'] == 'unused':
            self.typed = TypeUnused(self)
        elif yaml['type'] == 'flag':
            self.typed = TypeFlag(self)
        elif yaml['type'] == 'nul-string':
            self.typed = TypeNulString(self)
        elif yaml['type'] == 'binary':
            self.typed = TypeBinary(self)
        elif yaml['type'] == 'nest':
            self.typed = TypeNest(self)
        elif yaml['type'] == 'multi-attr':
            self.typed = TypeMultiAttr(self)
        elif yaml['type'] == 'array-nest':
            self.typed = TypeArrayNest(self)
        elif yaml['type'] == 'nest-type-value':
            self.typed = TypeNestTypeValue(self)
        else:
            raise Exception(f"No typed class for type {yaml['type']}")

    def __getitem__(self, key):
        return self.yaml[key]

    def __contains__(self, key):
        return key in self.yaml


class AttrSpace:
    def __init__(self, yaml):
        self.yaml = yaml

        self.attrs = dict()
        self.name = self.yaml['name']
        self.name_prefix = self.yaml['name-prefix']
        self.subspace_of = self.yaml['subspace-of'] if 'subspace-of' in self.yaml else None

        for elem in self.yaml['attributes']:
            self.attrs[elem['name']] = Attribute(elem)

    def __getitem__(self, key):
        return self.attrs[key]

    def __contains__(self, key):
        return key in self.yaml

    def __iter__(self):
        yield from self.attrs

    def items(self):
        return self.attrs.items()


class Operation:
    def __init__(self, family, yaml):
        self.yaml = yaml

        self.name = self.yaml['name']
        self.enum_name = family.op_prefix + self.name.upper()

    def __getitem__(self, key):
        return self.yaml[key]

    def __contains__(self, key):
        return key in self.yaml

    def add_notification(self, op_yaml):
        if 'notify' not in self.yaml:
            self.yaml['notify'] = dict()
            self.yaml['notify']['reply'] = self.yaml['do']['reply']
            self.yaml['notify']['cmds'] = []
        self.yaml['notify']['cmds'].append(op_yaml['name'])


class Family:
    def __init__(self, file_name):
        with open(file_name, "r") as stream:
            self.yaml = yaml.safe_load(stream)

        with open(os.path.dirname(os.path.dirname(file_name)) + '/schema.yaml', "r") as stream:
            schema = yaml.safe_load(stream)

        jsonschema.validate(self.yaml, schema)

        if 'constants' not in self.yaml:
            self.yaml['constants'] = []

        self.name = self.yaml['name']
        self.op_prefix = self.yaml['operations']['name-prefix']

        # dict space-name -> 'request': set(attrs), 'reply': set(attrs)
        self.root_spaces = dict()
        # dict space-name -> set('request', 'reply')
        self.pure_nested_spaces = dict()
        self.inherited_members = dict()
        self.all_notify = dict()

        self.ops = dict()
        self.attr_spaces = dict()

        self._dictify()
        self._load_root_spaces()
        self._load_nested_spaces()
        self._load_all_notify()

    def __getitem__(self, key):
        return self.yaml[key]

    def get(self, key, default=None):
        return self.yaml.get(key, default)

    def _dictify(self):
        for elem in self.yaml['attribute-spaces']:
            self.attr_spaces[elem['name']] = AttrSpace(elem)

        ntf = []
        for elem in self.yaml['operations']['list']:
            if 'notify' in elem:
                ntf.append(elem)
                continue
            if 'attribute-space' not in elem:
                continue
            self.ops[elem['name']] = Operation(self, elem)
        for n in ntf:
            self.ops[n['notify']].add_notification(n)

    def _load_root_spaces(self):
        for op_name, op in self.ops.items():
            if 'attribute-space' not in op:
                continue

            req_attrs = set()
            rsp_attrs = set()
            for op_mode in ['do', 'dump']:
                if op_mode in op and 'request' in op[op_mode]:
                    req_attrs.update(set(op[op_mode]['request']['attributes']))
                if op_mode in op and 'reply' in op[op_mode]:
                    rsp_attrs.update(set(op[op_mode]['reply']['attributes']))

            if op['attribute-space'] not in self.root_spaces:
                self.root_spaces[op['attribute-space']] = {'request': req_attrs, 'reply': rsp_attrs}
            else:
                self.root_spaces[op['attribute-space']]['request'].update(req_attrs)
                self.root_spaces[op['attribute-space']]['reply'].update(rsp_attrs)

    def _load_nested_spaces(self):
        for root_space, rs_members in self.root_spaces.items():
            for attr, spec in self.attr_spaces[root_space].items():
                if 'nested-attributes' in spec:
                    nested = spec['nested-attributes']
                    if nested not in self.root_spaces:
                        self.pure_nested_spaces[nested] = set()
                    if attr in rs_members['request']:
                        self.pure_nested_spaces[nested].add('request')
                    if attr in rs_members['reply']:
                        self.pure_nested_spaces[nested].add('reply')
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
        for op_name, op in self.ops.items():
            if not op:
                continue

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

    def p(self, line, add_ind=0):
        if self._nl:
            print()
            self._nl = False
        ind = self._ind
        if line[-1] == ':':
            ind -= 1
        if add_ind:
            ind += add_ind
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

    def writes_defines(self, defines):
        longest = 0
        for define in defines:
            if len(define[0]) > longest:
                longest = len(define[0])
        longest = ((longest + 8) // 8) * 8
        for define in defines:
            line = '#define ' + define[0]
            line += '\t' * ((longest - len(define[0]) + 7) // 8)
            if type(define[1]) is int:
                line += str(define[1])
            elif type(define[1]) is str:
                line += '"' + define[1] + '"'
            self.p(line)


scalars = {'u8', 'u16', 'u32', 'u64', 's32', 's64'}

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


def attr_enum_name(ri, attr):
    return f"{ri.family.attr_spaces[ri.attr_space].name_prefix}{attr.upper()}"


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
    aspace = ri.family.attr_spaces[ri.op['attribute-space']]
    return aspace[attr]


def type_name(ri, direction, deref=False):
    return f"struct {op_prefix(ri, direction, deref=deref)}"


def nest_op_prefix(ri, attr_space):
    return f"{ri.family['name']}_{attr_space.replace('-', '_')}"


def nest_type_name(ri, attr_space):
    return f"struct {nest_op_prefix(ri, attr_space)}"


def attribute_policy(ri, attr, prototype=True, suffix=""):
    aspace = ri.family.attr_spaces[ri.attr_space]
    spec = aspace[attr]
    policy = 'NLA_' + spec['type'].upper()
    policy = policy.replace('-', '_')

    mem = '{ .type = ' + policy
    if 'len' in spec:
        mem += ', .len = ' + str(spec['len'])
    mem += ' }'

    ri.cw.p(f"\t[{aspace.name_prefix}{attr.upper()}] = {mem},")


def _attribute_member_len(spec):
    if spec['len'][-4:] == " - 1":
        return f"{spec['len'][:-4]}"
    else:
        return f"{spec['len']} + 1"


def _attribute_member(ri, space, attr, prototype=True, suffix=""):
    spec = ri.family.attr_spaces[space][attr]
    scalar_pfx = '__' if ri.ku_space == 'user' else ''

    t = spec['type']
    if t == 'flag':
        # Nothing, presence is enough for a flag
        return []
    elif t == "binary":
        if prototype:
            t = 'const void *'
        else:
            t = 'unsigned char '
            suffix = f"[{spec['len']}]{suffix}"
    elif t == "nul-string":
        if prototype:
            t = 'const char *'
        else:
            t = 'char '
            suffix = f'[{_attribute_member_len(spec)}]{suffix}'
    elif t == 'array-nest' or t == 'multi-attr':
        if 'sub-type' not in spec or spec['sub-type'] == 'nest':
            t = f"struct {nest_op_prefix(ri, spec['nested-attributes'])} *"
        elif spec['sub-type'] in scalars:
            t = scalar_pfx + spec['sub-type'] + ' *'
        else:
            raise Exception(f"Sub-type {spec} not supported yet")

        if not prototype:
            ri.cw.p(f"unsigned int n_{attr};")
    elif t == 'nest' or t == 'nest-type-value':
        t = f"struct {nest_op_prefix(ri, spec['nested-attributes'])} "
    elif t in scalars:
        t = scalar_pfx + t + ' '
    else:
        raise Exception(f'Type {t} not supported yet')

    return [f"{t}{spec['c_name']}{suffix}"]


def attribute_member(ri, space, attr, prototype=True, suffix=""):
    members = _attribute_member(ri, space, attr, prototype, suffix)
    for line in members:
        ri.cw.p(line)


def attribute_pres_member(ri, space, attr, suffix=""):
    spec = ri.family.attr_spaces[space][attr]
    pfx = '__' if ri.ku_space == 'user' else ''

    if spec.typed.is_multi_val():
        return False

    ri.cw.p(f"{pfx}u32 {attr}_present:1{suffix}")
    return True


def attribute_setter(ri, space, attr, direction, deref=False, ref=None):
    ref = (ref if ref else []) + [attr]
    spec = ri.family.attr_spaces[space][attr]
    var = "req"
    member = f"{var}->{'.'.join(ref)}"

    code = []
    if spec['type'] == 'flag':
        pass
    elif spec['type'] == 'binary':
        code += [f"memcpy({member}, {attr}, {spec['len']});"]
    elif spec['type'] in scalars:
        code += [f"{member} = {attr};"]
    elif spec['type'] == 'nul-string':
        code += [f'strncpy({member}, {attr}, sizeof({member}));',
                 f'{member}[{spec["len"]}] = 0;']
    elif spec['type'] == 'nest':
        nested_space = spec['nested-attributes']
        for nested in ri.family.attr_spaces[nested_space]:
            attribute_setter(ri, nested_space, nested, direction, deref=deref, ref=ref)
        return
    else:
        raise Exception(f'Type {spec["type"]} not supported yet - setter')

    ri.cw.write_func_prot('static inline void',
                          f"{op_prefix(ri, direction, deref=deref)}_set_{'_'.join(ref)}",
                          [f'{type_name(ri, direction, deref=deref)} *{var}'] +
                          _attribute_member(ri, space, attr, prototype=True))
    ri.cw.block_start()
    for i in range(0, len(ref)):
        ri.cw.p(f"{var}->{'.'.join(ref[:i + 1])}_present = 1;")
    for line in code:
        ri.cw.p(line)
    ri.cw.block_end()


def attribute_parse_kernel(ri, attr, prototype=True, suffix=""):
    aspace = ri.family.attr_spaces[ri.attr_space]
    spec = aspace[attr]

    t = spec['type']
    ri.cw.block_start(line=f"if (tb[{aspace.name_prefix}{attr.upper()}])")
    ri.cw.p(f"req->{attr}_present = 1;")
    if t in scalars:
        ri.cw.p(f"req->{attr} = nla_get_{t}(tb[{aspace.name_prefix}{attr.upper()}]);")
    elif t == 'nul-string':
        ri.cw.p(f"strcpy(req->{attr}, nla_data(tb[{aspace.name_prefix}{attr.upper()}]));")
    ri.cw.block_end()


def print_prototype(ri, direction, terminate=True):
    suffix = ';' if terminate else ''

    fname = f"{ri.family['name']}_{ri.op_name}"
    if ri.op_mode == 'dump':
        fname += '_dump'

    args = ['struct ynl_sock *ys']
    if 'request' in ri.op[ri.op_mode]:
        args.append(f"{type_name(ri, direction)} *" + f"{direction_to_suffix[direction][1:]}")

    ret = 'int'
    if 'reply' in ri.op[ri.op_mode]:
        ret = f"{type_name(ri, rdir(direction))} *"

    ri.cw.write_func_prot(ret, fname, args, suffix)


def print_req_prototype(ri):
    print_prototype(ri, "request")


def print_dump_prototype(ri):
    print_prototype(ri, "request")


def put_req_nested(ri, attr_space):
    struct_type = nest_type_name(ri, attr_space)
    func_args = ['struct nlmsghdr *nlh',
                 'unsigned int attr_type',
                 f'{struct_type} *obj']

    ri.cw.write_func_prot('int', f'{nest_op_prefix(ri, attr_space)}_put', func_args)
    ri.cw.block_start()
    ri.cw.write_func_lvar('struct nlattr *nest;')

    ri.cw.p("nest = mnl_attr_nest_start(nlh, attr_type);")

    for _, arg in ri.family.attr_spaces[attr_space].items():
        arg.typed.attr_put(ri, "obj")

    ri.cw.p("mnl_attr_nest_end(nlh, nest);")

    ri.cw.nl()
    ri.cw.p('return 0;')
    ri.cw.block_end()
    ri.cw.nl()


def prep_multi_parse(ri, type_list, array_nests, multi_attrs, local_vars, attr_space=None):
    if attr_space is None:
        attr_space = ri.attr_space

    for arg in type_list:
        aspec = ri.family.attr_spaces[attr_space][arg]
        if aspec['type'] == 'array-nest':
            local_vars.append(f'const struct nlattr *attr_{arg};')
            array_nests.add(arg)
        if aspec['type'] == 'multi-attr':
            multi_attrs.add(arg)
    if array_nests or multi_attrs:
        local_vars.append('int i;')


def finalize_multi_parse(ri, nested, array_nests, multi_attrs, attr_space=None):
    if attr_space is None:
        attr_space = ri.attr_space

    if nested:
        iter_line = "mnl_attr_for_each_nested(attr, nested)"
    else:
        iter_line = "mnl_attr_for_each(attr, nlh, sizeof(struct genlmsghdr))"

    for anest in sorted(array_nests):
        aspec = ri.family.attr_spaces[attr_space][anest]
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

    for anest in sorted(multi_attrs):
        aspec = ri.family.attr_spaces[attr_space][anest]
        ri.cw.block_start(line=f"if (dst->n_{anest})")
        ri.cw.p(f"dst->{anest} = calloc(dst->n_{anest}, sizeof(*dst->{anest}));")
        ri.cw.p('i = 0;')
        ri.cw.block_start(line=iter_line)
        ri.cw.block_start(line=f"if (mnl_attr_get_type(attr) == {attr_enum_name(ri, anest)})")
        if 'nested-attributes' in aspec:
            ri.cw.p(f"{nest_op_prefix(ri, aspec['nested-attributes'])}_parse(&dst->{anest}[i], attr);")
        elif aspec['sub-type'] in scalars:
            t = aspec['sub-type']
            if t[0] == 's':
                t = 'u' + t[1:]
            ri.cw.p(f"dst->{aspec['c_name']}[i] = mnl_attr_get_{t}(attr);")
        else:
            raise Exception('Nest parsing type not supported yet')
        ri.cw.p('i++;')
        ri.cw.block_end()
        ri.cw.block_end()
        ri.cw.block_end()
    ri.cw.nl()


def parse_rsp_nested(ri, attr_space):
    struct_type = nest_type_name(ri, attr_space)

    func_args = [f'{struct_type} *dst',
                 'const struct nlattr *nested']
    for arg in sorted(ri.family.inherited_members[attr_space]):
        func_args.append('__u32 ' + arg)

    local_vars = ['const struct nlattr *attr;']

    array_nests = set()
    multi_attrs = set()
    prep_multi_parse(ri, ri.family.attr_spaces[attr_space],
                     array_nests, multi_attrs, local_vars, attr_space=attr_space)

    ri.cw.write_func_prot('int', f'{nest_op_prefix(ri, attr_space)}_parse', func_args)
    ri.cw.block_start()
    ri.cw.write_func_lvar(local_vars)

    for arg in sorted(ri.family.inherited_members[attr_space]):
        ri.cw.p(f'dst->{arg} = {arg};')
    if ri.family.inherited_members[attr_space]:
        ri.cw.nl()

    ri.cw.block_start(line="mnl_attr_for_each_nested(attr, nested)")

    for _, arg in ri.family.attr_spaces[attr_space].items():
        arg.typed.attr_get(ri, 'dst')

    ri.cw.block_end()
    ri.cw.nl()

    finalize_multi_parse(ri, True, array_nests, multi_attrs, attr_space)

    ri.cw.p('return 0;')
    ri.cw.block_end()
    ri.cw.nl()


def parse_rsp_msg(ri, deref=False):
    if 'reply' not in ri.op[ri.op_mode]:
        return
    struct_type = type_name(ri, "reply", deref=deref)

    func_args = ['const struct nlmsghdr *nlh',
                 'void *data']

    local_vars = [f'{struct_type} *dst = data;',
                  'const struct nlattr *attr;']

    array_nests = set()
    multi_attrs = set()
    prep_multi_parse(ri, ri.op[ri.op_mode]["reply"]['attributes'],
                     array_nests, multi_attrs, local_vars)

    ri.cw.write_func_prot('int', f'{op_prefix(ri, "reply", deref=deref)}_parse', func_args)
    ri.cw.block_start()
    ri.cw.write_func_lvar(local_vars)

    ri.cw.block_start(line="mnl_attr_for_each(attr, nlh, sizeof(struct genlmsghdr))")

    for arg in ri.op[ri.op_mode]["reply"]['attributes']:
        attr = ri.family.attr_spaces[ri.attr_space][arg]
        attr.typed.attr_get(ri, "dst")

    ri.cw.block_end()
    ri.cw.nl()

    finalize_multi_parse(ri, False, array_nests, multi_attrs)

    ri.cw.p('return MNL_CB_OK;')
    ri.cw.block_end()
    ri.cw.nl()


def print_req(ri):
    ret_ok = '0'
    ret_err = '-1'
    direction = "request"
    local_vars = ['struct nlmsghdr *nlh;',
                  'int len, err;']

    if 'reply' in ri.op[ri.op_mode]:
        ret_ok = 'rsp'
        ret_err = 'NULL'
        local_vars += [f'{type_name(ri, rdir(direction))} *rsp;']

    print_prototype(ri, direction, terminate=False)
    ri.cw.block_start()
    ri.cw.write_func_lvar(local_vars)

    ri.cw.p(f"nlh = ynl_gemsg_start_req(ys, {ri.nl.get_family_id()}, {ri.op.enum_name}, 1);")
    ri.cw.nl()

    for arg in ri.op[ri.op_mode]["request"]['attributes']:
        attr = ri.family.attr_spaces[ri.attr_space][arg]
        attr.typed.attr_put(ri, "req")
    ri.cw.nl()

    ri.cw.p(f"""err = mnl_socket_sendto(ys->sock, nlh, nlh->nlmsg_len);
	if (err < 0)
		return {ret_err};

	len = mnl_socket_recvfrom(ys->sock, ys->rx_buf, MNL_SOCKET_BUFFER_SIZE);
	if (len < 0)
		return {ret_err};""")
    ri.cw.nl()

    if 'reply' in ri.op[ri.op_mode]:
        ri.cw.p(f"""rsp = calloc(1, sizeof(*rsp));

	err = mnl_cb_run(ys->rx_buf, len, ys->seq, ys->portid,
			 {op_prefix(ri, "reply")}_parse, rsp);
	if (err < 0)
		goto err_free;""")
    ri.cw.nl()

    ri.cw.p(f"""\terr = ynl_recv_ack(ys, err);
	if (err)
		goto err_free;

	return {ret_ok};

err_free:""")
    if 'reply' in ri.op[ri.op_mode]:
        ri.cw.p(f"{call_free(ri, rdir(direction), 'rsp')}")
    ri.cw.p(f"return {ret_err};")
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
    ri.cw.p(f"nlh = ynl_gemsg_start_dump(ys, {ri.nl.get_family_id()}, {ri.op.enum_name}, 1);")
    ri.cw.nl()

    if "request" in ri.op[ri.op_mode]:
        for arg in ri.op[ri.op_mode]["request"]['attributes']:
            attr = ri.family.attr_spaces[ri.attr_space][arg]
            attr.typed.attr_put(ri, "req")
        ri.cw.nl()

    ri.cw.p(f"""err = mnl_socket_sendto(ys->sock, nlh, nlh->nlmsg_len);
	if (err < 0)
		return NULL;

	do {'{'}
		len = mnl_socket_recvfrom(ys->sock, ys->rx_buf, MNL_SOCKET_BUFFER_SIZE);
		if (len < 0)
			goto free_list;

		err = mnl_cb_run(ys->rx_buf, len, ys->seq, ys->portid,
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
    types = ri.family.attr_spaces[attr_space]
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
    if 'reply' not in ri.op[ri.op_mode]:
        return
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
    if 'reply' not in ri.op[ri.op_mode]:
        return
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
        spec = ri.family.attr_spaces[ri.attr_space][arg]
        if spec.typed.is_multi_val():
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
    types = ri.family.attr_spaces[attr_space]
    _free_type(ri, "", types)


def print_rsp_free(ri):
    if 'reply' not in ri.op[ri.op_mode]:
        return
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
    cw.p('len = mnl_socket_recvfrom(ys->sock, ys->rx_buf, MNL_SOCKET_BUFFER_SIZE);')
    cw.p('if (len < (ssize_t)(sizeof(*nlh) + sizeof(*genlh)))')
    cw.p('\treturn NULL;')
    cw.nl()
    cw.p('nlh = (void *)ys->rx_buf;')
    cw.p('genlh = mnl_nlmsg_get_payload(nlh);')
    cw.nl()
    cw.block_start(line='switch (genlh->cmd)')
    for ntf_op in sorted(family.all_notify.keys()):
        op = family.ops[ntf_op]
        ri = RenderInfo(cw, family, ku_mode, op, ntf_op, "notify")
        for ntf in op['notify']['cmds']:
            cw.p(f"case {family.op_prefix}{ntf.upper()}:")
        cw.p(f"rsp = calloc(1, sizeof({type_name(ri, 'notify')}));")
        cw.p(f"parse = {op_prefix(ri, 'reply', deref=True)}_parse;")
        cw.p('break;')
    cw.p('default:')
    cw.p('return NULL;')
    cw.block_end()
    cw.nl()
    cw.p("""err = mnl_cb_run(ys->rx_buf, len, 0, 0, parse, rsp);
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


def uapi_enum_start(family, cw, obj, key):
    start_line = 'enum'
    if key in obj:
        start_line = 'enum ' + family.name + '_' + obj[key]
    cw.block_start(line=start_line)


def render_uapi(family, cw):
    defines = [(family["name"].upper() + '_FAMILY_NAME', family["name"]),
               (family["name"].upper() + '_VERSION', family.get('version', 1))]
    cw.writes_defines(defines)
    cw.nl()

    for const in family['constants']:
        if const['type'] == 'enum':
            uapi_enum_start(family, cw, const, 'name')
            for item in const['values']:
                item_name = item
                if 'value-prefix' in const:
                    item_name = const['value-prefix'] + item.upper()
                cw.p(item_name + ',')
            cw.block_end(line=';')
            cw.nl()
        elif const['type'] == 'flags':
            uapi_enum_start(family, cw, const, 'name')
            i = 0
            for item in const['values']:
                item_name = item
                if 'value-prefix' in const:
                    item_name = const['value-prefix'] + item.upper()
                cw.p(f'{item_name} = {1 << i},')
                i += 1
            cw.block_end(line=';')
            cw.nl()

    for aspace in family['attribute-spaces']:
        if 'subspace-of' in aspace:
            continue

        uapi_enum_start(family, cw, aspace, 'name-enum')
        for attr in aspace['attributes']:
            attr_name = aspace['name-prefix'] + attr['name'].upper()
            cw.p(attr_name + ',')
        cw.nl()
        cw.p(f"__{aspace['name-prefix']}MAX")
        cw.block_end(line=';')
        cw.p(f"#define {aspace['name-prefix']}_MAX (__{aspace['name-prefix']}MAX - 1)")
        cw.nl()

    uapi_enum_start(family, cw, family['operations'], 'name-enum')
    for op in family['operations']['list']:
        op_name = family['operations']['name-prefix'] + op['name'].upper()
        cw.p(op_name + ',')
    cw.block_end(line=';')

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

    if args.mode == 'uapi':
        render_uapi(parsed, cw)
        return

    if args.mode == 'kernel':
        cw.p(f'#include <net/netlink.h>')
        cw.nl()
    headers = []
    for header_type in ['uapi', args.mode]:
        if header_type not in parsed['headers']:
            continue
        one = parsed['headers'][header_type]
        if type(one) is str:
            one = [one]
        headers += one
    for one in headers:
        cw.p(f"#include <{one}>")
    cw.nl()

    if args.mode == "user":
        if not args.header:
            cw.p("#include <stdlib.h>")
            cw.p("#include <stdio.h>")
            cw.p("#include <string.h>")
            cw.p("#include <libmnl/libmnl.h>")
            cw.p("#include <linux/genetlink.h>")
            cw.nl()
            for one in args.user_header:
                cw.p(f'#include "{one}"')
        else:
            cw.p('struct ynl_sock;')
        cw.nl()

    has_ntf = False
    if args.header:
        if args.mode == "user":
            cw.p('// Common nested types')
            for attr_space in sorted(parsed.pure_nested_spaces.keys()):
                ri = RenderInfo(cw, parsed, args.mode, "", "", "", attr_space)
                print_type_full(ri, attr_space)

        for op_name, op in parsed.ops.items():
            if not op:
                continue

            cw.p(f"/* ============== {parsed['operations']['name-prefix']}{op_name.upper()} ============== */")

            if 'do' in op:
                cw.p(f"// {parsed['operations']['name-prefix']}{op_name.upper()} - do")
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
                cw.p(f"// {parsed['operations']['name-prefix']}{op_name.upper()} - dump")
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
                cw.p(f"// {parsed['operations']['name-prefix']}{op_name.upper()} - notify")
                ri = RenderInfo(cw, parsed, args.mode, op, op_name, 'notify')
                if args.mode == "user":
                    has_ntf = True
                    if not ri.type_consistent:
                        raise Exception('Only notifications with consistent types supported')
                    print_wrapped_type(ri)

        if has_ntf:
            cw.p('// --------------- Common notification parsing --------------- //')
            print_ntf_parse_prototype(parsed, cw)
    else:
        if args.mode == "user":
            cw.p('// Common nested types')
            for attr_space in sorted(parsed.pure_nested_spaces.keys()):
                ri = RenderInfo(cw, parsed, args.mode, "", "", "", attr_space)

                free_rsp_nested(ri, attr_space)
                if 'request' in parsed.pure_nested_spaces[attr_space]:
                    put_req_nested(ri, attr_space)
                if 'reply' in parsed.pure_nested_spaces[attr_space]:
                    parse_rsp_nested(ri, attr_space)

        for op_name, op in parsed.ops.items():
            cw.p(f"/* ============== {parsed['operations']['name-prefix']}{op_name.upper()} ============== */")

            if 'do' in op:
                cw.p(f"// {parsed['operations']['name-prefix']}{op_name.upper()} - do")
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
                cw.p(f"// {parsed['operations']['name-prefix']}{op_name.upper()} - dump")
                ri = RenderInfo(cw, parsed, args.mode, op, op_name, "dump")
                if args.mode == "user":
                    if not ri.type_consistent:
                        parse_rsp_msg(ri, deref=True)
                    print_dump_type_free(ri)
                    print_dump(ri)
                    cw.nl()

            if 'notify' in op:
                cw.p(f"// {parsed['operations']['name-prefix']}{op_name.upper()} - notify")
                ri = RenderInfo(cw, parsed, args.mode, op, op_name, 'notify')
                if args.mode == "user":
                    has_ntf = True
                    if not ri.type_consistent:
                        raise Exception('Only notifications with consistent types supported')
                    print_ntf_type_free(ri)

        if has_ntf:
            cw.p('// --------------- Common notification parsing --------------- //')
            print_ntf_type_parse(parsed, cw, args.mode)


if __name__ == "__main__":
    main()
