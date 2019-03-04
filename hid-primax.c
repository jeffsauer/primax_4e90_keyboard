/*
 * HID driver for primax and similar keyboards with in-band modifiers
 *
 * Copyright 2011 Google Inc. All Rights Reserved
 *
 * Author:
 *	Terry Lambert <tlambert@google.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/device.h>
#include <linux/hid.h>
#include <linux/module.h>

#include "hid-ids.h"
static void fix_hid_descriptor_primax(__u8 *rdesc)
{
        __u8 desc[16];
        int i=0;

        /* There are variants which do not have the issue these must be excluded */
        if (rdesc[46] == 0x05 && rdesc[47] == 0x07 && rdesc[48] == 0x19 &&
            rdesc[49] == 0x00 && rdesc[50] == 0x2A && rdesc[51] == 0xFF &&
            rdesc[52] == 0x00 && rdesc[53] == 0x15 && rdesc[54] == 0x00 &&
            rdesc[55] == 0x26 && rdesc[56] == 0xFF && rdesc[57] == 0x00 &&
            rdesc[58] == 0x95 && rdesc[59] == 0x06 && rdesc[60] == 0x75 &&
            rdesc[61] == 0x08)
                return;

        desc[0] = rdesc[56];
        desc[1] = rdesc[57];
        desc[2] = rdesc[51];
        desc[3] = rdesc[52];
        desc[4] = rdesc[53];
        desc[5] = rdesc[54];
        desc[6] = rdesc[55];
        desc[7] = rdesc[46];
        desc[8] = rdesc[47];
        desc[9] = rdesc[48];
        desc[10] = rdesc[49];
        desc[11] = rdesc[50];
        desc[12] = rdesc[60];
        desc[13] = rdesc[61];
        desc[14] = rdesc[58];
        desc[15] = rdesc[59];
        for (i=46;i<=61;i++) {
                rdesc[i] = desc[i-46];
        }
}

static __u8 *primax_report_fixup(struct hid_device *hid, __u8 *rdesc,
                unsigned int *rsize)
{
        switch (hid->product) {
        /* gottwald@igel.com Workaround to get Primax based keyboards working */
        case USB_DEVICE_ID_PRIMAX_KEYBOARD_NEW:
                if (*rsize > 61) {
                        fix_hid_descriptor_primax(rdesc);
                }
                break;
        }
        return rdesc;
}

static int px_raw_event(struct hid_device *hid, struct hid_report *report,
	 u8 *data, int size)
{
	int idx = size;

	if (hid->product == USB_DEVICE_ID_PRIMAX_KEYBOARD_NEW)
		return 0;

	switch (report->id) {
	case 0:		/* keyboard input */
		/*
		 * Convert in-band modifier key values into out of band
		 * modifier bits and pull the key strokes from the report.
		 * Thus a report data set which looked like:
		 *
		 * [00][00][E0][30][00][00][00][00]
		 * (no modifier bits + "Left Shift" key + "1" key)
		 *
		 * Would be converted to:
		 *
		 * [01][00][00][30][00][00][00][00]
		 * (Left Shift modifier bit + "1" key)
		 *
		 * As long as it's in the size range, the upper level
		 * drivers don't particularly care if there are in-band
		 * 0-valued keys, so they don't stop parsing.
		 */
		while (--idx > 1) {
			if (data[idx] < 0xE0 || data[idx] > 0xE7)
				continue;
			data[0] |= (1 << (data[idx] - 0xE0));
			data[idx] = 0;
		}
		hid_report_raw_event(hid, HID_INPUT_REPORT, data, size, 0);
		return 1;

	default:	/* unknown report */
		/* Unknown report type; pass upstream */
		hid_info(hid, "unknown report type %d\n", report->id);
		break;
	}

	return 0;
}

static const struct hid_device_id px_devices[] = {
	{ HID_USB_DEVICE(USB_VENDOR_ID_PRIMAX, USB_DEVICE_ID_PRIMAX_KEYBOARD) },
	{ HID_USB_DEVICE(USB_VENDOR_ID_PRIMAX, USB_DEVICE_ID_PRIMAX_KEYBOARD_NEW) },
	{ }
};
MODULE_DEVICE_TABLE(hid, px_devices);

static struct hid_driver px_driver = {
	.name = "primax",
	.id_table = px_devices,
	.raw_event = px_raw_event,
	.report_fixup = primax_report_fixup,
};
module_hid_driver(px_driver);

MODULE_AUTHOR("Terry Lambert <tlambert@google.com>");
MODULE_LICENSE("GPL");
