//-------------------------------------------------------------------------
// <copyright file="pddVClas.cpp" company="Microsoft">
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
//    Low level USB video interface code
// </summary>
//-------------------------------------------------------------------------
//======================================================================
// pddVClas.cpp - Low level USB video interface code
//
// Author: Douglas Boling
//======================================================================
#include <windows.h>                // For all that Windows stuff
#include <USBdi.h>					// USB includes
#include <usb100.h>					// USB includes
#include <usbclient.h>				// USB client driver helper code
#include "usbvideo.h"				// USB Video Specification defs

#include "webcamsdk.h"				// IOCTL defs for driver
#include "WebCam.h" 				// Local driver includes
#include "pddVClas.h"				// PDD defs


//
// Controllable features of the Camera.  The order of this table must be the same as the bitfields of the 
// Camera input descriptor feature bit fields.
//
/*
FEATURESTRUCT CamFeatures[] = 
{
//    Feature ID                  USB Video Class Command ID                     Unit ID            Interface  Length
	{FEAT_SCANNING_MODE,      USB_VIDEO_CT_CS_SCANNING_MODE_CTL,           USB_VIDEO_VC_INPUT_TERMINAL, 0,  1},
	{FEAT_AUTO_EXPOSURE_MODE, USB_VIDEO_CT_CS_AE_MODE_CTL               ,  USB_VIDEO_VC_INPUT_TERMINAL, 0,  1},
	{FEAT_AUTO_EXPOSURE_PRI,  USB_VIDEO_CT_CS_AE_PRIORITY_CTL           ,  USB_VIDEO_VC_INPUT_TERMINAL, 0,  1},
	{FEAT_EXPOSURE_TIME_ABS,  USB_VIDEO_CT_CS_EXPOSURE_TIME_ABSOLUTE_CTL,  USB_VIDEO_VC_INPUT_TERMINAL, 0,  4},
	{FEAT_EXPOSURE_TIME_REL,  USB_VIDEO_CT_CS_EXPOSURE_TIME_RELATIVE_CTL,  USB_VIDEO_VC_INPUT_TERMINAL, 0,  1},
	{FEAT_FOCUS_ABS,          USB_VIDEO_CT_CS_FOCUS_ABSOLUTE_CTL        ,  USB_VIDEO_VC_INPUT_TERMINAL, 0,  2},
	{FEAT_FOCUS_REL,          USB_VIDEO_CT_CS_FOCUS_RELATIVE_CTL        ,  USB_VIDEO_VC_INPUT_TERMINAL, 0,  2},
	{FEAT_IRIS_ABS,           USB_VIDEO_CT_CS_IRIS_ABSOLUTE_CTL         ,  USB_VIDEO_VC_INPUT_TERMINAL, 0,  1},
	{FEAT_IRIS_REL,           USB_VIDEO_CT_CS_IRIS_RELATIVE_CTL         ,  USB_VIDEO_VC_INPUT_TERMINAL, 0,  2},
	{FEAT_ZOOM_ABS,           USB_VIDEO_CT_CS_ZOOM_ABSOLUTE_CTL         ,  USB_VIDEO_VC_INPUT_TERMINAL, 0,  1},
	{FEAT_ZOOM_REL,           USB_VIDEO_CT_CS_ZOOM_RELATIVE_CTL         ,  USB_VIDEO_VC_INPUT_TERMINAL, 0,  2},
	{FEAT_PANTILT_ABS,        USB_VIDEO_CT_CS_PANTILT_ABSOLUTE_CTL      ,  USB_VIDEO_VC_INPUT_TERMINAL, 0,  3},
	{FEAT_PANTILT_REL,        USB_VIDEO_CT_CS_PANTILT_RELATIVE_CTL      ,  USB_VIDEO_VC_INPUT_TERMINAL, 0,  8},
	{FEAT_ROLL_ABS,           USB_VIDEO_CT_CS_ROLL_ABSOLUTE_CTL         ,  USB_VIDEO_VC_INPUT_TERMINAL, 0,  4},
	{FEAT_ROLL_REL,           USB_VIDEO_CT_CS_ROLL_RELATIVE_CTL         ,  USB_VIDEO_VC_INPUT_TERMINAL, 0,  2},
	{FEAT_UNSUPPORTED, 0, 0, 0, 0},  //Reserved 
	{FEAT_UNSUPPORTED, 0, 0, 0, 0},  //Reserved
	{FEAT_FOCUS_AUTO ,        USB_VIDEO_CT_CS_FOCUS_AUTO_CTL            ,  USB_VIDEO_VC_INPUT_TERMINAL, 0, 2},
	{FEAT_PRIVACY ,           USB_VIDEO_CT_CS_PRIVACY_CTL               ,  USB_VIDEO_VC_INPUT_TERMINAL, 0, 1},
};*/
FEATURESTRUCT CamFeatures[] = 
{
//    Feature ID                  USB Video Class Command ID                     Unit ID            Interface  Length
	{FEAT_SCANNING_MODE,      USB_VIDEO_CT_CS_SCANNING_MODE_CTL,           FEAT_UNSUPPORTED, 0,  1},
	{FEAT_AUTO_EXPOSURE_MODE, USB_VIDEO_CT_CS_AE_MODE_CTL               ,  FEAT_UNSUPPORTED, 0,  1},
	{FEAT_AUTO_EXPOSURE_PRI,  USB_VIDEO_CT_CS_AE_PRIORITY_CTL           ,  FEAT_UNSUPPORTED, 0,  1},
	{FEAT_EXPOSURE_TIME_ABS,  USB_VIDEO_CT_CS_EXPOSURE_TIME_ABSOLUTE_CTL,  FEAT_UNSUPPORTED, 0,  4},
	{FEAT_EXPOSURE_TIME_REL,  USB_VIDEO_CT_CS_EXPOSURE_TIME_RELATIVE_CTL,  FEAT_UNSUPPORTED, 0,  1},
	{FEAT_FOCUS_ABS,          USB_VIDEO_CT_CS_FOCUS_ABSOLUTE_CTL        ,  FEAT_UNSUPPORTED, 0,  2},
	{FEAT_FOCUS_REL,          USB_VIDEO_CT_CS_FOCUS_RELATIVE_CTL        ,  FEAT_UNSUPPORTED, 0,  2},
	{FEAT_IRIS_ABS,           USB_VIDEO_CT_CS_IRIS_ABSOLUTE_CTL         ,  FEAT_UNSUPPORTED, 0,  1},
	{FEAT_IRIS_REL,           USB_VIDEO_CT_CS_IRIS_RELATIVE_CTL         ,  FEAT_UNSUPPORTED, 0,  2},
	{FEAT_ZOOM_ABS,           USB_VIDEO_CT_CS_ZOOM_ABSOLUTE_CTL         ,  FEAT_UNSUPPORTED, 0,  1},
	{FEAT_ZOOM_REL,           USB_VIDEO_CT_CS_ZOOM_RELATIVE_CTL         ,  FEAT_UNSUPPORTED, 0,  2},
	{FEAT_PANTILT_ABS,        USB_VIDEO_CT_CS_PANTILT_ABSOLUTE_CTL      ,  FEAT_UNSUPPORTED, 0,  3},
	{FEAT_PANTILT_REL,        USB_VIDEO_CT_CS_PANTILT_RELATIVE_CTL      ,  FEAT_UNSUPPORTED, 0,  8},
	{FEAT_ROLL_ABS,           USB_VIDEO_CT_CS_ROLL_ABSOLUTE_CTL         ,  FEAT_UNSUPPORTED, 0,  4},
	{FEAT_ROLL_REL,           USB_VIDEO_CT_CS_ROLL_RELATIVE_CTL         ,  FEAT_UNSUPPORTED, 0,  2},
	{FEAT_UNSUPPORTED, 0, FEAT_UNSUPPORTED, 0, 0},  //Reserved 
	{FEAT_UNSUPPORTED, 0, FEAT_UNSUPPORTED, 0, 0},  //Reserved
	{FEAT_FOCUS_AUTO ,        USB_VIDEO_CT_CS_FOCUS_AUTO_CTL            ,  FEAT_UNSUPPORTED, 0, 2},
	{FEAT_PRIVACY ,           USB_VIDEO_CT_CS_PRIVACY_CTL               ,  FEAT_UNSUPPORTED, 0, 1},
};

//
// Controllable features of the processor unit
//
/*
FEATURESTRUCT ProcFeatures[] = 
{
	{FEAT_BRIGHTNESS,               USB_VIDEO_PU_CS_BRIGHTNESS_CTL,                USB_VIDEO_VC_PROCESSING_UNIT, 0,  2},
	{FEAT_CONTRAST,                 USB_VIDEO_PU_CS_CONTRAST_CTL,                  USB_VIDEO_VC_PROCESSING_UNIT, 0,  2},
	{FEAT_HUE,                      USB_VIDEO_PU_CS_HUE_CTL,                       USB_VIDEO_VC_PROCESSING_UNIT, 0,  2},
	{FEAT_SATURATION,               USB_VIDEO_PU_CS_SATURATION_CTL,                USB_VIDEO_VC_PROCESSING_UNIT, 0,  2},
	{FEAT_SHARPNESS,                USB_VIDEO_PU_CS_SHARPNESS_CTL,                 USB_VIDEO_VC_PROCESSING_UNIT, 0,  2},
	{FEAT_GAMMA,                    USB_VIDEO_PU_CS_GAMMA_CTL,                     USB_VIDEO_VC_PROCESSING_UNIT, 0,  2},
	{FEAT_WHITE_BAL_TEMP,           USB_VIDEO_PU_CS_WHITE_BALANCE_TEMP_CTL,        USB_VIDEO_VC_PROCESSING_UNIT, 0,  2},
	{FEAT_WHITE_BAL_COMPONENT,      USB_VIDEO_PU_CS_WHITE_BALANCE_COMPONENT_CTL,   USB_VIDEO_VC_PROCESSING_UNIT, 0,  4},
	{FEAT_BACKLIGHT_COMPENSATION,   USB_VIDEO_PU_CS_BACKLIGHT_COMPENSATION_CTL,    USB_VIDEO_VC_PROCESSING_UNIT, 0,  2},
	{FEAT_GAIN,                     USB_VIDEO_PU_CS_GAIN_CTL,                      USB_VIDEO_VC_PROCESSING_UNIT, 0,  2},
	{FEAT_POWER_LINE_FREQ,          USB_VIDEO_PU_CS_POWER_LINE_FREQUENCY_CTL,      USB_VIDEO_VC_PROCESSING_UNIT, 0,  1},
	{FEAT_AUTO_HUE,                 USB_VIDEO_PU_CS_HUE_AUTO_CTL,                  USB_VIDEO_VC_PROCESSING_UNIT, 0,  1},
	{FEAT_AUTO_WHITE_BAL_TEMP,      USB_VIDEO_PU_CS_WHITE_BALANCE_TEMP_AUTO_CTL,   USB_VIDEO_VC_PROCESSING_UNIT, 0,  1},
	{FEAT_AUTO_WHITE_BAL_COMPONENT, USB_VIDEO_PU_CS_WHITE_BALANCE_COMPONENT_AUTO_CTL,  USB_VIDEO_VC_PROCESSING_UNIT, 0, 1},
	{FEAT_DIGITAL_MULTIPLIER,       USB_VIDEO_PU_CS_DIGITAL_MULTIPLIER_CTL,        USB_VIDEO_VC_PROCESSING_UNIT, 0, 2},
	{FEAT_DIGITAL_MULTIPLIER_LIMIT, USB_VIDEO_PU_CS_DIGITAL_MULTIPLIER_LIMIT_CTL,  USB_VIDEO_VC_PROCESSING_UNIT, 0, 2},
	{FEAT_ANALOG_VIDEO_STANDARD,    USB_VIDEO_PU_CS_ANALOG_VIDEO_STANDARD_CTL,     USB_VIDEO_VC_PROCESSING_UNIT, 0, 1},
	{FEAT_ANALOG_VIDEO_LOCK_STATUS, USB_VIDEO_PU_CS_ANALOG_LOCK_STATUS_CTL,        USB_VIDEO_VC_PROCESSING_UNIT, 0, 1},
};
*/
FEATURESTRUCT ProcFeatures[] = 
{
	{FEAT_BRIGHTNESS,               USB_VIDEO_PU_CS_BRIGHTNESS_CTL,                FEAT_UNSUPPORTED, 0,  2},
	{FEAT_CONTRAST,                 USB_VIDEO_PU_CS_CONTRAST_CTL,                  FEAT_UNSUPPORTED, 0,  2},
	{FEAT_HUE,                      USB_VIDEO_PU_CS_HUE_CTL,                       FEAT_UNSUPPORTED, 0,  2},
	{FEAT_SATURATION,               USB_VIDEO_PU_CS_SATURATION_CTL,                FEAT_UNSUPPORTED, 0,  2},
	{FEAT_SHARPNESS,                USB_VIDEO_PU_CS_SHARPNESS_CTL,                 FEAT_UNSUPPORTED, 0,  2},
	{FEAT_GAMMA,                    USB_VIDEO_PU_CS_GAMMA_CTL,                     FEAT_UNSUPPORTED, 0,  2},
	{FEAT_WHITE_BAL_TEMP,           USB_VIDEO_PU_CS_WHITE_BALANCE_TEMP_CTL,        FEAT_UNSUPPORTED, 0,  2},
	{FEAT_WHITE_BAL_COMPONENT,      USB_VIDEO_PU_CS_WHITE_BALANCE_COMPONENT_CTL,   FEAT_UNSUPPORTED, 0,  4},
	{FEAT_BACKLIGHT_COMPENSATION,   USB_VIDEO_PU_CS_BACKLIGHT_COMPENSATION_CTL,    FEAT_UNSUPPORTED, 0,  2},
	{FEAT_GAIN,                     USB_VIDEO_PU_CS_GAIN_CTL,                      FEAT_UNSUPPORTED, 0,  2},
	{FEAT_POWER_LINE_FREQ,          USB_VIDEO_PU_CS_POWER_LINE_FREQUENCY_CTL,      FEAT_UNSUPPORTED, 0,  1},
	{FEAT_AUTO_HUE,                 USB_VIDEO_PU_CS_HUE_AUTO_CTL,                  FEAT_UNSUPPORTED, 0,  1},
	{FEAT_AUTO_WHITE_BAL_TEMP,      USB_VIDEO_PU_CS_WHITE_BALANCE_TEMP_AUTO_CTL,   FEAT_UNSUPPORTED, 0,  1},
	{FEAT_AUTO_WHITE_BAL_COMPONENT, USB_VIDEO_PU_CS_WHITE_BALANCE_COMPONENT_AUTO_CTL,  FEAT_UNSUPPORTED, 0, 1},
	{FEAT_DIGITAL_MULTIPLIER,       USB_VIDEO_PU_CS_DIGITAL_MULTIPLIER_CTL,        FEAT_UNSUPPORTED, 0, 2},
	{FEAT_DIGITAL_MULTIPLIER_LIMIT, USB_VIDEO_PU_CS_DIGITAL_MULTIPLIER_LIMIT_CTL,  FEAT_UNSUPPORTED, 0, 2},
	{FEAT_ANALOG_VIDEO_STANDARD,    USB_VIDEO_PU_CS_ANALOG_VIDEO_STANDARD_CTL,     FEAT_UNSUPPORTED, 0, 1},
	{FEAT_ANALOG_VIDEO_LOCK_STATUS, USB_VIDEO_PU_CS_ANALOG_LOCK_STATUS_CTL,        FEAT_UNSUPPORTED, 0, 1},
};

