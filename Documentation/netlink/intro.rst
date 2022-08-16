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

Opening a socket
================

Netlink communication happens over sockets, a socket needs to be
opened first:

.. code-block:: c

  fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_GENERIC);

Not much to be said about the first two arguments, the last argument
is the Netlink protocol, which used to identify the subsystem with
which the socket will communicate.

Initial implementation of Netlink depended on a static allocation
of IDs to subsystems and provided little supporting infrastructure.
Let us refer to those protocols collectively as Classic Netlink.
Generic Netlink (introduced in 2005) allows dynamic ID allocation,
introspection and simplifies dealing with Netlink on the kernel side.

The following section describes how to use Generic Netlink, as the
number of subsystems using Generic Netlink outnumbers the older
protocols by an order of magnitude. Information on how to communicate
with core networking parts of the Linux kernel (or one of the other
20-something subsystems using Classic Netlink listed at the top of
``include/uapi/linux/netlink.h``) is provided later in this document.

Generic Netlink
===============

Netlink protocols define their own fixed metadata headers. A Netlink
message always starts with struct nlmsghdr, which is followed by
a protocol-specific header. In case of Generic Netlink that header
is struct genlmsghdr.

The practical meaning of the fields is as follows:

.. code-block:: c

  struct nlmsghdr {
	__u32	nlmsg_len;	/* Length of message including header */
	__u16	nlmsg_type;	/* Family ID */
	__u16	nlmsg_flags;	/* Request or dump */
	__u32	nlmsg_seq;	/* Sequence number */
	__u32	nlmsg_pid;	/* Endpoint ID, set to 0 */
  };
  struct genlmsghdr {
	__u8	cmd;		/* Command, as defined by family */
	__u8	version;	/* Irrelevant, set to 1 */
	__u16	reserved;	/* Reserved, set to 0 */
  };
  /* TLV attributes follow... */

In Classic Netlink :c:member:`nlmsghdr.nlmsg_type` used to identify
which operation within the sybsystem the message was referring to
(e.g. get information about a netdev). Generic Netlink needs to mux
multiple subsystems in a single protocol so it uses this field to
identify the subsystem, and :c:member:`genlmsghdr.cmd` identifies
the operation instead. (See :ref:`res_fam` for
information on how to find the Family ID of the subsystem of interest.)
Note that the first 16 values (0-15) of this field are reserved for
general protocol messaging both in Classic Netlink and Generic Netlink.
See :ref:`nl_msg_type` for more details.

There are 3 usual types of message exchanges on a Netlink socket:
 - performing a single action (``do`` in Generic Netlink);
 - dumping information (``dump`` in Generic Netlink);
 - getting asynchronous notifications (``multicast`` in Generic Netlink).

Classic Netlink is very flexible and presumably allows other types
of exchanges to happen, but in practice those are the three that get
used.

Asynchronous notifications are sent by the kernel and received by
the user sockets which subscribed to them. ``do`` and ``dump`` requests
are initiated by the user. :c:member:`nlmsghdr.nlmsg_flags` should
be set as follows:

 - ``do`` - ``NLM_F_REQUEST | NLM_F_ACK``
 - ``dump`` - ``NLM_F_REQUEST | NLM_F_ACK | NLM_F_DUMP``

:c:member:`nlmsghdr.nlmsg_seq` should be a set to a monotonically
increasing value. The value gets echoed back in responses and doesn't
matter in practice, but setting it to an increasing value for each
message sent is considered good hygiene.

:c:member:`nlmsghdr.nlmsg_pid` is the Netlink equivalent of an address.
Kernel has the address of ``0`` hence this field should be set to ``0``.
See :ref:`nlmsg_pid` for a historical sidebar.

The expected use for :c:member:`genlmsghdr.version` was to allow
versioning of the APIs provided by the subsystems. No subsystem to
date made significant use of this field, so setting it to ``1`` seems
like a safe bet.

