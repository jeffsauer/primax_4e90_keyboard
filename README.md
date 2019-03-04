Along with the 2018 HP Envy 34" curved all in one computer comes the HP Wireless Keyboard and Mouse 800 Combo made by PRIMAX:

https://store.hp.com/us/en/pdp/hp-pavilion-wireless-keyboard-and-mouse-800

It has a vendor ID of 0x0461 and device ID 0x4e90 (list via lsusb). With existing kernel driver (as of 4.20.13), the mouse would work, however, the keyboard would only respond with certain keys... the super key, and keys used to control brightness, volume, etc.  All letter, number, keypad, and other normal keys did not respond.

After some digging, found this possible fix:

https://unix.stackexchange.com/questions/377830/linux-hid-driver-for-primax-wireless-keyboards/402288#402288

After applying the patches, and updating to account for the new device ID of 0x4e90, the keyboard now works great, but the mouse no longer responds. While unfortunate, not a huge loss as the mouse is really not that great. But the keyboard is rather nice. Until a complete fix is found, will substitute in a working Logitech wireless mouse for now.

Patching kernel drivers: e.g. linux-4.20.13

Download kernel source from kernel.org

cd /kernels/linux-4.20.13/drivers/hid

dff and edit hid-ids.h, hid-primax.c, and hid-quirks.c accordingly.

Update kernel Makefile SUBLEVEL to match running kernel. e.g. set SUBLEVEL = 13-042013-generic

Copy config file to kernel directory. e.g. cp /boot/config-4.20.13-042013-generic .config

cd ~/kernels/linux-4.20.13

make prepare scripts

make modules SUBDIRS=drivers/hid

sudo -i
cd /lib/modules/4.20.13-042013-generic/kernel/drivers/hid
mv hid-primax.ko hid-primax.ko.orig
cp ~/kernels/linux-4.20.13/drivers/hid/hid-primax.ko .

reboot