//-----------------------------------------------------------------------
// Pdd_DeviceAttach - Called by MDD during USBDeviceAttach.  
// Return:
//     0 - Not my interface, ignore
//     1 - Claim interface
//     2 - Pass along interface to LoadGenericInterfaceDriver
// 
int pdd_DeviceAttach (USB_HANDLE hDevice, LPCUSB_FUNCS lpUsbFuncs,
                  LPCUSB_INTERFACE lpInterface, LPCWSTR szUniqueDriverId,
                  LPCUSB_DRIVER_SETTINGS lpDriverSettings, DWORD *pdwContext)
{
	int rc = 0;

	// Call the device to query all interfaces
   	LPCUSB_DEVICE lpUsbDev = (lpUsbFuncs->lpGetDeviceInfo)(hDevice);
	if (!lpUsbDev)
		return 0;

	// See if this device has the interface we need...
	// Only accept video control interface
	if ((DWORD)lpInterface != 0)
	{
		if (lpInterface->Descriptor.bInterfaceSubClass == 0x02)
		{
			return 2;
		}
	}

	// Allocate the Phys dev driver context structure.  We'll add this
	// pointer to the pdd field available in the device driver context
	// structure.
	PPDDCONTEXT pPDD = (PPDDCONTEXT)LocalAlloc (LPTR, sizeof (PDDCONTEXT)); 
	if (pPDD == 0)
	{
		SetLastError (ERROR_NOT_ENOUGH_MEMORY);
		return 0;
	}
	pPDD->dwSize = sizeof (PDDCONTEXT);
	InitializeCriticalSection (&pPDD->csStill);
	pPDD->hStillEvent = CreateEvent (NULL, FALSE, FALSE, NULL);
	pPDD->wReadThreadState = STREAMTHD_STOPPED;
	pPDD->wCurrFormatIndex = 0xffff;
	pPDD->wCurrFrameIndex = 0xffff;
	pPDD->hVendorEvent = CreateEvent (NULL, FALSE, FALSE, NULL);
	pPDD->hNewFrameEvent = CreateEvent (NULL, TRUE, FALSE, NULL); // Manual reset event

	// Set up default stream receive buffer
	pPDD->strStreamDefault.dwNumBuffs = NUMDEFBUFFS;
	InitializeCriticalSection (&pPDD->strStreamDefault.csBuffSync);
	pPDD->strStreamDefault.hNewFrame = NULL;
	pPDD->strStreamDefault.dwBuffSize = 0;
	pPDD->strStreamDefault.pFrame[0].pBuff = 0;
	pPDD->strStreamDefault.pFrame[1].pBuff = 0;

	// See if we can find the proper interfaces.  First, look for the
	// standard Video Class Device interfaces...
	if (!ParseStreamInterfaces (pPDD, lpUsbDev, 0x0e, 1, 0x0e, 2))
	{
		// If failed to open std interface, try vendor specific
		if (!ParseStreamInterfaces (pPDD, lpUsbDev, 0xff, 1, 0xff, 2))
		{
			FreePDDContext (pPDD);
			return 0;
		} 
	}
	DEBUGMSG(ZONE_USBLOAD, (DTAG TEXT("Video Class interface found.\n"), rc)); 

	// Parse to see what features are supported by the camera
	if (ParseFeatureParameters (pPDD))
		DEBUGMSG(ZONE_USBLOAD, (DTAG TEXT("Error calling ParseFeatureParameters\n"))); 

	rc = 1;  // We accept the device

	// Save our hardware context in the driver context structure.
	*pdwContext = (DWORD)pPDD;
	return rc;
}
//---------------------------------------------------------------------------------------
// FreePDDContext - Free the PDD context structure.
//
void FreePDDContext (PPDDCONTEXT pPDD)
{
	int i;
	if (pPDD == 0)
		return;
	__try 
	{
		if (pPDD->dwSize == sizeof (PDDCONTEXT))
		{
			// Stop the read thread if its running.
			StopReadThread (pPDD);

			DeleteCriticalSection (&pPDD->csStill);

			if (pPDD->hStillEvent)
				CloseHandle (pPDD->hStillEvent);

			// Free the stream descriptors
			if (pPDD->usbstrmIF) 
			{
				for (i = 0; i < pPDD->nStreamInterfaces; i++)
					if (pPDD->usbstrmIF[i].lpepExtDesc)
						LocalFree (pPDD->usbstrmIF[i].lpepExtDesc);
				LocalFree (pPDD->usbstrmIF);
			}

			// Free any control interface extended descriptor
			if (pPDD->usbctlIF.lpepExtDesc)
				LocalFree (pPDD->usbctlIF.lpepExtDesc);

			if (pPDD->hReadThread)
				CloseHandle (pPDD->hReadThread);

			if (pPDD->strStreamDefault.dwReserved)
				CloseHandle ((HANDLE)pPDD->strStreamDefault.dwReserved);

			for (DWORD i = 0; i < pPDD->strStreamDefault.dwNumBuffs; i++)
				if (pPDD->strStreamDefault.pFrame[i].pBuff)
					LocalFree (pPDD->strStreamDefault.pFrame[i].pBuff);

			DeleteCriticalSection (&pPDD->strStreamDefault.csBuffSync);

			if (pPDD->hVendorEvent)
				CloseHandle (pPDD->hVendorEvent);

			if (pPDD->hNewFrameEvent)
				CloseHandle (pPDD->hNewFrameEvent);
		}
		LocalFree (pPDD);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		DEBUGMSG(ZONE_ERROR, (DTAG TEXT("Exception freeing PDD context structure\n"))); 
	}
	return;
}
//-----------------------------------------------------------------------
// Pdd_DeviceDetach - Called by MDD during unload notification but
// before the stream driver is deactivated.
// 
int pdd_DeviceDetach (PDRVCONTEXT pDrv)
{
	// Set the stream interface to turn off the video stream.
	int rc = SetStreamInterface (pDrv, 1, 0);

	// Free up PDD context structure
	FreePDDContext ((PPDDCONTEXT)pDrv->dwPddContext);
	return 0;
}

//---------------------------------------------------------------------------------------
//
//
int pdd_DrvInit (PDRVCONTEXT pDrv) 
{
	return 1;
}

//---------------------------------------------------------------------------------------
//
//
void pdd_DrvUninit (PDRVCONTEXT pDrv) 
{
	return;
}

//---------------------------------------------------------------------------------------
// pdd_DrvOpen - Called when driver is opened
//
int pdd_DrvOpen (PDRVCONTEXT pDrv) 
{
	BOOL fPwr;

	// Make sure the camera is powered
	GetPower (pDrv, &fPwr);

	if (!fPwr)
		SetPower (pDrv, TRUE);
	return 1;
}

//---------------------------------------------------------------------------------------
// pdd_DrvClose - Called when driver is closed
//
void pdd_DrvClose (PDRVCONTEXT pDrv) 
{
	int rc;

	rc = pdd_StopVidStream (pDrv);

	// Turn off the camera
	SetPower (pDrv, FALSE);
	return;
}

//---------------------------------------------------------------------------------------
// Return a list of Feature IDs that is supported by this camera
//
int pdd_GetFeatureList (PDRVCONTEXT pDrv, DWORD *pList, int nListSize)
{
	int nCnt = 0; 
	int i;

	// Count the number of supported features
	for (i = 0; i < dim (CamFeatures); i++)
	{
//		if (CamFeatures[i].bFeatureID != FEAT_UNSUPPORTED)
		if (CamFeatures[i].bUnit != FEAT_UNSUPPORTED)
			nCnt++;
	}
	for (i = 0; i < dim (ProcFeatures); i++)
	{
//		if (ProcFeatures[i].bFeatureID != FEAT_UNSUPPORTED)
		if (ProcFeatures[i].bUnit != FEAT_UNSUPPORTED)
			nCnt++;
	}

	// See if they're just asking for the number of features
	if (pList == 0)
		return nCnt;

	if (nListSize < nCnt)
		return 0;

	// Now compile the list of supported IDs
	for (i = 0; i < dim (CamFeatures); i++)
	{
//		if (CamFeatures[i].bFeatureID != FEAT_UNSUPPORTED)
		if (CamFeatures[i].bUnit != FEAT_UNSUPPORTED)
			*pList++ = CamFeatures[i].bFeatureID;
	}
	for (i = 0; i < dim (ProcFeatures); i++)
	{
//		if (ProcFeatures[i].bFeatureID != FEAT_UNSUPPORTED)
		if (ProcFeatures[i].bUnit != FEAT_UNSUPPORTED)
			*pList++ = ProcFeatures[i].bFeatureID;
	}
	return nCnt;
}
//-----------------------------------------------------------------------
// pdd_QueryFeature - Gets the min and max values value of a feature 
// 
int pdd_QueryFeature (PDRVCONTEXT pDrv, DWORD dwFeatureID, PFEATUREPROP pfp) 
{
	int rc;

	DEBUGMSG (ZONE_FUNC | ZONE_FEATURE, 
	          (DTAG TEXT("pdd_QueryFeature++  ID: %d\r\n"), dwFeatureID));

	// Look up the feature data from the ID
	PFEATURESTRUCT pfs = FindFeatureInfo ((BYTE)dwFeatureID);
	if (!pfs)
		return ERROR_NOT_SUPPORTED;

	pfp->dwFeatureID = dwFeatureID;
	pfp->dwFlags = 0;
	pfp->nMin = 0;
	pfp->nMax = 0;

	// Get min value
	rc = DoVendorTransfer (pDrv, USBVID_GET_MIN, pfs->bCmdID, pfs->bInterface, 
	                       pfs->bUnit, (PBYTE)&pfp->nMin, pfs->wLength);
	if (rc)
		DEBUGMSG (ZONE_ERROR | ZONE_FEATURE,
		          (DTAG TEXT("Error getting min value for feat %d. rc %d\r\n"),
				  dwFeatureID, rc));
	// Get max value
	rc = DoVendorTransfer (pDrv, USBVID_GET_MAX, pfs->bCmdID, pfs->bInterface, 
	                       pfs->bUnit, (PBYTE)&pfp->nMax, pfs->wLength);
	if (rc)
		DEBUGMSG (ZONE_ERROR | ZONE_FEATURE,
		          (DTAG TEXT("Error getting max value for feat %d. rc %d\r\n"),
				  dwFeatureID, rc));

	DEBUGMSG (ZONE_FUNC | ZONE_FEATURE, 
	          (DTAG TEXT("pdd_QueryFeature--  rc %d  min: %xh max %xh\r\n"), 
	                      rc, pfp->nMin, pfp->nMax));
	return rc;
}
//---------------------------------------------------------------------------------------
// pdd_GetParameter - Gets the current value of a camera parameter
//
int pdd_GetParameter (PDRVCONTEXT pDrv, DWORD dwFeatureID, PDWORD pData)
{
	int rc;

	DEBUGMSG (ZONE_FUNC | ZONE_FEATURE, 
	          (DTAG TEXT("pdd_GetParameter++  ID: %d\r\n"), dwFeatureID));

	// Look up the feature data from the ID
	PFEATURESTRUCT pfs = FindFeatureInfo ((BYTE)dwFeatureID);
	if (pfs)
	{
		*pData = 0;
		// Send packet to the device to get the parameter
		rc = DoVendorTransfer (pDrv, USBVID_GET_CUR, pfs->bCmdID, pfs->bInterface, 
							   pfs->bUnit, (PBYTE)pData, pfs->wLength);
	} 
	else
		rc = ERROR_NOT_SUPPORTED;

	DEBUGMSG (ZONE_FUNC | ZONE_FEATURE, 
	          (DTAG TEXT("pdd_GetParameter--  rc %d  val: %xh\r\n"), rc, *pData));
	return rc;
}
//---------------------------------------------------------------------------------------
// pdd_SetParameter - Sets a camera parameter
//
int pdd_SetParameter (PDRVCONTEXT pDrv, DWORD dwFeatureID, DWORD dwVal) 
{
	int rc;
	DEBUGMSG (ZONE_FUNC | ZONE_FEATURE, 
	          (DTAG TEXT("pdd_SetParameter++  ID: %d Val: %xh\r\n"), dwFeatureID, dwVal));

	// Look up the feature data from the ID
	PFEATURESTRUCT pfs = FindFeatureInfo ((BYTE)dwFeatureID);
	if (pfs)
	{
		// Send packet to the device to set the parameter
		rc = DoVendorTransfer (pDrv, USBVID_SET_CUR, pfs->bCmdID, pfs->bInterface, 
							   pfs->bUnit, (PBYTE)&dwVal, pfs->wLength);
	}
	else
		rc = ERROR_NOT_SUPPORTED;

	DEBUGMSG (ZONE_FUNC | ZONE_FEATURE, 
	          (DTAG TEXT("pdd_SetParameter--  rc %d\r\n"), rc));
	return rc;
}
//-----------------------------------------------------------------------
// GetFormatParameters - This routine parses the video class control 
// interface descriptor to find information on the requested format
// 
int pdd_GetFormatParameters (PDRVCONTEXT pDrv, BYTE bFormatIndex, BYTE bFrameIndex, 
							 BOOL fStill, PFORMATPROPS pProps) 
{
	int rc;
	BYTE bLastSubType = USB_VIDEO_VS_UNDEFINED;
	BOOL fFound = FALSE;

	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("pdd_GetFormatParameters++ Fmt:%d  Frm:%d\r\n"), bFormatIndex, bFrameIndex));

	// Get our pdd specific contextlpex
	PPDDCONTEXT pPDD = (PPDDCONTEXT)pDrv->dwPddContext; 

	// Find the Frame descriptors
	PUSBVIDSTREAMIFDESCRIPTOR pHdr = (PUSBVIDSTREAMIFDESCRIPTOR)pPDD->usbstrmIF->lpepExtDesc;
	__try {
		// Sanity check on header IDs
		if (!pHdr || (pHdr->bType != 0x24) || (pHdr->bSubtype != 1)) 
		{
			DEBUGMSG (ZONE_ERROR, (DTAG TEXT("Bad Extended Stream Descriptor\r\n")));
			return -1;
		}
		PBYTE pData = (PBYTE)pPDD->usbstrmIF->lpepExtDesc;
		PBYTE pEnd = (PBYTE)pPDD->usbstrmIF->lpepExtDesc + pHdr->wTotalLen;
		PUSBVIDSTDDESCHDR pStd = (PUSBVIDSTDDESCHDR)pHdr;
		// Loop through all the descriptors
		while (pData + pStd->bLen < pEnd)
		{
			pData += pStd->bLen;
			pStd = (PUSBVIDSTDDESCHDR)pData;

			if (pStd->bType != USB_VIDEO_CS_INTERFACE)
			{
				DEBUGMSG (1, (TEXT("Unexpected header type %xh\r\n"), pStd->bType));
				break;
			}
			// Are we looking for still formats or stream?
			if (!fStill)
			{
				switch (pStd->bSubtype) 
				{
				//TODO:: Need to support other formats
				case USB_VIDEO_VS_FORMAT_MJPEG:
					rc = ProcessFrameFormats (pPDD, pStd, VIDFORMAT_MJPEG, 
					                          bFormatIndex, bFrameIndex, pProps, &fFound);
					break;
				case USB_VIDEO_VS_FORMAT_UNCOMPRESSED:
					rc = ProcessFrameFormats (pPDD, pStd, VIDFORMAT_UNCOMPRESSED, 
					                          bFormatIndex, bFrameIndex, pProps, &fFound);
					break;
				case USB_VIDEO_VS_FORMAT_FRAME_BASED:
					rc = ProcessFrameFormats (pPDD, pStd, USB_VIDEO_VS_FRAME_FRAME_BASED, 
					                          bFormatIndex, bFrameIndex, pProps, &fFound);
					break;
				case USB_VIDEO_VS_FORMAT_MPEG2TS:
					break;
				case USB_VIDEO_VS_FORMAT_DV:
					break;
				default:
					break;
				}
				if (fFound)
					return rc;
			}
			else
			{
				if (pStd->bSubtype == USB_VIDEO_VS_STILL_IMAGE_FRAME)
				{
					PUSBVIDSTREAMIF_STILLIMGDESCRIPTOR pFrmStill = (PUSBVIDSTREAMIF_STILLIMGDESCRIPTOR)pStd;
					// See if index is within range
					if (bFrameIndex < pFrmStill->bNumImageSizePatterns)
					{
						memset (pProps, 0, sizeof (FORMATPROPS));
						pProps->cbSize = sizeof (FORMATPROPS);
						pProps->wFormatType = bLastSubType;
						pProps->wFormatIndex = bFormatIndex;
						pProps->wFrameIndex = bFrameIndex;
						pProps->dwHeight = pFrmStill->sStillFmt[bFrameIndex].wHeight;
						pProps->dwWidth = pFrmStill->sStillFmt[bFrameIndex].wWidth;
						//Maxbuff below assumes worst case 32bpp
						pProps->dwMaxBuff = pProps->dwHeight * pProps->dwWidth * 4; 
						return 0;
					}
					return -4;
				}
			}
			// Save the last format type
			bLastSubType = pStd->bSubtype;
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		DEBUGMSG (1, (TEXT("Exception scanning extended stream descriptor\r\n")));
		return -2;
	}

	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("pdd_GetFormatParameters-- found:%d\r\n"), fFound));
	return -3;
}	

