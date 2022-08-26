.. SPDX-License-Identifier: BSD-3-Clause

================================================
YAML support for legacy Generic Netlink families
================================================

This document describes the many additional quirks and properties
required to describe older Generic Netlink families which form
the ``genetlink-legacy`` protocol level.

The spec is a work in progress, some of the quirks are just documented
for future reference (and a warning about things *not* to do in new families).

Specification
=============

Other quirks
============

Multi-message DO
----------------

New Netlink families should never respond to a DO operation with multiple
replies, with ``NLM_F_MULTI`` set. Use a filtered dump instead.

At the spec level we can define a ``dumps`` property for the ``do``,
perhaps with values of ``combine`` and ``multi-object`` depending
on how the parsing should be implemented (parse into a single reply
vs list of objects i.e. pretty much a dump).
