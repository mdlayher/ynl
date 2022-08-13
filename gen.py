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

    def parse_cb_run(self, cb, data, is_dump=False, indent=1):
        ind = '\n\t\t' + '\t' * indent + ' '
        if is_dump:
            return f"mnl_cb_run2(ys->rx_buf, len, 0, 0, {cb}, {data},{ind}ynl_cb_array, NLMSG_MIN_TYPE)"
        else:
            return f"mnl_cb_run2(ys->rx_buf, len, ys->seq, ys->portid,{ind}{cb}, {data},{ind}" + \
                   "ynl_cb_array, NLMSG_MIN_TYPE)"


class Type:
    def __init__(self, family, attr_set, attr):
        self.family = family
        self.attr = attr
        self.name = attr['name']
        self.type = attr['type']

        if 'len' in attr:
            self.len = attr['len']
        if 'nested-attributes' in attr:
            self.nested_attrs = attr['nested-attributes']
            self.nested_render_name = f"{family['name']}_{self.nested_attrs.replace('-', '_')}"

        self.enum_name = f"{attr_set.name_prefix}{self.name}"
        self.enum_name = self.enum_name.upper().replace('-', '_')
        self.c_name = self.name.replace('-', '_')
        if self.c_name in c_kw:
            self.c_name += '_'

    def __getitem__(self, key):
        return self.attr[key]

    def __contains__(self, key):
        return key in self.attr

    def is_multi_val(self):
        return None

    def is_scalar(self):
        return self.type in {'u8', 'u16', 'u32', 'u64', 's32', 's64'}

    def presence_member(self, ri):
        pfx = '__' if ri.ku_space == 'user' else ''
        ri.cw.p(f"{pfx}u32 {self.name}_present:1;")

    def _complex_member_type(self, ri):
        return None

    def arg_member(self, ri):
        member = self._complex_member_type(ri)
        if member:
            return [member + ' *' + self.c_name]
        raise Exception(f"Struct member not implemented for class type {self.type}")

    def struct_member(self, ri):
        if self.is_multi_val():
            ri.cw.p(f"unsigned int n_{self.c_name};")
        member = self._complex_member_type(ri)
        if member:
            ptr = ''
            if self. is_multi_val():
                ptr = '*'
            ri.cw.p(f"{member} {ptr}{self.c_name};")
            return
        members = self.arg_member(ri)
        for one in members:
            ri.cw.p(one + ';')

    def _attr_typol(self):
        raise Exception(f"Type policy not implemented for class type {self.type}")

    def attr_typol(self, cw):
        typol = self._attr_typol()
        cw.p(f'[{self.enum_name}] = {"{"} .name = "{self.name}", {typol}{"}"},')

    def _attr_put_line(self, ri, var, line):
        ri.cw.p(f"if ({var}->{self.name}_present)")
        ri.cw.p(f"{line};", add_ind=1)

    def _attr_put_simple(self, ri, var, put_type):
        line = f"mnl_attr_put_{put_type}(nlh, {self.enum_name}, {var}->{self.name})"
        self._attr_put_line(ri, var, line)

    def attr_put(self, ri, var):
        raise Exception(f"Put not implemented for class type {self.type}")

    def _attr_get(self, ri, var, lines, init_lines=None, local_vars=None):
        if type(lines) is str:
            lines = [lines]
        if type(init_lines) is str:
            init_lines = [init_lines]

        ri.cw.block_start(line=f"if (mnl_attr_get_type(attr) == {self.enum_name})")
        if local_vars:
            for local in local_vars:
                ri.cw.p(local)
            ri.cw.nl()

        if not self.is_multi_val():
            ri.cw.p(f"if (ynl_attr_validate(yarg, attr))")
            ri.cw.p(f"\treturn MNL_CB_ERROR;")
            ri.cw.p(f"{var}->{self.name}_present = 1;")

        if init_lines:
            ri.cw.nl()
            for line in init_lines:
                ri.cw.p(line)

        for line in lines:
            ri.cw.p(line)
        ri.cw.block_end()

    def attr_get(self, ri, var):
        raise Exception(f"Put not implemented for class type {self.type}")

    def _setter_lines(self, ri, member):
        raise Exception(f"Setter not implemented for class type {self.type}")

    def setter(self, ri, space, direction, deref=False, ref=None):
        ref = (ref if ref else []) + [self.c_name]
        var = "req"
        member = f"{var}->{'.'.join(ref)}"

        code = []
        for i in range(0, len(ref)):
            code.append(f"{var}->{'.'.join(ref[:i + 1])}_present = 1;")
        code += self._setter_lines(ri, member)

        ri.cw.write_func('static inline void',
                         f"{op_prefix(ri, direction, deref=deref)}_set_{'_'.join(ref)}",
                         body=code,
                         args=[f'{type_name(ri, direction, deref=deref)} *{var}'] + self.arg_member(ri))


