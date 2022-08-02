


What we want out of it:

 - auto-generate the ops
 - auto-generate the policies
 - auto-generate the help tables (docs)

 - auto-generate uAPI header?

 - auto-generate the kernel parser
 - auto-generate the user space binding

 - auto-generate user space attr breakdown on error
   (use the same table for parse?)

Assumptions we make to retain sanity:
 - types are either root or nested, but not both
 - notifications usually match one of the gets
but:
 - spaces do not correspond to types, requests are usually quite a cut
   down version of the space
