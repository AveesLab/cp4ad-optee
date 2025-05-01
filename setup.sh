cd ~
wget https://developer.nvidia.com/downloads/embedded/l4t/r35_release_v6.0/release/jetson_linux_r35.6.0_aarch64.tbz2
sudo tar -xvf jetson_linux_r35.6.0_aarch64.tbz2
cd ~/Linux_for_Tegra/
./source_sync.sh -t jetson_35.6

echo "export CROSS_COMPILE_AARCH64_PATH=/usr" >> ~/.bashrc
echo "export CROSS_COMPILE_AARCH64=\$CROSS_COMPILE_AARCH64_PATH/bin/" >> ~/.bashrc
echo "export UEFI_STMM_PATH=~/Linux_for_Tegra/bootloader/standalonemm_optee_t234.bin" >> ~/.bashrc
source ~/.bashrc

cd ~/Linux_for_Tegra/sources/tegra/optee-src/atf/arm-trusted-firmware
make BUILD_BASE=./build \
CROSS_COMPILE="${CROSS_COMPILE_AARCH64}" \
DEBUG=0 LOG_LEVEL=20 PLAT=tegra SPD=opteed TARGET_SOC=t234 V=0

pip install --upgrade cryptography
cd ~/Linux_for_Tegra/sources/tegra/optee-src/nv-optee
sudo apt install python3-pyelftools
./optee_src_build.sh -p t234

cd ~/Linux_for_Tegra/sources/tegra/optee-src/nv-optee
sudo apt install device-tree-compiler
dtc -I dts -O dtb -o ./optee/tegra234-optee.dtb ./optee/tegra234-optee.dts

cd ~/Linux_for_Tegra/sources/tegra/optee-src
cp ~/Linux_for_Tegra/nv_tegra/tos-scripts/gen_tos_part_img.py .

./gen_tos_part_img.py \
--monitor ./atf/arm-trusted-firmware/build/tegra/t234/release/bl31.bin \
--os ./nv-optee/optee/build/t234/core/tee-raw.bin \
--dtb ./nv-optee/optee/tegra234-optee.dtb \
--tostype optee \
./tos.img

SCRIPT_DIR="$(dirname $(readlink -f "${0}"))"
cd "/${SCRIPT_DIR="$(dirname $(readlink -f "${0}"))"}"
source optee_src_build.sh -p t234