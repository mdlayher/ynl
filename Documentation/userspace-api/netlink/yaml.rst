.. SPDX-License-Identifier: BSD-3-Clause

=========================================
Netlink protocol specifications (in YAML)
=========================================

Netlink protocol specifications are complete, machine readable descriptions of
Netlink protocols written in YAML. The goal of the specifications is to allow
separating Netlink parsing from user space logic and minimize the amount of
hand written Netlink code for each new family. Netlink specs should be complete
and not depend on any other spec or C header file, making it easy to use
in languages which can't include kernel headers directly.

Kernel user the YAML specs to generate:

 - the C uAPI header
 - documentation of the protocol as a ReST file
 - policy tables for input attribute validation
 - operation tables
 
There are four schema levels for Netlink specs, each next level has all
the attributes from the previous one, with some extra attributes allowing
it to describe older Netlink families. The levels are:

 - genetlink - most streamlined, should be used by all new families
 - genetlink-c - superset of genetlink with extra attributes allowing
   customization of define and enum type and value names; this schema should
   be equivalent to ``genetlink`` for all implementations which don't interact
   directly with kernel C uAPI headers
 - genetlink-legacy - Generic Netlink catch all schema supporting quirks of
   all old genetlink families
 - raw-netlink - catch all schema supporting pre-generic netlink protocols
   such as ``NETLINK_ROUTE``

The definition of the schemas (in ``jsonschema``) which can be found
in ``Documentation/netlink/``.

Schema structure
================

YAML schema has the following conceptual sections:

 - globals
 - definitions
 - attributes
 - operations

Most properties in the schema accept (or in fact require) a ``description``
sub-property documenting the defined object.

genetlink
=========

Globals
-------

Attributes listed directly at the root level of the YAML file.

name
~~~~

Name of the family.

version
~~~~~~~

Generic Netlink family version, default is 1.

protocol
~~~~~~~~

The schema level, default is ``genetlink``, which is the only value
allowed for new ``genetlink`` families.

definitions
-----------

Array of type and constant definitions.

name
~~~~

Name of the type / constant.

type
~~~~

One of the following types:

 - const - a single, standalone constant
 - enum - defines an integer enumeration, with values for each entry
          incrementing by 1, (e.g. 0, 1, 2, 3)
 - flags - defines an integer enumeration, with values for each entry
          occupying a bit, starting from bit 0, (e.g. 1, 2, 4, 8)

value
~~~~~

The value for the ``const``.
	  
value-start
~~~~~~~~~~~

The first value for ``enum`` and ``flags``, allows overriding the default
start value of ``0`` (for ``enum``) and bit 0 (for ``flags``).

Sparse enumerations are not supported.

entries
~~~~~~~

Names of the entries.

header
~~~~~~

For C-compatible languages, header which already defines this value.
In case the definition is shared by multiple families (e.g. ``IFNAMSIZ``)
code generators for C-compabile languages may prefer to add an appropriate
include instead of rendering a new definition.


Attribute Spaces
----------------

First of the main two sections is ``attribute-sets``. This property contains
information about netlink attributes of the family. All families have at least
one attribute space, most have multiple. ``attribute-sets`` is an array/list,
with each entry describing a single space. The ``name`` of the space is not used
in uAPI/C codegen, it's internal to the spec itself, used by operations and nested
attributes to refer to a space.

Each attribute space has properties used to render uAPI header enums. ``name-prefix``
is prepended to the name of each attribute, allowing the attribute names to be shorter
compared to the enum names in the uAPI.
Optionally attribute space may contain ``enum-name`` if the uAPI header's enum should
have a name. Most netlink uAPIs do not name attribute enums, the attribute names are
heavily prefixed, which is sufficient.

Most importantly each attribute space contains a list of attributes under the ``attributes``
property. The properties of an attribute should look fairly familiar to anyone who ever
wrote netlink code (``name``, ``type``, optional validation constraints like ``len`` and
reference to the internal space for nests).

Note that the YAML spec is "flattened" and is not meant to visually resemble
the format of the netlink messages (unlike certain ad-hoc documentation
formats seen in kernel comments). In the YAML spec subordinate attribute sets
are not defined inline as a nest, but defined in a separate attribute set
referred to with a ``nested-attributes`` property of the container.

YAML spec may also contain fractional spaces - spaces which contain a ``subset-of``
property. Such spaces describe a section of a full space, allowing narrowing down which
attributes are allowed in a nest or refining the validation criteria. Fractional spaces
can only be used in nests. They are not rendered to the uAPI in any fashion.

Operations and notifications
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This section describes messages passed between the kernel and the user space.
There are three types of entries in this section - operations, notifications
and events.

Notifications and events both refer to the asynchronous messages sent by the kernel
to members of a multicast group. The difference between the two is that a notification
shares its contents with a GET operation (the name of the GET operation is specified
in the ``notify`` property). This arrangement is commonly used for notifications about
objects where the notification carries the full object definition.

Events are more focused and carry only a subset of information rather than full
object state (a made up example would be a link state change event with just
the interface name and the new link state).
Events are considered less idiomatic for netlink and notifications
should be preferred. After all, if the information in an event is sufficiently
complete to be useful, it should also be useful enough to have a corresponding
GET command.

Operations describe the most common request - response communication. User
sends a request and kernel replies. Each operation may contain any combination
of the two modes familiar to netlink users - ``do`` and ``dump``.
``do`` and ``dump`` in turn contain a combination of ``request`` and ``response``
properties. If no explicit message with attributes is passed in a given
direction (e.g. a ``dump`` which does not accept filter, or a ``do``
of a SET operation to which the kernel responds with just the netlink error code)
``request`` or ``response`` section can be skipped. ``request`` and ``response``
sections list the attributes allowed in a message. The list contains only
the names of attributes from a space referred to by the ``attribute-set``
property.

An astute reader will notice that there are two ways of defining sub-spaces.
A full fractional space with a ``subset-of`` property and a de facto subspace
created by list attributes for an operation. This is only for convenience.
The abilities to refine the selection of attributes and change their definition
afforded by the fractional space result in much more verbose YAML, and the full
definition of a space (i.e. containing all attributes) is always required to render
the uAPI header, anyway. So the per-operation attribute selection is a form of
a shorthand.

Multicast groups
~~~~~~~~~~~~~~~~

This section lists the multicast groups of the family, not much to be said.
