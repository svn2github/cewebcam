//-------------------------------------------------------------------------
// <copyright file="Webcam.h" company="Microsoft">
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
//    WebCam Driver Header file.
// </summary>
//-------------------------------------------------------------------------
//======================================================================
// Header file
//
// Author: Douglas Boling
//======================================================================

// Enable the following to redirect DEBUGMSG macro out a console
#if 0
#ifdef DEBUG
#undef DEBUGMSG
#define DEBUGMSG(cond,printf_exp)   \
   ((void)((cond)?(wprintf printf_exp),1:0))
#endif
#endif


#define dim(a)  (sizeof(a)/sizeof(a[0]))
//
// Debug zone support
//
// Used as a prefix string for all debug zone messages.
#define DTAG        TEXT ("WebCam: ")

// Debug zone constants
#define ZONE_ERROR      DEBUGZONE(0)
#define ZONE_WARNING    DEBUGZONE(1)
#define ZONE_FUNC       DEBUGZONE(2)
#define ZONE_INIT       DEBUGZONE(3)
#define ZONE_DRVCALLS   DEBUGZONE(4)
#define ZONE_EXENTRY  (ZONE_FUNC | ZONE_DRVCALLS)
#define ZONE_USBLOAD    DEBUGZONE(5)
#define ZONE_THREAD     DEBUGZONE(6)
#define ZONE_THSTATE	DEBUGZONE(7)
#define ZONE_PACKETS    DEBUGZONE(8)
#define ZONE_TIMING     DEBUGZONE(9)
#define ZONE_READDATA   DEBUGZONE(10)
#define ZONE_PROBE      DEBUGZONE(11)
#define ZONE_STILL      DEBUGZONE(12)
#define ZONE_VIDFRAME   DEBUGZONE(13)
#define ZONE_FEATURE    DEBUGZONE(14)
#define ZONE_DESCRIPTORS DEBUGZONE(15)

//
// Declare the driver entry points here. 
//
#ifdef __cplusplus
extern "C" {
#endif //__cplusplus
DWORD CAM_Init (DWORD dwContext);
BOOL  CAM_Deinit (DWORD dwContext);
DWORD CAM_Open (DWORD dwContext, DWORD dwAccess, DWORD dwShare);
BOOL  CAM_Close (DWORD dwOpen);
DWORD CAM_Read (DWORD dwOpen, LPVOID pBuffer, DWORD dwCount);
DWORD CAM_Write (DWORD dwOpen, LPVOID pBuffer, DWORD dwCount);
DWORD CAM_Seek (DWORD dwOpen, long lDelta, WORD wType);
DWORD CAM_IOControl (DWORD dwOpen, DWORD dwCode, 
                     PBYTE pIn, DWORD dwIn,
                     PBYTE pOut, DWORD dwOut, 
                     DWORD *pdwBytesWritten);
void CAM_PowerDown (DWORD dwContext);
void CAM_PowerUp (DWORD dwContext);

#ifdef __cplusplus
} // extern "C"
#endif //__cplusplus

//
// Our current version
//
#define DRV_MAJORVER   0
#define DRV_MINORVER   9
//
// USB constants
//

#define DRIVER_NAME   TEXT("Webcam.dll")

#define DEVICE_PREFIX TEXT("CAM")

#define CLASS_NAME_SZ    TEXT("Video_Class")
#define CLIENT_REGKEY_SZ TEXT("Drivers\\USB\\ClientDrivers\\Video_Class")

//
// USB Video Interface Descriptor
//
#define USB_MISC_INTERFACE_CLASS       0xEF
#define USB_COMMON_INTERFACE_SUBCLASS  0x02
#define USB_DEVICE_CLASS_VIDEO         0x0E

