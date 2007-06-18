//======================================================================
// USBCode - USB support entry points for driver
//
// Copyright (C) 2005 Douglas Boling
//======================================================================
//#include <USBdi.h>
//#include <usb100.h>
//#include <usbclient.h>
//#include "USBVideo.h"


#define dim(a) (sizeof(a)/sizeof(a[0]))
DWORD Num (PBYTE p, int nCntBytes);
void DumpGUID (PBYTE pData);

int DumpEndpointDescriptor (LPCUSB_ENDPOINT_DESCRIPTOR lpepd);
int DumpInterfaceDescriptor (PCUSB_INTERFACE_DESCRIPTOR lpDes);
int DumpUsbEndpoint (LPCUSB_ENDPOINT lpepd, int i);
int DumpDeviceDescriptor (USB_HANDLE hDevice, LPCUSB_FUNCS lpUsbFuncs, LPUSB_DEVICE_DESCRIPTOR lpdev);
int DumpUsbConfiguration (USB_HANDLE hDevice, LPCUSB_FUNCS lpUsbFuncs, LPCUSB_CONFIGURATION lpcfg);
int DumpUsbInterface (LPCUSB_INTERFACE lpif);

BOOL InstallCamDriver (HWND hWnd, LPCWSTR szDriverLibFile, DWORD dwVendor, DWORD dwProd);

extern HINSTANCE g_hInst;
DWORD GetUsbString (USB_HANDLE hDevice, LPCUSB_FUNCS lpUsbFuncs, UCHAR id, LPTSTR sz, int nSize);

BOOL CALLBACK AboutDlgProc (HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam);

//----------------------------------------------------------------------
//
//
int DumpUSBDeviceInfo (USB_HANDLE hDevice, LPCUSB_FUNCS lpUsbFuncs,
                      LPCUSB_INTERFACE lpInterface, LPCWSTR szUniqueDriverId,
                      LPCUSB_DRIVER_SETTINGS lpDriverSettings)
{
	DEBUGMSG (1, (TEXT("=========================================================\r\n")));
	DEBUGMSG (1, (TEXT("=========================================================\r\n")));
	DEBUGMSG (1, (TEXT("================  DumpUSBDeviceInfo  ====================\r\n")));
	DEBUGMSG (1, (TEXT("=========================================================\r\n")));
	DEBUGMSG (1, (TEXT("=========================================================\r\n")));

	if (lpInterface)
		DumpUsbInterface (lpInterface);
	//
	// Dump DriverSettings structure
	//
	DEBUGMSG (1, (TEXT("DriverSettings structure\r\n")));
	DEBUGMSG (1, (TEXT("\t dwVendorId:       %08xh (%d)\r\n"), lpDriverSettings->dwVendorId));
	DEBUGMSG (1, (TEXT("\t dwProductId:      %08xh (%d)\r\n"), lpDriverSettings->dwProductId));
	DEBUGMSG (1, (TEXT("\t dwReleaseNumber:  %08xh (%d)\r\n\r\n"), lpDriverSettings->dwReleaseNumber));

	DEBUGMSG (1, (TEXT("\t dwDeviceClass:    %08xh (%d)\r\n"), lpDriverSettings->dwDeviceClass));
	DEBUGMSG (1, (TEXT("\t dwDeviceSubClass: %08xh (%d)\r\n"), lpDriverSettings->dwDeviceSubClass));
	DEBUGMSG (1, (TEXT("\t dwDeviceProtocol: %08xh (%d)\r\n\r\n"), lpDriverSettings->dwDeviceProtocol));

	DEBUGMSG (1, (TEXT("\t dwInterfaceClass:    %08xh (%d)\r\n"), lpDriverSettings->dwInterfaceClass));
	DEBUGMSG (1, (TEXT("\t dwInterfaceSubClass: %08xh (%d)\r\n"), lpDriverSettings->dwInterfaceSubClass));
	DEBUGMSG (1, (TEXT("\t dwInterfaceProtocol: %08xh (%d)\r\n\r\n"), lpDriverSettings->dwInterfaceProtocol));

	// Dump driver ID string
	DEBUGMSG (1, (TEXT("szUniqueDriverId: >%s<\r\n\r\n"), szUniqueDriverId));


	
	//
	// Query the device for more information
	//
	DEBUGMSG (1, (TEXT("\r\n\r\n")));
	DEBUGMSG (1, (TEXT("Querying the device...\r\n\r\n")));

	LPCUSB_DEVICE lpUsbDev = (lpUsbFuncs->lpGetDeviceInfo)(hDevice);
	if (lpUsbDev)
	{
		DEBUGMSG (1, (TEXT("USB_DEVICE dwCount: %d\r\n"), lpUsbDev->dwCount));
		DumpDeviceDescriptor (hDevice, lpUsbFuncs, (LPUSB_DEVICE_DESCRIPTOR) &lpUsbDev->Descriptor);

		DEBUGMSG (1, (TEXT("\r\n\r\n")));
		DEBUGMSG (1, (TEXT("Dumping Configuration\r\n")));
		DumpUsbConfiguration (hDevice, lpUsbFuncs, (LPCUSB_CONFIGURATION) lpUsbDev->lpConfigs);
	}
	else
		DEBUGMSG (1, (TEXT("\r\nGetDeviceInfo failed rc=%d\r\n\r\n"), GetLastError()));

	return 0;
}

