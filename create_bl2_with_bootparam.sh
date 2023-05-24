#!/bin/bash
echo -e "\n[Creating bootparams.bin]"
SIZE=$(stat -L --printf="%s" bl2.bin)
SIZE_ALIGNED=$(expr $SIZE + 3)
SIZE_ALIGNED2=$((SIZE_ALIGNED & 0xFFFFFFFC))
SIZE_HEX=$(printf '%08x\n' $SIZE_ALIGNED2)
echo "  bl2.bin size=$SIZE, Aligned size=$SIZE_ALIGNED2 (0x${SIZE_HEX})"
STRING=$(echo \\x${SIZE_HEX:6:2}\\x${SIZE_HEX:4:2}\\x${SIZE_HEX:2:2}\\x${SIZE_HEX:0:2})
printf "$STRING" > bootparams.bin
for i in `seq 1 506`e ; do printf '\xff' >> bootparams.bin ; done
printf '\x55\xaa' >> bootparams.bin
# Combine bootparams.bin and bl2.bin into single binary
# Only if a new version of bl2.bin is created
if [ "bl2.bin" -nt "bl2_bp.bin" ] || ! [ -e "bl2_bp.bin" ] ; then
	echo -e "\n[Adding bootparams.bin to bl2.bin]"
	cat bootparams.bin bl2.bin > bl2_bp.bin
fi
