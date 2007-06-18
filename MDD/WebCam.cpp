//-------------------------------------------------------------------------
// <copyright file="WebCam.cpp" company="Microsoft">
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
//    USB Webcam driver for Windows CE
// </summary>
//-------------------------------------------------------------------------
//======================================================================
// WebCam - USB Webcam driver for Windows CE
//
// Author: Douglas Boling
//======================================================================
#include <windows.h>				// For all that Windows stuff
#include <winioctl.h>				// Needed for CTLCODE macro
#include <USBdi.h>					// USB includes
#include <usb100.h>					// USB includes

#include "webcamsdk.h"				// IOCTL defs for driver
#include "WebCam.h" 				// Local driver includes

// _WIN32_WINCE
// Globals
//
HINSTANCE hInst;					 // DLL instance handle

//
// Debug zone support
//
#ifdef DEBUG
// Debug zone structure
DBGPARAM dpCurSettings = 
{
	TEXT("WebCam"), 
	{
		TEXT("Errors"),TEXT("Warnings"),TEXT("Functions"), TEXT("Init"),
		TEXT("Driver Calls"),TEXT("USBLoad"),TEXT("Thread"),TEXT("Frame Info"), 
		TEXT("Packets"),TEXT("Timing"),TEXT("Read Data"),TEXT("Probe/Commit"),
		TEXT("Still Cap"),TEXT("Stream Frames"),TEXT("Feature Ctls"), TEXT("Descriptors") 
	},
	0x21c3
//	0x0003
};
#endif //DEBUG