//
// USB_DRIVER_SETTINGS
//
#define DRIVER_SETTINGS \
            sizeof(USB_DRIVER_SETTINGS),  \
            USB_NO_INFO,   \
            USB_NO_INFO,   \
            USB_NO_INFO,   \
            USB_NO_INFO,   \
            USB_NO_INFO,   \
            USB_NO_INFO,   \
            USB_DEVICE_CLASS_VIDEO,   \
            USB_NO_INFO,   \
            USB_NO_INFO

// Used to describe a memory mapped object
typedef struct {
	HANDLE h;
	PBYTE p;
} MMOBJSTRUCT, *PMMOBJSTRUCT;

//
// Driver instance structure  
//
typedef struct {
    DWORD dwSize;
	CRITICAL_SECTION csDCall;
    INT nNumOpens;
	HANDLE hStreamDevice;
	// USB support
	USB_HANDLE hDevice;
	LPCUSB_FUNCS lpUsbFuncs;
	HANDLE hUnload;
	BOOL fUnloadFlag;
	// Driver stuff
	MMOBJSTRUCT mmFrameBuff;
	DWORD dwPddContext;
	BOOL fBusy;             // Added in 0.6 in case of multiple GetNextFrame calls
} DRVCONTEXT, *PDRVCONTEXT;

//
// Structure used by the driver to manage the frame buffers in the 
// memory mapped object.  In almost all cases, applications
// don't need to know about this structure.
//
typedef struct {
	DWORD cbSize;				// Size of the structure
	WORD  wFormatIndex;			// Video format index
	WORD  wFrameIndex;			// Video frame size index
	DWORD dwInterval;			// Video frame interval 
	HANDLE hNewFrame;			// New Frame event returned to caller
	DWORD dwLastGoodBuff;		// Index of buff containing latest complete frame
	DWORD dwCurrBuff;			// Index of buff currenting being written to by driver
	DWORD dwValidFrames;		// Number of frames since last GetFrame call (will be at least 1)
	DWORD dwNumBuffs;			// Number of buffers
	DWORD dwBuffSize;			// Max Frame size of each buffer 
								// Real buffer size == dwBuffSize + dwPreBuffSize + dwPostBuffSize
	DWORD dwReserved;			// Used by driver to hold local handle of new frame event
	CRITICAL_SECTION csBuffSync;// Used by driver to hold critical section (unused by app)
	FRAMEBUFFSTRUCT pFrame[1];	// First value in array of buffer structures
} VIDSTREAMSTRUCT, *PVIDSTREAMSTRUCT;

// Default next frame time out value.  This is long because
// a still capture can take a long time and interrupt the
// video stream.
#define DEFNEXTFRAMETIMEOUT  5000

// Used in pdd_GetNextVideoFrame
#define FRAME_OLDEST         0x0001
#define FRAME_NEWESTAVAIL    0x0002
#define FRAME_PURGEOLD       0x0004

PDRVCONTEXT GetConfigData (DWORD dwContext);

//
// MDD functions
//
int mdd_GetVersion (PDRVCONTEXT pDrv, PBYTE pOut, DWORD dwOut, PDWORD pdwBytesWritten);
int mdd_GetParameterList (PDRVCONTEXT pDrv, PBYTE pOut, DWORD dwOut, PDWORD pdwBytesWritten);
int mdd_GetParameter (PDRVCONTEXT lpDrv, PBYTE pIn, DWORD dwIn, PBYTE pOut, DWORD dwOut, PDWORD pdwBytesWritten);
int mdd_SetParameter (PDRVCONTEXT pDrv, PBYTE pIn, DWORD dwIn, PBYTE pOut, DWORD dwOut, PDWORD pdwBytesWritten);
int mdd_QueryVideoFormats (PDRVCONTEXT pDrv, PBYTE pIn, DWORD dwIn, PBYTE pOut, DWORD dwOut, PDWORD pdwBytesWritten);
int mdd_GetVideoFormat (PDRVCONTEXT pDrv, PBYTE pIn, DWORD dwIn, PBYTE pOut, DWORD dwOut, PDWORD pdwBytesWritten);
int mdd_SetVideoFormat (PDRVCONTEXT lpDrv, PBYTE pIn, DWORD dwIn, PDWORD pdwBytesWritten);
int mdd_GetNextVideoFrame (PDRVCONTEXT pDrv, PBYTE pIn, DWORD dwIn, PBYTE pOut, DWORD dwOut, PDWORD pdwBytesWritten);
int mdd_GetCurrentFormat (PDRVCONTEXT lpDrv, PBYTE pOut, DWORD dwOut, PDWORD pdwBytesWritten);
int mdd_StopVideoStream (PDRVCONTEXT lpDrv);
int mdd_QueryStillFormats (PDRVCONTEXT pDrv, PBYTE pIn, DWORD dwIn, PBYTE pOut, DWORD dwOut, PDWORD pdwBytesWritten);
int mdd_GetStillImage (PDRVCONTEXT pDrv, PBYTE pIn, DWORD dwIn, PBYTE pOut, DWORD dwOut, PDWORD pdwBytesWritten);
int mdd_WaitOnCameraEvent (PDRVCONTEXT pDrv, PBYTE pIn, DWORD dwIn, PBYTE pOut, DWORD dwOut, PDWORD pdwBytesWritten);


