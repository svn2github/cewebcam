//-------------------------------------------------------------------------
// <copyright file="USBCode.cpp" company="Microsoft">
//    Copyright (c) Microsoft Corporation.  All rights reserved.
//
//    The use and distribution terms for this software are covered by the
//    Microsoft Limited Permissive License (Ms-LPL) 
//    http://www.microsoft.com/resources/sharedsource/licensingbasics/limitedpermissivelicense.mspx 
//    which can be found in the file MS-LPL.txt at the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by
//    the terms of this license.
//
//    You must not remove this notice, or any other, from this software.
// </copyright>
// 
// <summary>
//    USB support entry points for driver
// </summary>
//-------------------------------------------------------------------------
//======================================================================
// USBCode - USB support entry points for driver
//
// Author: Douglas Boling
//======================================================================
#include <windows.h>                 // For all that Windows stuff
#include <USBdi.h>					// USB includes
#include <usb100.h>					// USB includes
#include <usbclient.h>				// USB client driver helper code

#include "usbvideo.h"				// USB Video Specification defs

#include "webcamsdk.h"				// IOCTL defs for driver
#include "WebCam.h" 				// Local driver includes

typedef struct {
	LPTSTR lpszCamName;
	WORD wVendorID;
	WORD wProductID;
} USBCAMSTRUCT, *PUSBCAMSTRUCT;

#define VID_LOGITECH           1133
#define PID_QUICKCAMPRO5000    2245

//
// This structure contains the list of vendor specific cameras
// that closely follow the USB Vid Specification even though they
// don't say they do.  If your camera works with the driver, you
// can add your camera to this list.  
//
USBCAMSTRUCT csCameras[] = 
{
	{ TEXT("Logitech QuickCam Pro5000"),      VID_LOGITECH, PID_QUICKCAMPRO5000},
	{ TEXT("Logitech QuickCam Fusion"),       VID_LOGITECH, 0x08c1},
	{ TEXT("Logitech QuickCam Orbit"),        VID_LOGITECH, 0x08c2},
	{ TEXT("Logitech QuickCam Notebook Pro"), VID_LOGITECH, 0x08c3},
};


// function to dunp USB info
BOOL USBDeviceNotificationCallback (LPVOID lpvNotifyParameter, DWORD dwCode,
									LPDWORD* dwInfo1, LPDWORD* dwInfo2,
									LPDWORD* dwInfo3, LPDWORD* dwInfo4);

