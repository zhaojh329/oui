#!/bin/sh

url="$1"
method="$2"

object=
submethod=
subparams=

usage() {
    echo "Usage: sid=\"b750e4673dd38cb9545d1b26e960ba28\" $0 url method params"
    echo "  http://192.168.1.1  login '{\"username\": \"admin\", \"password\": \"123\"}'"
    echo "  http://192.168.1.1  call oui get_lang"
    echo "  http://192.168.1.1  call file md5sum '{\"path\": \"/tmp/test.bin\"}'"
    exit 1
}
[ $# -lt 3 ] && usage

case $method in
    login)
        params="$3"
        ;;
    logout)
        params="$3"
        ;;
    call)
        [ $# -lt 4 ] && usage
        object=$3
        submethod=$4
        subparams="$5"
        [ -z "$subparams" ] && subparams='{}'
        ;;
    *)
        usage
        ;;
esac

which curl > /dev/null || {
    echo "Please install curl"
    exit 1
}

[ -z "$object" ] || {
    params="[\"$sid\", \"$object\", \"$submethod\", $subparams]"
}

curl "$url/rpc" -d "{\"jsonrpc\": \"2.0\", \"id\": 0, \"method\": \"$method\", \"params\": $params}"

echo