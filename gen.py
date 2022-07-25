#!/usr/bin/env python

import argparse
import os
import yaml


class Family:
    def __init__(self, file_name):
        with open(file_name, "r") as stream:
            self.yaml = yaml.safe_load(stream)

    def __getitem__(self, key):
        return self.yaml[key]


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
            self.type_name = attr_space


scalars = {'u8', 'u16', 'u32'}

direction_to_suffix = {
    'reply': '_rsp',
    'request': '_req',
    '': ''
}


def attribute_policy(family, space, attr, prototype=True, suffix=""):
    aspace = family["attributes"]["list"][space]
    spec = aspace["list"][attr]
    policy = 'NLA_' + spec['type'].upper()
    policy = policy.replace('-', '_')

    mem = '{ .type = ' + policy
    if 'len' in spec:
        mem += ', .len = ' + spec['len']
    mem += ' }'

    print(f"\t[{aspace['name-prefix']}{attr.upper()}] = {mem},")


def attribute_member(ri, space, attr, prototype=True, suffix=""):
    spec = ri.family["attributes"]["list"][space]["list"][attr]

    t = spec['type']
    if t == "nul-string":
        if prototype:
            t = 'const char *'
        else:
            t = 'char '
            if spec['len'][-4:] == " - 1":
                suffix = f"[{spec['len'][:-4]}]" + suffix
            else:
                suffix = f"[{spec['len']} + 1]" + suffix
    elif t == 'array-nest':
        t = f"struct {ri.family['name']}_{spec['nested-attributes']} *"
        if not prototype:
            print(f"\tunsigned int n_{attr};")
    elif t in scalars:
        pfx = '__' if ri.ku_space == 'user' else ''
        t = pfx + t + ' '

    print(f"\t{t}{attr}{suffix}")


def attribute_pres_member(ri, space, attr, suffix=""):
    spec = ri.family["attributes"]["list"][space]["list"][attr]
    pfx = '__' if ri.ku_space == 'user' else ''

    if 'required' in spec:
        return False

    print(f"\t{pfx}u32 {attr}_present:1{suffix}")
    return True


def attribute_parse_kernel(family, space, attr, prototype=True, suffix=""):
    aspace = family["attributes"]["list"][space]
    spec = aspace["list"][attr]

    t = spec['type']
    print(f"\tif (tb[{aspace['name-prefix']}{attr.upper()}]) " + '{')
    print(f"\t\treq->{attr}_present = 1;")
    if t in scalars:
        print(f"\t\treq->{attr} = nla_get_{t}(tb[{aspace['name-prefix']}{attr.upper()}]);")
    elif t == 'nul-string':
        print(f"\t\tstrcpy(req->{attr}, nla_data(tb[{aspace['name-prefix']}{attr.upper()}]));")
    print('\t}')


def print_prototype(ri, fam_name, op, mode, op_name, direction):
    print(f"int {fam_name}_{op_name}(")
    prev = None
    for arg in op[mode][direction]:
        if prev:
            attribute_member(ri, op["attribute-space"], prev, suffix=',')
        prev = arg
    if prev:
        attribute_member(ri, op["attribute-space"], prev)
    print(");")


def print_req_prototype(ri, fam_name, op, mode, op_name):
    print_prototype(ri, fam_name, op, mode, op_name, "request")


def _print_type(ri, direction, type_list):
    suffix = f'_{ri.type_name}{direction_to_suffix[direction]}'

    print(f"struct {ri.family['name']}{suffix} " + '{')
    any_presence = False
    for arg in type_list:
        any_presence |= \
            attribute_pres_member(ri, ri.attr_space, arg, suffix=';')
    if any_presence:
        print()

    for arg in type_list:
        attribute_member(ri, ri.attr_space, arg, prototype=False, suffix=';')
    print("};")
    print(f"void {ri.family['name']}{suffix}_free(" +
          f"struct {ri.family['name']}{suffix} *{ri.op_name});")


def print_type(ri, direction):
    return _print_type(ri, direction, ri.op[ri.op_mode][direction])


def print_type_full(ri, aspace):
    return _print_type(ri, "", aspace)


def print_parse_prototype(ri, direction, terminate=True):
    suffix = "_rsp" if direction == "reply" else "_req"
    term = ';' if terminate else ''

    print(f"void {ri.family['name']}_{ri.op_name}{suffix}_parse(const struct nlattr **tb," +
          f" struct {ri.family['name']}_{ri.op_name}{suffix} *req){term}")


def print_parse_kernel(ri, direction):
    print_parse_prototype(ri, direction, terminate=False)
    print('{')
    for arg in ri.op[ri.op_mode][direction]:
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
    for arg in ri.op[ri.op_mode]["request"]:
        attribute_policy(ri.family, ri.attr_space, arg)
    print("};")


def main():
    parser = argparse.ArgumentParser(description='Netlink simple parsing generator')
    parser.add_argument('--mode', dest='mode', type=str, required=True)
    parser.add_argument('--spec', dest='spec', type=str, required=True)
    args = parser.parse_args()

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

    fam = parsed["name"]

    print("// Header content")
    print()

    print('// Common nested types')
    for attr_space in parsed['attributes']['list']:
        aspace = parsed['attributes']['list'][attr_space]['list']
        for attr in aspace:
            aspec = aspace[attr]
            if 'nested-attributes' in aspec:
                ri = RenderInfo(parsed, args.mode, "", "", "", aspec['nested-attributes'])
                print_type_full(ri, parsed['attributes']['list'][aspec['nested-attributes']]['list'])
    print()

    for op_name in parsed['operations']['list']:
        op = parsed['operations']['list'][op_name]

        print(f"// {parsed['operations']['name-prefix']}{op_name.upper()}")

        if op and "do" in op:
            ri = RenderInfo(parsed, args.mode, op, op_name, "do")
            if args.mode == "user":
                print_req_prototype(ri, fam, op, "do", op_name)
                print()
                print_rsp_type(ri)
            elif args.mode == "kernel":
                print_req_type(ri)
                print_parse_prototype(ri, "request")
                print_req_policy_fwd(ri)
            print()

    print("// Source content")
    print()

    for op_name in parsed['operations']['list']:
        op = parsed['operations']['list'][op_name]

        print(f"// {parsed['operations']['name-prefix']}{op_name.upper()}")

        if op and "do" in op:
            ri = RenderInfo(parsed, args.mode, op, op_name, "do")
            if args.mode == "kernel":
                print_parse_kernel(ri, "request")
                print()
                print_req_policy(ri)
            print()


if __name__ == "__main__":
    main()
