echo "export CROSS_COMPILE_AARCH64_PATH=/usr" >> ~/.bashrc
echo "export CROSS_COMPILE_AARCH64=\$CROSS_COMPILE_AARCH64_PATH/bin/" >> ~/.bashrc
echo "export UEFI_STMM_PATH=~/Linux_for_Tegra/bootloader/standalonemm_optee_t234.bin" >> ~/.bashrc
source ~/.bashrc