//======================================================================
// DllMain - DLL initialization entry point
//
BOOL WINAPI DllMain (HANDLE hinstDLL, DWORD dwReason, 
					 LPVOID lpvReserved) {
	hInst = (HINSTANCE)hinstDLL;

	switch (dwReason) {
		case DLL_PROCESS_ATTACH:
			DEBUGREGISTER(hInst);
			// Improve performance by passing on thread attach calls
			DisableThreadLibraryCalls (hInst);
		break;
	
		case DLL_PROCESS_DETACH:
			DEBUGMSG(ZONE_INIT, (DTAG TEXT("DLL_PROCESS_DETACH\r\n")));
			break;
	}
	return TRUE;
}
//======================================================================
// CAM_Init - Driver initialization function
//
DWORD CAM_Init (DWORD dwContext, LPCVOID lpvBusContext) {
	PDRVCONTEXT pDrv;

	// Allocate a drive instance structure.
	pDrv = GetConfigData (dwContext);
	if (pDrv)
	{
		if (!pdd_DrvInit (pDrv))
		{
			// If failure, the drv context structure will be freed by
			// USBDeviceAttach
			DEBUGMSG (ZONE_ERROR | ZONE_INIT, (DTAG TEXT("pdd_Init Failure!")));
			return 0;
		}
	}
	return (DWORD)pDrv;
}
//======================================================================
// CAM_Deinit - Driver de-initialization function
//
BOOL CAM_Deinit (DWORD dwContext) 
{
	PDRVCONTEXT pDrv = (PDRVCONTEXT) dwContext;

	pdd_DrvUninit (pDrv);
	// The drv context structure will be freed by 
	// USBDeviceNotificationCallback
	return TRUE;
}
//======================================================================
// CAM_Open - Called when driver opened
//
DWORD CAM_Open (DWORD dwContext, DWORD dwAccess, DWORD dwShare) 
{
	PDRVCONTEXT pDrv = (PDRVCONTEXT) dwContext;

	DEBUGMSG (ZONE_EXENTRY, 
			  (DTAG TEXT("CAM_Open++ dwContext: %x\r\n"), dwContext));

	// Verify that the context handle is valid.
	if ((pDrv == 0) || (pDrv->dwSize != sizeof (DRVCONTEXT))) {
		return 0;
	}
	EnterCriticalSection (&pDrv->csDCall);

	if (pDrv->nNumOpens)
	{
		//We're already open, fail...
		LeaveCriticalSection (&pDrv->csDCall);
		SetLastError(ERROR_ACCESS_DENIED);
		return 0;
	}
	pDrv->nNumOpens++;
	
	pdd_DrvOpen (pDrv);

	LeaveCriticalSection (&pDrv->csDCall);

	return (DWORD)pDrv;
}
//======================================================================
// CAM_Close - Called when driver closed
//
BOOL CAM_Close (DWORD dwOpen) {
	PDRVCONTEXT pDrv = (PDRVCONTEXT) dwOpen;

	DEBUGMSG (ZONE_EXENTRY, 
			  (DTAG TEXT("CAM_Close++ dwOpen: %x\r\n"), dwOpen));

	EnterCriticalSection (&pDrv->csDCall);

	if (pDrv && (pDrv->dwSize != sizeof (DRVCONTEXT))) {
		LeaveCriticalSection (&pDrv->csDCall);
		DEBUGMSG (ZONE_FUNC | ZONE_ERROR, 
				  (DTAG TEXT("CAM_Close failed\r\n")));
		SetLastError(ERROR_INVALID_HANDLE);
		return 0;
	}

	// Notify the PDD that the app is closing the driver.  The 
	// PDD will stop any running stream.
	pdd_DrvClose (pDrv);

	// If we allocated the frame buffer, free it, 
	if (pDrv->mmFrameBuff.p)
	{
		DEBUGMSG (ZONE_ERROR, 
		          (DTAG TEXT ("Driver closed with active stream running!  Stop stream before closing\r\n")));
		// Free up the current frame buffer
		FreeSharedObject (&pDrv->mmFrameBuff);
		pDrv->mmFrameBuff.p = 0;
	}

	if (pDrv->nNumOpens)
		pDrv->nNumOpens--;

	LeaveCriticalSection (&pDrv->csDCall);

	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("CAM_Close--\r\n")));
	return TRUE;
}
//======================================================================
// CAM_Read - Called when driver read
//
DWORD CAM_Read (DWORD dwOpen, LPVOID pBuffer, DWORD dwCount) {
	DWORD dwBytesRead = 0;
	DEBUGMSG (ZONE_EXENTRY, 
			  (DTAG TEXT("CAM_Read++ dwOpen: %x\r\n"), dwOpen));


	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("CAM_Read--\r\n")));
	return dwBytesRead;
}
//======================================================================
// CAM_Write - Called when driver written
//
DWORD CAM_Write (DWORD dwOpen, LPVOID pBuffer, DWORD dwCount) {
	DWORD dwBytesWritten = 0;
	DEBUGMSG (ZONE_EXENTRY, 
			  (DTAG TEXT("CAM_Write++ dwOpen: %x\r\n"), dwOpen));

	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("CAM_Write--\r\n")));
	return dwBytesWritten;
}
//======================================================================
// CAM_Seek - Called when SetFilePtr called
//
DWORD CAM_Seek (DWORD dwOpen, long lDelta, WORD wType) {
	DEBUGMSG (ZONE_EXENTRY,(DTAG TEXT("CAM_Seek++ dwOpen:%x %d %d\r\n"), 
			  dwOpen, lDelta, wType));

	DEBUGMSG (ZONE_EXENTRY, (DTAG TEXT("CAM_Seek--\r\n")));
	return 0;
}
//======================================================================
// CAM_IOControl - Called when DeviceIOControl called
// 
DWORD CAM_IOControl (DWORD dwOpen, DWORD dwCode, PBYTE pIn, DWORD dwIn,
					 PBYTE pOut, DWORD dwOut, DWORD *pdwBytesWritten) {
	PDRVCONTEXT pDrv;
	DWORD err = ERROR_INVALID_PARAMETER;

	DEBUGMSG (ZONE_EXENTRY, 
			  (DTAG TEXT("CAM_IOControl++ dwOpen: %x  dwCode: %x\r\n"),
			  dwOpen, dwCode));

	pDrv = (PDRVCONTEXT) dwOpen;
	EnterCriticalSection (&pDrv->csDCall);
	
	switch (dwCode) {
		case IOCTL_CAMERA_DEVICE_GETDRIVERVERSION:
			err = mdd_GetVersion (pDrv, pOut, dwOut, pdwBytesWritten);
			break;
		case IOCTL_CAMERA_DEVICE_QUERYPARAMETERARARY:
			err = mdd_GetParameterList (pDrv, pOut, dwOut, pdwBytesWritten);
			break;
		case IOCTL_CAMERA_DEVICE_QUERYPARAMETER:
			err = mdd_GetParameter (pDrv, pIn, dwIn, pOut, dwOut, pdwBytesWritten);
			break;
		case IOCTL_CAMERA_DEVICE_SETPARAMETER:
			err = mdd_SetParameter (pDrv, pIn, dwIn, pOut, dwOut, pdwBytesWritten);
			break;
		case IOCTL_CAMERA_DEVICE_QUERYVIDEOFORMATS:
			err = mdd_QueryVideoFormats (pDrv, pIn, dwIn, pOut, dwOut, pdwBytesWritten);
			break;
		case IOCTL_CAMERA_DEVICE_GETVIDEOFORMAT:
			err = mdd_GetVideoFormat (pDrv, pIn, dwIn, pOut, dwOut, pdwBytesWritten);
			break;
		case IOCTL_CAMERA_DEVICE_STARTVIDEOSTREAM:
			err = mdd_SetVideoFormat (pDrv, pIn, dwIn, pdwBytesWritten);
			break;
		case IOCTL_CAMERA_DEVICE_GETNEXTVIDEOFRAME:
			err = mdd_GetNextVideoFrame (pDrv, pIn, dwIn, pOut, dwOut, pdwBytesWritten);
			break;
		case IOCTL_CAMERA_DEVICE_GETCURRENVIDEOFORMAT:
			err = mdd_GetCurrentFormat (pDrv, pOut, dwOut, pdwBytesWritten);
			break;
		case IOCTL_CAMERA_DEVICE_STOPVIDEOSTREAM:
			err = mdd_StopVideoStream (pDrv);
			if (pdwBytesWritten)
				*pdwBytesWritten = 0;
			break;
		case IOCTL_CAMERA_DEVICE_QUERYSTILLFORMATS:
			err = mdd_QueryStillFormats (pDrv, pIn, dwIn, pOut, dwOut, pdwBytesWritten);
			break;
		case IOCTL_CAMERA_DEVICE_GETSTILLIMAGE:
			err = mdd_GetStillImage (pDrv, pIn, dwIn, pOut, dwOut, pdwBytesWritten);
			break;
		case IOCTL_CAMERA_DEVICE_WAITONCAMERAEVENT:
			err = mdd_WaitOnCameraEvent (pDrv, pIn, dwIn, pOut, dwOut, pdwBytesWritten);
			break;

		default:
			DEBUGMSG (ZONE_ERROR, 
			 (DTAG TEXT("CAM_IOControl: unknown code %x\r\n"), dwCode));
			err = ERROR_INVALID_PARAMETER;
	}
	SetLastError (err);
	LeaveCriticalSection (&pDrv->csDCall);
	DEBUGMSG (ZONE_EXENTRY, (DTAG TEXT("CAM_IOControl-- rc = %d\r\n"), err));
	return err ? FALSE : TRUE;
}
//======================================================================
// CAM_PowerDown - Called when system suspends
//
void CAM_PowerDown (DWORD dwContext) 
{
	return;
}
//======================================================================
// CAM_PowerUp - Called when resumes
//
void CAM_PowerUp (DWORD dwContext) 
{
	return;
}
//----------------------------------------------------------------------
// GetConfigData - Get the pointer to the driver context structure
//
PDRVCONTEXT GetConfigData (DWORD dwContext)
{
	int nLen, rc;
	DWORD dwLen, dwType, dwSize = 0;
	HKEY hKey;
	PDRVCONTEXT pDrv = 0;

	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("GetConfigData++\r\n")));
	nLen = 0;
	// If ptr < 65K, it's a value, not a pointer.  
	if (dwContext < 0x10000) {
		return 0; 
	} else {
		__try {
			nLen = lstrlen ((LPTSTR)dwContext);
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			nLen = 0;
		}
	}
	if (!nLen) {
		DEBUGMSG (ZONE_ERROR, (DTAG TEXT("dwContext not a ptr\r\n")));
		return 0;
	}

	// Open the Active key for the driver.
	rc = RegOpenKeyEx(HKEY_LOCAL_MACHINE,(LPTSTR)dwContext,0, 0, &hKey);

	if (rc == ERROR_SUCCESS) {
		// Read the key value.
		dwLen = sizeof(pDrv);
		rc = RegQueryValueEx (hKey, TEXT("ClientInfo"), NULL, &dwType,
								   (PBYTE)&pDrv, &dwLen);

		RegCloseKey(hKey);
		if ((rc == ERROR_SUCCESS) && (dwType == REG_DWORD))
		{
			__try {
				if (pDrv->dwSize != sizeof (DRVCONTEXT))
					pDrv = 0;
			}
			__except (EXCEPTION_EXECUTE_HANDLER) {
				pDrv = 0;
			}
		}
		else 
		{
			DEBUGMSG (ZONE_ERROR, (DTAG TEXT("Error reading registry\r\n")));
			pDrv = 0;
		}
	} else
		DEBUGMSG (ZONE_ERROR, (DTAG TEXT("Error opening Active key\r\n")));

	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("GetConfigData--\r\n")));
	return pDrv;
}
//======================================================================
// MDD functions
//======================================================================
//---------------------------------------------------------------------------------------
// mdd_GetParameterList 
// 
int mdd_GetVersion (PDRVCONTEXT pDrv, PBYTE pOut, DWORD dwOut, PDWORD pdwBytesWritten) 
{
	int rc;
	DWORD dwOutSize = sizeof (DWORD);
	
	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("mdd_GetVersion++ \r\n")));

	// See if they passed the proper parameters
	if ((pOut == 0) || (pdwBytesWritten == 0))
		return ERROR_INVALID_PARAMETER;
		
	// See if output buffer is big enough
	if (dwOut < dwOutSize)
		rc = ERROR_NOT_ENOUGH_MEMORY;
	else
	{
		__try 
		{
			*(DWORD *)pOut = MAKELONG (DRV_MINORVER, DRV_MAJORVER);
			*pdwBytesWritten = dwOutSize;
			rc = 0;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) 
		{
			rc = ERROR_INVALID_PARAMETER;
		}
	}
	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("mdd_GetVersion-- rc %d\r\n"),rc));
	return rc;

}

