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


scalars = {'u8', 'u16', 'u32'}


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


def attribute_member(family, space, attr, prototype=True, suffix=""):
    spec = family["attributes"]["list"][space]["list"][attr]

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
        t = f"struct {family['name']}_{spec['nested-attributes']} *"
        if not prototype:
            print(f"\tunsigned int n_{attr};")
    elif t in scalars:
        t = '__' + t + ' '

    print(f"\t{t}{attr}{suffix}")


def attribute_parse_kernel(family, space, attr, prototype=True, suffix=""):
    aspace = family["attributes"]["list"][space]
    spec = aspace["list"][attr]

    t = spec['type']
    if t in scalars:
        print(f"\tif (tb[{aspace['name-prefix']}{attr.upper()}]) " + '{')
        print(f"\t\treq->{attr}_present = 1;")
        print(f"\t\treq->{attr} = nla_get_{t}(tb[{aspace['name-prefix']}{attr.upper()}]);")
        print('\t}')


def print_prototype(family, fam_name, op, mode, op_name, direction):
    print(f"int {fam_name}_{op_name}(")
    prev = None
    for arg in op[mode][direction]:
        if prev:
            attribute_member(family, op["attribute-space"], prev, suffix=',')
        prev = arg
    if prev:
        attribute_member(family, op["attribute-space"], prev)
    print(");")


def print_req_prototype(family, fam_name, op, mode, op_name):
    print_prototype(family, fam_name, op, mode, op_name, "request")


def print_type(family, fam_name, op, mode, op_name, direction):
    suffix = "_rsp" if direction == "reply" else "_req"
    print(f"struct {fam_name}_{op_name}{suffix} " + '{')
    for arg in op[mode][direction]:
        attribute_member(family, op["attribute-space"], arg, prototype=False, suffix=';')
    print("};")
    print(f"void {fam_name}_{op_name}_rsp_free(struct {fam_name}_{op_name}_rsp *{op_name});")


def print_parse_kernel(family, fam_name, op, mode, op_name, direction):
    suffix = "_rsp" if direction == "reply" else "_req"
    print(f"int {fam_name}_{op_name}{suffix}_parse(.. req) ")
    print('{')
    for arg in op[mode][direction]:
        attribute_parse_kernel(family, op["attribute-space"], arg, prototype=False, suffix=';')
    print("}")


def print_req_type(family, fam_name, op, mode, op_name):
    print_type(family, fam_name, op, mode, op_name, "request")


def print_rsp_type(family, fam_name, op, mode, op_name):
    print_type(family, fam_name, op, mode, op_name, "reply")


def print_req_policy(family, fam_name, op, mode, op_name):
    print(f"static const struct nla_policy {fam_name}_{op_name}_policy[] = " + '{')
    for arg in op[mode]["request"]:
        attribute_policy(family, op["attribute-space"], arg)
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

    print(f"#include <{parsed['headers'][args.mode]}>\n")

    fam = parsed["name"]

    for op_name in parsed['operations']['list']:
        op = parsed['operations']['list'][op_name]

        print(f"// Codegen for {parsed['operations']['name-prefix']}{op_name.upper()}")

        if op and "do" in op:
            if args.mode == "user":
                print_req_prototype(parsed, fam, op, "do", op_name)
                print()
                print_rsp_type(parsed, fam, op, "do", op_name)
            elif args.mode == "kernel":
                print_req_type(parsed, fam, op, "do", op_name)
                print()
                print_parse_kernel(parsed, fam, op, "do", op_name, "request")
                print()
                print_req_policy(parsed, fam, op, "do", op_name)
            print()


if __name__ == "__main__" :
    main()


