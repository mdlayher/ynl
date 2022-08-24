.. SPDX-License-Identifier: BSD-3-Clause

==============================
Netlink YAML C code generation
==============================

This document describes how YAML specifications are used to render
C code (uAPI, policies etc.). It also defines the additional properties
allowed in older families by the ``genetlink-c`` protocol level, to
control the naming directly.

Names
=====

For C codegen we use the attr set name to prefix the enum name.                 
The main attr space, however, is often prefix-less. So let's                    
use assume that if the space name is $family then the name should               
be skipped when rendering uAPI.  

Old
===

Each attribute set has properties used to render uAPI header enums. ``name-prefix``
is prepended to the name of each attribute, allowing the attribute names to be shorter
compared to the enum names in the uAPI.

Optionally attribute set may contain ``enum-name`` if the uAPI header's enum should
have a name. Most netlink uAPIs do not name attribute enums, the attribute names are
heavily prefixed, which is sufficient.
