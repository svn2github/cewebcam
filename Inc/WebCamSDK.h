//-------------------------------------------------------------------------
// <copyright file="WebCamSDK.h" company="Microsoft">
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
//    WebCam driver IOCTL commands
// </summary>
//-------------------------------------------------------------------------
//======================================================================
// WebCamSDK Header file.
//
// This file describes the stream IOCTL commands and necessary 
// structures needed to talk to the webcam driver.
//
// Copyright (C) 2005 Douglas Boling
//======================================================================

#ifndef _WEBCAM_SDK_H_
#define _WEBCAM_SDK_H_


// This is dereferenced from the CTLCODE macros below because I'm
// not sure this is the best device type for this non-standard driver
#define MYIOCTLDEVTYPE  FILE_DEVICE_VIDEO

//
// Returns a FEATUREPROPS structure that describes the features 
// available on the camera
//
#define IOCTL_CAMERA_DEVICE_QUERYPARAMETERARARY \
    CTL_CODE(MYIOCTLDEVTYPE, 0x100, METHOD_BUFFERED, FILE_ANY_ACCESS)

// 
// Querys the state of a given camera feature
//
#define IOCTL_CAMERA_DEVICE_QUERYPARAMETER \
    CTL_CODE(MYIOCTLDEVTYPE, 0x101, METHOD_BUFFERED, FILE_ANY_ACCESS)