//---------------------------------------------------------------------------------------
// mdd_GetParameterList 
// 
int mdd_GetParameterList (PDRVCONTEXT pDrv, PBYTE pOut, DWORD dwOut, PDWORD pdwBytesWritten) 
{
	int rc, nCnt;

	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("mdd_GetParameterList++ \r\n")));

	// Get size of feature list
	nCnt = pdd_GetFeatureList (pDrv, NULL, 0);
	DWORD dwOutSize = nCnt * sizeof (FEATUREPROP) + sizeof (FEATUREPROPS);

	// See if they want the size of the buffer
	if ((pOut == 0) && pdwBytesWritten)
	{
		__try 
		{
			*pdwBytesWritten = dwOutSize;
			return 0;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) 
		{
			return ERROR_INVALID_PARAMETER;
		}
	}
	// See if output buffer is big enough
	if (dwOut < dwOutSize)
		return ERROR_NOT_ENOUGH_MEMORY;

	// Get the list of feature IDs
	PDWORD pList = (PDWORD)LocalAlloc (LPTR, nCnt * sizeof (DWORD));
	rc = pdd_GetFeatureList (pDrv, pList, nCnt);
	if (rc == 0)  // No features returned
	{
		LocalFree (pList);
		return 0;
	}
	// Query each feature from the PDD and copy to the apps buffer
	FEATUREPROP fp;
	PFEATUREPROPS pfps = (PFEATUREPROPS)pOut;
	__try 
	{
		pfps->nNumProps = nCnt;
		for (int i = 0; i < nCnt; i++)
		{
			rc = pdd_QueryFeature (pDrv, pList[i], &fp);
			if (rc) 
			{
				fp.dwFlags |= FLAG_FP_ERROR;
				fp.nMax = rc;
				fp.nMin = rc;
			}
			pfps->fpArray[i] = fp;
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER) 
	{
		rc = ERROR_INVALID_PARAMETER;
	}
	LocalFree ((PVOID)pList);
	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("mdd_GetParameterList-- rc %d\r\n"),rc));
	return rc;
}

//---------------------------------------------------------------------------------------
// mdd_GetParameter - Handles IOCTL_CAMERA_DEVICE_GETPARAMETER Ioctl command
//
int mdd_GetParameter (PDRVCONTEXT pDrv, PBYTE pIn, DWORD dwIn, PBYTE pOut, DWORD dwOut, 
					  PDWORD pdwBytesWritten) 
{

	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("mdd_GetParameter++\r\n")));

	// Check parameters
	if (!pIn || (dwIn < sizeof (DWORD)) || !pOut || (dwOut < sizeof (DWORD)))
		ERROR_INVALID_PARAMETER;

	DWORD dwFeat;
	DWORD dwVal;
	int rc;

	__try {
		dwFeat = *pIn;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return ERROR_INVALID_PARAMETER;
	}
	rc = pdd_GetParameter (pDrv, dwFeat, &dwVal);
	if (rc == 0)
	{
		__try 
		{
			memcpy (pOut, &dwVal, sizeof (dwVal));
			*pdwBytesWritten = sizeof (dwVal);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			rc = ERROR_INVALID_PARAMETER;
		}
	}
	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("mdd_GetParameter-- rc %d\r\n"),rc));
	return rc;
}

