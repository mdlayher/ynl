#!/bin/bash
# SPDX-License-Identifier: BSD-3-Clause

TOOL=$(dirname $(realpath $0))/ynl-gen-c.py

force=

while [ ! -z "$1" ]; do
  case "$1" in
    -f ) force=yes; shift ;;
    * )  echo "Unrecognized option '$1'"; exit 1 ;;
  esac
done

KDIR=$(dirname $(dirname $(dirname $(dirname $(realpath $0)))))

files=$(git grep --files-with-matches '^// YNL-GEN \(kernel\|uapi\)')
for f in $files; do
    # params:  0     1  2       3      4     5
    #         // $YAML // YNL-GEN kernel $mode
    params=( $(git grep -B1 -h '// YNL-GEN' $f) )

    if [ $f -nt ${params[1]} -a -z "$force" ]; then
	echo -e "\tSKIP $f"
	continue
    fi

    echo -e "\tGEN ${params[4]}\t$f"
    $TOOL --mode ${params[4]} --${params[5]} --spec $KDIR/${params[1]} > $f
done
