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
//    Defines useful for USB driver interfaces
// </summary>
//-------------------------------------------------------------------------
//======================================================================
// USBVideo.h - Defines useful for USB driver interfacing with 
// devices compiling with the USB Video Specification
//
// Author: Douglas Boling
//======================================================================

//
// USB Video Interface Descriptor
//
#define USB_MISC_INTERFACE_CLASS                 0xEF
#define USB_COMMON_INTERFACE_SUBCLASS            0x02

#define USB_DEVICE_CLASS_VIDEO                   0x0E

// Video interface subclasses
#define USB_VIDEO_SC_UNDEFINED                   0x00 
#define USB_VIDEO_SC_VIDEOCONTROL                0x01 
#define USB_VIDEO_SC_VIDEOSTREAMING              0x02
#define USB_VIDEO_SC_VIDEO_INTERFACE_COLLECTION  0x03

// Interface numbers for the two video interfaces
#define VID_IF_CTL       0
#define VID_IF_STREAM    1

//
// Video Class request types
//
#define USBVID_SET_CUR              0x01 
#define USBVID_GET_CUR              0x81 
#define USBVID_GET_MIN              0x82 
#define USBVID_GET_MAX              0x83 
#define USBVID_GET_RES              0x84 
#define USBVID_GET_DEF              0x87 
#define USBVID_GET_LEN              0x85 
#define USBVID_GET_INFO             0x86 

// Video interface protocols
#define USB_VIDEO_PC_PROTOCOL_UNDEFINED          0x00

// Video interface class specific descriptors
#define USB_VIDEO_CS_UNDEFINED                   0x20
#define USB_VIDEO_CS_DEVICE                      0x21
#define USB_VIDEO_CS_CONFIGURATION               0x22
#define USB_VIDEO_CS_STRING                      0x23
#define USB_VIDEO_CS_INTERFACE                   0x24
#define USB_VIDEO_CS_ENDPOINT                    0x25

// Video interface class Video Control descriptors
#define USB_VIDEO_VC_DESCRIPTOR_UNDEFINED        0x00
#define USB_VIDEO_VC_HEADER                      0x01
#define USB_VIDEO_VC_INPUT_TERMINAL              0x02
#define USB_VIDEO_VC_OUTPUT_TERMINAL             0x03
#define USB_VIDEO_VC_SELECTOR_UNIT               0x04
#define USB_VIDEO_VC_PROCESSING_UNIT             0x05
#define USB_VIDEO_VC_EXTENSION_UNIT              0x06

// Video interface class Video Stream descriptors
#define USB_VIDEO_VS_UNDEFINED                   0x00
#define USB_VIDEO_VS_INPUT_HEADER                0x01
#define USB_VIDEO_VS_OUTPUT_HEADER               0x02
#define USB_VIDEO_VS_STILL_IMAGE_FRAME           0x03
#define USB_VIDEO_VS_FORMAT_UNCOMPRESSED         0x04
#define USB_VIDEO_VS_FRAME_UNCOMPRESSED          0x05
#define USB_VIDEO_VS_FORMAT_MJPEG                0x06
#define USB_VIDEO_VS_FRAME_MJPEG                 0x07
#define USB_VIDEO_VS_FORMAT_MPEG2TS              0x0A
#define USB_VIDEO_VS_FORMAT_DV                   0x0C
#define USB_VIDEO_VS_COLORFORMAT                 0x0D
#define USB_VIDEO_VS_FORMAT_FRAME_BASED          0x10
#define USB_VIDEO_VS_FRAME_FRAME_BASED           0x11
#define USB_VIDEO_VS_FORMAT_STREAM_BASED         0x12

//
// Control Selector Codes
//

// Video Control Interface control selectors
#define USB_VIDEO_VC_CS_VIDEO_POWER_MODE_CONTROL    0x01
#define USB_VIDEO_VC_CS_REQUEST_ERROR_CODE_CONTROL  0x02