//---------------------------------------------------------------------------------------
// mdd_SetParameter - Handles IOCTL_CAMERA_DEVICE_SETPARAMETER Ioctl command
//
int mdd_SetParameter (PDRVCONTEXT pDrv, PBYTE pIn, DWORD dwIn, PBYTE pOut, DWORD dwOut, 
					  PDWORD pdwBytesWritten) 
{
	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("mdd_SetParameter++\r\n")));

	// Check parameters
	if (!pIn || (dwIn < sizeof (SETFEATURESTRUCT)))
		return ERROR_INVALID_PARAMETER;

	DWORD dwFeat;
	DWORD dwVal;
	PSETFEATURESTRUCT pFeat = (PSETFEATURESTRUCT)pIn;
	int rc;

	__try {
		if (pFeat->cbSize != sizeof (SETFEATURESTRUCT))
			return ERROR_INVALID_PARAMETER;

		dwFeat = pFeat->dwFeatureID;
		dwVal = pFeat->dwVal;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return ERROR_INVALID_PARAMETER;
	}
	rc = pdd_SetParameter (pDrv, dwFeat, dwVal);
	if ((rc == 0) &&  pOut && (dwOut >= sizeof (DWORD)))
	{
		// Now, if the user has specified an out buffer, query the feature just set
		rc = pdd_GetParameter (pDrv, dwFeat, &dwVal);
		if (rc == 0)
		{
			__try 
			{
				memcpy (pOut, &dwVal, sizeof (dwVal));
				*pdwBytesWritten = sizeof (dwVal);
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				rc = ERROR_INVALID_PARAMETER;
			}
		}
	}
	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("mdd_SetParameter-- rc %d\r\n"),rc));
	return rc;
}
//---------------------------------------------------------------------------------------
// mdd_QueryVideoFormats - Handles IOCTL_CAMERA_DEVICE_QUERYVIDEOFORMATS Ioctl command
//
int mdd_QueryVideoFormats (PDRVCONTEXT pDrv, PBYTE pIn, DWORD dwIn, PBYTE pOut, DWORD dwOut, 
						   PDWORD pdwBytesWritten) 
{
	int rc = 0;
	BYTE bFmt, bFrm;

	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("mdd_QueryVideoFormats++\r\n")));

	// Check parameters
	if (!pdwBytesWritten)
		return ERROR_INVALID_PARAMETER;

	// Get the number of frame sizes for each format
	DWORD dwCnt = 0;
	FORMATPROPS Props;
	PFORMATPROPS pPropOut = (PFORMATPROPS)pOut;
	for (bFmt = 0, rc = 0; (bFmt < 255) && (rc == 0); bFmt++)
	{
		DWORD dwCnt1 = dwCnt;
		for (bFrm = 0; (bFrm < 255) && (rc == 0); bFrm++)
		{
			rc = pdd_GetFormatParameters (pDrv, bFmt, bFrm, FALSE, &Props);
			if (rc == 0) 
			{
				if (pPropOut)
				{
					if (dwOut > sizeof (FORMATPROPS))
					{
						// Write the data to the output buffer
						__try 
						{
							*pPropOut = Props;
						}
						__except (EXCEPTION_EXECUTE_HANDLER)
						{
							return ERROR_INVALID_PARAMETER;
						}
						dwOut -= sizeof (FORMATPROPS);
						pPropOut++;
					} 
					else
						return ERROR_NOT_ENOUGH_MEMORY;
				}
				dwCnt++;
			}
		}
		// Did we get any frame for the current format?
		if (dwCnt1 != dwCnt)
			rc = 0;
	}
	__try 
	{
		*pdwBytesWritten = dwCnt * sizeof (FORMATPROPS);
	}
	__except (EXCEPTION_EXECUTE_HANDLER) 
	{
		rc = ERROR_INVALID_PARAMETER;
	}
	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("mdd_QueryVideoFormats-- rc %d\r\n"),rc));
	return rc;
}

//---------------------------------------------------------------------------------------
// Handles IOCTL_CAMERA_DEVICE_GETVIDEOFORMAT Ioctl command
//
int mdd_GetVideoFormat (PDRVCONTEXT pDrv, PBYTE pIn, DWORD dwIn, PBYTE pOut, DWORD dwOut, 
						PDWORD pdwBytesWritten) 
{
	int rc;
	int nFrameIndex, nFormatIndex;

	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("mdd_GetVideoFormat++\r\n")));

	if (!pIn || (dwIn < sizeof (VIDFORMATSTRUCT)) || !pOut || 
		(dwOut < sizeof (FORMATPROPS)) || !pdwBytesWritten)
		return ERROR_INVALID_PARAMETER;

	PVIDFORMATSTRUCT pVid = (PVIDFORMATSTRUCT)pIn;
	__try 
	{	
		if (pVid->cbSize != sizeof (VIDFORMATSTRUCT))
		{
			DEBUGMSG (ZONE_ERROR, (DTAG TEXT("Size field of VIDFORMATSTRUCT not set\r\n")));
			return ERROR_INVALID_PARAMETER;
		}
		nFormatIndex = pVid->wFormatIndex;
		nFrameIndex = pVid->wFrameIndex;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		DEBUGMSG (ZONE_ERROR, (TEXT("Exception reading VIDFORMATSTRUCT structure\r\n")));
		return ERROR_INVALID_PARAMETER;
	}

	// Get the data
	FORMATPROPS Props;
	rc = pdd_GetFormatParameters (pDrv, nFormatIndex, nFrameIndex, FALSE, &Props);
	if (rc == 0)
	{
		__try 
		{	
			memcpy (pOut, &Props, sizeof (FORMATPROPS));
			*pdwBytesWritten = sizeof (FORMATPROPS);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			DEBUGMSG (ZONE_ERROR, (TEXT("Exception writing FORMATPROPS structure\r\n")));
			rc = ERROR_INVALID_PARAMETER;
		}
	}
	else
	{
		DEBUGMSG (ZONE_ERROR, (TEXT("Error calling pdd_GetFormatParameters rc %d\r\n"), rc));
		rc = ERROR_INVALID_PARAMETER;
	}
	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("mdd_GetVideoFormat-- rc %d\r\n"),rc));
	return rc;
}