//------------------------------------------------------------------------
//
//
int DumpDeviceDescriptor (USB_HANDLE hDevice, LPCUSB_FUNCS lpUsbFuncs, LPUSB_DEVICE_DESCRIPTOR lpdev)
{
	TCHAR sz[256];

	DEBUGMSG (1, (TEXT("================================\r\n")));
	DEBUGMSG (1, (TEXT("Device descriptor\r\n")));

	// Size changes between 5 and 6
//	if (lpdev->bLength != sizeof (USB_DEVICE_DESCRIPTOR))
//		DEBUGMSG (1, (TEXT("\t Unexpected structure length %d   *********** \r\n"), lpdev->bLength));

	if (lpdev->bDescriptorType != USB_DEVICE_DESCRIPTOR_TYPE)
		DEBUGMSG (1, (TEXT("\t Unexpected DescriptorType!  %02xh \r\n"), lpdev->bDescriptorType));

	DEBUGMSG (1, (TEXT("\t Expected USB version:  %d.%02d\r\n"), lpdev->bcdUSB>>8, lpdev->bcdUSB & 0xff));
	DEBUGMSG (1, (TEXT("\t bDeviceClass:          %02xh\r\n"), lpdev->bDeviceClass));
	DEBUGMSG (1, (TEXT("\t bDeviceSubClass:       %02xh\r\n"), lpdev->bDeviceSubClass));
	DEBUGMSG (1, (TEXT("\t bDeviceProtocol:       %02xh\r\n"), lpdev->bDeviceProtocol));
	DEBUGMSG (1, (TEXT("\t bMaxPacketSize0:       %02xh\r\n"), lpdev->bMaxPacketSize0));
	DEBUGMSG (1, (TEXT("\t idVendor:              %04xh\r\n"), lpdev->idVendor));
	DEBUGMSG (1, (TEXT("\t idProduct:             %04xh\r\n"), lpdev->idProduct));
	DEBUGMSG (1, (TEXT("\t Device version:        %d.%02d\r\n"), lpdev->bcdDevice>>8, lpdev->bcdDevice & 0xff));

	GetUsbString (hDevice, lpUsbFuncs, lpdev->iManufacturer, sz, dim(sz));
	DEBUGMSG (1, (TEXT("\t Manufacturer: (ID:%d)  >%s<\r\n"), lpdev->iManufacturer, sz));

	GetUsbString (hDevice, lpUsbFuncs, lpdev->iProduct, sz, dim(sz));
	DEBUGMSG (1, (TEXT("\t Product:      (ID:%d)  >%s<\r\n"), lpdev->iProduct, sz));

	GetUsbString (hDevice, lpUsbFuncs, lpdev->iSerialNumber, sz, dim(sz));
	DEBUGMSG (1, (TEXT("\t SerialNumber: (ID:%d)  >%s<\r\n"), lpdev->iSerialNumber, sz));

	DEBUGMSG (1, (TEXT("\t bNumConfigurations:    %02xh\r\n"), lpdev->bNumConfigurations));


	DEBUGMSG (1, (TEXT("\t Expected USB version:  %d.%02d\r\n"), lpdev->bcdUSB>>8, lpdev->bcdUSB & 0xff));
	DEBUGMSG (1, (TEXT("\t bDeviceClass:          %02xh\r\n"), lpdev->bDeviceClass));
	DEBUGMSG (1, (TEXT("\t bDeviceSubClass:       %02xh\r\n"), lpdev->bDeviceSubClass));
	DEBUGMSG (1, (TEXT("\t bDeviceProtocol:       %02xh\r\n"), lpdev->bDeviceProtocol));
	DEBUGMSG (1, (TEXT("\t bMaxPacketSize0:       %02xh\r\n"), lpdev->bMaxPacketSize0));
	DEBUGMSG (1, (TEXT("\t idVendor:              %04xh\r\n"), lpdev->idVendor));
	DEBUGMSG (1, (TEXT("\t idProduct:             %04xh\r\n"), lpdev->idProduct));
	DEBUGMSG (1, (TEXT("\t Device version:        %d.%02d\r\n"), lpdev->bcdDevice>>8, lpdev->bcdDevice & 0xff));

	GetUsbString (hDevice, lpUsbFuncs, lpdev->iManufacturer, sz, dim(sz));
	DEBUGMSG (1, (TEXT("\t Manufacturer: (ID:%d)  >%s<\r\n"), lpdev->iManufacturer, sz));

	GetUsbString (hDevice, lpUsbFuncs, lpdev->iProduct, sz, dim(sz));
	DEBUGMSG (1, (TEXT("\t Product:      (ID:%d)  >%s<\r\n"), lpdev->iProduct, sz));

	GetUsbString (hDevice, lpUsbFuncs, lpdev->iSerialNumber, sz, dim(sz));
	DEBUGMSG (1, (TEXT("\t SerialNumber: (ID:%d)  >%s<\r\n"), lpdev->iSerialNumber, sz));

	DEBUGMSG (1, (TEXT("\t bNumConfigurations:    %02xh\r\n"), lpdev->bNumConfigurations));

	return 0;
}

