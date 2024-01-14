#!/bin/bash
#
# Samsung SSD Firmware decompressor
# By Zibri / RamJam in 2024
#
# This version uses openssl.
#

if ! which &>/dev/null openssl
 	then
 	  echo "Please install openssl."
fi

if ! which &>/dev/null 7z
 	then
 	  echo "Please install 7zip."
fi

if ! which &>/dev/null xxd
 	then
 	  echo "Please install xxd."
fi

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
rm &>/dev/null -rf tmp magic key DSRD
echo "Decompressing iso..."
7z &>/dev/null x -y -otmp $1
echo "Decompressing initrd..."
7z &>/dev/null x -y -otmp/ tmp/initrd
echo "Extracting fumagician..."
7z &>/dev/null x -y -otmp/ tmp/initrd~ root/fumagician
echo "Dumping Key..."
key="$(strings tmp/root/fumagician/fumagician|grep -A 2 printk|tail -1|base64 -d|xxd -p -c 100)"
echo "KEY: $key"
echo "Decrypting..."
for a in tmp/root/fumagician/*.enc
do
openssl enc -aes-256-ecb -d -in $a -out $(basename ${a:0:-4}) -nopad -K $key
done
rm -rf tmp
echo "Done."