//---------------------------------------------------------------------------------------
// mdd_SetVideoFormat - Handles IOCTL_CAMERA_DEVICE_STARTVIDEOSTREAM
//
int mdd_SetVideoFormat (PDRVCONTEXT pDrv, PBYTE pIn, DWORD dwIn, PDWORD pdwBytesWritten) 
{
	int rc;
	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("mdd_SetVideoFormat++\r\n")));

	if (!pIn || (dwIn < sizeof (STARTVIDSTRUCT)) || !pdwBytesWritten)
		return ERROR_INVALID_PARAMETER;

	// Copying the struture will verify access.  We'll need to use the original
	// pointer when we call the PDD.
	STARTVIDSTRUCT vsData;
	__try 
	{	
		// Copy the data inside the driver
		vsData = *(PSTARTVIDSTRUCT)pIn;

		// verify the size of the structure
		if (vsData.cbSize != sizeof (STARTVIDSTRUCT))
		{
			DEBUGMSG (ZONE_ERROR, (TEXT("cbSize field of STARTVIDSTRUCT structure doesn't match structure size\r\n")));
			return ERROR_INVALID_PARAMETER;
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		DEBUGMSG (ZONE_ERROR, (TEXT("Exception reading STARTVIDSTRUCT structure\r\n")));
		return ERROR_INVALID_PARAMETER;
	}

	// See if for some reason, the app is trying a new stream before shutting down
	// the old one.
	if (pDrv->mmFrameBuff.p)
	{
		DEBUGMSG (ZONE_ERROR, (DTAG TEXT("Starting a new stream while another is running!\r\n")));
		return ERROR_BUSY;
	}

	// Get data about the requested format
	FORMATPROPS Props;
	rc = pdd_GetFormatParameters (pDrv, (BYTE)vsData.wFormatIndex, (BYTE)vsData.wFrameIndex, FALSE, &Props);
	if (rc)
	{
		DEBUGMSG (ZONE_ERROR, (DTAG TEXT("Bad still format requested Fmt:%d Frame:%d\r\n"), 
		          vsData.wFormatIndex, vsData.wFrameIndex));
		return ERROR_INVALID_PARAMETER;
	}

	//
	// Create the frame buffers.  Use a memory mapped object so we won't have to copy
	// the frame data to the calling application.
	//

	// Verify that we at least have three buffers
	if (vsData.dwNumBuffs < 3)
		vsData.dwNumBuffs = 3;

	// Allocate the memory mapped object
	rc = AllocSharedObject (sizeof(VIDSTREAMSTRUCT) + 
	                    ((Props.dwMaxBuff + vsData.dwPreBuffSize + vsData.dwPostBuffSize) * vsData.dwNumBuffs), 
						&pDrv->mmFrameBuff);
	if (rc)
	{
		DEBUGMSG (ZONE_ERROR, (DTAG TEXT("Can't allocate memory mapped object rc = %d\r\n"), rc));
		return ERROR_NOT_ENOUGH_MEMORY;
	}

	PVIDSTREAMSTRUCT pVid = (PVIDSTREAMSTRUCT)pDrv->mmFrameBuff.p;

	memset (pVid, 0, sizeof (VIDSTREAMSTRUCT));
	pVid->wFormatIndex = vsData.wFormatIndex;
	pVid->wFrameIndex = vsData.wFrameIndex;
	pVid->dwInterval = vsData.dwInterval;
	pVid->dwBuffSize = Props.dwMaxBuff;
	pVid->dwNumBuffs = vsData.dwNumBuffs;
	pVid->dwReserved = (DWORD)CreateEvent (NULL, FALSE, FALSE, NULL);
	InitializeCriticalSection (&pVid->csBuffSync);

	// Fill in the buffer pointers.
	for (DWORD i = 0; i < vsData.dwNumBuffs; i++)
	{
		pVid->pFrame[i].pBuff = pDrv->mmFrameBuff.p +                 // Start of mm object
		             sizeof (VIDSTREAMSTRUCT) +                       // Stream struct size
		             ((vsData.dwNumBuffs-1) * sizeof(DWORD)) +        // Size of ptr array after structure
			         vsData.dwPreBuffSize +                           // Offset into buff for prebuff
		             ((Props.dwMaxBuff + vsData.dwPostBuffSize) * i); // Size of each buffer inc post buff

		// Dump the buffer locations for debugging
		DEBUGMSG (ZONE_VIDFRAME, (DTAG TEXT ("MM buffer at %0x\r\n"), pVid->pFrame[i].pBuff));
	}

	if (rc == 0)
	{
		// Start the video stream
		rc = pdd_StartVidStream (pDrv, (BYTE)vsData.wFormatIndex, (BYTE)vsData.wFrameIndex, 
								 pVid, vsData.dwInterval); 
	}
	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("mdd_SetVideoFormat-- rc %d\r\n"),rc));
	return rc;
}

