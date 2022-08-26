.. SPDX-License-Identifier: BSD-3-Clause

==============================
Netlink YAML C code generation
==============================

This document describes how YAML specifications are used to render
C code (uAPI, policies etc.). It also defines the additional properties
allowed in older families by the ``genetlink-c`` protocol level,
to control the naming.

For brevity this document refers to ``name`` properties of various
objects by the object type. For example ``$attr`` is the value
of ``name`` in an attribute, and ``$family`` is the name of the
family (the global ``name`` attribute).

The upper case is used to denote literal values, e.g. ``$family-CMD``
means the concatenation of ``$family``, dash character and the literal
``CMD``.

The names of ``#defines`` and enum values are always in upper case,
and with dashes (``-``) replaced by underscores (``_``).

If the constructured name is a C keyword, an extra underscore is
appended (``do`` -> ``do_``).

Globals
=======

``c-family-name`` controls the name of the ``#define`` for the family
name, default is ``$family-FAMILY-NAME``.

``c-version-name`` controls the name of the ``#define`` for the version
of the family, default is ``$family-FAMILY-VERSION``.

``max-by-define`` selects if max values for enums are defined as a
``#define`` rathar than inside the enum.

Definitions
===========

Constants
---------

Every constant is rendered as a ``#define``.
The name of the constant is ``$family-$constant`` and the value
is rendered as a string or integer according to its type in YAML.

Enums and flags
---------------

Enums are named ``$family-$enum``. The full name can be set directly
or suppressed by specifying the ``enum-name`` property.
Default entry name is ``$family-$enum-$entry``.
If ``name-prefix`` is specified it replaces the ``$family-$enum``
portion of the entry name.

Attributes
==========

Each attribute set (excluding fractional sets) is rendered as an enum.

Attribute enums are traditionally unnamed in netlink headers.
If naming is desired ``enum-name`` can be used to specify the name.

The default attribute name prefix is ``$familyA`` if the name of the set
is the same as the name of the family and ``$familyA-$set`` if the names
differ. The prefix can be overriden by the ``name-prefix`` property of a set.
The rest of the section will refer to the prefix as ``$pfx``.

Attributes are named ``$pfx-$attribute``.

Attribute enums end with two special values ``__$pfx-MAX`` and ``$pfx-MAX``
which are used for sizing attribute tables.
These two names can be specified directly with the ``attr-cnt-name``
and ``attr-max-name`` properties respectively.

If ``max-by-define`` is set to ``true`` at the global level ``attr-max-name``
will be specified as a ``#define`` rather than an enum value.

Operations
==========

Operations are named ``$family-CMD-$operation``.
If ``name-prefix`` is specified it replaces the ``$family-CMD``
portion of the name.

Similarly to attribute enums operation enums end with special count and max
attributes. For operations those attributes can be renamed with
``cmd-cnt-name`` and ``cmd-max-name``. Max will be a define if ``max-by-define``
is ``true``.

Code generation
===============

uAPI header is assumed to come from ``<linux/$family.h>`` in the default header
search path. It can be changed using the ``uapi-header`` global property.
