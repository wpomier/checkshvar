# checkShellVars
## Compile:

    $ gcc -o checkshellvar checkshellvar.c

## Use:

    $ checkshellvar < script.sh > scriptout.sh

## Example:

    $ cat script.sh
    echo "Removing $a ${b} $(c $(c)) `d $d` ${10} $@ now"
    rm -rf $a ${b} $(c $(c)) `d $d` ${10} $@

    $ checkshellvar < script.sh
    echo "Removing $a ${b} $(c $(c)) `d $d` ${10} $@ now"
    rm -rf "$a" "${b}" "$(c "$(c)")" "$(d "$d")" "${10}" "$@"