// Termainal Control Selectors
#define USB_VIDEO_TU_CS_UNDEFINED                   0x00

// Selector Unit Control Selectors
#define USB_VIDEO_SU_CS_INPUT_SELECT_CONTROL        0x01

// Camera Terminal Control Selectors
#define USB_VIDEO_CT_CS_CONTROL_UNDEFINED           0x00
#define USB_VIDEO_CT_CS_SCANNING_MODE_CTL           0x01
#define USB_VIDEO_CT_CS_AE_MODE_CTL                 0x02
#define USB_VIDEO_CT_CS_AE_PRIORITY_CTL             0x03
#define USB_VIDEO_CT_CS_EXPOSURE_TIME_ABSOLUTE_CTL  0x04
#define USB_VIDEO_CT_CS_EXPOSURE_TIME_RELATIVE_CTL  0x05
#define USB_VIDEO_CT_CS_FOCUS_ABSOLUTE_CTL          0x06
#define USB_VIDEO_CT_CS_FOCUS_RELATIVE_CTL          0x07
#define USB_VIDEO_CT_CS_FOCUS_AUTO_CTL              0x08
#define USB_VIDEO_CT_CS_IRIS_ABSOLUTE_CTL           0x09
#define USB_VIDEO_CT_CS_IRIS_RELATIVE_CTL           0x0A
#define USB_VIDEO_CT_CS_ZOOM_ABSOLUTE_CTL           0x0B
#define USB_VIDEO_CT_CS_ZOOM_RELATIVE_CTL           0x0C
#define USB_VIDEO_CT_CS_PANTILT_ABSOLUTE_CTL        0x0D
#define USB_VIDEO_CT_CS_PANTILT_RELATIVE_CTL        0x0E
#define USB_VIDEO_CT_CS_ROLL_ABSOLUTE_CTL           0x0F
#define USB_VIDEO_CT_CS_ROLL_RELATIVE_CTL           0x10
#define USB_VIDEO_CT_CS_PRIVACY_CTL                 0x11

// Auto-Exposure Priority control bitfields
#define USB_VIDEO_AE_MODE_MANUAL                    0x01
#define USB_VIDEO_AE_MODE_AUTO_FULL                 0x02
#define USB_VIDEO_AE_MODE_AUTO_SHUTTER_PRIO         0x04
#define USB_VIDEO_AE_MODE_AUTO_APERTURE_PRIO        0x08

// Processing Unit Control Selectors
#define USB_VIDEO_PU_CS_CONTROL_UNDEFINED                   0x00
#define USB_VIDEO_PU_CS_BACKLIGHT_COMPENSATION_CTL          0x01
#define USB_VIDEO_PU_CS_BRIGHTNESS_CTL                      0x02
#define USB_VIDEO_PU_CS_CONTRAST_CTL                        0x03
#define USB_VIDEO_PU_CS_GAIN_CTL                            0x04
#define USB_VIDEO_PU_CS_POWER_LINE_FREQUENCY_CTL            0x05
#define USB_VIDEO_PU_CS_HUE_CTL                             0x06
#define USB_VIDEO_PU_CS_SATURATION_CTL                      0x07
#define USB_VIDEO_PU_CS_SHARPNESS_CTL                       0x08
#define USB_VIDEO_PU_CS_GAMMA_CTL                           0x09
#define USB_VIDEO_PU_CS_WHITE_BALANCE_TEMP_CTL              0x0A
#define USB_VIDEO_PU_CS_WHITE_BALANCE_TEMP_AUTO_CTL         0x0B
#define USB_VIDEO_PU_CS_WHITE_BALANCE_COMPONENT_CTL         0x0C
#define USB_VIDEO_PU_CS_WHITE_BALANCE_COMPONENT_AUTO_CTL    0x0D
#define USB_VIDEO_PU_CS_DIGITAL_MULTIPLIER_CTL              0x0E
#define USB_VIDEO_PU_CS_DIGITAL_MULTIPLIER_LIMIT_CTL        0x0F
#define USB_VIDEO_PU_CS_HUE_AUTO_CTL                        0x10
#define USB_VIDEO_PU_CS_ANALOG_VIDEO_STANDARD_CTL           0x11
#define USB_VIDEO_PU_CS_ANALOG_LOCK_STATUS_CTL              0x12