#ifdef DEBUG
#include "USBDebug.cpp"
#endif //DEBUG
//======================================================================
// USBDeviceAttach - Called when Host controller wants to load the driver
// 
BOOL USBDeviceAttach (USB_HANDLE hDevice, LPCUSB_FUNCS lpUsbFuncs,
                      LPCUSB_INTERFACE lpInterface, LPCWSTR szUniqueDriverId,
                      LPBOOL fAcceptControl,
                      LPCUSB_DRIVER_SETTINGS lpDriverSettings, DWORD dwUnused)
{
    WCHAR wsSubClassRegKey[] = CLIENT_REGKEY_SZ;
	DWORD dwContext;

    DEBUGMSG (ZONE_USBLOAD, (DTAG TEXT("USBDeviceAttach++\r\n")));

	// Default is that we won't accept this device
	*fAcceptControl = FALSE;
#ifdef DEBUG
	if (dpCurSettings.ulZoneMask & ZONE_DESCRIPTORS)
	{
		DumpUSBDeviceInfo (hDevice, lpUsbFuncs, lpInterface, szUniqueDriverId,
						   lpDriverSettings);
	}
#endif
	// 
	// Ask the PDD if we want this USB device
	//
	int rc = pdd_DeviceAttach (hDevice, lpUsbFuncs, lpInterface, szUniqueDriverId, 
	                       lpDriverSettings, &dwContext);

	switch (rc)
	{
		
	case 0:				// PDD rejected the interface
		return FALSE;
	case 1:				// PDD accepted the interface
		break;
	case 2:				// PDD wants the interface but don't load stream driver.
		*fAcceptControl = TRUE;
		return TRUE;
	case 3:				// PDD wants the interface passed on if possible.
		break;
	}

	// Create and initialize the driver instance structure.  Alloc
	// now since the PDD will need some of the info.  Free if PDD
	// rejects the interface.
	PDRVCONTEXT pDrv = (PDRVCONTEXT)LocalAlloc (LPTR, sizeof (DRVCONTEXT));
	if (pDrv == 0)
	{ 
		SetLastError (ERROR_NOT_ENOUGH_MEMORY);
		return FALSE;
	}
    pDrv->dwSize = sizeof (DRVCONTEXT);
	pDrv->hDevice = hDevice;
	pDrv->lpUsbFuncs = lpUsbFuncs;
	InitializeCriticalSection (&pDrv->csDCall);
	pDrv->hUnload = CreateEvent (NULL, FALSE, FALSE, NULL);
	pDrv->fUnloadFlag = FALSE;
	pDrv->dwPddContext = dwContext;
	pDrv->fBusy = FALSE;

	// Load us as a strean device driver...
	pDrv->hStreamDevice = ActivateDevice (wsSubClassRegKey, (DWORD)pDrv);

	if (pDrv->hStreamDevice) 
	{
		// register for USB callbacks
		if (lpUsbFuncs->lpRegisterNotificationRoutine (hDevice, USBDeviceNotificationCallback, pDrv))
		{
			// Accept this device as our own
			*fAcceptControl = TRUE;

		} else
			DEBUGMSG (ZONE_ERROR, (DTAG TEXT("Can't register for USB Callbacks! rc=%d\r\n"),
			          GetLastError()));
	} else
		DEBUGMSG (ZONE_ERROR, (DTAG TEXT("Can't activate stream device! rc=%d\r\n"),
		          GetLastError()));

    DEBUGMSG (ZONE_USBLOAD, (DTAG TEXT("USBDeviceAttach--\r\n")));
	return TRUE;
}
//======================================================================
// USBDeviceNotificationCallback - This routine is called by the USB
// host controller when a USB event occurs that the driver needs to
// know about.
// 
BOOL USBDeviceNotificationCallback (LPVOID lpvNotifyParameter, DWORD dwCode,
									LPDWORD* dwInfo1, LPDWORD* dwInfo2,
									LPDWORD* dwInfo3, LPDWORD* dwInfo4)
{
	if (dwCode == USB_CLOSE_DEVICE)
	{
		// Get the pointer to our device context.
		PDRVCONTEXT pDrv = (PDRVCONTEXT) lpvNotifyParameter;

		// Notify PDD that the hardware has been removed.
		pdd_DeviceDetach (pDrv);

		// Set flags and handles to notify stream driver its about to unload.
		pDrv->fUnloadFlag = TRUE;
		SetEvent (pDrv->hUnload);
		Sleep (0);

		// Unload the stream driver
		DeactivateDevice (pDrv->hStreamDevice);

		Sleep (1000);
		LocalFree (pDrv);
		return TRUE;
	}
	return FALSE;
}
//======================================================================
// USBInstallDriver - Called by Host controller to have the driver
// register itself.  This call is made in response to the user
// entering the driver's DLL name in the "Unknown Device" message box.
// 
BOOL USBInstallDriver (LPCWSTR szDriverLibFile)
{
    WCHAR wsUsbDeviceID[] = CLASS_NAME_SZ;
    WCHAR wsSubClassRegKey[] = CLIENT_REGKEY_SZ;
    USB_DRIVER_SETTINGS usbDriverSettings = { DRIVER_SETTINGS };
	BOOL bSuccess;
	LPREGISTER_CLIENT_DRIVER_ID lpfnRegisterClientDriverID;
	LPREGISTER_CLIENT_SETTINGS lpfnRegisterClientSettings;

    DEBUGMSG (ZONE_USBLOAD, (DTAG TEXT("USBInstallDriver++\r\n")));

	HINSTANCE hUSBD = LoadLibrary (TEXT("usbd.dll"));
	if (hUSBD == 0)
	{
	    DEBUGMSG (ZONE_ERROR, (DTAG TEXT("Can't load USBD.DLL\r\n")));
		return FALSE;
	}

	// Explicitly link to USBD DLL.  
	lpfnRegisterClientDriverID = (LPREGISTER_CLIENT_DRIVER_ID) GetProcAddress (hUSBD, TEXT("RegisterClientDriverID"));
	lpfnRegisterClientSettings = (LPREGISTER_CLIENT_SETTINGS) GetProcAddress (hUSBD, TEXT("RegisterClientSettings"));
	if ((lpfnRegisterClientDriverID == 0) || (lpfnRegisterClientSettings == 0))
	{
		FreeLibrary (hUSBD);
		return FALSE;
	}
	//
	// register with USBD
	//	 
	bSuccess = (lpfnRegisterClientDriverID) (wsUsbDeviceID);
	if (!bSuccess) 
	{
		DEBUGMSG (ZONE_ERROR, (TEXT("RegisterClientDriverID error:%d\n"), GetLastError()));
		FreeLibrary (hUSBD);
		return FALSE;
	}
	//
	// Call USBD to create registry entries for USB client registration
	//
	bSuccess = (lpfnRegisterClientSettings) (szDriverLibFile, wsUsbDeviceID, 
	                                         NULL, &usbDriverSettings);
	if (!bSuccess) 
	{
		DEBUGMSG (ZONE_ERROR, (TEXT("RegisterClientSettings error:%d\n"), GetLastError()));
		FreeLibrary (hUSBD);
		return FALSE;
	}
	//
	// Now add the vendor specific entries
	//
	usbDriverSettings.dwInterfaceClass = USB_NO_INFO;		// Remove the Video class specifier
	for (int i = 0; i < dim (csCameras); i++)
	{
		usbDriverSettings.dwVendorId = csCameras[i].wVendorID;
		usbDriverSettings.dwProductId = csCameras[i].wProductID;

		// Call USBD to create registry entries for this specific product
		bSuccess = (lpfnRegisterClientSettings) (szDriverLibFile, wsUsbDeviceID, 
												 NULL, &usbDriverSettings);
		if (!bSuccess) 
		{
			DEBUGMSG (ZONE_ERROR, (TEXT("RegisterClientSettings1 error:%d\n"), GetLastError()));
			FreeLibrary (hUSBD);
			return FALSE;
		}
	}

	//
	// The USB host controller driver has a utility function to make it
	// simple to create (and later query) the proper registery entries 
	// for loading a stream driver.  First, we create a table of registry 
	//values we want to set.
	//
    REG_VALUE_DESCR usbCamKeyValues[] = {
        (TEXT("Dll")),               REG_SZ,    0, (PBYTE)(DRIVER_NAME),
        (TEXT("Prefix")),            REG_SZ,    0, (PBYTE)(DEVICE_PREFIX),
        NULL, 0, 0, NULL
    };

    // Set the default in the registry values.  This routine is in the 
	// USBClient library that we link to.
    bSuccess = GetSetKeyValues (wsSubClassRegKey, usbCamKeyValues, SET, TRUE );
	if (!bSuccess)
	{
        DEBUGMSG (ZONE_ERROR, (TEXT("GetSetKeyValues failed!\n")));
		FreeLibrary (hUSBD);
		return FALSE;
    }

	FreeLibrary (hUSBD);
    DEBUGMSG (ZONE_USBLOAD, (DTAG TEXT("USBInstallDriver--\r\n")));
	return TRUE;
}

