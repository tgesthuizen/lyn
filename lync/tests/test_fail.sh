#!/bin/sh

if $1 $2
then
    echo "lync succeeded for fail test $2"
    exit 1
fi

exit 0