//------------------------------------------------------------------------
//
//
int DumpConfigurationDescriptor (USB_HANDLE hDevice, LPCUSB_FUNCS lpUsbFuncs, LPUSB_CONFIGURATION_DESCRIPTOR lpcfg)
{
	TCHAR sz[256];
//	DEBUGMSG (1, (TEXT("Configuration descriptor\r\n")));

	if (lpcfg->bLength != sizeof (USB_CONFIGURATION_DESCRIPTOR))
		DEBUGMSG (1, (TEXT("\t Unexpected structure length %d    ***********\r\n"), lpcfg->bLength));

	if (lpcfg->bDescriptorType != USB_CONFIGURATION_DESCRIPTOR_TYPE)
		DEBUGMSG (1, (TEXT("\t Unexpected DescriptorType!  %02xh \r\n"), lpcfg->bDescriptorType));

	DEBUGMSG (1, (TEXT("\t wTotalLength:          %02xh\r\n"), lpcfg->wTotalLength));
	DEBUGMSG (1, (TEXT("\t bNumInterfaces:        %02xh\r\n"), lpcfg->bNumInterfaces));
	DEBUGMSG (1, (TEXT("\t bConfigurationValue:   %02xh\r\n"), lpcfg->bConfigurationValue));

	GetUsbString (hDevice, lpUsbFuncs, lpcfg->iConfiguration, sz, dim(sz));
	DEBUGMSG (1, (TEXT("\t Configuration: (ID:%d) >%s<\r\n"), lpcfg->iConfiguration, sz));

	memset (sz, 0, sizeof (sz));
	if (lpcfg->bmAttributes & USB_CONFIG_REMOTE_WAKEUP)
		lstrcat (sz, TEXT("Remote Wakeup  "));

	if (lpcfg->bmAttributes & USB_CONFIG_SELF_POWERED)
		lstrcat (sz, TEXT("Self Powered"));

	DEBUGMSG (1, (TEXT("\t bmAttributes:  (%02xh) >%s<\r\n"), lpcfg->bmAttributes, sz));
	DEBUGMSG (1, (TEXT("\t MaxPower:      (%02xh) %d mA\r\n"), lpcfg->MaxPower, lpcfg->MaxPower*2));
	return 0;
}
//------------------------------------------------------------------------
// DumpExtendedConfigurationDescriptor
// 
int DumpExtendedConfigDescriptor (LPBYTE lpex)
{
	if (lpex == 0)
		return -1;

	BYTE len;
	BYTE desType;
	__try {
		len = *lpex;
		desType = *(lpex+1);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		DEBUGMSG (1, (TEXT("Exception dumping extended descriptor\r\n")));
		return -1;
	}

	if ((len >= 8) && (desType == 0x0b)) // is it an Interface Association Descriptor?
	{
		DEBUGMSG (1, (TEXT("Interface Association Descriptor\r\n")));
		__try {
			DEBUGMSG (1, (TEXT("\t First Interface:          %02xh\r\n"), *(lpex+2)));
			DEBUGMSG (1, (TEXT("\t Interface Cnt:            %02xh\r\n"), *(lpex+3)));
			DEBUGMSG (1, (TEXT("\t Function Class:           %02xh\r\n"), *(lpex+4)));
			DEBUGMSG (1, (TEXT("\t Function Subclass:        %02xh\r\n"), *(lpex+5)));
			DEBUGMSG (1, (TEXT("\t Function Protocol:        %02xh\r\n"), *(lpex+6)));
			DEBUGMSG (1, (TEXT("\t iFunction:                %02xh\r\n"), *(lpex+7)));
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			DEBUGMSG (1, (TEXT("Exception dumping extended descriptor\r\n")));
			return -1;
		}
	}
	else
	{
		DEBUGMSG (1, (TEXT("Extended Configuration descriptor\r\n")));
		DEBUGMSG (1, (TEXT("\t Descriptor Length:        %02xh\r\n"), len));
		DEBUGMSG (1, (TEXT("\t I/F Descriptor Type:      %02xh\r\n"), desType));
	}
	return 0;
}
//------------------------------------------------------------------------
// DumpUsbConfiguration
// 
int DumpUsbConfiguration (USB_HANDLE hDevice, LPCUSB_FUNCS lpUsbFuncs, LPCUSB_CONFIGURATION lpcfg)
{
	DEBUGMSG (1, (TEXT("================================\r\n")));
	DEBUGMSG (1, (TEXT("Configuration\r\n")));

	if (lpcfg->dwCount != sizeof (USB_CONFIGURATION))
		DEBUGMSG (1, (TEXT("Unexpected CUSB_CONFIGURATION dwCount: %d  ***********\r\n"), lpcfg->dwCount));

	DumpConfigurationDescriptor (hDevice, lpUsbFuncs, (LPUSB_CONFIGURATION_DESCRIPTOR)&lpcfg->Descriptor);

	if (lpcfg->lpvExtended)
		DumpExtendedConfigDescriptor ((LPBYTE)lpcfg->lpvExtended);
	else
		DEBUGMSG (1, (TEXT("No Extended information\r\n")));


	DEBUGMSG (1, (TEXT("================================\r\n")));
	DEBUGMSG (1, (TEXT("CUSB_CONFIGURATION dwNumInterfaces: %d\r\n"), lpcfg->dwNumInterfaces));

	LPCUSB_INTERFACE lpInterfaces = lpcfg->lpInterfaces;
	DWORD i;
	__try {
		for (i = 0; i < lpcfg->dwNumInterfaces; i++)
		{
			DumpUsbInterface (lpInterfaces);
			lpInterfaces = (LPCUSB_INTERFACE)((DWORD) lpInterfaces + lpInterfaces->dwCount);
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		DEBUGMSG (1, (TEXT("Exception dumping interfaces\r\n")));
		return -1;
	}
	return 0;
}

//------------------------------------------------------------------------
//
//
int DumpEndpointDescriptor (LPCUSB_ENDPOINT_DESCRIPTOR lpepd)
{
//	DEBUGMSG (1, (TEXT("Endpoint descriptor\r\n")));

	if (lpepd->bLength != sizeof (USB_ENDPOINT_DESCRIPTOR))
		DEBUGMSG (1, (TEXT("\t\t Unexpected structure length %d   ***********\r\n"), lpepd->bLength));

	if (lpepd->bDescriptorType != USB_ENDPOINT_DESCRIPTOR_TYPE)
		DEBUGMSG (1, (TEXT("\t\t Unexpected DescriptorType!  %02xh \r\n"), lpepd->bDescriptorType));

	DEBUGMSG (1, (TEXT("\t\t bEndpointAddress:  %02xh  %s\r\n"), (lpepd->bEndpointAddress &0x0f), (lpepd->bEndpointAddress &0x80) ? TEXT("In") : TEXT("Out")));
	TCHAR szType[32] = TEXT("unknown");

	switch (lpepd->bmAttributes & USB_ENDPOINT_TYPE_MASK)
	{
	case USB_ENDPOINT_TYPE_CONTROL:
		lstrcpy (szType, TEXT("Control"));
		break;
	case USB_ENDPOINT_TYPE_ISOCHRONOUS:
		lstrcpy (szType, TEXT("Isochronous"));
		break;
	case USB_ENDPOINT_TYPE_BULK:
		lstrcpy (szType, TEXT("Bulk"));
		break;
	case USB_ENDPOINT_TYPE_INTERRUPT:
		lstrcpy (szType, TEXT("Interrupt"));
		break;
	}
	DEBUGMSG (1, (TEXT("\t\t bmAttributes:      %s\r\n"), szType));
	DEBUGMSG (1, (TEXT("\t\t wMaxPacketSize:    %xh (%d)\r\n"), lpepd->wMaxPacketSize, lpepd->wMaxPacketSize));
	DEBUGMSG (1, (TEXT("\t\t bInterval:         %xh (%d)\n"), lpepd->bInterval, lpepd->bInterval));
	return 0;
}
//------------------------------------------------------------------------
//
//
int DumpUsbEndpoint (LPCUSB_ENDPOINT lpepd, int i)
{
	DEBUGMSG (1, (TEXT("\t--------------------------------\r\n")));
	DEBUGMSG (1, (TEXT("\tEndpoint  %d\r\n"), i));

	if (lpepd->dwCount != sizeof (USB_ENDPOINT))
		DEBUGMSG (1, (TEXT("\tUnexpected CUSB_ENDPOINT dwCount: %d   ***********\r\n"), lpepd->dwCount));

	if (lpepd->lpvExtended)
		DEBUGMSG (1, (TEXT("\tCUSB_ENDPOINT lpvExtended: %08x\r\n"), lpepd->lpvExtended));

	DumpEndpointDescriptor (&lpepd->Descriptor);
	return 0;
}
//------------------------------------------------------------------------
//
//
LPTSTR GetVideoStreamingSubclassName (UCHAR id) 
{
	typedef struct {
		UCHAR id;
		LPTSTR szName;
	} SUBCLASS, *PSUBCLASS;

	SUBCLASS scVideoStreaming[] = 
	{
		{0x00,             TEXT("Undefined")},
		{0x01,             TEXT("Control")},
		{0x02,             TEXT("Streaming")},
		{0x03,             TEXT("Interface Collection")},
	};
	// Look up ID
	int i;
	for (i = 0; i < dim(scVideoStreaming); i++)
	{
		if (id == scVideoStreaming[i].id)
			return scVideoStreaming[i].szName;
	}
	return TEXT("Unknown");
}
//------------------------------------------------------------------------
//
//
LPTSTR GetIFName (UCHAR id) 
{
	struct {
		UCHAR id;
		LPTSTR szName;
	} ifIDs [] =
	{
		{USB_DEVICE_CLASS_RESERVED,          TEXT("Reserved")},
		{USB_DEVICE_CLASS_AUDIO,             TEXT("Audio")},
		{USB_DEVICE_CLASS_COMMUNICATIONS,    TEXT("Communications")},
		{USB_DEVICE_CLASS_HUMAN_INTERFACE,   TEXT("Human Interface")},
		{USB_DEVICE_CLASS_MONITOR,           TEXT("Monitor")},
		{USB_DEVICE_CLASS_PHYSICAL_INTERFACE,TEXT("Physical I/F")},
		{USB_DEVICE_CLASS_POWER,             TEXT("Power")},
		{USB_DEVICE_CLASS_PRINTER,           TEXT("Printer")},
		{USB_DEVICE_CLASS_STORAGE,           TEXT("Storage")},
		{USB_DEVICE_CLASS_HUB,               TEXT("Hub")},
		// From USB.org...
		{0x0a,                               TEXT("CDC-Data")},
		{0x0b,                               TEXT("Chip/Smartcard")},
//		{0x0c,                               TEXT("")},
		{0x0d,                               TEXT("Content-Security")},
		{0x0e,                               TEXT("Video")},
		{0xdc,                               TEXT("Diagnostic Device")},
		{0xe0,                               TEXT("Wireless Controller")},
		{0xfe,                               TEXT("Application Specific")},
		{USB_DEVICE_CLASS_VENDOR_SPECIFIC,   TEXT("Vendor Specific")},
	};
	// Look up ID
	int i;
	for (i = 0; i < dim(ifIDs); i++)
	{
		if (id == ifIDs[i].id)
			return ifIDs[i].szName;
	}
	return TEXT("Unknown");
}
//------------------------------------------------------------------------
//
//
int DumpInterfaceDescriptor (PCUSB_INTERFACE_DESCRIPTOR lpDes)
{
	__try
	{
		if (lpDes->bLength != sizeof(USB_INTERFACE_DESCRIPTOR))
			DEBUGMSG (1, (TEXT("\t Unexpected bLength:      %02xh   ***********\r\n"), lpDes->bLength));

		if (lpDes->bDescriptorType != USB_INTERFACE_DESCRIPTOR_TYPE)
			DEBUGMSG (1, (TEXT("\t Unexpected DescriptorType!  %02xh \r\n"), lpDes->bDescriptorType));

		DEBUGMSG (1, (TEXT("\t bInterfaceNumber:   %02xh \r\n"), lpDes->bInterfaceNumber));
		DEBUGMSG (1, (TEXT("\t bAlternateSetting:  %02xh \r\n"), lpDes->bAlternateSetting));
		DEBUGMSG (1, (TEXT("\t bNumEndpoints:      %02xh \r\n"), lpDes->bNumEndpoints));
		DEBUGMSG (1, (TEXT("\t bInterfaceClass:    %02xh (%s)\r\n"), lpDes->bInterfaceClass, GetIFName(lpDes->bInterfaceClass)));
		if (lpDes->bInterfaceClass == 0xe)
			DEBUGMSG (1, (TEXT("\t bInterfaceSubClass: %02xh (%s)\r\n"), lpDes->bInterfaceSubClass, GetVideoStreamingSubclassName(lpDes->bInterfaceSubClass)));
		else
			DEBUGMSG (1, (TEXT("\t bInterfaceSubClass: %02xh \r\n"), lpDes->bInterfaceSubClass));
		DEBUGMSG (1, (TEXT("\t bInterfaceProtocol: %02xh \r\n"), lpDes->bInterfaceProtocol));
		DEBUGMSG (1, (TEXT("\t iInterface:         %02xh \r\n"), lpDes->iInterface));
	}

	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		DEBUGMSG (1, (TEXT("Exception dumping interface descriptor\r\n")));
		return -1;
	}
	return 0;
}
//------------------------------------------------------------------------
// DumpByteStream
// 
int DumpByteStream (LPBYTE lpb, int nCnt, LPTSTR psz, int sMax)
{
	*psz = TEXT('\0');
	for (int i = 0; i < nCnt; i++)
	{
		wsprintf (&psz[lstrlen(psz)], TEXT(" %02x"), *(lpb+i));
		if (i && ((i % 16) == 0))
			lstrcat (psz, TEXT("\r\n"));
	}
	return 0;
}
//------------------------------------------------------------------------
// DumpExtendedConfigurationDescriptor
// 
int DumpExtendedInterfaceDescriptor (LPBYTE lpex)
{
	if (lpex == 0)
		return -1;

	TCHAR sz[256];
	PUSBVIDSTDDESCHDR pStd = (PUSBVIDSTDDESCHDR)lpex;

	__try 
	{
		DEBUGMSG (1, (TEXT("Extended Interface Descriptor\r\n")));
		DEBUGMSG (1, (TEXT("\t Descriptor Length:        %02xh\r\n"), pStd->bLen));
		DEBUGMSG (1, (TEXT("\t I/F Descriptor Type:      %02xh\r\n"), pStd->bType));
		DEBUGMSG (1, (TEXT("\t I/F Descriptor Subtype:   %02xh\r\n"), pStd->bSubtype));
		lstrcpy (sz, TEXT("        "));
		DumpByteStream (lpex+3, pStd->bLen - 3, &sz[lstrlen (sz)], dim (sz) - lstrlen (sz));
		DEBUGMSG (1, (TEXT("%s\r\n"), sz));
	}

	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		DEBUGMSG (1, (TEXT("Exception dumping extended descriptor\r\n")));
		return -1;
	}
	return 0;
}
//------------------------------------------------------------------------
// DumpExtendedConfigurationDescriptor
// 