class TypeUnused(Type):
    def _attr_typol(self):
        return '.type = YNL_PT_REJECT, '


class TypeScalar(Type):
    def _mnl_type(self):
        t = self.type
        # mnl does not have a helper for signed types
        if t[0] == 's':
            t = 'u' + t[1:]
        return t

    def _attr_typol(self):
        return f'.type = YNL_PT_U{self.type[1:]}, '

    def arg_member(self, ri):
        if 'enum' in self.attr:
            t = f"enum {ri.family.name}_{self.attr['enum']} "
        else:
            scalar_pfx = '__' if ri.ku_space == 'user' else ''
            t = scalar_pfx + self.type + ' '
        return [f'{t}{self.c_name}']

    def attr_put(self, ri, var):
        self._attr_put_simple(ri, var, self._mnl_type())

    def attr_get(self, ri, var):
        self._attr_get(ri, var, f"{var}->{self.c_name} = mnl_attr_get_{self._mnl_type()}(attr);")

    def _setter_lines(self, ri, member):
        return [f"{member} = {self.c_name};"]


class TypeFlag(Type):
    def arg_member(self, ri):
        return []

    def _attr_typol(self):
        return '.type = YNL_PT_FLAG, '

    def attr_put(self, ri, var):
        self._attr_put_line(ri, var, f"mnl_attr_put(nlh, {self.enum_name}, 0, NULL)")

    def attr_get(self, ri, var):
        self._attr_get(ri, var, [])

    def _setter_lines(self, ri, member):
        return []


class TypeNulString(Type):
    def arg_member(self, ri):
        return [f"const char *{self.c_name}"]

    def _byte_len(self):
        if self.len[-4:] == " - 1":
            return f"{self.len[:-4]}"
        else:
            return f"{self.len} + 1"

    def struct_member(self, ri):
        ri.cw.p(f"char {self.c_name}[{self._byte_len()}];")

    def _attr_typol(self):
        return f'.type = YNL_PT_NUL_STR, .len = {self._byte_len()}, '

    def attr_put(self, ri, var):
        self._attr_put_simple(ri, var, 'strz')

    def attr_get(self, ri, var):
        self._attr_get(ri, var,
                       [f"strncpy({var}->{self.c_name}, mnl_attr_get_str(attr), {self.len});",
                        f"{var}->{self.c_name}[{self.len}] = 0;"])

    def _setter_lines(self, ri, member):
        return [f'strncpy({member}, {self.c_name}, sizeof({member}));',
                f'{member}[{self.len}] = 0;']


class TypeBinary(Type):
    def arg_member(self, ri):
        return [f"const void *{self.c_name}"]

    def struct_member(self, ri):
        ri.cw.p(f"unsigned char {self.c_name}[{self.len}];")

    def _attr_typol(self):
        return f'.type = YNL_PT_NUL_STR, .len = {self.len}, '

    def attr_put(self, ri, var):
        self._attr_put_line(ri, var, f"mnl_attr_put(nlh, {self.enum_name}, " +
                            f"{self.len}, {var}->{self.name})")

    def attr_get(self, ri, var):
        self._attr_get(ri, var, f"memcpy({var}->{self.name}, mnl_attr_get_payload(attr), {self.len});")

    def _setter_lines(self, ri, member):
        return [f"memcpy({member}, {self.c_name}, {self.len});"]


class TypeNest(Type):
    def _complex_member_type(self, ri):
        return f"struct {self.nested_render_name}"

    def _attr_typol(self):
        return f'.type = YNL_PT_NEST, .nest = &{self.nested_render_name}_nest, '

    def attr_put(self, ri, var):
        self._attr_put_line(ri, var, f"{self.nested_render_name}_put(nlh, " +
                            f"{self.enum_name}, &{var}->{self.name})")

    def attr_get(self, ri, var):
        get_lines = [f"{self.nested_render_name}_parse(&parg, attr);"]
        init_lines = [f"parg.rsp_policy = &{self.nested_render_name}_nest;",
                      f"parg.data = &{var}->{self.name};"]
        self._attr_get(ri, var, get_lines, init_lines=init_lines)

    def setter(self, ri, space, direction, deref=False, ref=None):
        ref = (ref if ref else []) + [self.c_name]

        for _, attr in ri.family.pure_nested_structs[self.nested_attrs].member_list():
            attr.setter(ri, self.nested_attrs, direction, deref=deref, ref=ref)


