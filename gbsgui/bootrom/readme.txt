This folder corresponds to the /opt/STM/STAPI_SDK43/stapp/platform/b2128/h205/bootrom.

A. Build bootloader:
   1. Backup the original files ("bootrom.S" and "makefile") in /opt/STM/STAPI_SDK43/stapp/platform/b2128/h205/bootrom
   2. Copy the file bootrom.S to /opt/STM/STAPI_SDK43/stapp/platform/b2128/h205/bootrom
   3. Copy the file "makefile_BOOTLOADER" to /opt/STM/STAPI_SDK43/stapp/platform/b2128/h205/bootrom, rename as "makefile"
   4. Run commands showed at step C
   5. The file flash_b2128_h253_ST40_OS21_32BITS.bin in /opt/STM/STAPI_SDK43/stapp is the bootloader file.

B. Build application:
   1. Backup the original file "makefile" in /opt/STM/STAPI_SDK43/stapp/platform/b2128/h205/bootrom
   2. Copy the file "makefile_MAINAPP" to /opt/STM/STAPI_SDK43/stapp/platform/b2128/h205/bootrom, rename as "makefile"
   3. Run commands showed at at step C
   4. The file flash_b2128_h253_ST40_OS21_32BITS.bin in /opt/STM/STAPI_SDK43/stapp is the application file.

C. Commands:
   root@ubuntu:/opt/STM/STAPI_SDK43/stapp# export RUN_FROM_FLASH=1
   root@ubuntu:/opt/STM/STAPI_SDK43/stapp# make rom ZIP=1