//---------------------------------------------------------------------------------------
// mdd_GetNextVideoFrame 
//
int mdd_GetNextVideoFrame (PDRVCONTEXT pDrv, PBYTE pIn, DWORD dwIn, PBYTE pOut, DWORD dwOut, PDWORD pdwBytesWritten) 
{
	int rc;
	PBYTE pFrameBuff = 0;
	PBYTE *ppFrameBuff;
	PBYTE pFrameRet;
	DWORD dwValidFrames, dwFrameBytes, dwTimeout;
	DWORD dwFrameFlags;

	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("mdd_GetNextVideoFrame++\r\n")));

	if (!pIn || (dwIn < sizeof (GETFRAMESTRUCT)) || !pOut || (dwOut < sizeof (GETFRAMESTRUCTOUT)) || !pdwBytesWritten)
		return ERROR_INVALID_PARAMETER;

	PGETFRAMESTRUCT pGetFrStruct = (PGETFRAMESTRUCT)pIn;
	PGETFRAMESTRUCTOUT pGetFrStructOut = (PGETFRAMESTRUCTOUT)pOut;
	__try 
	{	
		// Test the structure by checking the size field. For speed reasons, I'm 
		// not copying the entire structure into the driver.
		if (pGetFrStruct->cbSize != sizeof (GETFRAMESTRUCT))
		{
			DEBUGMSG (ZONE_ERROR, (TEXT("Bad size field in GETFRAMESTRUCT structure\r\n")));
			return ERROR_INVALID_PARAMETER;
		}

		// Test the out structure by checking the size field. 
		if (pGetFrStructOut->cbSize != sizeof (GETFRAMESTRUCTOUT))
		{
			DEBUGMSG (ZONE_ERROR, (TEXT("Bad size field in GETFRAMESTRUCTOUT structure\r\n")));
			return ERROR_INVALID_PARAMETER;
		}

		// See if we're already in this call, waiting on a frame
		if (pDrv->fBusy)
		{
			DEBUGMSG (ZONE_ERROR, (TEXT("Error. Already in GetNextFrame call.\r\n")));
			return ERROR_BUSY;
		}

		// See if app wants a new frame
		if (pGetFrStruct->dwFlags & GETFRAMEFLAG_NOFRAME_WANTED)
			ppFrameBuff = 0;
		else
			ppFrameBuff = &pFrameBuff;

		// See if app is returning a buffer
		if (pGetFrStruct->dwFlags & GETFRAMEFLAG_FREEBUFF_VALID)
			pFrameRet = pGetFrStruct->pFrameDataRet;
		else
			pFrameRet = 0;

		// Get timeout value
		if (pGetFrStruct->dwFlags & GETFRAMEFLAG_TIMEOUT_VALID)
			dwTimeout = pGetFrStruct->dwTimeout;
		else
			dwTimeout = DEFNEXTFRAMETIMEOUT;

		// See if the caller wants the oldest or newest frame
		dwFrameFlags = 0;
		if (pGetFrStruct->dwFlags & GETFRAMEFLAG_GET_OLDESTFRAME)
		{
			dwFrameFlags |= FRAME_OLDEST;
		}
		else if (pGetFrStruct->dwFlags & GETFRAMEFLAG_GETNEWESTAVAILABLE)
		{
			dwFrameFlags |= FRAME_NEWESTAVAIL;
		}

		// See if they want to purge other frames from the queue
		if (pGetFrStruct->dwFlags & GETFRAMEFLAG_PURGEOLDERFRAMES)
			dwFrameFlags |= FRAME_PURGEOLD; 
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		DEBUGMSG (ZONE_ERROR, (TEXT("Exception reading GETFRAMESTRUCT structure\r\n")));
		return ERROR_INVALID_PARAMETER;
	}

	// Don't allow another call to GetNextFrame while we're waiting
	pDrv->fBusy = TRUE;  

	// Get the data, first leave the driver CS.  since this can block.  
	LeaveCriticalSection (&pDrv->csDCall);
	__try 
	{
		rc = pdd_GetNextVideoFrame (pDrv, ppFrameBuff, &dwFrameBytes, 
		                            &dwValidFrames, pFrameRet, dwTimeout, dwFrameFlags);
	}
	__finally
	{
		EnterCriticalSection (&pDrv->csDCall);
		pDrv->fBusy = FALSE;  
	}
	if (rc == 0)
	{
		// Write to the output structure
		__try 
		{	
			pGetFrStructOut->dwMissedFrames = dwValidFrames;
			pGetFrStructOut->pFrameData = pFrameBuff;
			pGetFrStructOut->dwFrameSize = dwFrameBytes;

			*pdwBytesWritten = sizeof (GETFRAMESTRUCTOUT);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			DEBUGMSG (ZONE_ERROR, (TEXT("Exception writing output data.\r\n")));
			rc = ERROR_INVALID_PARAMETER;
		}
	}
	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("mdd_GetNextVideoFrame-- rc %d\r\n"),rc));
	return rc;
}

//---------------------------------------------------------------------------------------
// mdd_GetCurrentFormat - Called to process IOCTL_CAMERA_DEVICE_GETCURRENVIDEOFORMAT ioctl
//
int mdd_GetCurrentFormat (PDRVCONTEXT pDrv, PBYTE pOut, DWORD dwOut, PDWORD pdwBytesWritten) 
{
	int rc;
	FORMATPROPS Props;
	PFORMATPROPS pPropsOut;
	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("mdd_GetCurrentFormat++\r\n")));

	if (!pOut || (dwOut < sizeof (FORMATPROPS)) || !pdwBytesWritten)
		return ERROR_INVALID_PARAMETER;

	// Write to the output structure
	__try 
	{	
		*pdwBytesWritten = 0;

		pPropsOut = (PFORMATPROPS)pOut;

		if (pPropsOut->cbSize != sizeof (FORMATPROPS))
		{
			DEBUGMSG (ZONE_ERROR, (TEXT("Bad structure size\r\n")));
			rc = ERROR_INVALID_PARAMETER;
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		DEBUGMSG (ZONE_ERROR, (TEXT("Exception writing output data.\r\n")));
		rc = ERROR_INVALID_PARAMETER;
	}

	rc = pdd_GetCurrentFormat (pDrv, &Props);
	if (rc == 0)
	{
		// Write to the output structure
		__try 
		{	
			*pPropsOut = Props;
			*pdwBytesWritten = sizeof (FORMATPROPS);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			DEBUGMSG (ZONE_ERROR, (TEXT("Exception writing output data.\r\n")));
			rc = ERROR_INVALID_PARAMETER;
		}
	}
	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("mdd_GetCurrentFormat-- rc %d\r\n"),rc));
	return rc;
}