class TypeMultiAttr(Type):
    def is_multi_val(self):
        return True

    def presence_member(self, ri):
        return

    def _complex_member_type(self, ri):
        if 'sub-type' not in self.attr or self.attr['sub-type'] == 'nest':
            return f"struct {self.nested_render_name}"
        elif self.attr['sub-type'] in scalars:
            scalar_pfx = '__' if ri.ku_space == 'user' else ''
            return scalar_pfx + self.attr['sub-type']
        else:
            raise Exception(f"Sub-type {self.attr['sub-type']} not supported yet")

    def _attr_typol(self):
        if 'sub-type' not in self.attr or self.attr['sub-type'] == 'nest':
            return f'.type = YNL_PT_NEST, .nest = &{self.nested_render_name}_nest, '
        elif self.attr['sub-type'] in scalars:
            return f".type = YNL_PT_U{self.attr['sub-type'][1:]}, "
        else:
            raise Exception(f"Sub-type {self.attr['sub-type']} not supported yet")

    def attr_get(self, ri, var):
        self._attr_get(ri, var, f'{var}->n_{self.name}++;')


class TypeArrayNest(Type):
    def is_multi_val(self):
        return True

    def presence_member(self, ri):
        return

    def _complex_member_type(self, ri):
        if 'sub-type' not in self.attr or self.attr['sub-type'] == 'nest':
            return f"struct {self.nested_render_name}"
        elif self.attr['sub-type'] in scalars:
            scalar_pfx = '__' if ri.ku_space == 'user' else ''
            return scalar_pfx + self.attr['sub-type']
        else:
            raise Exception(f"Sub-type {self.attr['sub-type']} not supported yet")

    def _attr_typol(self):
        return f'.type = YNL_PT_NEST, .nest = &{self.nested_render_name}_nest, '

    def attr_get(self, ri, var):
        local_vars = ['const struct nlattr *attr2;']
        get_lines = [f'attr_{self.name} = attr;',
                     'mnl_attr_for_each_nested(attr2, attr)',
                     f'\t{var}->n_{self.name}++;']
        self._attr_get(ri, var, get_lines, local_vars=local_vars)


class TypeNestTypeValue(Type):
    def _complex_member_type(self, ri):
        return f"struct {self.nested_render_name}"

    def _attr_typol(self):
        return f'.type = YNL_PT_NEST, .nest = &{self.nested_render_name}_nest, '

    def attr_get(self, ri, var):
        prev = 'attr'
        tv_args = ''
        get_lines = []
        local_vars = []
        init_lines = [f"parg.rsp_policy = &{self.nested_render_name}_nest;",
                      f"parg.data = &{var}->{self.name};"]
        if 'type-value' in self.attr:
            local_vars += [f'const struct nlattr *attr_{", *attr_".join(self.attr["type-value"])};']
            local_vars += [f'__u32 {", ".join(self.attr["type-value"])};']
            for level in self.attr["type-value"]:
                get_lines += [f'attr_{level} = mnl_attr_get_payload({prev});']
                get_lines += [f'{level} = mnl_attr_get_type(attr_{level});']
                prev = 'attr_' + level

            tv_args = f", {', '.join(self.attr['type-value'])}"

        get_lines += [f"{self.nested_render_name}_parse(&parg, {prev}{tv_args});"]
        self._attr_get(ri, var, get_lines, init_lines=init_lines, local_vars=local_vars)


class Struct:
    def __init__(self, family, space_name, type_list=None, inherited=None):
        self.family = family
        self.space_name = space_name
        self.attr_set = family.attr_sets[space_name]
        # Use list to catch comparisons with empty sets
        self.inherited = inherited if inherited is not None else []

        self.nested = type_list is None
        self.render_name = f"{family['name']}_{space_name.replace('-', '_')}"
        self.struct_name = 'struct ' + self.render_name
        self.ptr_name = self.struct_name + ' *'

        self.request = False
        self.reply = False

        self.attr_list = []
        self.attrs = dict()
        if type_list:
            for t in type_list:
                self.attr_list.append((t, self.attr_set[t]),)
        else:
            for t in self.attr_set:
                self.attr_list.append((t, self.attr_set[t]),)
        for name, attr in self.attr_list:
            self.attrs[name] = attr

    def __iter__(self):
        yield from self.attrs

    def __getitem__(self, key):
        return self.attrs[key]

    def member_list(self):
        return self.attr_list

    def set_inherited(self, new_inherited):
        if self.inherited != new_inherited:
            raise Exception("Inheriting different members not supported")