//-----------------------------------------------------------------------
// pdd_GetCurrentFormat - Returns the format of the current stream
// but only if the stream is not internal. 
// 
int pdd_GetCurrentFormat (PDRVCONTEXT pDrv, PFORMATPROPS pProps) 
{
	int rc = ERROR_VC_DISCONNECTED;

	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("pdd_GetCurrentFormat++\r\n")));

	// Get our pdd specific context
	PPDDCONTEXT pPDD = (PPDDCONTEXT)pDrv->dwPddContext; 

	// See if stream running
	if (pPDD->wReadThreadState != STREAMTHD_STOPPED)
	{
		// Make sure its not an internal stream for still capture
		if ((DWORD)pPDD->pstrStream != (DWORD)&pPDD->strStreamDefault)
		{
			rc = pdd_GetFormatParameters (pDrv, (BYTE)pPDD->wCurrFormatIndex,
										  (BYTE)pPDD->wCurrFrameIndex, FALSE, 
										  pProps);
			// Save the stream rate
			pProps->nNumInterval = 1;
			pProps->dwInterval[0] = pPDD->dwCurrValidInterval;
		}
	}
	if (rc == ERROR_VC_DISCONNECTED)
		DEBUGMSG (ZONE_ERROR, (DTAG TEXT("Error No stream active\r\n")));

	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("pdd_GetCurrentFormat-- rc %d\r\n"), rc));
	return rc;
}
//-----------------------------------------------------------------------
// pdd_StartVidStream - Set streaming video from camera
// 
int pdd_StartVidStream (PDRVCONTEXT pDrv, BYTE bFormatIndex, BYTE bFrameIndex, 
						PVIDSTREAMSTRUCT pstrStream, DWORD dwFrameInterval) 
{
	int i, rc = 0;
	BYTE bInterface = VID_IF_STREAM;
	BYTE bUnit = 0;
	DWORD dwValidInterval;
	int nIntIndex;

	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("pdd_StartVidStream++  Fmt %d\r\n"), bFrameIndex));

	// Get our pdd specific context
	PPDDCONTEXT pPDD = (PPDDCONTEXT)pDrv->dwPddContext; 

	//
	// If we're starting just for a still capture, if any streaming is going
	// on, we'll take it and return.
	//
	if ((bFormatIndex == 0xff) && (bFrameIndex == 0xff))
	{
		if (pPDD->pstrStream != 0)
		{
			DEBUGMSG (ZONE_STILL, (DTAG TEXT("StartVidStream already running.  Exiting\r\n")));
			return ERROR_STREAM_ALREADY_RUNNING;
		}
		bFormatIndex = 1;
		bFrameIndex = 1;
		dwFrameInterval = 0;
	}
	//
	// Query the parameters for the new format.  This validates format and frame values
	//
	FORMATPROPS Props;
	rc = pdd_GetFormatParameters (pDrv, bFormatIndex, bFrameIndex, FALSE, &Props);
	if (rc)
	{
		DEBUGMSG (ZONE_ERROR, (DTAG TEXT("Bad format or frame fmt:%d frame:%d rc %d\r\n"),
		          bFormatIndex, bFrameIndex, rc));
		return ERROR_INVALID_PARAMETER;
	}

	//
	// Validate the frame interval
	//

	// Discrete intervals?
	if (Props.nNumInterval != 0)
	{
		// See if min
		if (dwFrameInterval == -1)
			nIntIndex = Props.nNumInterval-1;

		// If 0, set to fastest interval
		else if (dwFrameInterval == 0)
			nIntIndex = 0;

		// Else look up to see if valid
		else
		{
			// Loop through the allowed descrete intervals
			for (nIntIndex = 0; nIntIndex < Props.nNumInterval; nIntIndex++)
			{
				if (dwFrameInterval == Props.dwInterval[nIntIndex])
					break;
			}
			if (nIntIndex == Props.nNumInterval)
				rc = ERROR_INVALID_PARAMETER;
		}
	}
	else
	{
		// See if min
		if (dwFrameInterval == -1)
			dwValidInterval = Props.dwInterval[0]; //Min value

		// If 0, set to fastest interval
		else if (dwFrameInterval == 0)
			dwValidInterval = Props.dwInterval[1]; //Max value

		// Else see if in proper range
		else
			if ((dwFrameInterval >= Props.dwInterval[0]) && (dwFrameInterval <= Props.dwInterval[1]))
				dwValidInterval = dwFrameInterval;
			else
				rc = ERROR_INVALID_PARAMETER;
	}
	if (rc)
	{
		DEBUGMSG (ZONE_ERROR, (DTAG TEXT ("Bad frame interval %d specified\r\n"), dwFrameInterval));
		return rc;
	}

	//
	// Do probe commit for video stream
	//
	// Discrete intervals?
	if (Props.nNumInterval != 0)
	{
		// Try all frame intervals for a given Format/Frame size setting
		for (i = nIntIndex; i < Props.nNumInterval; i++)
		{
			// See if any quality setting for these parameters can work
			rc = NegotiateQuality (pDrv, bFormatIndex, bFrameIndex, Props.dwInterval[i]);
			if (rc == 0)
				break;
		}
	}
	else
	{
		// Starting with specified interval increment until over max
		while (dwValidInterval <= Props.dwInterval[1])
		{
			// See if any quality setting for these parameters can work
			rc = NegotiateQuality (pDrv, bFormatIndex, bFrameIndex, dwValidInterval);
			if (rc == 0)
				break;
			// Increment by granularity
			dwValidInterval += Props.dwInterval[2]; // rate granularity
		}
	}
	if (rc)
		DEBUGMSG (ZONE_ERROR, (DTAG TEXT("Probe/Commit failed.  Not enough bandwidth for Fmt: %d  Frm: %d\r\n"), 
		          bFormatIndex, bFrameIndex));

	//
	// Allocate the streaming buffers if an internal stream just for a still
	//
	if (rc == 0)
	{
		// See if we need to set up a default stream or if we're changing the dest buffers
		if (pstrStream == 0) 
		{
			// Allocate buffers internally for the stream
			rc = AllocateInternalStreamBuffers (pPDD, Props.dwMaxBuff);
			if (rc == 0)
			{
				pPDD->pstrStream = (PVIDSTREAMSTRUCT)&pPDD->strStreamDefault;
			} 
		}
		// Else, we have a stream strcture passed to us.  Use it.
		else
		{
			pPDD->pstrStream = pstrStream;
		}
	}

	//
	// Start the read thread to get the data
	//
	if (rc == 0)
	{
		// Start of read thread if not already started
		if (pPDD->wReadThreadState == STREAMTHD_STOPPED)
		{
			// Create the thread
			pPDD->wReadThreadState = 1;
			pPDD->hReadThread = CreateThread (NULL, 0, ReadIsocThread, (PVOID)pDrv, 0, NULL);
			if (pPDD->hReadThread == 0)
			{
				DEBUGMSG (ZONE_ERROR, (DTAG TEXT("Error creating read thread  ExtErr: %d\r\n"), GetLastError()));
				pPDD->wReadThreadState = 0;
				rc = -1;
			} else	
				Sleep (10);
		}
	}
	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("StartVidStream--  rc %d\r\n"), rc));
	return rc;
}	
//---------------------------------------------------------------------------------------
// pdd_GetNextVideoFrame - Returns the latest valid frame in the frame buffer.
//
// Note: Entrypoint modified in 0.6 to change the way we request the type of frame.
//
// Note on frame structure:
//    pBuff == 0            -> Buffer 'held' by app
//    dwTicksAtCapture == 0 -> Buffer not 'valid' perhaps due to purge request by app
//
int pdd_GetNextVideoFrame (PDRVCONTEXT pDrv, PBYTE *ppFrameBuff, DWORD *pdwFrameBytes, 
						   DWORD *pdwValidFrames, PBYTE pFrameReturn, 
						   DWORD dwTimeout, DWORD dwFrameFlags)
{
	int rc = 0; 
	DWORD i;
	BOOL fLeaveCS = TRUE;
	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("pdd_GetNextVideoFrame++\r\n")));

	PPDDCONTEXT pPDD = (PPDDCONTEXT)pDrv->dwPddContext; 

	// Sync with read thread
	EnterCriticalSection (&pPDD->pstrStream->csBuffSync);
	__try 
	{
		//
		// See if there is a frame to return
		//
		if (pFrameReturn != 0)
		{
			// Trivial sanity check for returned buffer. Buffer addresses will always
			// be above the first entry in the buffer array.
			if ((DWORD)pFrameReturn < (DWORD)&pPDD->pstrStream->pFrame[0].pBuff)
			{
				rc = ERROR_INVALID_PARAMETER;
				DEBUGMSG (ZONE_ERROR, (DTAG TEXT("Returned buffer pointer %08x not valid \r\n"), 
				          pFrameReturn));
				__leave;
			}
			// Find a free slot in the array
			for (i = 0; i < pPDD->pstrStream->dwNumBuffs; i++)
			{
				// If slot free, add the buffer back.  Correct for packet header offset
				if (pPDD->pstrStream->pFrame[i].pBuff == 0)
				{
					pPDD->pstrStream->pFrame[i].pBuff = pFrameReturn - sizeof (USBVIDPAYLOADHDR);
					pPDD->pstrStream->pFrame[i].dwTicksAtCapture = 0;
				}
			}
		}
		//
		// See if they want a new frame
		//
		if (ppFrameBuff != 0)
		{
			int nCnt = 0;
			int nFrameToGrab = -1;
			int nOldest = -1, nNewest = 0x7fffffff;
			DWORD dwOldestTicks = 0, dwNewestTicks = 0, dwCurTicks = GetTickCount();

			// See if we have enough buffers remaining for this task. We
			// need to leave at least 2 buffers in the list
			for (i = 0; (i < pPDD->pstrStream->dwNumBuffs); i++)
			{
				// If slot used, count it.
				if (pPDD->pstrStream->pFrame[i].pBuff != 0) 
				{
					nCnt++;
				    if (pPDD->pstrStream->pFrame[i].dwTicksAtCapture != 0)
					{
						// Find oldest valid frame.
						if (dwCurTicks - pPDD->pstrStream->pFrame[i].dwTicksAtCapture > dwOldestTicks)
						{
							dwOldestTicks = dwCurTicks - pPDD->pstrStream->pFrame[i].dwTicksAtCapture;
							nOldest = i;
						}
						if (dwCurTicks - pPDD->pstrStream->pFrame[i].dwTicksAtCapture < dwNewestTicks)
						{
							dwNewestTicks = dwCurTicks - pPDD->pstrStream->pFrame[i].dwTicksAtCapture;
							nNewest = i;
						}
					}
				}
			}
			if (nCnt < 2)
			{
				rc = ERROR_NOT_ENOUGH_MEMORY;
				DEBUGMSG (ZONE_ERROR, (DTAG TEXT("Not enough free buffers to return one\r\n")));
				__leave;
			}
			// See if they want the oldest, newest avail, or they want to wait...
			if (dwFrameFlags & FRAME_OLDEST) 
			{
				// If there is an oldest frame
				if (nOldest != -1)
				{
					nFrameToGrab = nOldest;
				}
			} 
			else if (dwFrameFlags & FRAME_NEWESTAVAIL)
			{
				// If there is an newest frame
				if (nNewest != -1)
				{
					nFrameToGrab = nNewest;
				}
			}			
			// See if we need to wait for a frame either because they want to wait 
			// on the next frame or because there isn't an oldest frame.
			if (nFrameToGrab == -1)
			{
				// We need to leave the CS to allow the isoc thread to complete
				// a frame.
				LeaveCriticalSection (&pPDD->pstrStream->csBuffSync);
				fLeaveCS = FALSE;

				// Wait for next frame event
				rc = WaitForSingleObject ((HANDLE)pPDD->pstrStream->dwReserved, dwTimeout);
				if (rc == WAIT_OBJECT_0)
				{
					// Retake the CS so we can access the struct again
					EnterCriticalSection (&pPDD->pstrStream->csBuffSync);

					nFrameToGrab = pPDD->pstrStream->dwLastGoodBuff;
					fLeaveCS = TRUE;
				} 
				else
				{
					DEBUGMSG (ZONE_ERROR, (DTAG TEXT("Next Frame event not signaled rc %d\r\n"), rc));
					__leave;
				}
			}

			// Get lastest buffer, and its size. Skip past the frame header
			*ppFrameBuff = pPDD->pstrStream->pFrame[nFrameToGrab].pBuff + sizeof (USBVIDPAYLOADHDR);
			*pdwFrameBytes = pPDD->pstrStream->pFrame[nFrameToGrab].dwSize;

			// Take it out of the list
			pPDD->pstrStream->pFrame[nFrameToGrab].pBuff = 0;

			// Tell the caller how many frames have gone by since last call
			if (pdwValidFrames)
				*pdwValidFrames = pPDD->pstrStream->dwValidFrames;
			pPDD->pstrStream->dwValidFrames = 0;

			// See if we need to purge the other frames.
			if (dwFrameFlags & FRAME_PURGEOLD)
			{
				for (i = 0; (i < pPDD->pstrStream->dwNumBuffs); i++)
					pPDD->pstrStream->pFrame[i].dwTicksAtCapture = 0;
			}
		}
	}
	__finally 
	{
		if (fLeaveCS)
			LeaveCriticalSection (&pPDD->pstrStream->csBuffSync);
	}

	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("pdd_GetNextVideoFrame--  rc %d\r\n"), rc));
	return rc;
}
//-----------------------------------------------------------------------
// pdd_StopVidStream - Set streaming video from camera
// 
int pdd_StopVidStream (PDRVCONTEXT pDrv) 
{
	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("pdd_StopVidStream++\r\n")));

	// Get our pdd specific context
	PPDDCONTEXT pPDD = (PPDDCONTEXT)pDrv->dwPddContext; 

	StopReadThread (pPDD);
	pPDD->wCurrFormatIndex = 0xffff;
	pPDD->wCurrFrameIndex = 0xffff;

	// Change to null interface
	SetStreamInterface (pDrv, 1, 0);  

	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("pdd_StopVidStream--\r\n")));
	return 0;
}
//---------------------------------------------------------------------------------------
// pdd_GetStillImage - Captures a single frame of video.  
//
int pdd_GetStillImage (PDRVCONTEXT pDrv, int nFrameIndex, int nFormatIndex, PBYTE pOut, 
					   DWORD dwOut, PDWORD pdwBytesWritten)
{
	PPDDCONTEXT pPDD = (PPDDCONTEXT)pDrv->dwPddContext; 
	int rc = 0;

	// If we can't enter the still cap CS, we're already capturing an image, so fail
	if (!TryEnterCriticalSection (&pPDD->csStill))
	{
		return ERROR_BUSY;
	}
	__try
	{
		// See if stream running, or if simply an internal stream
		if ((pPDD->wReadThreadState == STREAMTHD_STOPPED) ||
			((DWORD)pPDD->pstrStream != (DWORD)&pPDD->strStreamDefault))
		{
			// If current stream doesn't match, change it.
			if ((nFormatIndex != pPDD->wCurrFormatIndex) || (nFrameIndex != pPDD->wCurrFrameIndex))
			{
				// According to the Vid Class spec, the video stream should be running for a still capture
				rc = pdd_StartVidStream (pDrv, nFormatIndex, nFrameIndex, NULL, -1); 
				DEBUGMSG( 1, (TEXT("StartVidStream returned %d.\n"), rc)); 
				Sleep (500); // Let the camera settle a bit
			}
		}
		if (rc)
			__leave;

		PBYTE pFrameBuff;
		DWORD dwSize;
		// Get the next frame
		rc = pdd_GetNextVideoFrame (pDrv, &pFrameBuff, &dwSize, 0, 0, 2000, 0);
		if (rc == 0)
		{
			__try
			{
				// Copy the frame to the output buffer
				memcpy (pOut, pFrameBuff, dwSize);
				*pdwBytesWritten = dwSize;
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				DEBUGMSG (ZONE_ERROR, (TEXT("Exception writing to output buffer\r\n")));
				rc = ERROR_INVALID_PARAMETER;
			}
			// Return the frame buffer pointer
			pdd_GetNextVideoFrame (pDrv, 0, &dwSize, 0, pFrameBuff, 0, 0);
		}
	}
	__finally
	{
		// Leave the critical section
		LeaveCriticalSection (&pPDD->csStill);
	}
	return rc;
}
//-----------------------------------------------------------------------
// pdd_WaitOnCameraEvent - Set streaming video from camera
// 
int pdd_WaitOnCameraEvent (PDRVCONTEXT pDrv, DWORD dwEventMask, DWORD dwTimeout, 
						   DWORD *pdwSignaledEvent)
{
	HANDLE hHandles[32];
	DWORD dwWaitFlags[32];  
	int rc, nCnt;

	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("pdd_WaitOnCameraEvent++\r\n")));

	// Get our pdd specific context
	PPDDCONTEXT pPDD = (PPDDCONTEXT)pDrv->dwPddContext; 

	// Add handles to the array
	nCnt = 0;
	if (dwEventMask & WAITCAM_NEWFRAME) 
	{
		hHandles[nCnt] = pPDD->hNewFrameEvent;
		dwWaitFlags[nCnt] = WAITCAM_NEWFRAME;
		nCnt++;
	}
	// Add new events here...

	// Now wait if at least one flag was set.
	*pdwSignaledEvent = 0;
	if (nCnt) 
	{
		// Time to wait
		rc = WaitForMultipleObjects (nCnt, hHandles, FALSE, dwTimeout);

		// Set which event was signaled.
		if ((rc >= WAIT_OBJECT_0) && (rc < WAIT_OBJECT_0 + nCnt))
		{
			*pdwSignaledEvent |= dwWaitFlags[rc - WAIT_OBJECT_0];
		}
		else if (rc == WAIT_TIMEOUT)
		{
			*pdwSignaledEvent = WAITCAM_WAITTIMEOUT;
		}
	}
	else
	{
		DEBUGMSG (ZONE_ERROR, (TEXT("No wait flags set.\r\n")));
		rc = ERROR_INVALID_PARAMETER;
	}
	
	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("pdd_WaitOnCameraEvent-- rc = %x\r\n"), rc));
	return 0;
}

