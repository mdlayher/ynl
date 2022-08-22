.. SPDX-License-Identifier: BSD-3-Clause

==============================
Netlink YAML C code generation
==============================

Should this be in the kernel section?

Names
=====

For C codegen we use the attr set name to prefix the enum name.                 
The main attr space, however, is often prefix-less. So let's                    
use assume that if the space name is $family then the name should               
be skipped when rendering uAPI.  