class AttrSpace:
    def __init__(self, family, yaml):
        self.yaml = yaml

        self.attrs = dict()
        self.name = self.yaml['name']
        if 'subset-of' not in yaml:
            self.subspace_of = None
            self.name_prefix = self.yaml['name-prefix'].upper().replace('-', '_')
        else:
            self.subspace_of = self.yaml['subset-of']
            self.name_prefix = family.attr_sets[self.subspace_of].name_prefix

        self.c_name = self.name.replace('-', '_')
        if self.c_name in c_kw:
            self.c_name += '_'

        for elem in self.yaml['attributes']:
            if elem['type'] in scalars:
                attr = TypeScalar(family, self, elem)
            elif elem['type'] == 'unused':
                attr = TypeUnused(family, self, elem)
            elif elem['type'] == 'flag':
                attr = TypeFlag(family, self, elem)
            elif elem['type'] == 'nul-string':
                attr = TypeNulString(family, self, elem)
            elif elem['type'] == 'binary':
                attr = TypeBinary(family, self, elem)
            elif elem['type'] == 'nest':
                attr = TypeNest(family, self, elem)
            elif elem['type'] == 'multi-attr':
                attr = TypeMultiAttr(family, self, elem)
            elif elem['type'] == 'array-nest':
                attr = TypeArrayNest(family, self, elem)
            elif elem['type'] == 'nest-type-value':
                attr = TypeNestTypeValue(family, self, elem)
            else:
                raise Exception(f"No typed class for type {elem['type']}")

            self.attrs[elem['name']] = attr

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

        if 'proto' in self.yaml and self.yaml['proto'] != 'genetlink':
            raise Exception("Codegen only supported for genetlink")

        if 'constants' not in self.yaml:
            self.yaml['constants'] = []

        self.name = self.yaml['name']
        self.op_prefix = self.yaml['operations']['name-prefix']

        self.attr_cnt_suffix = self.yaml.get('attr-cnt-suffix', 'MAX')

        self.consts = dict()
        self.ops = dict()
        self.attr_sets = dict()

        # dict space-name -> 'request': set(attrs), 'reply': set(attrs)
        self.root_sets = dict()
        # dict space-name -> set('request', 'reply')
        self.pure_nested_structs = dict()
        self.all_notify = dict()

        self._dictify()
        self._load_root_sets()
        self._load_nested_sets()
        self._load_all_notify()

    def __getitem__(self, key):
        return self.yaml[key]

    def get(self, key, default=None):
        return self.yaml.get(key, default)

    def _dictify(self):
        for elem in self.yaml['constants']:
            self.consts[elem['name']] = elem

        for elem in self.yaml['attribute-sets']:
            self.attr_sets[elem['name']] = AttrSpace(self, elem)

        ntf = []
        for elem in self.yaml['operations']['list']:
            if 'notify' in elem:
                ntf.append(elem)
                continue
            if 'attribute-set' not in elem:
                continue
            self.ops[elem['name']] = Operation(self, elem)
        for n in ntf:
            self.ops[n['notify']].add_notification(n)

    def _load_root_sets(self):
        for op_name, op in self.ops.items():
            if 'attribute-set' not in op:
                continue

            req_attrs = set()
            rsp_attrs = set()
            for op_mode in ['do', 'dump']:
                if op_mode in op and 'request' in op[op_mode]:
                    req_attrs.update(set(op[op_mode]['request']['attributes']))
                if op_mode in op and 'reply' in op[op_mode]:
                    rsp_attrs.update(set(op[op_mode]['reply']['attributes']))

            if op['attribute-set'] not in self.root_sets:
                self.root_sets[op['attribute-set']] = {'request': req_attrs, 'reply': rsp_attrs}
            else:
                self.root_sets[op['attribute-set']]['request'].update(req_attrs)
                self.root_sets[op['attribute-set']]['reply'].update(rsp_attrs)

    def _load_nested_sets(self):
        for root_set, rs_members in self.root_sets.items():
            for attr, spec in self.attr_sets[root_set].items():
                if 'nested-attributes' in spec:
                    inherit = set()
                    nested = spec['nested-attributes']
                    if nested not in self.root_sets:
                        self.pure_nested_structs[nested] = Struct(self, nested, inherited=inherit)
                    if attr in rs_members['request']:
                        self.pure_nested_structs[nested].request = True
                    if attr in rs_members['reply']:
                        self.pure_nested_structs[nested].reply = True

                    if 'type-value' in spec:
                        if nested in self.root_sets:
                            raise Exception("Inheriting members to a space used as root not supported")
                        inherit.update(set(spec['type-value']))
                    elif spec['type'] == 'array-nest':
                        inherit.add('idx')
                    self.pure_nested_structs[nested].set_inherited(inherit)

    def _load_all_notify(self):
        for op_name, op in self.ops.items():
            if not op:
                continue

            if 'notify' in op:
                self.all_notify[op_name] = op['notify']['cmds']