//-----------------------------------------------------------------------
// GetPower - Gets the camera power 
// 
int GetPower (PDRVCONTEXT pDrv, BOOL *pbVal)
{
	int rc = 0;
	BYTE bInterface = VID_IF_CTL;
	BYTE bUnit = 0;
	BYTE bVal;

	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("GetPower++ \r\n")));

	// Get our pdd specific context
	PPDDCONTEXT pPDD = (PPDDCONTEXT)pDrv->dwPddContext; 
	bVal = 0;

	rc = DoVendorTransfer (pDrv, USBVID_GET_CUR, USB_VIDEO_VC_CS_VIDEO_POWER_MODE_CONTROL,
	                       bInterface, bUnit, &bVal, 1);

	if (rc)
		DEBUGMSG (ZONE_ERROR, (DTAG TEXT("Error reading power setting from camera rc %d\r\n"), rc));
	if (bVal & 1)
		*pbVal = TRUE;
	else
		*pbVal = FALSE;

	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("GetPower-- rc %d error code %d\r\n"), rc, bVal));
	return rc;
}

//-----------------------------------------------------------------------
// SetPower - Sets the camera power 
// 
int SetPower (PDRVCONTEXT pDrv, BOOL fOn)
{
	int rc = 0;
	BYTE buff[64];
	BYTE bInterface = VID_IF_CTL;
	BYTE bUnit = 0;

	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("SetPower++ \r\n")));

	// Get our pdd specific context
	PPDDCONTEXT pPDD = (PPDDCONTEXT)pDrv->dwPddContext; 

	if (fOn)
		buff[0] = 0x01;
	else
		buff[0] = 0x00;

	rc = DoVendorTransfer (pDrv, USBVID_SET_CUR, USB_VIDEO_VC_CS_VIDEO_POWER_MODE_CONTROL,
	                       bInterface, bUnit, (PBYTE)buff, 1);

	if (rc)
		DEBUGMSG (ZONE_ERROR, (DTAG TEXT("Error set camera power rc %d\r\n"), rc));

	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("SetPower-- rc %d error code %d\r\n"), rc, buff[0]));
	return rc;
}
//-----------------------------------------------------------------------
// DoVendorTransfer - Called to communicate with the camera.  Since I've
// noticed that the camera sometimes overruns the output buffer, this 
// function pipes the data into a fairly big buffer and then copies the
// data to the exact size buffer passed.
// 
int DoVendorTransfer (PDRVCONTEXT pDrv, BYTE bRequest, BYTE bCmd, BYTE bInterface, 
					  BYTE bUnit, PBYTE pVal, WORD wLen)
{
	USB_DEVICE_REQUEST req;
	DWORD dwBytes;
	DWORD dw, dwErr;
	BOOL fSet;
	static BYTE bTBuff[512];

	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("DoVendorTransfer++\r\n")));

	if (bRequest == USBVID_SET_CUR)
		fSet = TRUE;
	else
		fSet = FALSE;

	// Get our pdd specific context
	PPDDCONTEXT pPDD = (PPDDCONTEXT)pDrv->dwPddContext; 

	//
	// Set transfer flags depending on read or write
	//
	if (fSet)
		req.bmRequestType = USB_REQUEST_HOST_TO_DEVICE | USB_REQUEST_CLASS | USB_REQUEST_FOR_INTERFACE;
	else
		req.bmRequestType = USB_REQUEST_DEVICE_TO_HOST | USB_REQUEST_CLASS | USB_REQUEST_FOR_INTERFACE;

	req.bRequest = bRequest;
	req.wValue   = MAKEWORD (0, bCmd);
	req.wIndex   = MAKEWORD (bInterface, bUnit);
	req.wLength  = wLen;

	dwBytes = 0;
	dwErr = USB_NO_ERROR;

	dw = IssueVendorTransfer (pDrv->lpUsbFuncs, pDrv->hDevice, 
							DefaultTransferComplete, pPDD->hVendorEvent,
							(fSet ? USB_OUT_TRANSFER : USB_IN_TRANSFER) | USB_SHORT_TRANSFER_OK,
							&req, fSet && (req.wLength < sizeof (bTBuff)) ? pVal : bTBuff, 
							NULL, &dwBytes, 2000, &dwErr);
	if (dw)
		DEBUGMSG (ZONE_ERROR, 
		          (DTAG TEXT("Error calling IssueVendorTransfer rc: %d  ExtErr: %d\r\n"), 
				  dw, GetLastError()));

	// If no data transferred, stuff in an error
	if (!dw && (dwBytes != wLen))
	{
		DEBUGMSG (ZONE_ERROR, 
		          (DTAG TEXT("IssueVendorTransfer returned different length than requested len: %d  expected: %d\r\n"), 
				  dwBytes, wLen));
		dw = ERROR_BAD_LENGTH;
	}
	// Copy data to output buffer
	if (!dw && !fSet && (dwBytes <= req.wLength))
		memcpy (pVal, bTBuff, dwBytes);

	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("DoVendorTransfer-- rc %d\r\n"), dw));
	return dw;
}
//-----------------------------------------------------------------------
// GetVendorTransferLen - Called to query the data length request 
// for a particular command.
// 
int GetVendorTransferLen (PDRVCONTEXT pDrv, BYTE bCmd, BYTE bInterface, 
					      BYTE bUnit, WORD *pwLen)
{
	USB_DEVICE_REQUEST req;
	DWORD dwBytes;
	DWORD dw, dwErr;
	static BYTE bTBuff[512];

	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("GetVendorTransferLen++\r\n")));

	// Get our pdd specific context
	PPDDCONTEXT pPDD = (PPDDCONTEXT)pDrv->dwPddContext; 

	req.bmRequestType = USB_REQUEST_DEVICE_TO_HOST | USB_REQUEST_CLASS | USB_REQUEST_FOR_INTERFACE;
	req.bRequest = USBVID_GET_LEN;
	req.wValue   = MAKEWORD (0, bCmd);
	req.wIndex   = MAKEWORD (bInterface, bUnit);
	req.wLength  = 2;

	dwBytes = 0;
	dwErr = USB_NO_ERROR;

	dw = IssueVendorTransfer (pDrv->lpUsbFuncs, pDrv->hDevice, 
							DefaultTransferComplete, pPDD->hVendorEvent,
							USB_IN_TRANSFER, &req, bTBuff, 
							NULL, &dwBytes, 2000, &dwErr);
	if ((dw == 0) && (dwBytes == 2))
	{
		*pwLen = *(WORD *)bTBuff;
	}
	else
	{
		DEBUGMSG (ZONE_ERROR, 
		          (DTAG TEXT("Error calling IssueVendorTransfer rc: %d  ExtErr: %d\r\n"), 
				  dw, GetLastError()));
	}
	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("GetVendorTransferLen-- len %d rc %d\r\n"), *pwLen, dw));
	return dw;
}
//-----------------------------------------------------------------------
// FindFeatureInfo - Look up feature in the table to get parameter 
// information.
// 
PFEATURESTRUCT FindFeatureInfo (BYTE bFeatureID)
{
	int i;
	for (i = 0; i < dim (CamFeatures); i++)
	{
		if (bFeatureID == CamFeatures[i].bFeatureID)
			return &CamFeatures[i];
	}
	for (i = 0; i < dim (ProcFeatures); i++)
	{
		if (bFeatureID == ProcFeatures[i].bFeatureID)
			return &ProcFeatures[i];
	}
	return 0;
}