// 
// Sets the state of a given camera feature
//
#define IOCTL_CAMERA_DEVICE_SETPARAMETER \
    CTL_CODE(MYIOCTLDEVTYPE, 0x102, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// Queries the streaming video formats supported by the camera 
//
#define IOCTL_CAMERA_DEVICE_QUERYVIDEOFORMATS \
    CTL_CODE(MYIOCTLDEVTYPE, 0x103, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// Gets the resolution, encoding type and possible frame ratee of
// a specfic video streaming format.
//
#define IOCTL_CAMERA_DEVICE_GETVIDEOFORMAT \
    CTL_CODE(MYIOCTLDEVTYPE, 0x104, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// Sets the video streaming format and starts streaming.  This 
// is a non-blocking call.
//
#define IOCTL_CAMERA_DEVICE_STARTVIDEOSTREAM \
    CTL_CODE(MYIOCTLDEVTYPE, 0x105, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// Returns a pointer to the lastest valid video frame. Also used 
// to return buffer pointers back to the driver.
//
#define IOCTL_CAMERA_DEVICE_GETNEXTVIDEOFRAME \
    CTL_CODE(MYIOCTLDEVTYPE, 0x106, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// Returns the current streaming parameters. This can be different
// from what was requested
//
#define IOCTL_CAMERA_DEVICE_GETCURRENVIDEOFORMAT \
    CTL_CODE(MYIOCTLDEVTYPE, 0x107, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// Stops the video stream 
//
#define IOCTL_CAMERA_DEVICE_STOPVIDEOSTREAM \
    CTL_CODE(MYIOCTLDEVTYPE, 0x108, METHOD_BUFFERED, FILE_ANY_ACCESS)

// Retuns an array of available still formats
#define IOCTL_CAMERA_DEVICE_QUERYSTILLFORMATS \
    CTL_CODE(MYIOCTLDEVTYPE, 0x109, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// Returns a single frame of video.  On some cameras, this will
// be a single frame of the stream, on other cameras, the still can
// be a different resolution from the stream.
//
#define IOCTL_CAMERA_DEVICE_GETSTILLIMAGE \
    CTL_CODE(MYIOCTLDEVTYPE, 0x110, METHOD_BUFFERED, FILE_ANY_ACCESS)

// 
// Returns the version of the driver.
//
#define IOCTL_CAMERA_DEVICE_GETDRIVERVERSION \
    CTL_CODE(MYIOCTLDEVTYPE, 0x111, METHOD_BUFFERED, FILE_ANY_ACCESS)

// 
// Returns when an event happens in the driver
//
#define IOCTL_CAMERA_DEVICE_WAITONCAMERAEVENT \
    CTL_CODE(MYIOCTLDEVTYPE, 0x112, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// Structure passed to driver when starting a video stream.
//
#define  STARTVIDFLAG_FAVORINTERVAL    0x0000 //Default

typedef struct {
	DWORD cbSize;				// Size of the structure
	DWORD dwFlags;				// STARTVIDFLAG_xxx flags 
	WORD  wFormatIndex;			// Video format index
	WORD  wFrameIndex;			// Video frame size index
	DWORD dwInterval;			// Requested video frame interval 
	DWORD dwNumBuffs;			// Number of buffers (>= 3)
	DWORD dwPreBuffSize;		// Size of prebuffer
	DWORD dwPostBuffSize;		// Size of post buffer
} STARTVIDSTRUCT, *PSTARTVIDSTRUCT;


//
// Structure passed to driver indicating if the
// application is returning a previous buffer and
// specifying which buffer they want
//
#define  GETFRAMEFLAG_GET_LATESTFRAME    0x0000 //Default
#define  GETFRAMEFLAG_GET_OLDESTFRAME    0x0001 //Implemented in 0.6
#define  GETFRAMEFLAG_FREEBUFF_VALID     0x0002
#define  GETFRAMEFLAG_NOFRAME_WANTED     0x0004
#define  GETFRAMEFLAG_TIMEOUT_VALID      0x0008
#define  GETFRAMEFLAG_GETNEWESTAVAILABLE 0x0010 //Added in 0.6
#define  GETFRAMEFLAG_PURGEOLDERFRAMES   0x0020 //Added in 0.6

typedef struct {
	DWORD cbSize;				// Size of the structure
	DWORD dwFlags;				// GETFRAMEFLAG_xxx flags (see above)
	DWORD dwTimeout;			// Time im mS to wait for frame
	PBYTE pFrameDataRet;		// Ptr to buffer to return to driver
} GETFRAMESTRUCT, *PGETFRAMESTRUCT;

//
// Structure returned by driver providing a pointer
// to the next frame in the video stream.
//
typedef struct {
	DWORD cbSize;				// Size of the structure
	DWORD dwMissedFrames;		// Number of frames missed if asking for latest frame
	PBYTE pFrameData;			// Ptr to buffer with new frame data
	DWORD dwFrameSize;			// Size of the data returned
} GETFRAMESTRUCTOUT, *PGETFRAMESTRUCTOUT;

typedef struct {
	DWORD dwSize;				// Size of the frame data
	PBYTE pBuff;				// Ptr to frame data
	DWORD dwTicksAtCapture;		// Tick count at capture for ageing
} FRAMEBUFFSTRUCT, *PFRAMEBUFFSTRUCT;


//
// Structure to report the video formats available
//
#define  MAXINTERVALS  10

typedef struct {
	DWORD cbSize;				// Size of the structure
	WORD  wFormatType;          // Video format ID
	WORD  wFormatIndex;			// Video format index
	WORD  wFrameIndex;			// Video frame size index
	
	DWORD dwWidth;				// Width of frame
	DWORD dwHeight;				// Height of frame
	DWORD dwMaxBuff;			// Maximum size of single frame data
	int   nNumInterval;			// Number of frame intervals supported
								// If zero, frame interval values are
								// not descrete but are continuous. If 0, 
	                            // dwInterval[0] is min value and 
	                            // dwInterval[1] is max value and 
	                            // dwInterval[2] is step value
	DWORD dwInterval[MAXINTERVALS];
} FORMATPROPS, *PFORMATPROPS;

//
// These video format IDs match the formats supported
// by the USB Video spec.
//
#define VIDFORMAT_UNCOMPRESSED			0x0005
#define VIDFORMAT_MJPEG					0x0007
#define VIDFORMAT_MPEG2TS				0x000A
#define VIDFORMAT_DV					0x000C

//
// Structure to report the video formats available
//

#define FEAT_UNSUPPORTED                0
#define FEAT_SCANNING_MODE              1
#define FEAT_AUTO_EXPOSURE_MODE         2 
#define FEAT_AUTO_EXPOSURE_PRI          3
#define FEAT_EXPOSURE_TIME_ABS          4
#define FEAT_EXPOSURE_TIME_REL          5
#define FEAT_FOCUS_ABS                  6
#define FEAT_FOCUS_REL                  7
#define FEAT_IRIS_ABS                   8
#define FEAT_IRIS_REL                   9
#define FEAT_ZOOM_ABS                  10
#define FEAT_ZOOM_REL                  11 
#define FEAT_PANTILT_ABS               12
#define FEAT_PANTILT_REL               13
#define FEAT_ROLL_ABS                  14
#define FEAT_ROLL_REL                  15
#define FEAT_FOCUS_AUTO                16
#define FEAT_PRIVACY                   17
#define FEAT_BRIGHTNESS                18
#define FEAT_CONTRAST                  19
#define FEAT_HUE                       20
#define FEAT_SATURATION                21
#define FEAT_SHARPNESS                 22
#define FEAT_GAMMA                     23
#define FEAT_WHITE_BAL_TEMP            24
#define FEAT_WHITE_BAL_COMPONENT       25
#define FEAT_BACKLIGHT_COMPENSATION    26
#define FEAT_GAIN                      27
#define FEAT_POWER_LINE_FREQ           28
#define FEAT_AUTO_HUE                  29
#define FEAT_AUTO_WHITE_BAL_TEMP       30
#define FEAT_AUTO_WHITE_BAL_COMPONENT  31
#define FEAT_DIGITAL_MULTIPLIER        32
#define FEAT_DIGITAL_MULTIPLIER_LIMIT  33
#define FEAT_ANALOG_VIDEO_STANDARD     34
#define FEAT_ANALOG_VIDEO_LOCK_STATUS  35

// Structure describes a feature and its minimum and maximum values
typedef struct {
	DWORD dwFeatureID;			// Feature ID value (FEAT_xxx value above)
	DWORD dwFlags;				// Flags for ID
	int nMin;					// Minimum value supported
	int nMax;					// Maximum value supported
} FEATUREPROP, *PFEATUREPROP;

#define FLAG_FP_ERROR    0x0001	// An error occured reteriving the values
#define FLAG_FP_BITFIELD 0x0002	// The value is a bitfield, not a number


// Structure used to return the array of features supported by the camera
typedef struct {
	int nNumProps;				// Number of feature structures in FEATUREPROP array
	FEATUREPROP fpArray[1];		// Array of FEATUREPROP structs, 1 for each feature
} FEATUREPROPS, *PFEATUREPROPS;

// Structure used to set a parameter on the device. Note that not all values are
// not a DWORD in size. 
typedef struct {
	DWORD cbSize;				// Size of the structure
	DWORD dwFeatureID;			// Feature ID value (FEAT_xxx value above)
	DWORD dwVal;				// Current value
} SETFEATURESTRUCT, *PSETFEATURESTRUCT;

// Structure used to report video format currently streaming as well
typedef struct {
	DWORD cbSize;				// Size of the structure
	WORD  wFormatIndex;			// Video format
	WORD  wFrameIndex;			// Video frame size index
} VIDFORMATSTRUCT, *PVIDFORMATSTRUCT;

//
// Events the app can wait upon
//
#define  WAITCAM_NEWFRAME          0x00000001
//#define  WAITCAM_BUTTONPRESS     0x00000002  //Not implemented yet...

#define  WAITCAM_WAITTIMEOUT       0x80000000


// Structure used to tell driver what events to wait on.
typedef struct {
	DWORD cbSize;				// Size of the structure
	DWORD dwWaitMask;			// Flags indicating events to wait on
	DWORD dwTimeout;
} WAITEVENTSTUCT, *PWAITEVENTSTUCT;

#endif //_WEBCAM_SDK_H_