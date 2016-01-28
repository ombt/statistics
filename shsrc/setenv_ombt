#
# Copyright (C) 2016, OMBT LLC and Mike A. Rumore
# All rights reserved.
# Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
#
if [[ $# -ne 0 ]]
then
	cd ${1}
	ROOT=$(pwd)
	cd -
else
	ROOT=$(pwd)
	echo $ROOT
fi
export ROOT
#
LD_LIBRARY_PATH="${ROOT}/release/2.0/lib:${LD_LIBRARY_PATH}"
export LD_LIBRARY_PATH
#
PATH="${ROOT}/release/2.0/bin:${PATH}"
export PATH