class RenderInfo:
    def __init__(self, cw, family, ku_space, op, op_name, op_mode, attr_set=None):
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

        self.attr_set = attr_set
        if not self.attr_set:
            self.attr_set = op['attribute-set']

        if op:
            self.type_name = op_name.replace('-', '_')
        else:
            self.type_name = attr_set.replace('-', '_')

        self.cw = cw

        self.struct = dict()
        for op_dir in ['request', 'reply']:
            if op and op_dir in op[op_mode]:
                self.struct[op_dir] = Struct(family, self.attr_set,
                                             type_list=op[op_mode][op_dir]['attributes'])


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

    def write_func_prot(self, qual_ret, name, args=None, doc=None, suffix=''):
        if not args:
            args = ['void']

        if doc:
            self.p('/*')
            self.p(' * ' + doc)
            self.p(' */')

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

    def write_func(self, qual_ret, name, body, args=None, local_vars=None):
        self.write_func_prot(qual_ret=qual_ret, name=name, args=args)
        self.write_func_lvar(local_vars=local_vars)

        self.block_start()
        for line in body:
            self.p(line)
        self.block_end()

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


def rdir(direction):
    if direction == 'reply':
        return 'request'
    if direction == 'request':
        return 'reply'
    return direction


def op_prefix(ri, direction, deref=False):
    suffix = f"_{ri.type_name}"

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


def type_name(ri, direction, deref=False):
    return f"struct {op_prefix(ri, direction, deref=deref)}"


def attribute_policy(ri, attr, prototype=True, suffix=""):
    aspace = ri.family.attr_sets[ri.attr_set]
    spec = aspace[attr]

    policy = 'NLA_' + spec['type'].upper()
    policy = policy.replace('-', '_')

    if 'flags-mask' in spec:
        flags = ri.family.consts[spec['flags-mask']]
        flag_cnt = len(flags['values'])
        mem = f"NLA_POLICY_MASK({policy}, 0x{(1 << flag_cnt) - 1:x})"
    elif 'enum' in spec:
        enum = ri.family.consts[spec['enum']]
        cnt = len(enum['values'])
        mem = f"NLA_POLICY_MAX({policy}, {cnt})"
    else:
        mem = '{ .type = ' + policy
        if 'len' in spec:
            mem += ', .len = ' + str(spec['len'])
        mem += ' }'

    ri.cw.p(f"\t[{aspace.name_prefix}{attr.upper()}] = {mem},")


def attribute_parse_kernel(ri, attr, prototype=True, suffix=""):
    aspace = ri.family.attr_sets[ri.attr_set]
    spec = aspace[attr]

    t = spec['type']
    ri.cw.block_start(line=f"if (tb[{aspace.name_prefix}{attr.upper()}])")
    ri.cw.p(f"req->{attr}_present = 1;")
    if t in scalars:
        ri.cw.p(f"req->{attr} = nla_get_{t}(tb[{aspace.name_prefix}{attr.upper()}]);")
    elif t == 'nul-string':
        ri.cw.p(f"strcpy(req->{attr}, nla_data(tb[{aspace.name_prefix}{attr.upper()}]));")
    ri.cw.block_end()


def print_prototype(ri, direction, terminate=True, doc=None):
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

    ri.cw.write_func_prot(ret, fname, args, doc=doc, suffix=suffix)


def print_req_prototype(ri):
    print_prototype(ri, "request", doc=ri.op['description'])


def print_dump_prototype(ri):
    print_prototype(ri, "request")


def put_typol_fwd(cw, struct):
    cw.p(f'extern struct ynl_policy_nest {struct.render_name}_nest;')


def put_typol(cw, struct):
    type_max = f"{struct.attr_set.name_prefix}MAX"
    cw.block_start(line=f'struct ynl_policy_attr {struct.render_name}_policy[{type_max} + 1] =')

    for _, arg in struct.member_list():
        arg.attr_typol(cw)

    cw.block_end(line=';')
    cw.nl()

    cw.block_start(line=f'struct ynl_policy_nest {struct.render_name}_nest =')
    cw.p(f'.max_attr = {type_max},')
    cw.p(f'.table = {struct.render_name}_policy,')
    cw.block_end(line=';')
    cw.nl()