//---------------------------------------------------------------------------------------
// mdd_StopVideoStream - Called to process IOCTL_CAMERA_DEVICE_STOPVIDEOSTREAM ioctl
//
int mdd_StopVideoStream (PDRVCONTEXT pDrv) 
{
	int rc;

	// Stop the stream
	rc = pdd_StopVidStream (pDrv);

	// If we allocated the frame buffer, free it, 
	if (pDrv->mmFrameBuff.p)
	{
		// Free up the current frame buffer
		FreeSharedObject (&pDrv->mmFrameBuff);
		pDrv->mmFrameBuff.p = 0;
	}

	return rc;
}
//---------------------------------------------------------------------------------------
// mdd_QueryStillFormats - Returns the supported still image formats
//
int mdd_QueryStillFormats (PDRVCONTEXT pDrv, PBYTE pIn, DWORD dwIn, PBYTE pOut, DWORD dwOut, 
						   PDWORD pdwBytesWritten)
{
	int i, rc = 0;
	FORMATPROPS Props;
	DWORD dwStillFmt, dwBytes = 0;

	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("mdd_QueryStillFormats++\r\n")));

	// Check parameters
	if (!pIn || (dwIn < sizeof (DWORD)) || !pdwBytesWritten)
		ERROR_INVALID_PARAMETER;

	// Get the format the app wants
	__try {
		dwStillFmt = *pIn;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return ERROR_INVALID_PARAMETER;
	}
	// Format is actually a byte paramter
	if (dwStillFmt > 255)
		return ERROR_INVALID_PARAMETER;

	PFORMATPROPS pOutFmtProps = (PFORMATPROPS)pOut;

	// See if we're currently streaming
	rc = pdd_GetCurrentFormat (pDrv, &Props);

	// Yes, we're streaming, just return the current format
	if (rc == 0)
	{
		// Write it to the app. but only if they provided an output pointer
		if (pOutFmtProps)
		{
			__try 
			{
				pOutFmtProps[0] = Props;
			}
			__except (EXCEPTION_EXECUTE_HANDLER) 
			{
				DEBUGMSG (ZONE_ERROR, (TEXT("Exception writing to output buffer\r\n")));
				rc = ERROR_INVALID_PARAMETER;
			}
		}
		dwBytes += sizeof (FORMATPROPS);
	}
	else
	{
		// If not streaming, all formats are available
		// Get the number of frame sizes for the format
		for (i = 0, rc = 0; (i < 255) && (rc == 0); i++)
		{
			rc = pdd_GetFormatParameters (pDrv, (BYTE)dwStillFmt, (BYTE)i+1, FALSE, &Props);
			// if no more frame sizes, break
			if (rc)
			{
				rc = 0;
				break;
			}
			// Write it to the app. but only if they provided an output pointer
			if (pOutFmtProps)
			{
				__try 
				{
					pOutFmtProps[i] = Props;
				}
				__except (EXCEPTION_EXECUTE_HANDLER) 
				{
					DEBUGMSG (ZONE_ERROR, (TEXT("Exception writing to output buffer\r\n")));
					rc = ERROR_INVALID_PARAMETER;
					break;
				}
			}
			dwBytes += sizeof (FORMATPROPS);
		}
	}
	// Write the number of bytes written
	__try 
	{
		*pdwBytesWritten = dwBytes;
	}
	__except (EXCEPTION_EXECUTE_HANDLER) 
	{
		DEBUGMSG (ZONE_ERROR, (TEXT("Exception writing pdwBytesWritten\r\n")));
		rc = ERROR_INVALID_PARAMETER;
	}
	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("mdd_QueryStillFormats-- rc %d\r\n"),rc));
	return rc;
}

//---------------------------------------------------------------------------------------
// mdd_GetStillImage 
//
int mdd_GetStillImage (PDRVCONTEXT pDrv, PBYTE pIn, DWORD dwIn, PBYTE pOut, DWORD dwOut, 
					   PDWORD pdwBytesWritten)
{
	int rc, nFrameIndex, nFormatIndex;
	FORMATPROPS Props;

	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("mdd_GetStillImage++\r\n")));

	if (!pIn || !pdwBytesWritten || (dwIn < sizeof(VIDFORMATSTRUCT)))
		return ERROR_INVALID_PARAMETER;

	PVIDFORMATSTRUCT pVid = (PVIDFORMATSTRUCT)pIn;
	__try 
	{	
		if (pVid->cbSize != sizeof (VIDFORMATSTRUCT))
		{
			DEBUGMSG (ZONE_ERROR, (DTAG TEXT("Size field of VIDFORMATSTRUCT not set\r\n")));
			return ERROR_INVALID_PARAMETER;
		}
		nFormatIndex = pVid->wFormatIndex;
		nFrameIndex = pVid->wFrameIndex;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		DEBUGMSG (ZONE_ERROR, (TEXT("Exception reading VIDFORMATSTRUCT structure\r\n")));
		return ERROR_INVALID_PARAMETER;
	}
	// See if currently streaming.  If so, the format and frame must match the 
	// current stream.
	rc = pdd_GetCurrentFormat (pDrv, &Props);

	// Yes, we're streaming, just return the current format
	if (rc == 0)
	{
		if ((nFormatIndex != Props.wFormatIndex) || (nFrameIndex != Props.wFrameIndex))
		{
			DEBUGMSG (ZONE_ERROR, (TEXT("Can only get still if format matches current stream\r\n")));
			return ERROR_BUSY;
		}
	}

	// Verify that the output buffer is large enough
	rc = pdd_GetFormatParameters (pDrv, nFormatIndex, nFrameIndex, FALSE, &Props);
	if (rc)
	{
		DEBUGMSG (ZONE_ERROR, (DTAG TEXT("Bad still format requested %d\r\n"), nFrameIndex));
		return ERROR_INVALID_PARAMETER;
	}

	// See if they're just asking for the buffer size...
	if (pOut == 0)
	{
		__try 
		{	
			*pdwBytesWritten = Props.dwMaxBuff;
			return 0;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			DEBUGMSG (ZONE_ERROR, (TEXT("Exception writing pdwBytesWritten\r\n")));
			return ERROR_INVALID_PARAMETER;
		}
	}
	// See if output buffer is big enough
	if (dwOut < Props.dwMaxBuff)
		return ERROR_NOT_ENOUGH_MEMORY;

	// Call the PDD to get the image.  Since we're directly accessing the
	// callers buffer here, protect with a try/except
	__try
	{
		rc = pdd_GetStillImage (pDrv, nFrameIndex, nFormatIndex, pOut, dwOut, pdwBytesWritten);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		DEBUGMSG (ZONE_ERROR, (TEXT("Exception writing to output buffer\r\n")));
		return ERROR_INVALID_PARAMETER;
	}
	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("mdd_GetStillImage-- rc %d\r\n"),rc));
	return rc;
}