// Extension Unit Control Selectors
#define USB_VIDEO_EU_CS_UNDEFINED                   0x00

// Video Streaming Interface Control Selectors
#define USB_VIDEO_VS_CS_CTL_UNDEFINED             0x00
#define USB_VIDEO_VS_CS_PROBE_CTL                 0x01
#define USB_VIDEO_VS_CS_COMMIT_CTL                0x02
#define USB_VIDEO_VS_CS_STILL_PROBE_CTL           0x03
#define USB_VIDEO_VS_CS_STILL_COMMIT_CTL          0x04
#define USB_VIDEO_VS_CS_STILL_IMAGE_TRIGGER_CTL   0x05
#define USB_VIDEO_VS_CS_STREAM_ERROR_CODE_CTL     0x06
#define USB_VIDEO_VS_CS_GENERATE_KEY_FRAME_CTL    0x07
#define USB_VIDEO_VS_CS_UPDATE_FRAME_SEGMENT_CTL  0x08
#define USB_VIDEO_VS_CS_SYNCH_DELAY_CTL           0x09

#pragma pack(1)
/*  bad according to the usb spec. 
typedef struct {
	WORD bmHint;				//0
	BYTE bFormatIndex;			//2
	BYTE bFrameIndex;			//3
	DWORD dwFrameInterval;		//4
	WORD wKeyFrameRate;			//8
	WORD wPFrameRate;			//10
	WORD wCompQuality;			//12
	WORD wCompWindowSize;		//14
	WORD wDelay;				//16
	DWORD dwMaxVideoFrameSize;	//18
	DWORD dwMaxPayloadTransferSize; //22
	WORD dwClockFrequency;		//26
	WORD bmFramingInfo;			//28
} STREAM_PROBE_CONTROLSTRUCT, *PSTREAM_PROBE_CONTROLSTRUCT;
*/
typedef struct {
	WORD bmHint;				//0
	BYTE bFormatIndex;			//2
	BYTE bFrameIndex;			//3
	DWORD dwFrameInterval;		//4
	WORD wKeyFrameRate;			//8
	WORD wPFrameRate;			//10
	WORD wCompQuality;			//12
	WORD wCompWindowSize;		//14
	WORD wDelay;				//16
	DWORD dwMaxVideoFrameSize;	//18
	DWORD dwMaxPayloadTransferSize; //22
	DWORD dwClockFrequency;		//26
	BYTE bmFramingInfo;			//30
	BYTE bPreferedVersion;		//31
	BYTE bMinVersion;			//32
	BYTE bMaxVersion;			//33
} STREAM_PROBE_CONTROLSTRUCT, *PSTREAM_PROBE_CONTROLSTRUCT;
#pragma pack ()

#define USB_VIDEO_PROBE_HINT_FRAMEINTERVAL        0x0001
#define USB_VIDEO_PROBE_HINT_KEYFRAMERATE         0x0002
#define USB_VIDEO_PROBE_HINT_PFRAMERATE           0x0004
#define USB_VIDEO_PROBE_HINT_COMPRESSQUAL         0x0008
#define USB_VIDEO_PROBE_HINT_COMPRESSWNDSIZE      0x0010

#pragma pack(1)
typedef struct {
	BYTE bFormatIndex;
	BYTE bFrameIndex;
	BYTE bCompressionIndex;
	DWORD dwMaxVideoFrameSize;
	DWORD dwMaxPayloadTransferSize;
} STILL_PROBE_CONTROLSTRUCT, *PSTILL_PROBE_CONTROLSTRUCT;
#pragma pack ()
//
// Video Terminal Types
//

