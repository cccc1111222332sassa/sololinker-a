#!/bin/bash
find . -type f |xargs sed -i "s/mmcblk0/mmcblk1/g"
pushd tools/linux/toolchain/arm-rockchip830-linux-uclibcgnueabihf/
source env_install_toolchain.sh
popd
./build.sh lunch <<EOF
1
EOF
./build.sh
./build.sh firmware