.. _nl_msg_type:

Netlink message types
---------------------

As previously mentioned :c:member:`nlmsghdr.nlmsg_type` carries
protocol specific values but the first 16 identifiers are reserved
(first subsystem specific message type should be equal to
``NLMSG_MIN_TYPE`` which is ``0x10``).

The low-level Netlink protocol messages are:
 - ``NLMSG_NOOP`` - ignore the message, not used in practice;
 - ``NLMSG_ERROR`` - carries the return code of an operation;
 - ``NLMSG_DONE`` - marks the end of a dump;
 - ``NLMSG_OVERRUN`` - socket buffer has overflown.

``NLMSG_ERROR`` and ``NLMSG_DONE`` are of practical importance.
They carry return codes for an operations. Note that unless
the ``NLM_F_ACK`` flag is set Netlink will not report success,
only errors (read: you should always set ``NLM_F_ACK``).

The format of ``NLMSG_ERROR`` is described by struct nlmsgerr::

  ----------------------------------------------
  | struct nlmsghdr - response header          |
  ----------------------------------------------
  |    int error                               |
  ----------------------------------------------
  | struct nlmsghdr - originial request header |
  ----------------------------------------------
  | ** optionally payload of the request       |
  ----------------------------------------------
  | ** optionally extended ACK                 |
  ----------------------------------------------

``NLMSG_ERROR`` carries the information about the request which led
to the error. This should be useful when trying to match requests
to responses or re-parse the request to dump in an error logs.

The request is not echoed in messages reporting success (``error == 0``)
or if ``NETLINK_CAP_ACK`` setsockopt() was set. The latter is common
and perhaps recommended as having to read a copy of every request back
from the kernel is rather wasteful. The absence of request payload
is indicated by ``NLM_F_CAPPED`` being set in
:c:member:`nlmsghdr.nlmsg_flags`.

The second optional element of ``NLMSG_ERROR`` are the extended ACK
attributes. See :ref:`ext_ack` for more details. The presence
of extended ACK is indicated by ``NLM_F_ACK_TLVS`` being set in
:c:member:`nlmsghdr.nlmsg_flags`.

``NLMSG_DONE`` is simpler, the request is not echoed::

  ----------------------------------------------
  | struct nlmsghdr - response header          |
  ----------------------------------------------
  |    int error                               |
  ----------------------------------------------
  | ** optionally extended ACK                 |
  ----------------------------------------------

.. _res_fam:

Resolving the Family ID
-----------------------

This sections explains how to find the Family ID of a subsystem.
It also serves as a general example of Netlink communication.

Generic Netlink is itself a subsystem available the Generic Netlink API.
To avoid a circular dependency Generic Netlink has a statically allocated
Family ID (``GENL_ID_CTRL``). The Generic Netlink family implements
a command used to find out information about other families
(``CTRL_CMD_GETFAMILY``).

To get information about the Generic Netlink family called "test1"
we need to send the following message on the previously opened Generic
Netlink socket::

  struct nlmsghdr:
    __u32 nlmsg_len:	32
    __u16 nlmsg_type:	GENL_ID_CTRL
    __u16 nlmsg_flags:	NLM_F_REQUEST | NLM_F_ACK
    __u32 nlmsg_seq:	1
    __u32 nlmsg_pid:	0

  struct genlmsghdr:
    __u8 cmd:		CTRL_CMD_GETFAMILY
    __u8 version:	2 /* or 1, doesn't matter */
    __u16 reserved:	0

  struct nlattr:
    __u16 nla_len:	10
    __u16 nla_type:	CTRL_ATTR_FAMILY_NAME
    char data: 		test1\0

  (padding:)
    char data:		\0\0

Note that length fields in Netlink (:c:member:`nlmsghdr.nlmsg_len`
and :c:member:`nlattr.nla_len`) always include the header.
Headers in netlink must be aligned to 4 bytes, hence the extra ``\0\0``
at the end of the message. The attribute lengths exclude padding.

