Welcome to the primax_4e90_keyboard wiki!

Along with the 2018 HP Envy 34" curved all in one computer comes an HP Wireless 800 Combo keyboard and mouse made by PRIMAX. Vendor ID of 0x0461 and device ID 0x4e90. With existing kernel driver as of 4.20.13, the mouse would work, however the keyboard would only respond with certain keys... the super key, and keys used to control brightness, volume, etc.

After some digging, found this possible fix:

https://unix.stackexchange.com/questions/377830/linux-hid-driver-for-primax-wireless-keyboards/402288#402288

After applying the patches, and updating to account for the new device ID of 0x4e90, the keyboard now works great, but the mouse no longer responds. That is fine by me, as the mouse is not that great. But the keyboard is rather nice. Will substitute in a working Logitech wireless mouse.
