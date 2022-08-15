.. SPDX-License-Identifier: BSD-3-Clause

=======================
Introduction to Netlink
=======================

Netlink is often described as a ioctl() replacement mechanism.
The basic idea is to replace fixed-format C structures as supplied
to ioctl() with a format which allows an easy way to added or
extended the attributes/members.

To achieve this Netlink uses a minimal fixed-format metadata header
which followed by TLV (type, length, value) structures.

Unfortunately the protocol has evolved over the years, in an organic
and undocumented fashion making it hard to coherently explain.
To make the most practical sense this document starts by describing
netlink as it is used today and dives into more "historical" uses
in later sections.

Generic Netlink
===============