// USB Terminal Types
#define USB_VIDEO_TT_VENDOR_SPECIFIC                0x0100
#define USB_VIDEO_TT_STREAMING                      0x0101

// Input Terminal Types
#define USB_VIDEO_ITT_VENDOR_SPECIFIC               0x0200
#define USB_VIDEO_ITT_CAMERA                        0x0201
#define USB_VIDEO_ITT_MEDIA_TRANSPORT_INPUT         0x0202

// Output Terminal Types
#define USB_VIDEO_OTT_VENDOR_SPECIFIC               0x0300
#define USB_VIDEO_OTT_DISPLAY                       0x0301
#define USB_VIDEO_OTT_MEDIA_TRANSPORT_OUTPUT        0x0302


#pragma pack(1)
typedef struct {
	BYTE bLen;
	BYTE bType;
	BYTE bSubtype;
	WORD wTermType;
} USBVIDSTDDESCHDR, *PUSBVIDSTDDESCHDR;
#pragma pack ()

//Video Class Control Extended Interface Descriptor
#pragma pack(1)
typedef struct {
	BYTE bLen;
	BYTE bType;
	BYTE bSubtype;
	WORD wIFVersion;
	WORD wTotalLen;
	DWORD dwClkFreq;
	BYTE bInCollection;
	BYTE bInterface[1];
} USBVIDCTLIFDESCRIPTOR, *PUSBVIDCTLIFDESCRIPTOR;
#pragma pack ()

// Processing Unit Interface Descriptor  (Added by CDB)
#pragma pack(1)
typedef struct {
	BYTE bLen;
	BYTE bType;
	BYTE bSubtype;
	BYTE bUnitID;
	BYTE bSourceID;
	WORD wMaxMultiplier;
	BYTE bControlSize;
	BYTE bmControls[1];

} USBVIDPUNITIFDESCRIPTOR, *PUSBVIDPUNITIFDESCRIPTOR;
#pragma pack ()

// Input Terminal Interface Descriptor  (Added by CDB)
#pragma pack(1)
typedef struct {
	BYTE bLen;
	BYTE bType;
	BYTE bSubtype;
	BYTE bTerminalID;
	WORD wTerminalType;
	BYTE bAssocTerminal;
	BYTE iTerminal[1];
} USBVIDINPUTTERMIFDESCRIPTOR, *PUSBVIDINPUTTERMIFDESCRIPTOR;
#pragma pack ()

// Camera Input Terminal Interface Descriptor  (Added by CDB)
#pragma pack(1)
typedef struct {
	BYTE bLen;
	BYTE bType;
	BYTE bSubtype;
	BYTE bTerminalID;
	WORD wTerminalType;
	BYTE bAssocTerminal;
	BYTE iTerminal;
	WORD wObjectiveFocalLengthMin;
	WORD wObjectiveFocalLengthMax;
	WORD OcularFocalLength;
	BYTE bControlSize;
	BYTE bmControls[1];
} USBVIDCAMINPUTTERMIFDESCRIPTOR, *PUSBVIDCAMINPUTTERMIFDESCRIPTOR;
#pragma pack ()

//Video Class Color Matching Interface Descriptor
#pragma pack(1)
typedef struct {
	BYTE bLen;
	BYTE bType;
	BYTE bSubtype;
	BYTE bColorPrimaries;
	BYTE bTransferCharacteristics;
	BYTE bMatrixCoefficients;
} USBVIDCOLORIFDESCRIPTOR, *PUSBVIDCOLORIFDESCRIPTOR;
#pragma pack ()

