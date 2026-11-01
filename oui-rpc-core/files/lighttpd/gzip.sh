#!/bin/sh
#
# Copyright (C) 2026 Jianhui Zhao <zhaojh329@gmail.com>
#
# This is free software, licensed under the MIT.
#

root="${1:-/www}"
root="${root%/}"

find "$root" -type f -name '*.gz' | LC_ALL=C sort | awk -v root="$root" '
function quote(value) {
    gsub(/"/, "\\\"", value)
    return "\"" value "\""
}

{
    compressed = substr($0, length(root) + 1)
    original = substr(compressed, 1, length(compressed) - 3)

    if (original ~ /^\/oui-/)
        next

    print "$HTTP[\"url\"] == " quote(original) " {"
    print "    setenv.set-response-header = (\"Vary\" => \"Accept-Encoding\")"
    print "    $REQUEST_HEADER[\"Accept-Encoding\"] =~ \"(?i)(^|,)\\s*gzip\\s*(;|,|$)\" {"
    print "        $REQUEST_HEADER[\"Accept-Encoding\"] !~ \"(?i)(^|,)\\s*gzip\\s*;\\s*q=0(\\.0*)?\\s*(,|$)\" {"
    print "            url.rewrite-once = (\"^\" => " quote(compressed "${qsa}") ")"
    print "        }"
    print "    }"
    print "}"
    print "$HTTP[\"url\"] == " quote(compressed) " {"
    print "    setenv.set-response-header = (\"Content-Encoding\" => \"gzip\", \"Vary\" => \"Accept-Encoding\")"

    if (original ~ /\.css$/)
        print "    mimetype.assign = (\"\" => \"text/css\")"
    else if (original ~ /\.js$/)
        print "    mimetype.assign = (\"\" => \"application/javascript\")"

    print "}"
}'