//---------------------------------------------------------------------------------------
// mdd_WaitOnCameraEvent - Waits on events within the driver. 
//
int mdd_WaitOnCameraEvent (PDRVCONTEXT pDrv, PBYTE pIn, DWORD dwIn, PBYTE pOut, DWORD dwOut, 
						   PDWORD pdwBytesWritten)
{
	int rc;
	DWORD dwWaitMask, dwTimeout, dwEventReason;

	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("mdd_WaitOnCameraEvent++\r\n")));

	if (!pIn || !pOut || !pdwBytesWritten || 
		(dwIn < sizeof(WAITEVENTSTUCT)) || (dwIn < sizeof(DWORD)))
		return ERROR_INVALID_PARAMETER;

	// Get the data from the input buffer
	PWAITEVENTSTUCT pWait = (PWAITEVENTSTUCT)pIn;
	__try 
	{	
		if (pWait->cbSize != sizeof (WAITEVENTSTUCT))
		{
			DEBUGMSG (ZONE_ERROR, (DTAG TEXT("Size field of WAITEVENTSTUCT not set\r\n")));
			return ERROR_INVALID_PARAMETER;
		}
		dwWaitMask = pWait->dwWaitMask;

		dwTimeout = pWait->dwTimeout;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		DEBUGMSG (ZONE_ERROR, (TEXT("Exception reading WAITEVENTSTUCT structure\r\n")));
		return ERROR_INVALID_PARAMETER;
	}
	// See if we can write to the output buffer
	__try 
	{	
		DWORD dwTest = *(DWORD *)pOut;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		DEBUGMSG (ZONE_ERROR, (TEXT("Exception reading output buffer\r\n")));
		return ERROR_INVALID_PARAMETER;
	}

	// Validate the wait flags
	if ((dwWaitMask & ~(WAITCAM_NEWFRAME /* & WAITCAM_BUTTONPRESS */)) != 0)
	{
		DEBUGMSG (ZONE_ERROR, (TEXT("Invalid wait flags passed\r\n")));
		return ERROR_INVALID_PARAMETER;
	}

	// Call the PDD to wait, first leave the driver CS.  since this can block.  
	LeaveCriticalSection (&pDrv->csDCall);
	__try 
	{
		rc = pdd_WaitOnCameraEvent (pDrv, dwWaitMask, dwTimeout, &dwEventReason);
	}
	__finally
	{
		EnterCriticalSection (&pDrv->csDCall);
	}
	// Write the results to the output buffer
	__try 
	{	
		*(DWORD *)pOut = dwEventReason;
		*pdwBytesWritten = sizeof (DWORD);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		DEBUGMSG (ZONE_ERROR, (TEXT("Exception writing output buffer\r\n")));
		return ERROR_INVALID_PARAMETER;
	}
	DEBUGMSG (ZONE_FUNC, (DTAG TEXT("mdd_WaitOnCameraEvent-- rc %d\r\n"),rc));
	return rc;
}
//----------------------------------------------------------------------
// AllocSharedObject - Allocates a memory mapped object
//
int AllocSharedObject (int nSize, PMMOBJSTRUCT obj)
{
	if ((obj == 0) || (nSize == 0))
		return ERROR_INVALID_PARAMETER;

	// For 6.0, we can't use a Shared MM object any more.  Use the shared
	// heap.  Its R/W from the driver and R/O for the app, but that's fine.
#if (_WIN32_WCE >= 600)

	obj->p = (PBYTE) CeVirtualSharedAlloc (NULL, nSize, MEM_COMMIT|MEM_RESERVE);

#else
	obj->h = CreateFileMapping ((HANDLE)-1, NULL, PAGE_READWRITE | SEC_COMMIT,
	                            0, nSize, NULL);
	if (obj->h == 0)
		return GetLastError();
	obj->p = (PBYTE)MapViewOfFile (obj->h, FILE_MAP_WRITE, 0, 0, 0);
	if (obj->h == 0)
	{
		CloseHandle (obj->h);
		return GetLastError();
	}
#endif //_WIN32_WCE >= 600
	return 0;
}
//----------------------------------------------------------------------
// FreeSharedObject - Frees a memory mapped object
//
int FreeSharedObject (PMMOBJSTRUCT obj)
{
	if (obj == 0)
		return ERROR_INVALID_PARAMETER;

	// For 6.0, we can't use a Shared MM object any more.  Use the shared
	// heap.  Its R/W from the driver and R/O for the app, but that's fine.
#if (_WIN32_WCE >= 600)

	VirtualFree (obj->p, 0, MEM_RELEASE);

#else

	UnmapViewOfFile (obj->p);
	CloseHandle (obj->h);
#endif //_WIN32_WCE >= 600
	return 0;
}