//Video Class Extended Stream Interface Descriptor
#pragma pack(1)
typedef struct {
	BYTE bLen;
	BYTE bType;
	BYTE bSubtype;
	BYTE bNumFormats;
	WORD wTotalLen;
	BYTE bEndpointAddress;
	BYTE bmInfo;
	BYTE bTerminalLink;
	BYTE bStillCapMethod;
	BYTE bTriggerSupport;
	BYTE bTriggerUsage;
	BYTE bControlSize;
	BYTE bmaControls[1];
} USBVIDSTREAMIFDESCRIPTOR, *PUSBVIDSTREAMIFDESCRIPTOR;
#pragma pack ()

//Video Class Extended Stream Interface generic Format Descriptor
#pragma pack(1)
typedef struct {
	BYTE bLen;
	BYTE bType;
	BYTE bSubtype;
	BYTE bFormatIndex;
	BYTE bNumFrameDescriptors;
} USBVIDSTREAMIF_FORMATDESCRIPTOR, *PUSBVIDSTREAMIF_FORMATDESCRIPTOR;
#pragma pack ()

//Video Class Extended Stream Interface MJPEG Format Descriptor
#pragma pack(1)
typedef struct {
	BYTE bLen;
	BYTE bType;
	BYTE bSubtype;
	BYTE bFormatIndex;
	BYTE bNumFrameDescriptors;
	BYTE bmFlags;
	BYTE bDefaultFrameIndex;
	BYTE bAspectRatioX;
	BYTE bAspectRatioY;
	BYTE bmInterlaceFlags;
	BYTE bCopyProtect;
} USBVIDSTREAMIF_MJPEGFORMATDESCRIPTOR, *PUSBVIDSTREAMIF_MJPEGFORMATDESCRIPTOR;
#pragma pack ()

//Video Class Extended Stream Interface MJPEG Frame Descriptor
#pragma pack(1)
typedef struct {
	BYTE bLen;
	BYTE bType;
	BYTE bSubtype;
	BYTE bFrameIndex;
	BYTE bmCapabilities;
	WORD wWidth;
	WORD wHeight;
	DWORD dwMinBitRate;
	DWORD dwMaxBitRate;
	DWORD dwMaxVideoFrameBufferSize;
	DWORD dwDefaultFrameInterval;
	BYTE bFrameIntervalType;    // 0 - Use ContInterval, else num of DescIntervals
	union 
	{
		struct {
			DWORD dwMinFrameInterval;
			DWORD dwMaxFrameInterval;
			DWORD dwFrameIntervalStep;
		} strCont;
		DWORD dwDescrete[1];
	} Interval;
} USBVIDSTREAMIF_MJPEGFRAMEDESCRIPTOR, *PUSBVIDSTREAMIF_MJPEGFRAMEDESCRIPTOR;
#pragma pack ()

//Video Class Extended Stream Interface Still Image Frame Descriptor
#pragma pack(1)
typedef struct {
	BYTE bLen;
	BYTE bType;
	BYTE bSubtype;
	BYTE bEndpointAddress;
	BYTE bNumImageSizePatterns;
	struct {
		WORD wWidth;
		WORD wHeight;
	} sStillFmt[1];
} USBVIDSTREAMIF_STILLIMGDESCRIPTOR, *PUSBVIDSTREAMIF_STILLIMGDESCRIPTOR;
#pragma pack ()

//Video Class Payload header
#pragma pack(1)
typedef struct {
	BYTE bLen;
	BYTE bFlags;
	BYTE extra[10];
} USBVIDPAYLOADHDR, *PUSBVIDPAYLOADHDR;
#pragma pack ()
#define USBVID_PAYLOADHDR_EOH         0x80
#define USBVID_PAYLOADHDR_ERR         0x40
#define USBVID_PAYLOADHDR_STILLFRAME  0x20
#define USBVID_PAYLOADHDR_RESERVED    0x10
#define USBVID_PAYLOADHDR_SCKLFIELD   0x08
#define USBVID_PAYLOADHDR_PREFIELD    0x04
#define USBVID_PAYLOADHDR_FRAMEEND    0x02
#define USBVID_PAYLOADHDR_FRAMEID     0x01
