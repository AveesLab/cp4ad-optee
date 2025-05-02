sudo apt install pip python3-pyelftools device-tree-compiler -y

cd /home/avees
# wget https://developer.nvidia.com/downloads/embedded/l4t/r35_release_v6.0/release/jetson_linux_r35.6.0_aarch64.tbz2
sudo tar -xvf jetson_linux_r35.6.0_aarch64.tbz2
/home/avees/Linux_for_Tegra/source_sync.sh -t jetson_35.6

echo "export CROSS_COMPILE_AARCH64_PATH=/usr" >> /home/avees/.bashrc
echo "export CROSS_COMPILE_AARCH64=${CROSS_COMPILE_AARCH64_PATH}/bin/" >> /home/avees/.bashrc
echo "export UEFI_STMM_PATH=/home/avees/Linux_for_Tegra/bootloader/standalonemm_optee_t234.bin" >> /home/avees/.bashrc

export CROSS_COMPILE_AARCH64_PATH=/usr
export CROSS_COMPILE_AARCH64=${CROSS_COMPILE_AARCH64_PATH}/bin/
export UEFI_STMM_PATH=/home/avees/Linux_for_Tegra/bootloader/standalonemm_optee_t234.bin

echo "!!!!!!!!!!!  build atfy  !!!!!!!!!!!!"
cd /home/avees/Linux_for_Tegra/sources/tegra/optee-src/atf/arm-trusted-firmware
make BUILD_BASE=./build \
CROSS_COMPILE="${CROSS_COMPILE_AARCH64}" \
DEBUG=0 LOG_LEVEL=20 PLAT=tegra SPD=opteed TARGET_SOC=t234 V=0

echo "!!!!!!!!!!!  build optee  !!!!!!!!!!!!"
pip install --upgrade cryptography -y
cd /home/avees/Linux_for_Tegra/sources/tegra/optee-src/nv-optee
/home/avees/Linux_for_Tegra/sources/tegra/optee-src/nv-optee//optee_src_build.sh -p t234

echo "!!!!!!!!!!!  dtc  !!!!!!!!!!!!"
cd /home/avees/Linux_for_Tegra/sources/tegra/optee-src/nv-optee
dtc -I dts -O dtb -o ./optee/tegra234-optee.dtb ./optee/tegra234-optee.dts

cd /home/avees/Linux_for_Tegra/sources/tegra/optee-src
cp /home/avees/Linux_for_Tegra/nv_tegra/tos-scripts/gen_tos_part_img.py .

echo "!!!!!!!!!!!  build tos  !!!!!!!!!!!!"
./gen_tos_part_img.py \
--monitor ./atf/arm-trusted-firmware/build/tegra/t234/release/bl31.bin \
--os ./nv-optee/optee/build/t234/core/tee-raw.bin \
--dtb ./nv-optee/optee/tegra234-optee.dtb \
--tostype optee \
./tos.img

/home/avees/Linux_for_Tegra/sources/tegra/optee-src/nv-optee/optee_src_build.sh -p t234
