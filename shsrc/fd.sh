#!/usr/bin/bash
dir=${1:-"."}
exec /usr/bin/find ${dir} -type d -print