//======================================================================
// USBUnInstallDriver - Called to have the USB client clean up its
// registry entries.
// 
BOOL USBUnInstallDriver()
{
    WCHAR wsUsbDeviceID[] = CLASS_NAME_SZ;
    USB_DRIVER_SETTINGS usbDriverSettings = { DRIVER_SETTINGS };
	LPUN_REGISTER_CLIENT_SETTINGS lpfnUnRegisterClientSettings;
	BOOL bSuccess = FALSE;

    DEBUGMSG (ZONE_USBLOAD, (DTAG TEXT("USBUnInstallDriver++\r\n")));

	// Explicitly link to USBD DLL.  
	HINSTANCE hUSBD = LoadLibrary (TEXT("usbd.dll"));
	if (hUSBD == 0)
	{
	    DEBUGMSG (ZONE_ERROR, (DTAG TEXT("Can't load USBD.DLL\r\n")));
		return FALSE;
	}
	lpfnUnRegisterClientSettings = (LPUN_REGISTER_CLIENT_SETTINGS) GetProcAddress (hUSBD, TEXT("UnRegisterClientSettings"));
	// Clean up the registry
	if (lpfnUnRegisterClientSettings)
		bSuccess = (lpfnUnRegisterClientSettings) (wsUsbDeviceID, NULL, &usbDriverSettings);

	FreeLibrary (hUSBD);
    DEBUGMSG (ZONE_USBLOAD, (DTAG TEXT("USBUnInstallDriver--\r\n")));
	return bSuccess;
}