LPTSTR LabelBitmap (LPBYTE lpbm, int len, LPTSTR *pLbls, int cLbls, LPTSTR sz, int nStrSize)
{
	int index;
	BYTE bm;
	sz[0] = TEXT('\0');
	if (nStrSize < 6)
		return sz;
	for (index = 0; index < cLbls; index++)
	{
		// Do we need another byte?
		if ((index % 8) == 0)
		{
			// if no more bytes, leave
			if (len == 0) return sz;
			bm = *lpbm++;
			len--;
		}
		if (bm & 0x01)
		{
			if (lstrlen (sz) + lstrlen (pLbls[index]) < (DWORD)nStrSize)
			{
				lstrcat (sz, pLbls[index]);
				lstrcat (sz, TEXT(", "));
			}
			else
			{
				lstrcpy (sz, TEXT("<***>\r\n"));
				return sz;
			}

		}
		bm = bm >> 1;
	}
	return sz;	
}
LPTSTR DumpConfigBitmap (LPBYTE lpbm, int len, LPTSTR sz, int nStrSize)
{
	LPTSTR szLbls[] = {
		TEXT("Scanning Mode"),
		TEXT("Auto-Exposure Mode"),
		TEXT("Auto-Exposure Priority"),
		TEXT("Exposure Time (Abs)"),
		TEXT("Exposure Time (Rel)"),
		TEXT("Focus (Abs)"),
		TEXT("Focus (Rel)"),
		TEXT("Iris (Abs)"),
		TEXT("Iris (Rel)"),
		TEXT("Zoom (Abs)"),
		TEXT("Zoom (Rel)"),
		TEXT("PanTilt (Abs)"),
		TEXT("PanTilt (Rel)"),
		TEXT("Roll (Abs)"),
		TEXT("Roll (Rel)"),
		TEXT("Reserved"),
		TEXT("Reserved"),
		TEXT("Focus, Auto"),
		TEXT("Privacy"),
	};
	return LabelBitmap (lpbm, len, szLbls, dim(szLbls), sz, nStrSize);
}