def put_req_nested(ri, struct):
    func_args = ['struct nlmsghdr *nlh',
                 'unsigned int attr_type',
                 f'{struct.ptr_name}obj']

    ri.cw.write_func_prot('int', f'{struct.render_name}_put', func_args)
    ri.cw.block_start()
    ri.cw.write_func_lvar('struct nlattr *nest;')

    ri.cw.p("nest = mnl_attr_nest_start(nlh, attr_type);")

    for _, arg in struct.member_list():
        arg.attr_put(ri, "obj")

    ri.cw.p("mnl_attr_nest_end(nlh, nest);")

    ri.cw.nl()
    ri.cw.p('return 0;')
    ri.cw.block_end()
    ri.cw.nl()


def _multi_parse(ri, struct, init_lines, local_vars):
    if struct.nested:
        iter_line = "mnl_attr_for_each_nested(attr, nested)"
    else:
        iter_line = "mnl_attr_for_each(attr, nlh, sizeof(struct genlmsghdr))"

    array_nests = set()
    multi_attrs = set()
    needs_parg = False
    for arg, aspec in struct.member_list():
        if aspec['type'] == 'array-nest':
            local_vars.append(f'const struct nlattr *attr_{arg};')
            array_nests.add(arg)
        if aspec['type'] == 'multi-attr':
            multi_attrs.add(arg)
        needs_parg |= 'nested-attributes' in aspec
    if array_nests or multi_attrs:
        local_vars.append('int i;')
    if needs_parg:
        local_vars.append('struct ynl_parse_arg parg;')
        init_lines.append('parg.ys = yarg->ys;')

    ri.cw.block_start()
    ri.cw.write_func_lvar(local_vars)

    for line in init_lines:
        ri.cw.p(line)
    ri.cw.nl()

    for arg in sorted(struct.inherited):
        ri.cw.p(f'dst->{arg} = {arg};')

    ri.cw.nl()
    ri.cw.block_start(line=iter_line)

    for _, arg in struct.member_list():
        arg.attr_get(ri, 'dst')

    ri.cw.block_end()
    ri.cw.nl()

    for anest in sorted(array_nests):
        aspec = struct[anest]

        ri.cw.block_start(line=f"if (dst->n_{anest})")
        ri.cw.p(f"dst->{anest} = calloc(dst->n_{anest}, sizeof(*dst->{anest}));")
        ri.cw.p('i = 0;')
        ri.cw.p(f"parg.rsp_policy = &{aspec.nested_render_name}_nest;")
        ri.cw.block_start(line=f"mnl_attr_for_each_nested(attr, attr_{anest})")
        ri.cw.p(f"parg.data = &dst->{anest}[i];")
        ri.cw.p(f"{aspec.nested_render_name}_parse(&parg, attr, mnl_attr_get_type(attr));")
        ri.cw.p('i++;')
        ri.cw.block_end()
        ri.cw.block_end()
    ri.cw.nl()

    for anest in sorted(multi_attrs):
        aspec = struct[anest]
        ri.cw.block_start(line=f"if (dst->n_{anest})")
        ri.cw.p(f"dst->{anest} = calloc(dst->n_{anest}, sizeof(*dst->{anest}));")
        ri.cw.p('i = 0;')
        if 'nested-attributes' in aspec:
            ri.cw.p(f"parg.rsp_policy = &{aspec.nested_render_name}_nest;")
        ri.cw.block_start(line=iter_line)
        ri.cw.block_start(line=f"if (mnl_attr_get_type(attr) == {aspec.enum_name})")
        if 'nested-attributes' in aspec:
            ri.cw.p(f"parg.data = &dst->{anest}[i];")
            ri.cw.p(f"{aspec.nested_render_name}_parse(&parg, attr);")
        elif aspec['sub-type'] in scalars:
            t = aspec['sub-type']
            if t[0] == 's':
                t = 'u' + t[1:]
            ri.cw.p(f"dst->{aspec.c_name}[i] = mnl_attr_get_{t}(attr);")
        else:
            raise Exception('Nest parsing type not supported yet')
        ri.cw.p('i++;')
        ri.cw.block_end()
        ri.cw.block_end()
        ri.cw.block_end()
    ri.cw.nl()

    if struct.nested:
        ri.cw.p('return 0;')
    else:
        ri.cw.p('return MNL_CB_OK;')
    ri.cw.block_end()
    ri.cw.nl()


def parse_rsp_nested(ri, struct):
    func_args = ['struct ynl_parse_arg *yarg',
                 'const struct nlattr *nested']
    for arg in sorted(struct.inherited):
        func_args.append('__u32 ' + arg)

    local_vars = ['const struct nlattr *attr;',
                  f'{struct.ptr_name}dst = yarg->data;']
    init_lines = []

    ri.cw.write_func_prot('int', f'{struct.render_name}_parse', func_args)

    _multi_parse(ri, struct, init_lines, local_vars)