If the family is found kernel will reply with two messages::

  /* Message #1 - reply */
  struct nlmsghdr:
    __u32 nlmsg_len:	136
    __u16 nlmsg_type:	GENL_ID_CTRL
    __u16 nlmsg_flags:	0
    __u32 nlmsg_seq:	1 /* echoed from our request */
    __u32 nlmsg_pid:	5831 /* The PID of our user space process */

  struct genlmsghdr:
    __u8 cmd:		CTRL_CMD_GETFAMILY
    __u8 version:	2
    __u16 reserved:	0

  struct nlattr:
    __u16 nla_len:	10
    __u16 nla_type:	CTRL_ATTR_FAMILY_NAME
    char data: 		test1\0

  (padding:)
    data:		\0\0

  struct nlattr:
    __u16 nla_len:	6
    __u16 nla_type:	CTRL_ATTR_FAMILY_ID
    __u16: 		123 /* The Family ID we are after */

  (padding:)
    char data:		\0\0

  struct nlattr:
    __u16 nla_len:	9
    __u16 nla_type:	CTRL_ATTR_FAMILY_VERSION
    __u16: 		1
  /* ... etc, more attributes will follow. */

  /* Message #2 - the ACK */
  struct nlmsghdr:
    __u32 nlmsg_len:	36
    __u16 nlmsg_type:	NLMSG_ERROR
    __u16 nlmsg_flags:	NLM_F_CAPPED /* There won't be a payload */
    __u32 nlmsg_seq:	1 /* echoed from our request */
    __u32 nlmsg_pid:	5831 /* The PID of our user space process */

  int error:		0

  struct nlmsghdr: /* Copy of the request header as we sent it */
    __u32 nlmsg_len:	32
    __u16 nlmsg_type:	GENL_ID_CTRL
    __u16 nlmsg_flags:	NLM_F_REQUEST | NLM_F_ACK
    __u32 nlmsg_seq:	1
    __u32 nlmsg_pid:	0

The order of attributes (struct nlattr) is not guaranteed.

.. _ext_ack:

Extended ACK
------------

Extended ACK controls reporting of additional error/warning TLVs in
``NLMSG_ERROR`` and ``NLMSG_DONE`` messages. To maintain backward
compatibility this feature has to be explicitly enabled by setting
the ``NETLINK_EXT_ACK`` setsockopt() to ``1``.

Types of extended ack attributes are defined in enum nlmsgerr_attrs.
The two most commonly used attributes are ``NLMSGERR_ATTR_MSG`` and
``NLMSGERR_ATTR_OFFS``.

``NLMSGERR_ATTR_MSG`` carries a message in English describing the
encountered problem. These messages are far more detailed than
what can be expressed thru standard UNIX error codes.

``NLMSGERR_ATTR_OFFS`` points to the attribute which caused the problem.

Extended ACKs can be reported on errors as well as in case of success.
The latter should be treated as a warning.

Extended ACKs greatly improve the usability of Netlink and should
always be enabled, appropriately parsed and reported to the user.

.. _nlmsg_pid:

nlmsg_pid
---------

:c:member:`nlmsghdr.nlmsg_pid` is called PID because the protocol predates
wide spread use of multi-threading and the initial recommendation was
to use process IDs in this field. Process IDs start from 1 hence the use
of ``0`` to mean kernel. The field is still used today in rare cases
when kernel needs to send a unicast notification. User space application
can use bind() to associate its socket with a specific PID (similarly
to binding to a UDP port), it then communicates its PID to the kernel.
The kernel can now reach the user space process.

This sort of communication is utilized in UMH (user mode helper)-like
scenarios when kernel needs to consult user space logic or ask user
space for a policy decision.

Kernel will use the process ID for the field when responding to a request
sent from an unbound socket.

Classic Netlink
===============

TODO
