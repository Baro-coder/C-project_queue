#!/bin/bash

dir_lib="./lib/*.c"
dir_var="./var/"

status=0

COMPILE()
{
    file_out=${dir_var}${file##*/}
    file_out="${file_out%.c}.out"

    echo "  Compiling: $1 ..."
    echo

    gcc -o "$file_out" "$file"

    if [ $? -eq 0 ]; then
        echo "      Compiled successfully."
        echo
    else
        status=1
    fi

    echo "  --------------------------------"
    echo
}


for file in $dir_lib; do
    COMPILE $file
done

exit $status