#!/usr/bin/env python
# SPDX-License-Identifier: BSD-3-Clause

import argparse

from ynl import YnlFamily


def main():
    parser = argparse.ArgumentParser(description='YNL ethtool sample')
    parser.add_argument('--spec', dest='spec', type=str, required=True)
    parser.add_argument('--schema', dest='schema', type=str)
    parser.add_argument('--device', dest='dev_name', type=str)
    parser.add_argument('--ifindex', dest='ifindex', type=str)
    args = parser.parse_args()

    ynl = YnlFamily(args.spec)

    if args.dev_name:
        channels = ynl.channels_get({'header': {'dev_name': args.dev_name}})
    elif args.ifindex:
        channels = ynl.channels_get({'header': {'dev_index': args.ifindex}})
    else:
        return
    print('Netlink responded with:')
    print(channels)


if __name__ == "__main__":
    main()