//-----------------------------------------------------------------------
// EnableSupportedFeatures - Given a bit array from the interface
// descriptor, fill the feature table with the necessary data.
// 
int EnableSupportedFeatures (PBYTE pCtlBytes, int nCtlBytes, PFEATURESTRUCT pFeatArray, 
							 int nFeatArraySize, BYTE bUnit) 
{
	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("EnableSupportedFeatures++\r\n")));

	BYTE bm;
	BYTE index;

	// Loop through, Check the bit for each feature
	for (index = 0; index < nFeatArraySize; index++)
	{
		// Do we need another byte?
		if ((index % 8) == 0)
		{
			// if no more bytes, leave
			if (nCtlBytes == 0) 
				break;

			bm = *pCtlBytes++;
			nCtlBytes--;
		}
		if (bm & 0x01)
			pFeatArray[index].bUnit = bUnit;
		else
			pFeatArray[index].bUnit = FEAT_UNSUPPORTED;

		bm = bm >> 1;
	}

	// BUGFIX:  Reported by Himangshu Roy
	// Delete any features at the end of the list
	for (; index < nFeatArraySize; index++)
		pFeatArray[index].bUnit = FEAT_UNSUPPORTED;

	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("EnableSupportedFeatures--\r\n")));
	return 0;
}
//-----------------------------------------------------------------------
// ParseFeatureParameters - Parses the device interface descriptor to 
// see what features this camera supports.
//
// CDB 6-4-2006: Removed all references to "pData + #". Added missing descriptors
//               and changed all size checks to use descriptor sizes. These 
//				 changes allow this function work for ARM and MIPSIV compilers.
// 
int ParseFeatureParameters (PPDDCONTEXT pPDD) 
{
	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("ParseFeatureParameters++\r\n")));

	// Find the Control descriptors
	PUSBVIDCTLIFDESCRIPTOR pHdr = (PUSBVIDCTLIFDESCRIPTOR)pPDD->usbctlIF.lpepExtDesc;
	__try {
		// Sanity check on header IDs
		if ((pHdr->bType != 0x24) || (pHdr->bSubtype != 1)) 
		{
			DEBUGMSG (ZONE_ERROR, (DTAG TEXT("Bad Extended Stream Descriptor\r\n")));
			return -1;
		}
		PBYTE pData = (PBYTE)pHdr;
		PBYTE pEnd = (PBYTE)pHdr + pHdr->wTotalLen;

		PUSBVIDSTDDESCHDR pStd = (PUSBVIDSTDDESCHDR)pHdr;
		// Loop through all the descriptors
		while (pData + pStd->bLen < pEnd)
		{
			pData += pStd->bLen;
			pStd = (PUSBVIDSTDDESCHDR)pData;

			if (pStd->bType != USB_VIDEO_CS_INTERFACE)
			{
				DEBUGMSG (ZONE_ERROR, (TEXT("Unexpected header type %xh\r\n"), pStd->bType));
				break;
			}
			switch (pStd->bSubtype) 
			{
			case USB_VIDEO_VC_PROCESSING_UNIT:
				// Verify structure length
				if (pStd->bLen >= sizeof(USBVIDPUNITIFDESCRIPTOR))
				{
					PUSBVIDPUNITIFDESCRIPTOR pPUnit = (PUSBVIDPUNITIFDESCRIPTOR)pData;

					BYTE bCtlSize = pPUnit->bControlSize;
					PBYTE pCtls = pPUnit->bmControls;
					BYTE bUnit = pPUnit->bUnitID;

					EnableSupportedFeatures (pCtls, bCtlSize, ProcFeatures, 
					                         dim(ProcFeatures), bUnit);
				}
				break;
			case USB_VIDEO_VC_INPUT_TERMINAL:
				{
					WORD wType = pStd->wTermType;

					// Verify that it's the camera
					if ((wType == USB_VIDEO_ITT_CAMERA) ||
						(wType == USB_VIDEO_TT_STREAMING))
					{
						// Verify structure length
						if (pStd->bLen >= sizeof(USBVIDCAMINPUTTERMIFDESCRIPTOR))
						{
							PUSBVIDCAMINPUTTERMIFDESCRIPTOR pCamInputTerm = 
							                         (PUSBVIDCAMINPUTTERMIFDESCRIPTOR)pData;

							BYTE bCtlSize = pCamInputTerm->bControlSize;
							PBYTE pCtls = pCamInputTerm->bmControls;
							BYTE bTerm = pCamInputTerm->bTerminalID;

							EnableSupportedFeatures (pCtls, bCtlSize, CamFeatures, 
							                         dim(CamFeatures), bTerm);
						}
					}
				}
				break;
			}
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		DEBUGMSG (1, (TEXT("Exception scanning extended control descriptor\r\n")));
		return -2;
	}
	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("ParseFeatureParameters--\r\n")));
	return 0;
}

//-----------------------------------------------------------------------
// ProcessFrameFormats - Helper function that examines the frame information
// for each format and returns information on the given frame.
// 
int ProcessFrameFormats (PPDDCONTEXT pPDD, PUSBVIDSTDDESCHDR pStd, BYTE bDescID, 
						 BYTE bFormatIndex, BYTE bFrameIndex, 
						 PFORMATPROPS pProps, BOOL *pfFound)
{
	PUSBVIDSTREAMIF_FORMATDESCRIPTOR pFmt = (PUSBVIDSTREAMIF_FORMATDESCRIPTOR)pStd;
	BYTE bCnt = pFmt->bNumFrameDescriptors;
	*pfFound = FALSE;

	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("ProcessFrameFormats++\r\n")));

	if (bFrameIndex > bCnt)
		return -1;

	// See if this is the format we want
	if (pFmt->bFormatIndex != bFormatIndex)
		return 0;

	// Get the pointer to the first frame descriptor
	PUSBVIDSTREAMIF_MJPEGFRAMEDESCRIPTOR pFrmMJPEG = (PUSBVIDSTREAMIF_MJPEGFRAMEDESCRIPTOR)( (PBYTE)pStd+pStd->bLen);

	if (pFrmMJPEG->bSubtype != bDescID)
	{
		DEBUGMSG (ZONE_ERROR, (DTAG TEXT("ERROR: Expected Frame descriptor to follow format descriptor\r\n")));
		return -2;
	}
	// Loop through Frame descriptors looking for matching index
	for (int i = 0; i < bCnt; i++)
	{
		// See if this is the descriptor we want
		if (pFrmMJPEG->bFrameIndex == bFrameIndex)
		{
			// Grab the info we want
			pProps->cbSize = sizeof (FORMATPROPS);
			pProps->wFormatType = bDescID;
			pProps->wFormatIndex = bFormatIndex;
			pProps->wFrameIndex = bFrameIndex;
			pProps->dwHeight = pFrmMJPEG->wHeight;
			pProps->dwWidth = pFrmMJPEG->wWidth;
			pProps->dwMaxBuff = pFrmMJPEG->dwMaxVideoFrameBufferSize;
			if (pFrmMJPEG->bFrameIntervalType)
			{
				pProps->nNumInterval = pFrmMJPEG->bFrameIntervalType;
				for (int k = 0; k < pFrmMJPEG->bFrameIntervalType; k++)
				{
					pProps->dwInterval[k] = pFrmMJPEG->Interval.dwDescrete[k];
				}
			} 
			else
			{
				// Cont interval
				pProps->nNumInterval = -1;
				pProps->dwInterval[0] = pFrmMJPEG->Interval.strCont.dwMinFrameInterval;
				pProps->dwInterval[1] = pFrmMJPEG->Interval.strCont.dwMaxFrameInterval;
				pProps->dwInterval[2] = pFrmMJPEG->Interval.strCont.dwFrameIntervalStep;
			}
			*pfFound = TRUE;
			DEBUGMSG (ZONE_FUNC, (DTAG TEXT("ProcessFrameFormats--\r\n")));
			return 0;
		}
		pFrmMJPEG = (PUSBVIDSTREAMIF_MJPEGFRAMEDESCRIPTOR)((PBYTE)pFrmMJPEG + pFrmMJPEG->bLen);
	}
	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("ProcessFrameFormats--\r\n")));
	return -3; // We didn't find the index requested
}


//-----------------------------------------------------------------------
// StopReadThread - Stops the isoch read thread
// 
void StopReadThread (PPDDCONTEXT pPDD) 
{
	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("StopReadThread++\r\n")));

	// We need to stop the read thread if its running...
	if (pPDD->wReadThreadState != STREAMTHD_STOPPED)
	{
		pPDD->wReadThreadState = STREAMTHD_STOPPED;
		int rc = WaitForSingleObject (pPDD->hReadThread, 2000);
		if (rc != WAIT_OBJECT_0)
		{
			DEBUGMSG (ZONE_ERROR, (DTAG TEXT("Can't stop read thread!\r\n")));
			TerminateThread (pPDD->hReadThread, -1);
		}
	}
	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("StopReadThread--\r\n")));
	return;
}

//-----------------------------------------------------------------------
// GetMinMaxQuality - Returns the min, max, and increment values for 
// the compression paramter for a given format, frame size, and, frame rate
// 
int GetMinMaxQuality (PDRVCONTEXT pDrv, BYTE bFormatIndex, BYTE bFrameIndex, DWORD dwFrameInterval,
		              WORD *pwCompMin, WORD *pwCompMax, WORD *pwCompInc) 
{
	BYTE buff[64];
	int rc = 0;
	BYTE bInterface = VID_IF_STREAM;
	BYTE bUnit = 0;
	WORD wProbeLen = sizeof (PSTREAM_PROBE_CONTROLSTRUCT);

	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("GetMinMaxQuality++\r\n")));

	// Get our pdd specific context
	PPDDCONTEXT pPDD = (PPDDCONTEXT)pDrv->dwPddContext; 

	memset (buff, 0, sizeof (buff));
	PSTREAM_PROBE_CONTROLSTRUCT p = (PSTREAM_PROBE_CONTROLSTRUCT)buff;

	// Get the length of this struct, since it changes between cameras
	rc = GetVendorTransferLen (pDrv, USB_VIDEO_VS_CS_PROBE_CTL, bInterface, bUnit, &wProbeLen);

	// Set current probe vals
	memset (p, 0, sizeof (0));
	p->bFormatIndex = bFormatIndex;
	p->bFrameIndex = bFrameIndex;
	p->dwFrameInterval = dwFrameInterval;
	rc = DoVendorTransfer (pDrv, USBVID_SET_CUR, USB_VIDEO_VS_CS_PROBE_CTL,
	                       bInterface, bUnit, (PBYTE)p, wProbeLen);

	if (rc)
		DEBUGMSG (ZONE_ERROR, (DTAG TEXT("Error setting original probe vals  rc %d\r\n"), rc));

	if (rc == 0)
	{
		// Get minimum quality value
		p->bFormatIndex = bFormatIndex;
		p->bFrameIndex = bFrameIndex;
		p->dwFrameInterval = dwFrameInterval;

		rc = DoVendorTransfer (pDrv, USBVID_GET_MIN, USB_VIDEO_VS_CS_PROBE_CTL,
		                       bInterface, bUnit, (PBYTE)p, wProbeLen);
		if (rc)
			DEBUGMSG (ZONE_ERROR, (DTAG TEXT("Error getting min value. rc %d\r\n"), rc));
		*pwCompMin = p->wCompQuality;
	}

	if (rc == 0)
	{
		// Get maximum quality value
		p->bFormatIndex = bFormatIndex;
		p->bFrameIndex = bFrameIndex;
		p->dwFrameInterval = dwFrameInterval;

		rc = DoVendorTransfer (pDrv, USBVID_GET_MAX, USB_VIDEO_VS_CS_PROBE_CTL,
		                       bInterface, bUnit, (PBYTE)p, wProbeLen);
		if (rc)
			DEBUGMSG (ZONE_ERROR, (DTAG TEXT("Error getting max value. rc %d\r\n"), rc));
		*pwCompMax = p->wCompQuality;
	}

	if (rc == 0)
	{
		// Get maximum quality value
		p->bFormatIndex = bFormatIndex;
		p->bFrameIndex = bFrameIndex;
		p->dwFrameInterval = dwFrameInterval;

		rc = DoVendorTransfer (pDrv, USBVID_GET_RES, USB_VIDEO_VS_CS_PROBE_CTL,
		                       bInterface, bUnit, (PBYTE)p, wProbeLen);
		if (rc)
			DEBUGMSG (ZONE_ERROR, (DTAG TEXT("Error getting inc value. rc %d\r\n"), rc));
		*pwCompInc = p->wCompQuality;
	}

	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("GetMinMaxQuality-- rc %d\r\n"), rc));
	return rc;
}

