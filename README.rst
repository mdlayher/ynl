=========================
YAML netlink project tree
=========================

This is the development tree for the YAML netlink protocol description
project. It is a clone of the kernel tree.

Contributions most welcome, feel free to take up whatever parts of the
project seem interesting... that said the vision my be largely in my
head. Don't hesitate to ask questions.

The interesting bits are in the following locations:

 - user facing docs - Documentation/userspace-api/netlink/
 - actual YAML bindings - Documentation/netlink/
 - tools/net/ - code generators, samples, C library etc.

and this document itself :)

Goals
=====

Provide language independent and self-sufficient (no C header parsing required)
Netlink protocol description.

On the kernel side we'd like to generate the policy tables, ops tables,
uAPI and docs from the YAML for new families.

Some other info in the RFC posting: https://lore.kernel.org/all/20220811022304.583300-1-kuba@kernel.org/

TODO
====

finish the model
----------------

Current thinking is that we should have 4 models / protocols:

 - genetlink   - for new families, prescrpitive, forcing some consistency
 - genetlink-c - for old families, allowing extra attributes to control
   the c codegen like prefixes of names; for non-C languages it should
   be equivalent to "genetlink"
 - genetlink-legacy - allowing more weirdness and quirks and odd attribute types
 - netlink-raw - classic netlink, in case we want to use YAML to describe it

We need to beef up the "constants" section to also cover binary structs.
Probably rename it to "definitions" or something.

notification models
-------------------

I'm thinking we need to notification types - notifications and events.
The former are direct mapping of a GET, for brevity they don't have to
repeat all the arguments. Events have their own list of arguments.
This needs to be documented.

document the kernel side
------------------------

Guidance for YAML use, how to write a new family.

integrate with the kernel build etc.
------------------------------------

ynl-regen.sh is supposed to be a script which can be run on the Linux tree
and make sure that all the YAML-generated code is up to date.

The thinking is that after adding the new family user would run
ynl-gen-c.py manually and then only ynl-regen.sh to update attrs, ops, etc.

doc generation
--------------

The descriptions from the YAML schema should get rendered as part
of the user space API, Sphinx integration would be better than
codegen.

codegen cleanup
---------------

The ynl-gen-c.py is currently a dumpster fire of badly written Python.

struct support
--------------

structs are defined in the schema but not supported in attrs,
ops or any of the codegen.

Spec reshuffling
----------------

 - add support for variable binary and fixed string

Notes on existing families
==========================

Command enum model:

nlctrl: unified
VFS_DQUOT: weird but I think unified
thermal: notify-split
devlink: unified
ethtool: directional
netlabel: no damn idea
ncsi: unified
tcp_metrics: unified
mptcp: notify-split
seg6: unified
ioam6: unified
nl80211: unified
psample: unified