def parse_rsp_msg(ri, deref=False):
    if 'reply' not in ri.op[ri.op_mode]:
        return

    func_args = ['const struct nlmsghdr *nlh',
                 'void *data']

    local_vars = [f'{type_name(ri, "reply", deref=deref)} *dst;',
                  'struct ynl_parse_arg *yarg = data;',
                  'const struct nlattr *attr;']
    init_lines = ['dst = yarg->data;']

    ri.cw.write_func_prot('int', f'{op_prefix(ri, "reply", deref=deref)}_parse', func_args)

    _multi_parse(ri, ri.struct["reply"], init_lines, local_vars)


def print_req(ri):
    ret_ok = '0'
    ret_err = '-1'
    direction = "request"
    local_vars = ['struct nlmsghdr *nlh;',
                  'int len, err;']

    if 'reply' in ri.op[ri.op_mode]:
        ret_ok = 'rsp'
        ret_err = 'NULL'
        local_vars += [f'{type_name(ri, rdir(direction))} *rsp;',
                       'struct ynl_parse_arg yarg = { .ys = ys, };']

    print_prototype(ri, direction, terminate=False)
    ri.cw.block_start()
    ri.cw.write_func_lvar(local_vars)

    ri.cw.p(f"nlh = ynl_gemsg_start_req(ys, {ri.nl.get_family_id()}, {ri.op.enum_name}, 1);")

    ri.cw.p(f"ys->req_policy = &{ri.struct['request'].render_name}_nest;")
    ri.cw.nl()
    for _, attr in ri.struct["request"].member_list():
        attr.attr_put(ri, "req")
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
	yarg.data = rsp;

	err = {ri.nl.parse_cb_run(op_prefix(ri, "reply") + "_parse", '&yarg', False)};
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

    ri.cw.p('yds.ys = ys;')
    ri.cw.p('yds.alloc_sz = sizeof(*rsp);')
    ri.cw.p(f"yds.cb = {op_prefix(ri, 'reply', deref=True)}_parse;")
    ri.cw.nl()
    ri.cw.p(f"nlh = ynl_gemsg_start_dump(ys, {ri.nl.get_family_id()}, {ri.op.enum_name}, 1);")

    if "request" in ri.op[ri.op_mode]:
        ri.cw.p(f"ys->req_policy = &{ri.struct['request'].render_name}_nest;")
        ri.cw.nl()
        for _, attr in ri.struct["request"].member_list():
            attr.attr_put(ri, "req")
    ri.cw.nl()

    ri.cw.p(f"""err = mnl_socket_sendto(ys->sock, nlh, nlh->nlmsg_len);
	if (err < 0)
		return NULL;

	do {'{'}
		len = mnl_socket_recvfrom(ys->sock, ys->rx_buf, MNL_SOCKET_BUFFER_SIZE);
		if (len < 0)
			goto free_list;

		err = {ri.nl.parse_cb_run('ynl_dump_trampoline', '&yds', False, indent=2)};
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


def _print_type(ri, direction, struct):
    suffix = f'_{ri.type_name}{direction_to_suffix[direction]}'

    if ri.op_mode == 'dump':
        suffix += '_dump'

    ri.cw.block_start(line=f"struct {ri.family['name']}{suffix}")
    for _, attr in struct.member_list():
        attr.presence_member(ri)
    ri.cw.nl()

    for arg in sorted(struct.inherited):
        ri.cw.p(f"__u32 {arg};")

    for _, attr in struct.member_list():
        attr.struct_member(ri)

    ri.cw.block_end(line=';')
    ri.cw.nl()


def print_type(ri, direction):
    _print_type(ri, direction, ri.struct[direction])


def print_type_full(ri, struct):
    _print_type(ri, "", struct)


def print_type_helpers(ri, direction, deref=False):
    print_free_prototype(ri, direction)

    if ri.ku_space == 'user' and direction == 'request':
        for _, attr in ri.struct[direction].member_list():
            attr.setter(ri, ri.attr_set, direction, deref=deref)
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


def _free_type_members(ri, var, struct, ref=''):
    for arg, attr in struct.member_list():
        if attr.is_multi_val():
            ri.cw.p(f'free({var}->{ref}{arg});')
    ri.cw.p(f'free({var});')


def _free_type(ri, direction, struct):
    var = free_arg_name(ri, direction)

    print_free_prototype(ri, direction, suffix='')
    ri.cw.block_start()
    _free_type_members(ri, var, struct)
    ri.cw.block_end()
    ri.cw.nl()


def free_rsp_nested(ri, struct):
    _free_type(ri, "", struct)


def print_rsp_free(ri):
    if 'reply' not in ri.op[ri.op_mode]:
        return
    _free_type(ri, 'reply', ri.struct['reply'])


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

    _free_type_members(ri, 'rsp', ri.struct['reply'], ref='obj.')
    ri.cw.block_end()
    ri.cw.block_end()
    ri.cw.nl()


def print_ntf_type_free(ri):
    print_free_prototype(ri, 'reply', suffix='')
    ri.cw.block_start()
    _free_type_members(ri, 'rsp', ri.struct['reply'], ref='obj.')
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
                        'struct ynl_parse_arg yarg = { .ys = ys, };',
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
    cw.p('yarg.data = rsp->data;')
    cw.nl()
    cw.p(f"err = {cw.nlib.parse_cb_run('parse', '&yarg', True)};")
    cw.p('if (err)')
    cw.p('\tgoto err_free;')
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
    hdr_prot = f"_UAPI_LINUX_{family.name.upper()}_H"
    cw.p('#ifndef ' + hdr_prot)
    cw.p('#define ' + hdr_prot)
    cw.nl()

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
                    item_name = (const['value-prefix'] + item).upper().replace('-', '_')
                cw.p(item_name + ',')
            cw.block_end(line=';')
            cw.nl()
        elif const['type'] == 'flags':
            uapi_enum_start(family, cw, const, 'name')
            i = 0
            for item in const['values']:
                item_name = item
                if 'value-prefix' in const:
                    item_name = (const['value-prefix'] + item).upper().replace('-', '_')
                cw.p(f'{item_name} = {1 << i},')
                i += 1
            cw.block_end(line=';')
            cw.nl()

    for aspace in family['attribute-sets']:
        if 'subset-of' in aspace:
            continue

        pfx = aspace['name-prefix'].upper().replace('-', '_')
        uapi_enum_start(family, cw, aspace, 'name-enum')
        for attr in aspace['attributes']:
            attr_name = pfx + attr['name'].upper().replace('-', '_')
            cw.p(attr_name + ',')
        cw.nl()
        cw.p(f"__{pfx}{family.attr_cnt_suffix}")
        cw.block_end(line=';')
        cw.p(f"#define {pfx}MAX (__{pfx}{family.attr_cnt_suffix} - 1)")
        cw.nl()

    separate_ntf = 'async-prefix' in family['operations']

    uapi_enum_start(family, cw, family['operations'], 'name-enum')
    for op in family['operations']['list']:
        if separate_ntf and ('notify' in op or 'event' in op):
            continue

        op_name = family['operations']['name-prefix'] + op['name'].upper()
        suffix = ','
        if 'value' in op:
            suffix = f" = {op['value']},"
        cw.p(op_name.replace('-', '_') + suffix)
    cw.block_end(line=';')
    cw.nl()

    if separate_ntf:
        uapi_enum_start(family, cw, family['operations'], 'async-enum')
        for op in family['operations']['list']:
            if separate_ntf and not ('notify' in op or 'event' in op):
                continue

            op_name = family['operations']['async-prefix'] + op['name'].upper()
            suffix = ','
            if 'value' in op:
                suffix = f" = {op['value']},"
            cw.p(op_name.replace('-', '_') + suffix)
        cw.block_end(line=';')
        cw.nl()

    cw.p(f'#endif /* {hdr_prot} */')


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

    if args.header:
        hdr_prot = f"_LINUX_{parsed.name.upper()}_GEN_H"
        cw.p('#ifndef ' + hdr_prot)
        cw.p('#define ' + hdr_prot)
        cw.nl()

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
            for attr_set, struct in sorted(parsed.pure_nested_structs.items()):
                ri = RenderInfo(cw, parsed, args.mode, "", "", "", attr_set)
                print_type_full(ri, struct)

        for op_name, op in parsed.ops.items():
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
        cw.nl()

        cw.p(f'#endif /* {hdr_prot} */')
    else:
        if args.mode == "user":
            cw.p('// Policies')
            for name, _ in parsed.attr_sets.items():
                struct = Struct(parsed, name)
                put_typol_fwd(cw, struct)
            cw.nl()

            for name, _ in parsed.attr_sets.items():
                struct = Struct(parsed, name)
                put_typol(cw, struct)

            cw.p('// Common nested types')
            for attr_set, struct in sorted(parsed.pure_nested_structs.items()):
                ri = RenderInfo(cw, parsed, args.mode, "", "", "", attr_set)

                free_rsp_nested(ri, struct)
                if struct.request:
                    put_req_nested(ri, struct)
                if struct.reply:
                    parse_rsp_nested(ri, struct)

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