//-----------------------------------------------------------------------
// ProbeCommitVidStream - Negotiates the streaming parameters 
// 
int ProbeCommitVidStream (PDRVCONTEXT pDrv, BYTE bFormatIndex, BYTE bFrameIndex, 
		                  DWORD *pdwFrameInterval, WORD wCompression, PDWORD pdwMaxBandwidth) 
{
	int rc = 0;
	BYTE bInterface = VID_IF_STREAM;
	BYTE bUnit = 0;
	WORD wProbeLen = sizeof (PSTREAM_PROBE_CONTROLSTRUCT);

	DEBUGMSG (ZONE_FUNC | ZONE_PROBE, 
	          (DTAG TEXT("ProbeCommitVidStream++ Fmt: %d  Frm: %d  Interval %d  Compression %d\r\n"), 
	          bFormatIndex, bFrameIndex, *pdwFrameInterval, wCompression));

	// Get our pdd specific context
	PPDDCONTEXT pPDD = (PPDDCONTEXT)pDrv->dwPddContext; 

	// Get the length of this struct, since it changes between cameras
	rc = GetVendorTransferLen (pDrv, USB_VIDEO_VS_CS_PROBE_CTL, bInterface, bUnit, &wProbeLen);

	// Clear the buffer.  We use this because the USB stack (or camera) sometimes returns more
	// data than the size of the STREAM_PROBE_CONTROLSTRUCT structure.

	STREAM_PROBE_CONTROLSTRUCT pSPC;
	memset (&pSPC, 0, sizeof (STREAM_PROBE_CONTROLSTRUCT));

	// Set probe values
	pSPC.bFormatIndex = bFormatIndex;
	pSPC.bFrameIndex = bFrameIndex;
	pSPC.dwFrameInterval = *pdwFrameInterval;
	pSPC.wCompQuality = wCompression;
	pSPC.bmHint = USB_VIDEO_PROBE_HINT_FRAMEINTERVAL;

	rc = DoVendorTransfer (pDrv, USBVID_SET_CUR, USB_VIDEO_VS_CS_PROBE_CTL,
		                   bInterface, bUnit, (PBYTE)&pSPC, wProbeLen);
	if (rc)
		DEBUGMSG (ZONE_ERROR, (DTAG TEXT("Error setting probe values. rc %d\r\n"), rc));

	// Get back probe values
	if (rc == 0)
	{
		rc = DoVendorTransfer (pDrv, USBVID_GET_CUR, USB_VIDEO_VS_CS_PROBE_CTL,
		                       bInterface, bUnit, (PBYTE)&pSPC, wProbeLen);
		if (rc)
			DEBUGMSG (ZONE_ERROR, (DTAG TEXT("Error getting final probe values. rc %d\r\n"), rc));
	}

	// Set the commit values
	if (rc == 0)
	{
		rc = DoVendorTransfer (pDrv, USBVID_SET_CUR, USB_VIDEO_VS_CS_COMMIT_CTL,
		                       bInterface, bUnit, (PBYTE)&pSPC, wProbeLen);
		if (rc)
			DEBUGMSG (ZONE_ERROR, (DTAG TEXT("Error setting probe values. rc %d\r\n"), rc));
	}
	if (rc == 0)
	{
		// Save the max payload value, we need this for choosing the proper interface
		*pdwMaxBandwidth = (pSPC.dwMaxVideoFrameSize * 10000) / pSPC.dwFrameInterval;
	}
	DEBUGMSG (ZONE_FUNC | ZONE_PROBE, 
	          (DTAG TEXT("ProbeCommitVidStream-- Fmt: %d  Frm: %d  Compression %d Interval %d  Transfer size: %d Frame size %d BW %d\r\n"), 
	          pSPC.bFormatIndex, pSPC.bFrameIndex, pSPC.wCompQuality, pSPC.dwFrameInterval, pSPC.dwMaxPayloadTransferSize, pSPC.dwMaxVideoFrameSize, *pdwMaxBandwidth));
	
	return rc;
}

//-----------------------------------------------------------------------
// AllocateInternalStreamBuffers - If a still is requested and no
// stream is currently running, this routine allocates streaming buffers
// internally.
// 
int AllocateInternalStreamBuffers (PPDDCONTEXT pPDD, DWORD dwMaxBuff) 
{
	int i, rc = 0;

	//
	// See if the new format is too small for the current default
	//
	if (pPDD->strStreamDefault.dwBuffSize < dwMaxBuff)
	{
		// We/re changing buffers, stop the current read thread 
		StopReadThread (pPDD);

		// Free old buffers
		for (i = 0; i < NUMDEFBUFFS; i++)
		{
			if (pPDD->strStreamDefault.pFrame[i].pBuff) LocalFree (pPDD->strStreamDefault.pFrame[i].pBuff);
			pPDD->strStreamDefault.pFrame[i].pBuff = 0;
		}

		// Allocate new buffers
		for (i = 0; i < NUMDEFBUFFS; i++)
		{
			pPDD->strStreamDefault.pFrame[i].pBuff = (PBYTE)LocalAlloc (LPTR, dwMaxBuff);

			// Check to see if the alloc is successful
			if (pPDD->strStreamDefault.pFrame[i].pBuff == 0)
			{
				// If we've already allocated some buffs, we need to free the ones we did
				if (i) 
				{
					// Yes, I'm reusing 'i', but I'm bailing out of this loop
					for (i = 0; i < NUMDEFBUFFS; i++)
					{
						if (pPDD->strStreamDefault.pFrame[i].pBuff) LocalFree (pPDD->strStreamDefault.pFrame[i].pBuff);
						pPDD->strStreamDefault.pFrame[i].pBuff = 0;
					}
				}
				DEBUGMSG (ZONE_ERROR, (DTAG TEXT("Failure allocating stream buffers\r\n")));
				return ERROR_NOT_ENOUGH_MEMORY;
			}
		}

		// We've set up the default buffs, set the buff ptr.
		pPDD->strStreamDefault.dwBuffSize = dwMaxBuff;

		// Use the reserved field to store the event handle local to the driver
		if (pPDD->strStreamDefault.dwReserved == 0)
			pPDD->strStreamDefault.dwReserved = (DWORD)CreateEvent (NULL, FALSE, FALSE, NULL);
	} 
	return rc;
}
//-----------------------------------------------------------------------
// NegotiateQuality - Given a fixed set of format/frame/interval, see if a
// quality setting can be found that allows us to fit into the available
// bus bandwidth.
// 
int NegotiateQuality (PDRVCONTEXT pDrv, BYTE bFormatIndex, BYTE bFrameIndex, DWORD dwValidInterval)
{
	int rc;
	DWORD dwBandwidth;
	WORD wQuality, wQualityMin, wQualityMax, wQualityInc;

	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("NegotiateQuality++\r\n")));

	// Get our pdd specific context
	PPDDCONTEXT pPDD = (PPDDCONTEXT)pDrv->dwPddContext; 

	// Get the range of valid compression values for this format/frame/rate
	rc = GetMinMaxQuality (pDrv, bFormatIndex, bFrameIndex, dwValidInterval,
							&wQualityMin, &wQualityMax, &wQualityInc);
	if (rc == 0)
	{
		DEBUGMSG (ZONE_PROBE, (DTAG TEXT("Probe/Commit trying interval rate %d  Comp: min %d max %d int %d\r\n"), 
				  dwValidInterval, wQualityMin, wQualityMax, wQualityInc));
		
		// Loop through each compression quality setting finding one that works.
		for (wQuality = wQualityMax; wQuality >= wQualityMin; wQuality -= wQualityInc)
		{
			// Send the the paramters to the camera, it'll tell us the bandwidth it needs
			rc = ProbeCommitVidStream (pDrv, bFormatIndex, bFrameIndex, &dwValidInterval, wQuality,
									   &dwBandwidth);
			if (rc == 0) 
			{
				// Save the format and stream rate
				pPDD->wCurrFormatIndex = bFormatIndex;
				pPDD->wCurrFrameIndex = bFrameIndex;
				pPDD->dwCurrValidInterval = dwValidInterval;

				// Try to find an interface that will support the bandwidth
				rc = SetStreamInterface (pDrv, 1, dwBandwidth);
				if (rc == 0)
				{
					//Probe commit worked and we have an interface.  Yea! we're done.
					DEBUGMSG (ZONE_PROBE, (DTAG TEXT("Probe/Commit complete. Fmt: %d  Frm: %d  rate %d Compression %d Bandwidth %d\r\n"), 
							  bFormatIndex, bFrameIndex, dwValidInterval, wQuality, dwBandwidth));
					break;
				}
			}
		}
	}
	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("NegotiateQuality--  rc %d\r\n"), rc));
	return rc;
}
	
//-----------------------------------------------------------------------
// FindStreamInterface - Find an interface given packet size needed.  
// If fMin is true, it finds the slowest interface that meets the
// packet size.  If fMin is false, if finds the fastest interface
// under the specified packet size.
//
PUSBSTRMIF FindStreamInterface (PPDDCONTEXT pPDD, BYTE nInterface, 
								BOOL fMin, WORD wPacketSize)
{
	int i;
	PUSBSTRMIF lpFound = 0;

	// Find the interface
	for (i = 0; i < pPDD->nStreamInterfaces; i++)
	{

		if (pPDD->usbstrmIF[i].ifDesc.bInterfaceNumber == nInterface) 
		{
			// Does the interface even have an endpoint?
			if (pPDD->usbstrmIF[i].fEndpoint)
			{
				if (fMin)
				{
					// Find the IF with min packet size that meets the needs
					if (pPDD->usbstrmIF[i].epDesc.wMaxPacketSize >= wPacketSize)
					{ 
						if (lpFound == 0) lpFound = &pPDD->usbstrmIF[i];
						if(pPDD->usbstrmIF[i].epDesc.wMaxPacketSize < lpFound->epDesc.wMaxPacketSize)
						{
							lpFound = &pPDD->usbstrmIF[i];
						}
					}
				}
				else
				{
					// Find the IF with packet size requested
					if (pPDD->usbstrmIF[i].epDesc.wMaxPacketSize < wPacketSize) 
					{
						if (lpFound == 0) lpFound = &pPDD->usbstrmIF[i];
						if (pPDD->usbstrmIF[i].epDesc.wMaxPacketSize > lpFound->epDesc.wMaxPacketSize)
						{
							lpFound = &pPDD->usbstrmIF[i];
						}
					}
				}
			}
			// See if we want a pipe with no bandwidth
			else if (wPacketSize == 0)
				return &pPDD->usbstrmIF[i];
		}
	}
	if (lpFound)
		return lpFound;
	return 0;
}

//-----------------------------------------------------------------------
// SetStreamInterface - Sets the requested stream interface.  The
// different stream interfaces have different data bandwidths.
// 
BOOL SetStreamInterface (PDRVCONTEXT pDrv, BYTE nInterface, DWORD dwBandwidth) 
{
	USB_TRANSFER hTransfer;
	PUSBSTRMIF lpIF;
	int rc;

	// Get our pdd specific context
	PPDDCONTEXT pPDD = (PPDDCONTEXT)pDrv->dwPddContext; 

	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("SetStreamInterface++  Interface:%d  pktsize:%d\r\n"), 
	          nInterface, dwBandwidth));

	// Close the currently opened pipe if any
	if (pPDD->pipeStream.hEvent != 0)
	{
		pDrv->lpUsbFuncs->lpClosePipe (pPDD->pipeStream.hPipe);
		CloseHandle (pPDD->pipeStream.hEvent);
		pPDD->pipeStream.hEvent = 0;
	}

	// For USB 1.x, 1 mS frame rate 
	WORD wPacketSize = (WORD)dwBandwidth;

	// Find an interface that has the packet size we need
	lpIF = FindStreamInterface (pPDD, nInterface, TRUE, wPacketSize);

	// See if interface not found
	if (lpIF == 0)
		return -1;

	hTransfer = pDrv->lpUsbFuncs->lpSetInterface (pDrv->hDevice, NULL, NULL, 0, nInterface, 
	                                              lpIF->ifDesc.bAlternateSetting);
	if (hTransfer == 0)
	{
		DEBUGMSG (ZONE_ERROR, (TEXT("Error calling SetInterface rc=%d\r\n"), GetLastError()));
		return GetLastError();
	}
	if (!CloseTransferHandle (pDrv->lpUsbFuncs, hTransfer))
		rc = GetLastError();

	rc = 0;
	// Open a pipe to the end point
	if (lpIF->fEndpoint)
	{

		// Open the pipe
		pPDD->pipeStream.hPipe = pDrv->lpUsbFuncs->lpOpenPipe (pDrv->hDevice, &lpIF->epDesc);
		// Check for sucessful open
		if (pPDD->pipeStream.hPipe)
		{
			// Copy some pipe attributes
			pPDD->pipeStream.ucAddr = lpIF->epDesc.bEndpointAddress;
			pPDD->pipeStream.wPacketSize = lpIF->epDesc.wMaxPacketSize;

			// First, create an event for handshaking
			pPDD->pipeStream.hEvent = CreateEvent (NULL, TRUE, FALSE, NULL);
		} 
		else
		{
			rc = GetLastError();
			if (rc == 0) rc = ERROR_NOT_ENOUGH_MEMORY; // Sometimes, there isn't an extended error here
			DEBUGMSG (ZONE_ERROR, (DTAG TEXT("Error opening pipe  rc: %d\r\n"), rc));
		}
	}
	if (rc)
		DEBUGMSG (ZONE_ERROR, (DTAG TEXT("Error in SetStreamInterface  rc: %d  ExtErr: %d\r\n"), rc, GetLastError()));
	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("SetStreamInterface--  rc %d\r\n"), rc));
	return rc;
}
//-----------------------------------------------------------------------
// ParseStreamInterfaces - Walks the camera's USB interface descriptors
// to determine the supported freatures of the camera.
//

#define USB_CONFIGURATION_SIZE_5  28
#define USB_CONFIGURATION_SIZE_6  32