//------------------------------------------------------------------------
// DumpExtendedVideoControlInterfaceDescriptor
// 
int DumpExtendedVideoControlInterfaceDescriptor (LPBYTE lpex)
{
	if (lpex == 0)
		return -1;
	LPTSTR szpuLbls[] = {
		TEXT("Brightness"),
		TEXT("Contrast"),
		TEXT("Hue"),
		TEXT("Saturation"),
		TEXT("Sharpness"),
		TEXT("Gamma"),
		TEXT("White Balance Temp"),
		TEXT("White Balance Component"),
		TEXT("Backlight Compensation"),
		TEXT("Gain"),
		TEXT("Power Line Frequency"),
		TEXT("Hue-Auto"),
		TEXT("White Balance Temp-Auto"),
		TEXT("White Balance Component-Auto"),
		TEXT("Digital Multiplier"),
		TEXT("Digital Multiplier Limit"),
		TEXT("Analog Video Standard"),
		TEXT("Analog Video Lock Status"),
		TEXT("Reserved"),
	};

	TCHAR sz[256];
	PUSBVIDCTLIFDESCRIPTOR pDes = (PUSBVIDCTLIFDESCRIPTOR) lpex;
	int i;
	__try {

		if ((pDes->bType != 0x24) || (pDes->bSubtype != 1)) 
			return DumpExtendedInterfaceDescriptor(lpex);

		PBYTE pEnd = lpex + pDes->wTotalLen;
		DEBUGMSG (1, (TEXT("Video Control Interface Descriptor Header\r\n")));
		if (pDes->bLen >= 0x0d) 
		{
			DEBUGMSG (1, (TEXT("\t Subtype:                  %02xh\r\n"), pDes->bSubtype));
			DEBUGMSG (1, (TEXT("\t Interface Ver             %d.%02d\r\n"), pDes->wIFVersion>>8, pDes->wIFVersion & 0xff));
			DEBUGMSG (1, (TEXT("\t Total Length:             %04xh\r\n"), pDes->wTotalLen));
			DEBUGMSG (1, (TEXT("\t InCollection:             %02xh\r\n"), pDes->bInCollection));
			for (i = 0; i < pDes->bInCollection; i++)
			{
				DEBUGMSG (1, (TEXT("\t   baInterface(%d):          %02xh\r\n"), i, pDes->bInterface[i]));
			}
		}
		PUSBVIDSTDDESCHDR pStd = (PUSBVIDSTDDESCHDR)lpex;
		while (lpex + pStd->bLen < pEnd)
		{
			lpex += pStd->bLen;
			pStd = (PUSBVIDSTDDESCHDR)lpex;
			if (pStd->bType != 0x24)
			{
				DEBUGMSG (1, (TEXT("Unexpected header type %xh\r\n"), pStd->bType));
				break;
			}
			switch (pStd->bSubtype) 
			{
			case 0x1:
				DEBUGMSG (1, (TEXT("Video Control Interface Descriptor Header in list????\r\n")));
				break;
			case USB_VIDEO_VC_INPUT_TERMINAL:
				if (pStd->bLen >= 0x8)
				{
					WORD wType = *(LPWORD)(lpex+4);
					switch (wType)
					{
					case 0x0101: lstrcpy (sz, TEXT("Stream"));	break;
					case 0x0201: lstrcpy (sz, TEXT("Camera"));	break;
					case 0x0202: lstrcpy (sz, TEXT("Media Transport"));	break;
					case 0x0401: lstrcpy (sz, TEXT("Composite Connector")); break;
					case 0x0402: lstrcpy (sz, TEXT("S-Video Connector")); break;
					case 0x0403: lstrcpy (sz, TEXT("Component Connector")); break;
					default:     lstrcpy (sz, TEXT("")); break;
					}
					DEBUGMSG (1, (TEXT("Input Terminal Descriptor  (%s)\r\n"), sz));

					DEBUGMSG (1, (TEXT("\t Terminal ID:            %02xh\r\n"), *(lpex+3)));
					DEBUGMSG (1, (TEXT("\t Terminal Type:          %04xh\r\n"), wType));
					DEBUGMSG (1, (TEXT("\t Assoc Terminal:         %02xh\r\n"), *(lpex+6)));
					DEBUGMSG (1, (TEXT("\t iTerminal:              %02xh\r\n"), *(lpex+7)));
				} else
					DEBUGMSG (1, (TEXT("Input Terminal Descriptor\r\n")));

				if (pStd->bLen >= 0x11)
				{
					DEBUGMSG (1, (TEXT("\t Obj Focal Len (min)     %d\r\n"), *(LPWORD)(lpex+8)));
					DEBUGMSG (1, (TEXT("\t Obj Focal Len (max)     %d\r\n"), *(LPWORD)(lpex+10)));
					DEBUGMSG (1, (TEXT("\t Ocular Focal Len        %d\r\n"), *(LPWORD)(lpex+12)));
					DEBUGMSG (1, (TEXT("\t Control Size:           %02xh\r\n"), *(lpex+14)));
					DEBUGMSG (1, (TEXT("\t bmControls:             %02xh\r\n"), *(lpex+15)));
					DumpByteStream (lpex+15, *(lpex+14), sz, dim (sz));
					DEBUGMSG (1, (TEXT("\t bmControls:  >%s<\r\n"), sz));
					DumpConfigBitmap (lpex+15, *(lpex+14), sz, dim (sz));
					DEBUGMSG (1, (TEXT("\t bmControls:  >%s<\r\n"), sz));
				}
				break;
			case USB_VIDEO_VC_OUTPUT_TERMINAL:
				if (pStd->bLen >= 9)
				{
					WORD wType = *(LPWORD)(lpex+4);
					switch (wType)
					{
					case 0x0101: lstrcpy (sz, TEXT("Stream"));	break;
					case 0x0300: lstrcpy (sz, TEXT("Vendor")); break;
					case 0x0301: lstrcpy (sz, TEXT("Display")); break;
					case 0x0302: lstrcpy (sz, TEXT("Seq Media")); break;
					default:     lstrcpy (sz, TEXT("")); break;
					}
					DEBUGMSG (1, (TEXT("Output Terminal Descriptor  (%s)\r\n"), sz));

					DEBUGMSG (1, (TEXT("\t Terminal ID:            %02xh\r\n"), *(lpex+3)));
					DEBUGMSG (1, (TEXT("\t Terminal Type:          %04xh\r\n"), wType));
					DEBUGMSG (1, (TEXT("\t Assoc Terminal:         %02xh\r\n"), *(lpex+6)));
					DEBUGMSG (1, (TEXT("\t SourceID                %02xh\r\n"), *(lpex+7)));
					DEBUGMSG (1, (TEXT("\t iTerminal:              %02xh\r\n"), *(lpex+8)));
				} else
					DEBUGMSG (1, (TEXT("Output Terminal Descriptor\r\n")));
				break;
			case USB_VIDEO_VC_SELECTOR_UNIT:
				DEBUGMSG (1, (TEXT("Selector Unit Descriptor\r\n")));
				if (pStd->bLen >= 6)
				{
					DEBUGMSG (1, (TEXT("\t Unit ID:                %02xh\r\n"), *(lpex+3)));
				}
				break;
			case USB_VIDEO_VC_PROCESSING_UNIT:
				DEBUGMSG (1, (TEXT("Processing Unit Descriptor\r\n")));
				if (pStd->bLen >= 10)
				{
					int nbmCnt = Num(lpex+7,1);
					DEBUGMSG (1, (TEXT("\t Unit ID:                %02xh\r\n"), *(lpex+3)));
					DEBUGMSG (1, (TEXT("\t Source ID:              %02xh\r\n"), *(lpex+4)));
					DEBUGMSG (1, (TEXT("\t Max Multiplier:         %d.%02d"), Num(lpex+5,2)/100, Num(lpex+5,2)%100));
					DEBUGMSG (1, (TEXT("\t Control size:           %d\r\n"), nbmCnt));
					DumpByteStream (lpex+8, nbmCnt, sz, dim (sz));
					DEBUGMSG (1, (TEXT("\t bmControls:             >%s<\r\n"), sz));
					LabelBitmap (lpex+8, nbmCnt, szpuLbls, dim(szpuLbls), sz, dim(sz));
					DEBUGMSG (1, (TEXT("\t bmControls:             >%s<\r\n"), sz));
					DEBUGMSG (1, (TEXT("\t iProcessing:            %d\r\n"), *(lpex+8+nbmCnt)));
					DEBUGMSG (1, (TEXT("\t bmVideoStds:            %d\r\n"), *(lpex+9+nbmCnt)));
				}
				break;
			case USB_VIDEO_VC_EXTENSION_UNIT:
				DEBUGMSG (1, (TEXT("Extension Unit Descriptor\r\n")));
				if (pStd->bLen >= 24)
				{
					DEBUGMSG (1, (TEXT("\t Unit ID:                %02xh\r\n"), *(lpex+3)));
					DumpGUID (lpex+4);
					DEBUGMSG (1, (TEXT("\t Num Ctls:               %02xh\r\n"), *(lpex+20)));
					DEBUGMSG (1, (TEXT("\t Num In Pins:            %02xh\r\n"), *(lpex+21)));
					DEBUGMSG (1, (TEXT("\t Source ID(I)            %02xh\r\n"), *(lpex+22)));
					// there is more on this one...
				}
				break;
			default:
				DEBUGMSG (1, (TEXT("Unknown Interface Descriptor subtype  %xh\r\n"), pStd->bSubtype));
				lstrcpy (sz, TEXT("        "));    
				for (i = 3; i < pStd->bLen; i++)
				{
					wsprintf (&sz[lstrlen(sz)], TEXT(" %02x"), *(lpex+i));
					if ((i % 16) == 0)
					{
						DEBUGMSG (1, (TEXT("%s\r\n"), sz));
						sz[0] = TEXT('\0');
					}
				}
				DEBUGMSG (1, (TEXT("%s\r\n"), sz));
				break;
			}
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		DEBUGMSG (1, (TEXT("Exception dumping extended descriptor\r\n")));
		return -1;
	}
	return 0;
}
//------------------------------------------------------------------------
// Num
// 
DWORD Num (PBYTE p, int nCntBytes)
{
	int c = 0;
	DWORD b;
	DWORD num = 0;
	while (nCntBytes)
	{
		b = *p++;
		b = b << (c * 8);
		num += b;
		nCntBytes--;
		c++;
	}
	return num;
}
//------------------------------------------------------------------------
// DumpGUID 
// 
void DumpGUID (PBYTE pData)
{
	if (pData == 0)
		return;

	__try
	{
		DEBUGMSG (1, (TEXT("     GUID: %08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x\r\n"), 
		          Num (pData, 4), Num (pData+4, 2),Num (pData+6, 2), *(pData+8), *(pData+9),
				  *(pData+10), *(pData+11), *(pData+12), *(pData+13), *(pData+14), *(pData+15)));
	}

	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		DEBUGMSG (1, (TEXT("Exception dumping GUID\r\n")));
		return;
	}


	return;
}
//------------------------------------------------------------------------
// DumpExtendedVideoStreamInterfaceDescriptor
// 
int DumpExtendedVideoStreamInterfaceDescriptor (LPBYTE lpex)
{
	if (lpex == 0)
		return -1;

	TCHAR sz[256];
	int i;
	PUSBVIDSTREAMIFDESCRIPTOR pDes = (PUSBVIDSTREAMIFDESCRIPTOR) lpex;
	// Test access
	__try {

		if ((pDes->bType != 0x24) || (pDes->bSubtype != 1)) 
			return DumpExtendedInterfaceDescriptor(lpex);

		PBYTE pEnd = lpex + pDes->wTotalLen;
		int nNumFmts = *(lpex+3);
		DEBUGMSG (1, (TEXT("Video Stream interface Descriptor Header\r\n")));
		if (pDes->bLen >= 0x0d) 
		{
			DEBUGMSG (1, (TEXT("\t Subtype:                  %02xh\r\n"), pDes->bSubtype));
			DEBUGMSG (1, (TEXT("\t Num Formats:              %d\r\n"), pDes->bNumFormats));
			DEBUGMSG (1, (TEXT("\t Total Length:             %04xh\r\n"), pDes->wTotalLen));
			DEBUGMSG (1, (TEXT("\t EndPoint Addr:            %02xh\r\n"), pDes->bEndpointAddress));
			DEBUGMSG (1, (TEXT("\t bmInfo:                   %02xh\r\n"), pDes->bmInfo));
			DEBUGMSG (1, (TEXT("\t Terminal Link:            %02xh\r\n"), pDes->bTerminalLink));
			DEBUGMSG (1, (TEXT("\t Still Capture Method      %02xh\r\n"), pDes->bStillCapMethod));
			DEBUGMSG (1, (TEXT("\t Trigger Support           %02xh\r\n"), pDes->bTriggerSupport));
			DEBUGMSG (1, (TEXT("\t Trigger Usage             %02xh\r\n"), pDes->bTriggerUsage));
			int z;
			DEBUGMSG (1, (TEXT("\t Control Size              %02xh\r\n"), pDes->bControlSize));
			PBYTE p = pDes->bmaControls;
			for (z = 0; z < pDes->bNumFormats; z++, p += pDes->bControlSize)
			{
				DumpByteStream (p, pDes->bControlSize, sz, dim (sz));
				DEBUGMSG (1, (TEXT("\t bmControls:      %d       >%s<\r\n"), z, sz));
			}
		}

		PUSBVIDSTDDESCHDR pStd = (PUSBVIDSTDDESCHDR)lpex;
		while (lpex + pStd->bLen < pEnd)
		{
			lpex += pStd->bLen;
			pStd = (PUSBVIDSTDDESCHDR)lpex;

			if (pStd->bType != 0x24)			{
				DEBUGMSG (1, (TEXT("Unexpected header type %xh\r\n"), pStd->bType));
				break;
			}
			switch (pStd->bSubtype) 
			{
			case USB_VIDEO_VS_STILL_IMAGE_FRAME:
				DEBUGMSG (1, (TEXT("Still Image Frame\r\n")));
				{
					DEBUGMSG (1, (TEXT("\t Endpoint Address:         %02xh\r\n"), *(lpex+3)));
					DEBUGMSG (1, (TEXT("\t Num Image Sizes:          %d\r\n"), *(lpex+4)));
					PBYTE p = lpex+5;
					int k;
					for (k = 0; k < *(lpex+4); k++) {
						int x = Num (p, 2);
						int y = Num (p+2, 2);
						DEBUGMSG (1, (TEXT("\t    Img %2d  %3d x %3d\r\n"), k, x, y));
						p += 4;
					}
					// Get compression values  
					int m = *p++;
					DEBUGMSG (1, (TEXT("\t Compression cnt %d\r\n"), m));
					for (k = 0; k < m; k++) 
					{
						DEBUGMSG (1, (TEXT("\t    Cmp %2d  %d\r\n"), k, *p++));
					}
				}
				break;
			case USB_VIDEO_VS_FORMAT_UNCOMPRESSED:
				DEBUGMSG (1, (TEXT("Format Uncompressed\r\n")));
				break;
			case USB_VIDEO_VS_FRAME_UNCOMPRESSED:
				DEBUGMSG (1, (TEXT("Frame Uncompressed\r\n")));
				break;
			case USB_VIDEO_VS_FORMAT_MJPEG:
				DEBUGMSG (1, (TEXT("Format MJPEG\r\n")));
				if (pStd->bLen >= 11)
				{
					DEBUGMSG (1, (TEXT("\t Format Index:             %d\r\n"), *(lpex+3)));
					DEBUGMSG (1, (TEXT("\t Num Descriptors:          %d\r\n"), *(lpex+4)));
					DEBUGMSG (1, (TEXT("\t bmFlags                   %02xh\r\n"), *(lpex+5)));
					DEBUGMSG (1, (TEXT("\t Default Frame Index       %d\r\n"), *(lpex+6)));
					DEBUGMSG (1, (TEXT("\t Aspect Ratio X:           %d\r\n"), *(lpex+7)));
					DEBUGMSG (1, (TEXT("\t Aspect Ratio Y:           %d\r\n"), *(lpex+8)));
					DEBUGMSG (1, (TEXT("\t Interlace Flags:          %02xh\r\n"), *(lpex+9)));
					DEBUGMSG (1, (TEXT("\t CopyProtect Flag:         %02xh\r\n"), *(lpex+10)));
				}
				break;
			case USB_VIDEO_VS_FRAME_MJPEG:
				DEBUGMSG (1, (TEXT("Frame MJPEG\r\n")));
				if (pStd->bLen >= 30)
				{
					DEBUGMSG (1, (TEXT("\t Frame Index:              %d\r\n"), *(lpex+3)));
					DEBUGMSG (1, (TEXT("\t Capabilities:             %02xh\r\n"), *(lpex+4)));
					DEBUGMSG (1, (TEXT("\t Width:                    %d\r\n"), Num(lpex+5, 2)));
					DEBUGMSG (1, (TEXT("\t Height:                   %d\r\n"), Num(lpex+7, 2)));
					DEBUGMSG (1, (TEXT("\t Min Bit Rate:             %d\r\n"), Num(lpex+9, 4)));
					DEBUGMSG (1, (TEXT("\t Max Bit Rate:             %d\r\n"), Num(lpex+13, 4)));
					DEBUGMSG (1, (TEXT("\t Max Framebuff Size:       %d\r\n"), Num(lpex+17, 4)));
					DEBUGMSG (1, (TEXT("\t Def Interval:             %d\r\n"), Num(lpex+21, 4)));
					int k;
					k = Num (lpex+25, 1);
					if (k == 0)
					{
						DEBUGMSG (1, (TEXT("\t Min Frame Interval:       %d\r\n"), Num (lpex+26, 4)));
						DEBUGMSG (1, (TEXT("\t Max Frame Interval:       %d\r\n"), Num (lpex+30, 4)));
						DEBUGMSG (1, (TEXT("\t Frame Interval Step:      %d\r\n"), Num (lpex+34, 4)));
					} else
					{
						DEBUGMSG (1, (TEXT("\t Frame interval Cnt:       %d\r\n"), *(lpex+25)));
						PBYTE p = lpex+26;
						for (k = 0; k < *(lpex+25); k++)
						{
							int t = Num (p, 4);
							if (t <= 10000)
								DEBUGMSG (1, (TEXT("\t   Frame Interval:    %d  %7d     %3d.%03d mS  (%2d fps)\r\n"), 
								          k, t, t/10000, (t/10)%1000, 1000/(t/10000)));
							else
								DEBUGMSG (1, (TEXT("\t   Frame Interval:    %d  %7d\r\n"), k, t));
							p += 4;
						}
					}
				}
				break;
			case 0x8:
			case 0x9:
			case 0x0b:
			case 0x0e:
			case 0x0f:
				DEBUGMSG (1, (TEXT(" ***** Reserved\r\n")));
				break;
			case USB_VIDEO_VS_FORMAT_MPEG2TS:
				DEBUGMSG (1, (TEXT(" ***** Format MPEG2TS\r\n")));
				break;
			case USB_VIDEO_VS_COLORFORMAT:
				DEBUGMSG (1, (TEXT("Color Format\r\n")));
				if (pStd->bLen >= 6)
				{
					PUSBVIDCOLORIFDESCRIPTOR p = (PUSBVIDCOLORIFDESCRIPTOR)lpex;
					DEBUGMSG (1, (TEXT("\t bColorPrimaries:           %02x\r\n"), p->bColorPrimaries));
					DEBUGMSG (1, (TEXT("\t bTransferCharacteristics:  %02x\r\n"), p->bTransferCharacteristics));
					DEBUGMSG (1, (TEXT("\t bMatrixCoefficients        %02xh\r\n"), p->bMatrixCoefficients));
				}
				break;
			case USB_VIDEO_VS_FORMAT_FRAME_BASED:
				DEBUGMSG (1, (TEXT(" ***** Format Frame Based\r\n")));
				break;
			case USB_VIDEO_VS_FRAME_FRAME_BASED:
				DEBUGMSG (1, (TEXT(" ***** Frame Frame Based\r\n")));
				break;
			case USB_VIDEO_VS_FORMAT_STREAM_BASED:
				DEBUGMSG (1, (TEXT(" ***** Format Stream Based\r\n")));
				break;
			default:
				DEBUGMSG (1, (TEXT(" ***** Unknown Interface Descriptor subtype  %xh  ***** \r\n"), pStd->bSubtype));
				lstrcpy (sz, TEXT("        "));    
				for (i = 3; i < pStd->bLen; i++)
				{
					wsprintf (&sz[lstrlen(sz)], TEXT(" %02x"), *(lpex+i));
					if ((i % 16) == 0)
					{
						DEBUGMSG (1, (TEXT("%s\r\n"), sz));
						sz[0] = TEXT('\0');
					}
				}
				DEBUGMSG (1, (TEXT("%s\r\n"), sz));
				break;
			}
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		DEBUGMSG (1, (TEXT("Exception dumping extended descriptor\r\n")));
		return -1;
	}
	return 0;
}

//------------------------------------------------------------------------
//
//
int DumpUsbInterface (LPCUSB_INTERFACE lpif)
{
	DEBUGMSG (1, (TEXT("================================\r\n")));
	DEBUGMSG (1, (TEXT("USB Interface\r\n")));

	if (lpif->dwCount != sizeof (USB_INTERFACE)) 
		DEBUGMSG (1, (TEXT("Unexpected USB_INTERFACE size dwCount: %d   ***********\r\n"), lpif->dwCount));

	DumpInterfaceDescriptor (&lpif->Descriptor);

	if (lpif->lpvExtended)
	{
//		if (lpif->Descriptor.bInterfaceClass == 0x0e) //video
//		{
		__try 
		{
			if (lpif->Descriptor.bInterfaceSubClass == 0x01) //Control
				DumpExtendedVideoControlInterfaceDescriptor ((LPBYTE) lpif->lpvExtended);
			else if (lpif->Descriptor.bInterfaceSubClass == 0x02) //Stream
				DumpExtendedVideoStreamInterfaceDescriptor ((LPBYTE) lpif->lpvExtended);
			else
				DumpExtendedInterfaceDescriptor ((LPBYTE) lpif->lpvExtended);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			DEBUGMSG (1, (TEXT("Exception dumping endpoints\r\n")));
			return -1;
		}

//		}
//		else
//			DumpExtendedInterfaceDescriptor ((LPBYTE) lpif->lpvExtended);
	}
	
	int i;
	if (lpif->lpEndpoints)
	{
		__try 
		{
			LPCUSB_ENDPOINT ep = lpif->lpEndpoints;
			for (i = 0; i < lpif->Descriptor.bNumEndpoints; i++)
			{
				DumpUsbEndpoint(ep, i);
				ep = (LPCUSB_ENDPOINT)((DWORD) ep + ep->dwCount);
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			DEBUGMSG (1, (TEXT("Exception dumping endpoints\r\n")));
			return -1;
		}
	}

	return 0;
}
//------------------------------------------------------------------------
// Retrieves the requested string descriptor and validates it.
//
DWORD GetStringDescriptor (USB_HANDLE hDevice, LPCUSB_FUNCS lpUsbFuncs,
    BYTE bIdx, WORD wLangId, PBYTE pbBuffer, WORD  cbBuffer, PDWORD pbTransferred)
{
    PUSB_STRING_DESCRIPTOR pStringDesc = (PUSB_STRING_DESCRIPTOR) pbBuffer;
    USB_ERROR usbErr;
    USB_TRANSFER hTransfer;
    DWORD dwErr;

    DEBUGCHK(cbBuffer >= sizeof(USB_STRING_DESCRIPTOR));
    PREFAST_DEBUGCHK(pbTransferred != NULL);
    
    hTransfer = lpUsbFuncs->lpGetDescriptor (hDevice, NULL, NULL,
									USB_SHORT_TRANSFER_OK, USB_STRING_DESCRIPTOR_TYPE, bIdx, 
									wLangId, cbBuffer, pbBuffer);

    if (hTransfer != NULL) {
        GetTransferStatus (lpUsbFuncs, hTransfer, pbTransferred, &usbErr);
        CloseTransferHandle(lpUsbFuncs, hTransfer);

        if (usbErr != USB_NO_ERROR) {
            if (usbErr == USB_STALL_ERROR) {
                dwErr = ERROR_NOT_SUPPORTED;
            }        
            else {
                dwErr = ERROR_GEN_FAILURE;
            }

            goto EXIT;
        }
    }
    else {
        dwErr = GetLastError();
        goto EXIT;
    }
    
    // We've got a descriptor. Is it valid?
    if ( (*pbTransferred < (sizeof(USB_STRING_DESCRIPTOR) - sizeof(pStringDesc->bString))) || 
         (pStringDesc->bDescriptorType != USB_STRING_DESCRIPTOR_TYPE) ) { 
        DEBUGCHK(FALSE); // The device returned something strange.
        dwErr = ERROR_GEN_FAILURE;
        goto EXIT;
    }

    dwErr = ERROR_SUCCESS;

EXIT:
    if (dwErr != ERROR_SUCCESS) {
        DEBUGMSG(1, (_T("%s: Error getting string descriptor Err=%u\r\n"),
            dwErr));
    }
    
    return dwErr;    
}
//-----------------------------------------------------------------------------
// Get a device string. For predefined types see hiddi.h.
//
DWORD GetUsbString (USB_HANDLE hDevice, LPCUSB_FUNCS lpUsbFuncs, UCHAR id, LPTSTR sz, int nSize)
{
    PCUSB_DEVICE pDeviceInfo = NULL;
    DWORD dwErr;
    WORD wLangId;
    DWORD dwBytesTransferred;

    const DWORD CB_STRING_DESCRIPTOR_MAX = 0xff;

    union {
        BYTE rgbBuffer[CB_STRING_DESCRIPTOR_MAX];
        USB_STRING_DESCRIPTOR StringDesc;
    } StringDescriptor;
    

    // Get the Zero string descriptor to determine which LANGID to use.
    // We just use the first LANGID listed.
    dwErr = GetStringDescriptor (hDevice, lpUsbFuncs, 0, 0, (PBYTE) &StringDescriptor,
        sizeof(StringDescriptor), &dwBytesTransferred);
    if (dwErr != ERROR_SUCCESS) {
        goto EXIT;
    }

    DEBUGCHK(StringDescriptor.StringDesc.bLength >= sizeof(USB_STRING_DESCRIPTOR));
    DEBUGCHK(StringDescriptor.StringDesc.bDescriptorType == USB_STRING_DESCRIPTOR_TYPE);
    wLangId = StringDescriptor.StringDesc.bString[0];

    // Get the string descriptor for the first LANGID
    dwErr = GetStringDescriptor(hDevice, lpUsbFuncs, id, wLangId, (PBYTE) &StringDescriptor,
        sizeof(StringDescriptor), &dwBytesTransferred);
    if (dwErr != ERROR_SUCCESS) {
        goto EXIT;
    }

    __try {
        // Copy the character count and string into the user's buffer
        int cchActual = (StringDescriptor.StringDesc.bLength - 2) / sizeof(WCHAR); // Does not include NULL
        if (sz != NULL) {
            StringCchCopyN(sz, nSize, 
                StringDescriptor.StringDesc.bString, cchActual);
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        DEBUGMSG(1, (_T("GetUSBString: Exception writing to user buffer\r\n")));
        dwErr = ERROR_INVALID_PARAMETER;
    }

EXIT:
    return dwErr;    
}
