//-------------------------------------------------------------------------
// <copyright file="USBVideo.h" company="Microsoft">
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
//    Defines useful for USB driver Interfaces for USB Video Spec
// </summary>
//-------------------------------------------------------------------------
//======================================================================
// USBVideo.h - Defines useful for USB driver interfaceing with 
// devices compiling with the USB Video Specification
//
// Author: Douglas Boling
//======================================================================

typedef struct {
		USB_PIPE hPipe;
		UCHAR ucAddr;
		WORD wPacketSize;
		HANDLE hEvent;
} PIPE, *LPPIPE;

typedef struct {
	USB_INTERFACE_DESCRIPTOR ifDesc;
	PUSBVIDCTLIFDESCRIPTOR lpifExtDesc;
	BOOL fEndpoint;
	USB_ENDPOINT_DESCRIPTOR epDesc;
	LPVOID lpepExtDesc;
} USBCTLIF, *PUSBCTLIF;

typedef struct {
	USB_INTERFACE_DESCRIPTOR ifDesc;
	PUSBVIDSTREAMIFDESCRIPTOR lpifExtDesc;
	BOOL fEndpoint;
	USB_ENDPOINT_DESCRIPTOR epDesc;
	LPVOID lpepExtDesc;
} USBSTRMIF, *PUSBSTRMIF;

//typedef struct {
//	FEATUREPROP fpFeature;
//	DWORD dwB

//
// Isoc read thread states
//
#define STREAMTHD_STOPPED    0
#define STREAMTHD_FRAMESYNC  1
#define STREAMTHD_FIRSTCAP   2
#define STREAMTHD_CAPTURING  3
#define STREAMTHD_IDLE       4

//
// Copy of the strcture used to stream data to 
// application.  The driver needs to stream to itself
// during still capture according to the spec.  This
// struct differs in that it defaults to 2 buffers.
//
// This structure MUST track VIDSTREAMSTRUCT in WebCamSDK.h
//
#define NUMDEFBUFFS 2
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
	FRAMEBUFFSTRUCT pFrame[NUMDEFBUFFS];	// First value in array of buffer structures
} VIDSTREAMSTRUCT2, *PVIDSTREAMSTRUCT2;


//
// PDD driver instance structure  
//
typedef struct {
    DWORD dwSize;

	// Video streaming
	PIPE pipeStream;
	WORD wReadThreadState;
	WORD wCurrFrameIndex;
	WORD wCurrFormatIndex;
	DWORD dwCurrValidInterval;
	DWORD dwMaxTransferSize;
	HANDLE hReadThread;
	VIDSTREAMSTRUCT2 strStreamDefault;
	PVIDSTREAMSTRUCT pstrStream;
	HANDLE hVendorEvent;
	HANDLE hNewFrameEvent;		// Added in 0.6

	// Still capture 
	CRITICAL_SECTION csStill;
	HANDLE hStillEvent;

	// Device info
	USBCTLIF usbctlIF;		// Control interface descriptor
	int nStreamInterfaces;
	PUSBSTRMIF usbstrmIF;	// Array of Stream interface descriptors
} PDDCONTEXT, *PPDDCONTEXT;


typedef struct {
	BYTE bFeatureID;
	BYTE bCmdID;
	BYTE bUnit;
	BYTE bInterface;
	WORD wLength;
} FEATURESTRUCT, *PFEATURESTRUCT;

typedef struct {
	WORD wID;
	WORD wValue;
	WORD wIndex;
	WORD wLength;
} VENDTRANSPARAMS, *PVENDTRANSPARAMS;


int WriteJPEG (LPTSTR lpszName, PBYTE pData, int nSize);
#define STILL_START  1
#define STILL_ABORT  3

BOOL ParseStreamInterfaces (PPDDCONTEXT pPDD, LPCUSB_DEVICE lpUsbDev, BYTE bIFCtl, BYTE bIFSubCtl, BYTE bIFStrm, BYTE bIFSubStrm);

int DumpInterfaceDescriptor (PCUSB_INTERFACE_DESCRIPTOR lpDes);
int DumpUsbInterface (LPCUSB_INTERFACE lpif);

BOOL ParseStreamInterfaces (PDRVCONTEXT pDrv, BYTE bIFCtl, BYTE bIFSubCtl, BYTE bIFStrm, BYTE bIFSubStrm);
int SetStreamInterface (PDRVCONTEXT pDrv, BYTE nInterface, DWORD dwBandwidth);
int GetPower (PDRVCONTEXT pDrv, BOOL *pbVal);
int SetPower (PDRVCONTEXT pDrv, BOOL fOn);

int SetStillCaptureFormat (PDRVCONTEXT pDrv, BYTE bFormatIndex, BYTE bFrameIndex); 
int StartStillCapture (PDRVCONTEXT pDrv, BYTE bCmd);
int ParseFeatureParameters (PPDDCONTEXT pPDD);
int QueryFeatureRange (PDRVCONTEXT pDrv, BYTE bFeatureID, DWORD *pdwMinVal, DWORD *pdwMaxVal);
PFEATURESTRUCT FindFeatureInfo (BYTE bFeatureID);
int DoVendorTransfer (PDRVCONTEXT pDrv, BYTE bRequest, BYTE bCmd, BYTE bInterface, BYTE bUnit, PBYTE pVal, WORD wLen);
PUSBSTRMIF FindStreamInterface (PPDDCONTEXT pPDD, BYTE nInterface, BOOL fMin, WORD wPacketSize);
int ProcessFrameFormats (PPDDCONTEXT pPDD, PUSBVIDSTDDESCHDR pStd, BYTE bDescID, 
						 BYTE bFormatIndex, BYTE bFrameIndex, 
						 PFORMATPROPS pProps, BOOL *pfFound);
int NegotiateQuality (PDRVCONTEXT pDrv, BYTE bFormatIndex, BYTE bFrameIndex, DWORD dwValidInterval);
int AllocateInternalStreamBuffers (PPDDCONTEXT pPDD, DWORD dwMaxBuff);
DWORD WINAPI ReadIsocThread (PVOID pArg);
void FreePDDContext (PPDDCONTEXT pPDD);
void StopReadThread (PPDDCONTEXT pDrv);

#define ERROR_STREAM_ALREADY_RUNNING 0x7fffffff

#define FAV_FRAMERATE    1
#define FAV_QUALITY      2

DWORD IssueIsochTransfer (
   LPCUSB_FUNCS              pUsbFuncs,
   HANDLE                    hUsbDevice,
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
   PUSB_ERROR                pUsbRc			      // Returns USB_ERROR or USB_TRANSFER
   );
   DWORD WINAPI ReadIsocThread (PVOID pArg);

// Defs in case the driver is being compiled outside of PB
extern "C" {
DWORD GetCurrentPermissions (void); 
DWORD SetProcPermissions (DWORD);
}
