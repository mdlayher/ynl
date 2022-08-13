.. SPDX-License-Identifier: BSD-3-Clause

Netlink protocol specifications
===============================

Netlink protocol specifications are complete, machine readable descriptions of
genetlink protocols written in YAML. The schema (in ``jsonschema``) can be found
in the same directory as this documentation file.

Schema structure
----------------

YAML schema has the following conceptual sections. Most properties in the schema
accept (or in fact require) a ``description`` sub-property documenting the defined
object.

Globals
~~~~~~~

There is a handful of global attributes such as the family name, version of
the protocol, and additional C headers (used only for uAPI and C-compatible
codegen).

Global level also contains a handful of customization properties, like
``attr-cnt-suffix`` which allow accommodating quirks of existing families.
Those properties should not be used in new families.

Attribute Spaces
~~~~~~~~~~~~~~~~

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

Note that attribute spaces do not themselves nest, nested attributes refer to their internal
space via a ``nested-attributes`` property, so the YAML spec does not resemble the format
of the netlink messages directly.

YAML spec may also contain fractional spaces - spaces which contain a ``subspace-of``
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
A full fractional space with a ``subspace-of`` property and a de facto subspace
created by list attributes for an operation. This is only for convenience.
The abilities to refine the selection of attributes and change their definition
afforded by the fractional space result in much more verbose YAML, and the full
definition of a space (i.e. containing all attributes) is always required to render
the uAPI header, anyway. So the per-operation attribute selection is a form of
a shorthand.

Multicast groups
~~~~~~~~~~~~~~~~

This section lists the multicast groups of the family, not much to be said.