int AllocSharedObject (int nSize, PMMOBJSTRUCT obj);
int FreeSharedObject (PMMOBJSTRUCT obj);

//
// PDD Interface
//

int pdd_DeviceAttach (USB_HANDLE hDevice, LPCUSB_FUNCS lpUsbFuncs,
                      LPCUSB_INTERFACE lpInterface, LPCWSTR szUniqueDriverId,
                      LPCUSB_DRIVER_SETTINGS lpDriverSettings, PDWORD pdwContext);
int pdd_DeviceDetach (PDRVCONTEXT pDrv);

int pdd_DrvInit (PDRVCONTEXT);
void pdd_DrvUninit (PDRVCONTEXT);

int pdd_DrvOpen (PDRVCONTEXT);
void pdd_DrvClose (PDRVCONTEXT);

int pdd_GetFeatureList (PDRVCONTEXT pDrv, DWORD *, int);
int pdd_QueryFeature (PDRVCONTEXT pDrv, DWORD, PFEATUREPROP);
int pdd_GetParameter (PDRVCONTEXT pDrv, DWORD, PDWORD);
int pdd_SetParameter (PDRVCONTEXT pDrv, DWORD, DWORD);

int pdd_GetFormatParameters (PDRVCONTEXT pDrv, BYTE bFormatIndex, BYTE bFrameIndex, BOOL fStill, 
							 PFORMATPROPS pProps); 
int pdd_StartVidStream (PDRVCONTEXT pDrv, BYTE bFormatIndex, BYTE bFrameIndex, PVIDSTREAMSTRUCT pstrStream, 
						DWORD dwFrameInterval);
// Modified in 0.6
//int pdd_GetNextVideoFrame (PDRVCONTEXT pDrv, BOOL fGetLatest, PBYTE *ppFrameBuff, DWORD *dwFrameBytes, 
//						   DWORD *pdwValidFrames, PBYTE pFrameReturn, DWORD dwTimeout);
int pdd_GetNextVideoFrame (PDRVCONTEXT pDrv, PBYTE *ppFrameBuff, DWORD *dwFrameBytes, 
						   DWORD *pdwValidFrames, PBYTE pFrameReturn, DWORD dwTimeout, DWORD dwFrameFlags);

int pdd_StopVidStream (PDRVCONTEXT pDrv);
int pdd_GetCurrentFormat (PDRVCONTEXT pDrv, PFORMATPROPS pProps);
int pdd_GetStillImage (PDRVCONTEXT pDrv, int nFrameIndex, int nFormatIndex, PBYTE pOut, DWORD dwOut, 
					   PDWORD pdwBytesWritten);

int pdd_WaitOnCameraEvent (PDRVCONTEXT pDrv, DWORD dwEventMask, DWORD dwTimeout, DWORD *pdwSignaledEvent);