BOOL ParseStreamInterfaces (PPDDCONTEXT pPDD, LPCUSB_DEVICE lpUsbDev, BYTE bIFCtl,
							BYTE bIFSubCtl, BYTE bIFStrm, BYTE bIFSubStrm)
{
    DEBUGMSG (ZONE_USBLOAD, (DTAG TEXT("ParseStreamInterfaces++\r\n")));

    DEBUGMSG (ZONE_USBLOAD, (DTAG TEXT("Looking for Ctl IF %d %d and Stream IF %d %d\r\n"), bIFCtl, bIFSubCtl, bIFStrm, bIFSubStrm));
	__try {
	
//(ce 6)		if (lpUsbDev->lpConfigs->dwCount != sizeof (USB_CONFIGURATION))
		//
		// Descriptor structures changed from 5.0 to 6.  Need to check for both sizes
		//
		if ((lpUsbDev->lpConfigs->dwCount != USB_CONFIGURATION_SIZE_5) && 
		    (lpUsbDev->lpConfigs->dwCount != USB_CONFIGURATION_SIZE_6))
		{
			DEBUGMSG (ZONE_ERROR, (DTAG TEXT("Unexpected CUSB_CONFIGURATION dwCount: %d  ***********\r\n"), lpUsbDev->lpConfigs->dwCount));
			return FALSE;
		}

		DWORD i;
		LPCUSB_INTERFACE lpIF;
		int nStreamAlts = 0;
		int nOtherStreams = 0;
		int nStreamID = -1;

		// Walk interface table to find the number of Video stream interface alternates and if there are 
		// other video streams that aren't alts.

		// (ce 6)
		// Need to auto adjust for different struct sizes
		//
		lpIF = lpUsbDev->lpConfigs->lpInterfaces;

		for (i = 0; i < lpUsbDev->lpConfigs->dwNumInterfaces; i++)
		{
			if ((lpIF->Descriptor.bInterfaceClass == bIFStrm) && (lpIF->Descriptor.bInterfaceSubClass == bIFSubStrm))
			{
				// Is first stream?
				if (nStreamID == -1)
					nStreamID = lpIF->Descriptor.bInterfaceNumber;
				else if (nStreamID == lpIF->Descriptor.bInterfaceNumber)
					nStreamAlts++;
				else
					nOtherStreams++;
			}
			// (ce 6) Inc by size of structure since it changed between 5 and 6
			lpIF = (LPCUSB_INTERFACE)((LPBYTE)lpIF + lpIF->dwCount);

		}
		if (nStreamID == -1)
		{
			DEBUGMSG (ZONE_ERROR, (DTAG TEXT("No stream interfaces\r\n")));
			return FALSE;
		}
		pPDD->nStreamInterfaces = nStreamAlts + 1;
	    DEBUGMSG (ZONE_USBLOAD, (DTAG TEXT("Found %d stream interfaces. ID %d\r\n"), pPDD->nStreamInterfaces, nStreamID));
		if (nOtherStreams)
		{
			DEBUGMSG (ZONE_WARNING, (DTAG TEXT("Multiple stream interfaces found on device.  Using first one.\r\n")));
		}

		// Allocate the array for the stream interface descriptors
		if (pPDD->nStreamInterfaces)
		{
			pPDD->usbstrmIF = (PUSBSTRMIF)LocalAlloc (LPTR, pPDD->nStreamInterfaces * sizeof (USBSTRMIF));
		}

		nStreamAlts = 0;  // We'll use this to keep track of which stream and alt we've found.

		// Walk interface table again to get the info we need
		lpIF = lpUsbDev->lpConfigs->lpInterfaces; //(ce 6)
		for (i = 0; i < lpUsbDev->lpConfigs->dwNumInterfaces; i++)
		{
			//	
			// See if this is the video control interface
			//
			if ((lpIF->Descriptor.bInterfaceClass == bIFCtl) && (lpIF->Descriptor.bInterfaceSubClass == bIFSubCtl))
			{
				// Copy descriptor info
				pPDD->usbctlIF.ifDesc = lpIF->Descriptor;
				if (lpIF->lpvExtended)
				{
					// Cast the extended ptr as a video control extended header ptr
					PUSBVIDCTLIFDESCRIPTOR lpExDesc = (PUSBVIDCTLIFDESCRIPTOR)lpIF->lpvExtended;


					// Verify the header length
					if (lpExDesc->bLen > 0x0a)
					{
						// Allocate the space for the descriptor
						pPDD->usbctlIF.lpepExtDesc = LocalAlloc (LPTR, lpExDesc->wTotalLen);
						if (pPDD->usbctlIF.lpepExtDesc)
						{
							// Copy over the extended descriptor
							memcpy (pPDD->usbctlIF.lpepExtDesc, lpExDesc, lpExDesc->wTotalLen); 
						} 
						else
						{
							DEBUGMSG (ZONE_ERROR, (DTAG TEXT("Out of memory allocating extended if descriptor\r\n")));
							return FALSE;
						}
					}
					else
					{
						DEBUGMSG (ZONE_ERROR | ZONE_USBLOAD, 
						          (DTAG TEXT("Unexpected extended Ctl header format")));
						return FALSE;
					}
				} 
				else
					pPDD->usbctlIF.lpifExtDesc = 0;

				// Grab the interrupt end point if specified
				if (lpIF->lpEndpoints)
				{
					pPDD->usbctlIF.epDesc = lpIF->lpEndpoints->Descriptor;
					pPDD->usbctlIF.fEndpoint = TRUE;
				}
				else
					pPDD->usbctlIF.fEndpoint = FALSE;
			}
			//
			// See if this is the video stream interface
			//
			if ((lpIF->Descriptor.bInterfaceClass == bIFStrm) && 
				(lpIF->Descriptor.bInterfaceSubClass == bIFSubStrm) &&
				(lpIF->Descriptor.bInterfaceNumber == nStreamID))   // Also check for matching interface number found above
			{
				// Copy descriptor info
				pPDD->usbstrmIF[nStreamAlts].ifDesc = lpIF->Descriptor;
				if (lpIF->lpvExtended)
				{
					// Cast the extended ptr as a video control extended header ptr
					PUSBVIDSTREAMIFDESCRIPTOR lpExDesc = (PUSBVIDSTREAMIFDESCRIPTOR)lpIF->lpvExtended;

					// Verify the header length
					if (lpExDesc->bLen > 0x0a)
					{
						// Allocate the space for the descriptor
						pPDD->usbstrmIF[nStreamAlts].lpepExtDesc = LocalAlloc (LPTR, lpExDesc->wTotalLen);
						if (pPDD->usbstrmIF[nStreamAlts].lpepExtDesc)
						{
							// Copy over the extended descriptor
							memcpy (pPDD->usbstrmIF[nStreamAlts].lpepExtDesc, lpExDesc, lpExDesc->wTotalLen); 
						} 
						else
						{
							DEBUGMSG (ZONE_ERROR, (DTAG TEXT("Out of memory allocating extended if descriptor\r\n")));
							return FALSE;
						}
					} 
					else
					{
						DEBUGMSG (ZONE_ERROR | ZONE_USBLOAD, 
						          (DTAG TEXT("Unexpected extended Stream header format")));
						return FALSE;
					}

				} 
				else
					pPDD->usbstrmIF[nStreamAlts].lpifExtDesc = 0;

				// Grab the interrupt end point if specified
				if (lpIF->lpEndpoints)
				{
					pPDD->usbstrmIF[nStreamAlts].epDesc = lpIF->lpEndpoints->Descriptor;
					pPDD->usbstrmIF[nStreamAlts].fEndpoint = TRUE;
				}
				else
					pPDD->usbstrmIF[nStreamAlts].fEndpoint = FALSE;
				
				nStreamAlts++;  // Inc index into stream interface array
			}

			// (ce 6) Inc by size of structure since it changed between 5 and 6
			lpIF = (LPCUSB_INTERFACE)((LPBYTE)lpIF + lpIF->dwCount);
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		DEBUGMSG (ZONE_ERROR, (DTAG TEXT("Exception walking device interfaces\r\n")));
		return FALSE;
	}
    DEBUGMSG (ZONE_USBLOAD, (DTAG TEXT("ParseStreamInterfaces--\r\n")));
	return TRUE;
}

//-----------------------------------------------------------------------
// GetCameraError - Queries the error code from the device
// 
int GetCameraError (PDRVCONTEXT pDrv, BYTE *pErr)
{
	int rc = 0;
	BYTE bInterface = VID_IF_STREAM;
	BYTE bUnit = 0;

	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("GetCameraError++\r\n")));

	// Get our pdd specific context
	PPDDCONTEXT pPDD = (PPDDCONTEXT)pDrv->dwPddContext; 

	*pErr = 0;
	rc = DoVendorTransfer (pDrv, USBVID_SET_CUR, USB_VIDEO_VS_CS_PROBE_CTL,
	                       bInterface, bUnit, pErr, 1);

	if (rc)
		DEBUGMSG (ZONE_ERROR, (DTAG TEXT("Error reading error code from camera rc %d\r\n"), rc));

	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("GetCameraError-- rc %d error code %d\r\n"), rc, *pErr));
	return rc;
}

