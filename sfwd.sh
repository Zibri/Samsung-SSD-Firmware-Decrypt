#!/bin/bash
#
# Samsung SSD Firmware decompressor
# By Zibri / RamJam in 2024
#
if [ $# -eq 0 ]
  then
    echo "Usage $0 <Samsung FW iso file>"
    echo "Download it from: https://semiconductor.samsung.com/consumer-storage/support/tools/"
    exit 1
fi
if ! [ -f $1 ]; then
    echo "File does not exist."
    exit 1
fi
echo "Decompressing iso..."
7z &>/dev/null x -otmp $1
echo "Decompressing initrd..."
7z &>/dev/null x -otmp/ tmp/initrd
echo "Extracting fumagician..."
7z &>/dev/null x -otmp/ tmp/initrd~ root/fumagician
echo "Dumping Key..."
strings tmp/root/fumagician/fumagician|grep -A 2 printk|tail -1|base64 -d >key
echo "Decrypting..."
for a in tmp/root/fumagician/*.enc
do
./sdec key $a $(basename ${a:0:-4}) magic
done
rm -rf tmp
echo "Done."
