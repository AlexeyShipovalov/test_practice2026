# Conan
## Этот документ на английском языке / This document in English
- [README.md](README.md)
## Установка
Есть два варианта установки: через Aurora Platform SDK и основной Aurora SDK.
### Aurora Platfrom SDK
Этапы установки:
1. Скачать Platfrom SDK по ссылке: https://developer.auroraos.ru/downloads/psdk
2. Установить Platfrom SDK по инструкции: https://developer.auroraos.ru/doc/sdk/psdk/setup
3. Запустить виртуальное окружение через утилиту **sdk-chroot**: ``` {path_to_platformSDK}/sdks/aurora_psdk/sdk-chroot ```
4. Выбрать нужный таргет (rootfs-контейнер, который будет использован для генерации пакетов целевого устройства), например, AuroraOS-5.1.2.10-base-aarch64, AuroraOS-5.1.2.10-base-armv7hl или AuroraOS-5.1.2.10-base-x86_64
5. Узнать список уже установленных в окружении таргетов можно командой:
``` sdk-assistant list ```
6. Зайти на нужный таргет: ``` sb2 -t {target_name} -m sdk-install -R ```, например, ``` sb2 -t AuroraOS-5.1.2.10-base-armv7hl -m sdk-install -R ```
7. После захода обновить репозитории и пакеты: ``` zypper ref && zypper dup ```
8. Вернуться в Platform SDK: ``` exit ```
9. Перейти в директорию с архивом ConanClientRpm и распаковать его: ``` tar -xvf ConanClientRpm-main.tar.gz ```
10. Зайти в распакованный архив и запустить установочный скрипт командой: ``` ./install-conan ```
11. Дождаться окончания установки (может занять продолжительное время)


### Aurora SDK
Этапы установки:
_Вариант 1:_
1. Скачать SDK по ссылке: https://developer.auroraos.ru/downloads/sdk/
2. Установить SDK по инструкции: https://developer.auroraos.ru/doc/sdk/app_development/setup
3. Запустить виртуальную машину/Docker-контейнер с Build Engine вручную или через SDK
4. Скопировать архив ConanClientRpm в Build Engine:
``` scp -P 2222 -i {path_to_auroraOS}/vmshare/ssh/private_keys/sdk ConanClientRpm-main.tar.gz mersdk@localhost:~/ ```
5. Зайти в Build Engine после запуска (через SDK или Virtualbox): 
``` ssh -p 2222 -i {path_to_auroraOS}/vmshare/ssh/private_keys/sdk mersdk@localhost ```
Подробнее: https://developer.auroraos.ru/doc/sdk/app_development/build/build_engine
6. Перейти в директорию с архивом ConanClientRpm и распаковать его: ``` tar -xvf ConanClientRpm-main.tar.gz ```
7. Выбрать нужный таргет (rootfs-контейнер, который будет использован для генерации пакетов целевого устройства), например, AuroraOS-5.1.2.10-base-aarch64, AuroraOS-5.1.2.10-base-armv7hl или AuroraOS-5.1.2.10-base-x86_64
8. Узнать список уже установленных в окружении таргетов можно командой:``` sdk-assistant list ```
9. Зайти на нужный таргет: ``` sb2 -t {target_name} -m sdk-install -R ```, например, ``` sb2 -t AuroraOS-5.1.2.10-base-armv7hl -m sdk-install -R ```
10. После захода обновить репозитории и пакеты: ``` zypper ref && zypper dup ```
11. Вернуться в Build Engine: ``` exit ```
12. Зайти в распакованный архив и запустить установочный скрипт командой: ``` ./install-conan ```
13. Дождаться окончания установки (может занять продолжительное время)


_Вариант 2 (установка через sfdk):_
1. Скачать SDK по ссылке: https://developer.auroraos.ru/downloads/sdk/
2. Установить SDK по инструкции: https://developer.auroraos.ru/doc/sdk/app_development/setup
3. Запустить виртуальную машину/Docker-контейнер с Build Engine вручную или через SDK
4. Скопировать архив ConanClientRpm в Build Engine:
``` scp -P 2222 -i {path_to_auroraOS}/vmshare/ssh/private_keys/sdk ConanClientRpm-main.tar.gz mersdk@localhost:~/ ```
5. Перейти в рабочую область Aurora SDK (директория с проектами)
6. Активировать Build Engine через sfdk:
``` {path_to_auroraOS}/bin/sfdk engine exec ```
7. Перейти в директорию с архивом ConanClientRpm и распаковать его: ``` tar -xvf ConanClientRpm-main.tar.gz ```
8. Перейти в папку со скриптом установки ConanClientRpm-main и запустить его командой: ``` ./install-conan ```
9. Дождаться окончания установки (может занять продолжительное время)


## Удаление
1. Зайти в Build Engine после запуска (через SDK или Virtualbox): 
``` ssh -p 2222 -i {path_to_auroraOS}/vmshare/ssh/private_keys/sdk mersdk@localhost ```
2. Выбрать таргет, для которого нужно удалить Conan (узнать список уже установленных в окружении таргетов можно командой:``` sdk-assistant list ```)
3. Зайти на нужный таргет: ``` sb2 -t {target_name} -m sdk-install -R ```
4. Проверить наличие Conan: ``` zypper pa | grep conan ```
5. Удалить Conan: ``` zypper remove conan ```