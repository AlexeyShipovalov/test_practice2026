
# Conan
## This document in Russian / Перевод этого документа на русский язык
- [README.ru.md](README.ru.md)
Этот документ на русском
## Installation
There are two installation variants: via Aurora Platform SDK and the main Aurora SDK.
### Aurora Platfrom SDK
Installation steps:
1. Download Platform SDK from the link: https://developer.auroraos.ru/downloads/psdk
2. Install Platform SDK according to the instructions: https://developer.auroraos.ru/doc/sdk/psdk/setup
3. Launch the virtual environment through the utility **sdk-chroot**: ``` {path_to_platformSDK}/sdks/aurora_psdk/sdk-chroot ```
4. Choose the needed target (rootfs-container which will be used for target device packages generation), for exapmle, AuroraOS-5.1.2.10-base-aarch64, AuroraOS-5.1.2.10-base-armv7hl или AuroraOS-5.1.2.10-base-x86_64
5. To find out the list of targets already installed in the environment, use the command: ``` sdk-assistant list ```
6. Enter into the needed target: ``` sb2 -t {target_name} -m sdk-install -R ```, for example, ``` sb2 -t AuroraOS-5.1.2.10-base-armv7hl -m sdk-install -R ```
7. After the enter update the repositories and packages: ``` zypper ref && zypper dup ```
8. Return to the Platform SDK: ``` exit ```
9. Go to the directory with the ConanClientRpm archive and unpack it:``` tar -xvf ConanClientRpm-main.tar.gz ```
10. Enter the unpacked archive and launch the installation script using command: ``` ./install-conan ```
11. Wait the installation to finish (it may take a long time)

### Aurora SDK
Installation steps:
_Variant 1:_
1. Download Platform SDK from the link: https://developer.auroraos.ru/downloads/psdk
2. Install Platform SDK according to the instructions: https://developer.auroraos.ru/doc/sdk/psdk/setup
3. Launch the virtual machine/Docker-container with the Build Engine manually or via the SDK
4. Copy the ConanClientRpm archive to the Build Engine: ``` scp -P 2222 -i {path_to_auroraOS}/vmshare/ssh/private_keys/sdk ConanClientRpm-main.tar.gz mersdk@localhost:~/ ```
5. Enter into the Build Engine after launch (via SDK or Virtualbox): ``` ssh -p 2222 -i {path_to_auroraOS}/vmshare/ssh/private_keys/sdk mersdk@localhost ```
More detailed: https://developer.auroraos.ru/doc/sdk/app_development/build/build_engine
6. Go to the directory with the ConanClientRpm archive and unpack it:``` tar -xvf ConanClientRpm-main.tar.gz ```
7. Choose the needed target (rootfs-container which will be used for target device packages generation), for exapmle, AuroraOS-5.1.2.10-base-aarch64, AuroraOS-5.1.2.10-base-armv7hl или AuroraOS-5.1.2.10-base-x86_64
8. To find out the list of targets already installed in the environment, use the command: ``` sdk-assistant list ```
9. Enter into the needed target: ``` sb2 -t {target_name} -m sdk-install -R ```, for example, ``` sb2 -t AuroraOS-5.1.2.10-base-armv7hl -m sdk-install -R ```
10. After the enter update the repositories and packages: ``` zypper ref && zypper dup ```
11. Return to the Build Engine: ``` exit ```
12. Enter the unpacked archive and launch the installation script using command: ``` ./install-conan ```
13. Wait the installation to finish (it may take a long time)

_Variant 2 (installation via sfdk):_
1. Download Platform SDK from the link: https://developer.auroraos.ru/downloads/psdk
2. Install Platform SDK according to the instructions: https://developer.auroraos.ru/doc/sdk/psdk/setup
3. Launch the virtual machine/Docker-container with the Build Engine manually or via the SDK
4. Copy the ConanClientRpm archive to the Build Engine: ``` scp -P 2222 -i {path_to_auroraOS}/vmshare/ssh/private_keys/sdk ConanClientRpm-main.tar.gz mersdk@localhost:~/ ```
5. Go to the Aurora SDK workspace (project directory)
6. Activate the Build Engine via sfdk: ``` {path_to_auroraOS}/bin/sfdk engine exec ```
7. Go to the directory with the ConanClientRpm archive and unpack it:``` tar -xvf ConanClientRpm-main.tar.gz ```
8. Go to the folder with the ConanClientRpm-main installation script and run it with the command: ``` ./install-conan ```
9. Wait the installation to finish (it may take a long time)

## Removal
1. Enter into the Build Engine after launch (via SDK or Virtualbox): ``` ssh -p 2222 -i {path_to_auroraOS}/vmshare/ssh/private_keys/sdk mersdk@localhost ```
2. Select the target for which you want to delete Conan (you can find out the list of targets already installed in the environment using the command: ``` sdk-assistant list ```)
3. Enter into the needed target: ``` sb2 -t {target_name} -m sdk-install -R ```, for example, ``` sb2 -t AuroraOS-5.1.2.10-base-armv7hl -m sdk-install -R ```
4. Check the availability of Conan: ``` zypper pa | grep conan ```
5. Remove Conan: ``` zypper remove conan ```