//=========================================================================
// ReadIsocThread - Thread that calls the Iscoh transfer function to 
// read the image data from the camera.
//
//#define MAXPACKETS 1
#define MAXPACKETS 8
DWORD WINAPI ReadIsocThread (PVOID pArg) {

	PDRVCONTEXT pDrv = (PDRVCONTEXT)pArg;
	PPDDCONTEXT pPDD = (PPDDCONTEXT)pDrv->dwPddContext; 
	PBYTE pDataBuff, pStart, pEnd;
	int nNumPackets;
	DWORD dw;
	DWORD dwFrameLen[MAXPACKETS];
	DWORD dwUsbRc[MAXPACKETS];
	DWORD dwBytes;
#ifdef DEBUG
	DWORD dwTicksAtFrameEnd = 0;
#endif
	BYTE bSaveBytes[16];
	DWORD dwVFrameTotal;
	BYTE bError;
	int packetindex;

    DEBUGMSG (ZONE_FUNC | ZONE_THREAD, 
	          (DTAG TEXT("ReaReadIsocThreaddThread++ ID:%08x\r\n"), GetCurrentThread()));

	// Tweak the thread priority since we need priority over apps 
	// to read the pipe on time.
	SetThreadPriority (GetCurrentThread (), THREAD_PRIORITY_HIGHEST);
	
	// Initialize the ptrs
	pPDD->pstrStream->dwCurrBuff = 0;  
	pPDD->pstrStream->dwValidFrames = 0;  // No valid frames yet...
	pDataBuff = pPDD->pstrStream->pFrame[pPDD->pstrStream->dwCurrBuff].pBuff;
	pEnd = pDataBuff + pPDD->pstrStream->dwBuffSize;

	memset (dwFrameLen, 0, sizeof (dwFrameLen));
	memset (dwUsbRc, 0, sizeof (dwUsbRc));

	// Compare the buffer size with the packet size to make sure we at least can
	// read something.
	if (pPDD->pstrStream->dwBuffSize < pPDD->pipeStream.wPacketSize)
	{
	    DEBUGMSG (ZONE_ERROR, (DTAG TEXT("ReadThread failure, read buff size smaller than packet size\r\n")));
		return -1;
	}
#if MAXPACKETS > 1
	// See if we need multiple packets because stream packet size is > 1024
	{
//		nNumPackets = (pPDD->pipeStream.wPacketSize / 1024) + 1;
		nNumPackets = (pPDD->pipeStream.wPacketSize / 1024) + 2; //hack to request at least 2 packets per transfer. 
		if (nNumPackets > MAXPACKETS)
		{
			DEBUGMSG (ZONE_ERROR, (DTAG TEXT("Packetsize %d > larger than supported\r\n"), pPDD->pipeStream.wPacketSize));
			return -2;
		}
	}
#endif	
	//
	//  Isoc pipe read loop
	//
	while (pPDD->wReadThreadState != STREAMTHD_STOPPED)	
	{
		// If we're looking for the first frame, reset ptr to start of first buffer
		if (pPDD->wReadThreadState == STREAMTHD_FRAMESYNC)
		{
			pDataBuff = pPDD->pstrStream->pFrame[pPDD->pstrStream->dwCurrBuff].pBuff;
			pEnd = pDataBuff + pPDD->pstrStream->dwBuffSize;
		}

		// Init size of frame buffers
		int nPacketSize = pPDD->pipeStream.wPacketSize;
#if MAXPACKETS > 1
		
//		for (nNumPackets = 0; (nPacket > 0) && (nNumPackets < MAXPACKETS); nNumPackets++)
		for (packetindex = 0; (packetindex < MAXPACKETS); packetindex++)
		{
			// Max transfer in USB is 1K
			dwFrameLen[packetindex] = min (nPacketSize, 1024);
//			nPacket -= dwFrameLen[packetindex];
		}
#else	
		dwFrameLen[0] = min (pPDD->pipeStream.wPacketSize, 1024);
		nNumPackets = 1;
#endif

		// Limit check on buffer
		if (pDataBuff + dwFrameLen[0] >= pEnd)
		{
			DEBUGMSG (ZONE_ERROR | ZONE_THREAD, (DTAG TEXT("Buffer too small for next isoc data packet!\r\n")));
			break;
		}

		__try {

			dw = IssueIsochTransfer (pDrv->lpUsbFuncs, pPDD->pipeStream.hPipe, 
									 DefaultTransferComplete, pPDD->pipeStream.hEvent,
									 USB_IN_TRANSFER | USB_SHORT_TRANSFER_OK | USB_START_ISOCH_ASAP | USB_COMPRESS_ISOCH,
									 0,               // Using USB_START_ISOCH_ASAP precludes the need for a start frame
									 nNumPackets,      // Number of frames to read
									 dwFrameLen,      // Array that receives the length of each frame
									 pDataBuff,       // Pointer to transfer buffer
									 0,               // Specifies the physical address, which may be NULL, of the data buffer
									 &dwBytes,	      // Number of bytes transferred by USB
									 1500,            // Timeout in msec
									 dwUsbRc);	      // Returns USB_ERROR or USB_TRANSFER

			DEBUGMSG (ZONE_READDATA && (dwBytes > 12), (DTAG TEXT("Read %5d bytes at %x  dw: %d\r\n"), dwBytes, pDataBuff, dw));
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			DEBUGMSG (ZONE_ERROR | ZONE_THREAD, (DTAG TEXT ("Exception calling IssueIsochTransfer %d %d\r\n"), dw, GetLastError()));
			break;
		}
		if (dw)
		{
			DEBUGMSG (ZONE_ERROR | ZONE_THREAD, (DTAG TEXT ("Error calling IssueIsochTransfer %d %d\r\n"), dw, GetLastError()));
			if (dw == USB_DATA_OVERRUN_ERROR) // USB overrun error.
				continue;
//			else
//				break;

			int rc = GetCameraError (pDrv, &bError);
			if (rc)
				DEBUGMSG (ZONE_ERROR, (DTAG TEXT("Can't read camera error code rc %d\r\n"), rc));
			else
				DEBUGMSG (ZONE_ERROR, (DTAG TEXT("Camera error code %d\r\n"), bError));
		
		
		}
		// See if we read at least the header
		if (dwBytes == 0)
		{
			DEBUGMSG (ZONE_READDATA, (DTAG TEXT("packet size read if %d bytes smaller than standard header\r\n"), dwBytes));
			continue;
		}

		// ------------------------------
		// Process each frame in buffer
		// ------------------------------
		PBYTE pFramePtr = pDataBuff;
		for (packetindex = 0; packetindex < nNumPackets; packetindex++)
		{
			if ((dwUsbRc[packetindex] != 0) && (dwUsbRc[packetindex] != USB_DATA_UNDERRUN_ERROR))
				DEBUGMSG (ZONE_ERROR, (TEXT("Error in packet[%d] rc = %d  packet size %d\r\n"), 
				          packetindex, dwUsbRc[packetindex], dwFrameLen[packetindex]));

			// Look at payload header
			PUSBVIDPAYLOADHDR pPktHdr = (PUSBVIDPAYLOADHDR)(pFramePtr + (packetindex * nPacketSize));
			BYTE bPktFlags;
			if (pPktHdr->bLen != sizeof (USBVIDPAYLOADHDR))
			{
				DEBUGMSG (ZONE_PACKETS, (DTAG TEXT("============================================\r\n")));
				DEBUGMSG (ZONE_PACKETS, (DTAG TEXT("=== Unexpected packet header size  %d    === \r\n"), pPktHdr->bLen));
				DEBUGMSG (ZONE_PACKETS, (DTAG TEXT("============================================\r\n")));
				continue;
			}

			// Check for error in packet
			if (pPktHdr->bFlags & USBVID_PAYLOADHDR_ERR)
			{
				DEBUGMSG (ZONE_ERROR && ZONE_PACKETS, (DTAG TEXT("=== Frame Error !!!       === Flags: %02x\r\n"), pPktHdr->bFlags));

				int rc = GetCameraError (pDrv, &bError);
				if (rc)
					DEBUGMSG (ZONE_ERROR, (DTAG TEXT("Can't read camera error code rc %d\r\n"), rc));
				else if (bError)
					DEBUGMSG (ZONE_ERROR, (DTAG TEXT("Camera error code %d\r\n"), bError));

				// Need to reaquire the feed at the next frame
				pPDD->wReadThreadState = STREAMTHD_FRAMESYNC;  
				continue;
			}
			
			if (pPktHdr->bFlags & USBVID_PAYLOADHDR_STILLFRAME)
			{
				DEBUGMSG (ZONE_PACKETS, (DTAG TEXT("=============================\r\n")));
				DEBUGMSG (ZONE_PACKETS, (DTAG TEXT("=== Still packet found.   ===\r\n")));
				DEBUGMSG (ZONE_PACKETS, (DTAG TEXT("=============================\r\n")));
				// For now, we ignore still capture packets
				continue;
			}
			// Cache the packet flags since we may need them after the hdr is overwritten
			bPktFlags = pPktHdr->bFlags;

#if MAXPACKETS > 1
			DWORD dwPacketBytes = dwFrameLen[packetindex];
#else
			DWORD dwPacketBytes = dwBytes;
#endif
			DWORD dwPacketDataBytes = dwPacketBytes - sizeof(USBVIDPAYLOADHDR);
			//
			// State machine for frame capture
			//
			switch (pPDD->wReadThreadState)	{

			case STREAMTHD_FIRSTCAP:
				DEBUGMSG (ZONE_THSTATE, (DTAG TEXT ("STREAMTHD_FIRSTCAP\r\n")));
				if (dwBytes > sizeof (USBVIDPAYLOADHDR))
					DEBUGMSG (ZONE_VIDFRAME, (DTAG TEXT ("Frame start. buff: %x\r\n"), pDataBuff));

				// Number of bytes of data actually written
				dwVFrameTotal = dwPacketDataBytes;

				// If we actually received some data this frame, proceed to next state.
				if (dwVFrameTotal)
				{
#if MAXPACKETS > 1
					// If the start packet isn't at the start of a frame,
					// copy it to the start of the new frame buffer
					if (pStart != (PBYTE)pPktHdr)
					{
						memcpy (pDataBuff, (PBYTE)pPktHdr, dwPacketBytes);
					} 
					pDataBuff += dwPacketBytes;
#else
					// Update the data ptr to the end of the currently read data - the size of packet header
					pDataBuff += dwPacketDataBytes;

					// Save the bytes we want to restore later
					memcpy (bSaveBytes, pDataBuff, min (sizeof (USBVIDPAYLOADHDR), sizeof (bSaveBytes)));
#endif
					pPDD->wReadThreadState = STREAMTHD_CAPTURING;
				}
				break;

			case STREAMTHD_CAPTURING:
				DEBUGMSG (ZONE_THSTATE, (DTAG TEXT ("STREAMTHD_CAPTURING\r\n")));
				// 
				// In an effort to make this fast, read directly into output buffer.  Since all packets
				// have a 12 byte header (USB Video Spec) we back up over the last 12 bytes of the last
				// packet.  Before the packet is read, save the 12 bytes that will be overwritten and
				// restore them over the top of the packet header after the packet has been read.
				//

#if MAXPACKETS > 1
				// Move the frame data to the end of the last frame
				memcpy (pDataBuff, (PBYTE)pPktHdr+sizeof(USBVIDPAYLOADHDR), dwPacketDataBytes);
#else
				// Restore the original bytes overwritten by packet hdr
				memcpy (pDataBuff, bSaveBytes, min (sizeof (USBVIDPAYLOADHDR), sizeof (bSaveBytes)));
#endif

				// See if the frame is complete 
				if (bPktFlags & USBVID_PAYLOADHDR_FRAMEEND)
				{
					DEBUGMSG (ZONE_THSTATE, (DTAG TEXT ("USBVID_PAYLOADHDR_FRAMEEND detected bytes %d\r\n"), dwVFrameTotal+dwPacketDataBytes));
					dwVFrameTotal += dwPacketDataBytes;

					// Sync with app calling thread
					EnterCriticalSection (&pPDD->pstrStream->csBuffSync);
					__try 
					{
						// Inc frames since last GetFrame call count
						pPDD->pstrStream->dwValidFrames++;

						// Save the size of the data
						pPDD->pstrStream->pFrame[pPDD->pstrStream->dwCurrBuff].dwSize = dwVFrameTotal;

						// Save the tick count for ageing test
						pPDD->pstrStream->pFrame[pPDD->pstrStream->dwCurrBuff].dwTicksAtCapture = GetTickCount();

						// Make the current index the last good frame index
						pPDD->pstrStream->dwLastGoodBuff = pPDD->pstrStream->dwCurrBuff;

						// Find next buffer. Keep looking until we find a valid pointer
						do
						{
							pPDD->pstrStream->dwCurrBuff = (pPDD->pstrStream->dwCurrBuff+1) % pPDD->pstrStream->dwNumBuffs;
						} while (pPDD->pstrStream->pFrame[pPDD->pstrStream->dwCurrBuff].pBuff == 0);

						// Get Ptr to buffer
						pDataBuff = pPDD->pstrStream->pFrame[pPDD->pstrStream->dwCurrBuff].pBuff;
					}
					__finally
					{
						LeaveCriticalSection (&pPDD->pstrStream->csBuffSync);
					}
					pStart = pDataBuff;
					pEnd = pDataBuff + pPDD->pstrStream->dwBuffSize;

					DEBUGMSG (ZONE_VIDFRAME, (DTAG TEXT ("Frame end. size %d  cur buff[%d] next buff[%d]  time %d mS\r\n"), 
							  dwVFrameTotal, pPDD->pstrStream->dwLastGoodBuff, pPDD->pstrStream->dwCurrBuff,
							  dwTicksAtFrameEnd ? GetTickCount() - dwTicksAtFrameEnd : 0));
		#ifdef DEBUG
					dwTicksAtFrameEnd = GetTickCount();
		#endif
					// We use the reserved field since that is the local event handle
					if (!SetEvent ((HANDLE)pPDD->pstrStream->dwReserved))
						DEBUGMSG (ZONE_ERROR, (DTAG TEXT ("Can't set frame complete event rc %d\r\n"), GetLastError()));

					// Notify event routines that we have a new frame.
					PulseEvent (pPDD->hNewFrameEvent);

					pPDD->wReadThreadState = STREAMTHD_FIRSTCAP;
				} 
				// else, not frame end, just the middle of a packet
				else
				{
					// Point next read to end of current data
					pDataBuff += dwPacketDataBytes;
					dwVFrameTotal += dwPacketDataBytes;
					//DEBUGMSG (ZONE_THSTATE, (DTAG TEXT ("ptr update %6x %x %x diff %x\r\n"), pDataBuff, dwPacketBytes, sizeof (USBVIDPAYLOADHDR), dwPacketBytes - sizeof (USBVIDPAYLOADHDR)));

					// Save the bytes we want to restore later
					memcpy (bSaveBytes, pDataBuff, min (sizeof (USBVIDPAYLOADHDR), sizeof (bSaveBytes)));
				}
				break;

			case STREAMTHD_FRAMESYNC:
				DEBUGMSG (ZONE_THSTATE, (DTAG TEXT ("STREAMTHD_FRAMESYNC\r\n")));

				// If we're at the end of a frame, the next packet should start a new one
				if (bPktFlags & USBVID_PAYLOADHDR_FRAMEEND)
				{
					// Restart the data capture at the start of the current frame. Don't take
					// the CS since we're not modifying anything, just starting over with
					// the current frame buffer
					pDataBuff = pPDD->pstrStream->pFrame[pPDD->pstrStream->dwCurrBuff].pBuff;
					pStart = pDataBuff;
					pEnd = pDataBuff + pPDD->pstrStream->dwBuffSize;

					pPDD->wReadThreadState = STREAMTHD_FIRSTCAP;
				}
				break;
			} //switch
		} // for loop
		dwBytes = 0;
	}
	// Clean up

	// If the streaming structure isn't the default, close the local copy of the handle
	if (pPDD->pstrStream != (PVIDSTREAMSTRUCT)&pPDD->strStreamDefault)
	{
		CloseHandle ((HANDLE)pPDD->pstrStream->dwReserved);
	}
  
	pPDD->wReadThreadState = STREAMTHD_STOPPED;
	DEBUGMSG (ZONE_FUNC | ZONE_THREAD, (DTAG TEXT("ReaReadIsocThreaddThread--\r\n")));
	return 0;
}

//-----------------------------------------------------------------------
// IssueIsochTransfer - implements a synch isoch transfer to the device.
// This code was based on IssueVendorTransfer from USBClient.cpp 
// 
DWORD IssueIsochTransfer (LPCUSB_FUNCS pUsbFuncs, HANDLE hUsbDevice,
   LPTRANSFER_NOTIFY_ROUTINE NotifyRoutine,       // Transfer completion routine.
   PVOID                     NotifyContext,       // Single argument passed to the completion routine
   DWORD                     Flags,               // USB_XXX flags describing the transfer
   DWORD                     dwStartingFrame,     // Starting frame to read
   DWORD                     dwNumFrames,         // Number of frames to read
   PDWORD                    pdwFrameLengths,     // Array that receives the length of each frame
   LPVOID                    pBuffer,             // Pointer to transfer buffer
   ULONG                     PhysAddr,            // Specifies the physical address, which may be NULL, of the data buffer
   LPDWORD                   pBytesTransferred,   // Number of bytes transferred by USB
   DWORD                     dwTimeout,           // Timeout in msec
   PUSB_ERROR                pUsbRc)		      // Returns USB_ERROR or USB_TRANSFER
{
    USB_TRANSFER hTransfer;
    DWORD  dwErr = ERROR_SUCCESS;
    BOOL   bRc = TRUE;
    DWORD  z, dwWaitReturn;

    if (pUsbFuncs && hUsbDevice && pdwFrameLengths && pBytesTransferred && pUsbRc)  
	{
        *pUsbRc = USB_NO_ERROR;
        *pBytesTransferred = 0;

        if (NotifyContext && NotifyRoutine && dwTimeout) 
            ResetEvent(NotifyContext); // NotifyContext *must* be an EVENT

        hTransfer = pUsbFuncs->lpIssueIsochTransfer( hUsbDevice, NotifyRoutine,
		                         NotifyContext, Flags, dwStartingFrame, dwNumFrames, 
								 pdwFrameLengths, pBuffer,PhysAddr);
        if (hTransfer) 
		{
            //
            // Asynch call completed.
            // Get transfer status & number of bytes transferred
            //
            if (NotifyContext && NotifyRoutine) 
			{
                if (!dwTimeout) 
				{
                    *pUsbRc = (USB_ERROR)hTransfer;
                    return dwErr;
                }

                //
                // sync the transfer completion / timer
                //
                dwWaitReturn = WaitForSingleObject( NotifyContext,
                                                    dwTimeout );
                switch (dwWaitReturn) 
				{
                   case WAIT_OBJECT_0:
                      //
                      // The completion event was signalled by the callback.
                      // Get transfer status & number of bytes transferred
                      //
					  bRc = pUsbFuncs->lpGetIsochResults (hTransfer, dwNumFrames, pdwFrameLengths, pUsbRc);
					  // sum total data transferred.
					  for (z = 0; z < dwNumFrames; z++)
					  {
						  *pBytesTransferred += pdwFrameLengths[z];
						  if (pUsbRc[z])
						  {
							  if ((pUsbRc[z] != USB_DATA_TOGGLE_MISMATCH_ERROR) && 
								  (pUsbRc[z] != USB_DATA_UNDERRUN_ERROR)) //Ignore underrun error
							  {
								  DEBUGMSG (ZONE_ERROR, (DTAG TEXT("ERROR %d in frame %d\r\n"), pUsbRc[z], z)); 
								  dwErr = pUsbRc[z];
								  if (dwErr) break;
							  }
						  }
					  }
                      break;

                   case WAIT_TIMEOUT:
                      //
                      // The transfer reqest timed out.
                      // Get transfer status & number of bytes transferred
                      //
                      DEBUGMSG( ZONE_USBCLIENT, (TEXT("%s:WAIT_TIMEOUT on hT:0x%x\n"), (Flags & USB_IN_TRANSFER) ? TEXT("IN") : TEXT("OUT"), hTransfer ));

                      GetTransferStatus(pUsbFuncs, hTransfer, pBytesTransferred, pUsbRc);

                      //
                      // let caller know it timed out
                      //
                      dwErr = ERROR_TIMEOUT;
                      break;
       
                   default:
                      dwErr = ERROR_GEN_FAILURE;
                      DEBUGMSG( ZONE_USBCLIENT, (TEXT("*** Unhandled WaitReason:%d ***\n"), dwWaitReturn ));
                      // ASSERT(0);
                      break;
                }
            } 
			else 
			{
                //
                // Synch call completed.
                // Get transfer status & number of bytes transferred
                //
				bRc = pUsbFuncs->lpGetIsochResults (hTransfer, dwNumFrames, pdwFrameLengths, pUsbRc);
				// sum total data transferred.
				for (z = 0; z < dwNumFrames; z++)
				{
				  *pBytesTransferred += pdwFrameLengths[z];
				  if (pUsbRc[z])  
				  {
					  if (pUsbRc[z] != USB_DATA_UNDERRUN_ERROR) //Ignore underrun error
						  dwErr = pUsbRc[z];
				  }
				}
            }
            CloseTransferHandle(pUsbFuncs, hTransfer);

        } else 
		{
            dwErr = ERROR_GEN_FAILURE;
            DEBUGMSG( ZONE_USBCLIENT, (TEXT("*** IssueIsochTransfer ERROR(3, 0x%x) ***\n"), dwErr ));
        }
    
    } else
        dwErr = ERROR_INVALID_PARAMETER;

    if (ERROR_SUCCESS != dwErr) 
	{
        SetLastError(dwErr);
        DEBUGMSG (ZONE_USBCLIENT, 
			      (TEXT("IssueIsochTransfer ERROR(5, BytesTransferred:%d, Win32Err:%d )\n"), 
			      pBytesTransferred?*pBytesTransferred:-1, dwErr)); 
    }
    return dwErr;
}
