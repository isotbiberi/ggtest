///
/// @file libflipro.h
///
/// @brief Finger Lakes Instrumentation Camera API
///
/// The library uses the standard way of creating macros which make exporting
/// from a DLL simpler. All files within this DLL are compiled with the LIBFLIPRO_EXPORTS
/// symbol defined on the command line. This symbol should not be defined on any project
/// that uses this DLL. This way any other project whose source files include this file see 
/// LIBFLIPRO_API functions as being imported from a DLL, whereas this DLL sees symbols
/// defined with this macro as being exported.
///

#ifdef __cplusplus
extern "C" { 
#endif

#ifndef _LIBFLIPRO_H_
/// @cond DO_NOT_DOCUMENT
#define _LIBFLIPRO_H_
/// @endcond

#ifdef __linux__
#include <wchar.h>
#include <stdarg.h>
#endif
#include "stdbool.h"
#include "stdint.h"

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the LIBFLIPRO_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// LIBFLIPRO_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef _WIN32
#ifdef LIBFLIPRO_EXPORTS
#define LIBFLIPRO_API __declspec(dllexport) int32_t 
#define LIBFLIPRO_API_DATA __declspec(dllexport)
#define LIBFLIPRO_VOID __declspec(dllexport) void
#else
#define LIBFLIPRO_API __declspec(dllimport) int32_t 
#define LIBFLIPRO_API_DATA __declspec(dllimport)
#define LIBFLIPRO_VOID __declspec(dllimport) void
#endif

#endif

#ifdef __linux__
#define LIBFLIPRO_API int32_t
#define LIBFLIPRO_API_DATA
#define LIBFLIPRO_VOID void
#endif

////////////////////////////////////////////////////////////////////////
// Typedefs, Defines, Macros
////////////////////////////////////////////////////////////////////////
// Version information
/// @cond DO_NOT_DOCUMENT
#define FPRO_API_VERSION_MAJOR (1)
#define FPRO_API_VERSION_MINOR (6)    // Should change if must change the Camera side as well
#define FPRO_API_VERSION_BUILD (1)    // Minor changes not requiring Camera change
/// @endcond

//
// Some Helpful #defines
///
/// @brief Convert image size in  Bytes to Pixels
///
/// The frame size is 1.5 * width * height (12 bit pixels)
/// This macro only works if the pixel size is 12 bits.
#define FPRO_IMAGE_FRAMEBYTES_TO_PIXELS(__framebytes)  (((__framebytes) << 1) / 3)
///
/// @brief Convert Pixels to Image size in Bytes
///
/// This macro only works if the pixel size is 12 bits.
#define FPRO_IMAGE_PIXELS_TO_FRAMEBYTES(__pixels)  (((__pixels) & 0x1) ? ((((__pixels) * 3 ) >> 1) + 1) : (((__pixels) * 3 ) >> 1))
///
/// @brief Convert image dimensions in Pixels to Image size in Bytes
///
/// This macro only works if the pixel size is 12 bits.
#define FPRO_IMAGE_DIMENSIONS_TO_FRAMEBYTES(__width,__height)  FPRO_IMAGE_PIXELS_TO_FRAMEBYTES((__width) * (__height))
///
/// @brief Size of the meta data in bytes returned in an image frame.
///
/// Meta Data is prepended to every image frame, including Tracking frames.
/// The Format of the meta data is documented in your users manual.
/// This number is constant for all frames including Tracking Frames.
#define FPRO_METADATA_SIZE   (156)
///
/// @brief Maximum number of pre/post frame dummy rows supported by the API.
///
/// This number should be treated as a constant.
#define FPRO_DUMMY_ROW_MAX  (4094)
///
/// @brief Height of Thumbnail image in pixels.
#define FPRO_THUMBNAIL_ROWS  (512)
///
/// @brief Width of Thumbnail image in pixels.
#define FPRO_THUMBNAIL_COLUMNS  (512)
///
/// @brief Max pixel depth
///
/// Use this macro to extract the Max Pixel depth supported by
/// the device from the FPROCAP capabilities structure.
///
#define FPRO_GET_MAX_PIXEL_DEPTH(__pCapCam, __uiMax)	{	\
															if (__pCapCam)	\
															{	\
																for (int __ix = 0; __ix < 32; ++__ix)	\
																	if (__pCapCam->uiAvailablePixelDepths & (1 << __ix))	\
																		__uiMax = __ix + 1;	\
															}	\
															else   \
															{	\
																__uiMax = 12;	\
															}	\
														}


///
/// @brief Known Device Types
/// @enum FPRODEVICETYPE
///
/// These constants are returned in the Device Capabilities Structure 
/// FPROCAP in the uiDeviceType field.
/// See the user manual for a description of the capabilities for 
/// your device.
///
/// @var FPRODEVICETYPE::FPRO_CAM_DEVICE_TYPE_GSENSE400
/// @brief Enum value = 0x01000400
/// @var FPRODEVICETYPE::FPRO_CAM_DEVICE_TYPE_GSENSE2020
/// @brief Enum value = 0x01002020
/// @var FPRODEVICETYPE::FPRO_CAM_DEVICE_TYPE_GSENSE4040
/// @brief Enum value = 0x01004040
/// @var FPRODEVICETYPE::FPRO_CAM_DEVICE_TYPE_KODAK47051
/// @brief Enum value = 0x02047051
/// @var FPRODEVICETYPE::FPRO_CAM_DEVICE_TYPE_KODAK29050
/// @brief Enum value = 0x02029050
/// 
typedef enum
{
	FPRO_CAM_DEVICE_TYPE_GSENSE400= 0x01000400,
	FPRO_CAM_DEVICE_TYPE_GSENSE2020 = 0x01002020,
	FPRO_CAM_DEVICE_TYPE_GSENSE4040 = 0x01004040,
	FPRO_CAM_DEVICE_TYPE_KODAK47051 = 0x02047051,
	FPRO_CAM_DEVICE_TYPE_KODAK29050 = 0x02029050
} FPRODEVICETYPE;

///
/// @brief Maximum String Length
///
/// The maximum number of characters (not bytes) allowed in USB strings throughout
/// the API.
///
#define FPRO_USB_STRING_MAXLEN  (256)

///
/// @brief Known USB Connection Speeds
/// @enum FPROUSBSPEED
///
/// This enumeration is used as part of the FPRODEVICEINFO structure to
/// return the detected USB connection speed.  FLI Cameras require a
/// FPRO_USB_SUPERSPEED USB connection in order to transfer image data reliably.
///
/// @var FPROUSBSPEED::FPRO_USB_FULLSPEED 
/// @brief Full Speed Connection
///
/// @var FPROUSBSPEED::FPRO_USB_HIGHSPEED 
/// @brief High Speed Connection
///
/// @var FPROUSBSPEED::FPRO_USB_SUPERSPEED 
/// @brief Super Speed Connection
///
typedef enum
{
	FPRO_USB_FULLSPEED,
	FPRO_USB_HIGHSPEED,
	FPRO_USB_SUPERSPEED
} FPROUSBSPEED;

///
/// @typedef device_info_t FPRODEVICEINFO
/// @brief Device Information
///
/// This is used as the Camera Device enumeration structure.  It is
/// returned by the #FPROCam_GetCameraList function and contains the
/// list of detected cameras.  To open a connection to a specific camera,
/// a single FPRODEVICEINFO structure is passed to the #FPROCam_Open function.
///
/// @var FPRODEVICEINFO::cFriendlyName 
/// @brief Human readable friendly name of the USB device.
///        This string along with the cSerialNo field provide a unique name
///        for your device suitable for a user interface.
/// @var FPRODEVICEINFO::cSerialNo 
/// @brief The manufacturing serial number of the device.
/// @var FPRODEVICEINFO::uiVendorId 
/// @brief The USB vendor ID.
/// @var FPRODEVICEINFO::uiProdId 
/// @brief The USB Product ID.
/// @var FPRODEVICEINFO::eUSBSpeed 
/// @brief The USB connection speed of the device.
/// <br>NOTE: FLI Cameras require a FPRO_USB_SUPERSPEED USB connection 
///           in order to transfer image data reliably.
typedef struct device_info_t
{
	wchar_t      cFriendlyName[FPRO_USB_STRING_MAXLEN];
	wchar_t      cSerialNo[FPRO_USB_STRING_MAXLEN];
	uint32_t     uiVendorId;
	uint32_t     uiProdId;
	FPROUSBSPEED eUSBSpeed;
} FPRODEVICEINFO;


///
/// @brief Version String Lengths
///
/// Maximum length characters (not bytes) of version strings.
#define FPRO_VERSION_STRING_MAXLEN  (32)
///
/// @typedef device_version_info_t FPRODEVICEVERS
/// @brief Device Version Information
///
/// Contains the various version numbers supplied by the device.
/// See #FPROCam_GetDeviceVersion.
///
/// @var FPRODEVICEVERS::cFirmwareVersion
/// @brief The version of firmware on the internal device processor.
/// @var FPRODEVICEVERS::cFPGAVersion
/// @brief The version of firmware on the internal FPGA device.
/// @var FPRODEVICEVERS::cControllerVersion
/// @brief The version of firmware on the internal sensor controller device.
typedef struct device_version_info_t
{
	wchar_t cFirmwareVersion[FPRO_VERSION_STRING_MAXLEN];
	wchar_t cFPGAVersion[FPRO_VERSION_STRING_MAXLEN];
	wchar_t cControllerVersion[FPRO_VERSION_STRING_MAXLEN];
} FPRODEVICEVERS;

///
/// @enum FPROTESTIMAGETYPE
/// @brief Test Image Types
///
/// The Camera has the ability to generate test image data.  This enumeration 
/// is used to tell the camera how you would like the test image data to be
/// formatted: row order or column order by pixel.
///
/// @var FPROTESTIMAGETYPE::FLI_TESTIMAGE_TYPE_ROW
/// @brief Row order format.
/// <br>The first 'width' number of pixels will be 0, the second 'width'
///     number of pixels will be 1... etc.
/// @var FPROTESTIMAGETYPE::FLI_TESTIMAGE_TYPE_COL
/// @brief Column order format.
/// <br>The first pixel of the first row will be 0, the second pixel will be 1...
///     the n'th pixel of the row will n.  The first pixel of the second row
///     will be 0 again, followed by 1, etc...
///
typedef enum
{
	FLI_TESTIMAGE_TYPE_ROW,
	FLI_TESTIMAGE_TYPE_COL,
} FPROTESTIMAGETYPE;


///
/// @enum FPROEXTTRIGTYPE
/// @brief External Trigger Types
///
/// This enumeration defines the types of external triggers available.
/// There is a single external trigger line available to the camera.  This
/// enumeration governs how this signal behaves.  This enumeration is used with
/// the FPROCtrl_GetExternalTriggerEnable and FPROCtrl_SetExternalTriggerEnable API's.
///
/// @var FPROEXTTRIGTYPE::FLI_EXT_TRIGGER_RISING_EDGE
/// @brief Trigger Exposure on Rising Edge
/// <br>For this setting, when the external trigger line goes from low to high, it 
///     triggers the exposure to begin on the camera.  The exposure will complete based
///     on the exposure time set with the FPROCtrl_SetExposure API.
/// @var FPROEXTTRIGTYPE::FLI_EXT_TRIGGER_FALLING_EDGE
/// @brief Trigger Exposure on Falling Edge
/// <br>For this setting, when the external trigger line goes from high to low, it 
///     triggers the exposure to begin on the camera.  The exposure will complete based
///     on the exposure time set with the FPROCtrl_SetExposure API.
/// @var FPROEXTTRIGTYPE::FLI_EXT_TRIGGER_EXPOSE_ACTIVE_HIGH
/// @brief Exposure Active High
/// <br>For this setting, the exposure is active the entire time the external trigger signal is high.  
///     The exposure will complete when the external trigger line goes low or when the exposure time
///     has reached the value set with the FPROCtrl_SetExposure API (whichever occurs first).  
///     That is, in this case the value used in the FPROCtrl_SetExposure API acts as a maximum exposure time. 
/// @var FPROEXTTRIGTYPE::FLI_EXT_TRIGGER_EXPOSE_ACTIVE_LOW
/// @brief Exposure Active High
/// <br>For this setting, the exposure is active the entire time the external trigger signal is low.  
///     The exposure will complete when the external trigger line goes high or when the exposure time
///     has reached the value set with the FPROCtrl_SetExposure API (whichever occurs first).  
///     That is, in this case the value used in the FPROCtrl_SetExposure API acts as a maximum exposure time. 
///
typedef enum
{
	FLI_EXT_TRIGGER_FALLING_EDGE,
	FLI_EXT_TRIGGER_RISING_EDGE,
	FLI_EXT_TRIGGER_EXPOSE_ACTIVE_LOW,
	FLI_EXT_TRIGGER_EXPOSE_ACTIVE_HIGH
} FPROEXTTRIGTYPE;

///
/// @enum FPRODBGLEVEL
/// @brief Debug Capability
///
/// The API provides a debug interface.  This sets the level of debug information
/// that can be logged by your application.
///
/// @var FPRODBGLEVEL::FPRO_DEBUG_NONE 
/// @brief All debug disabled
/// @var FPRODBGLEVEL::FPRO_DEBUG_ERROR 
/// @brief Only ERROR level debug is output
/// @var FPRODBGLEVEL::FPRO_DEBUG_WARNING 
/// @brief WARNING and ERROR debug is output
/// @var FPRODBGLEVEL::FPRO_DEBUG_INFO 
/// @brief INFO, WARNING, and ERROR debug is output
/// @var FPRODBGLEVEL::FPRO_DEBUG_TRACE 
/// @brief TRACE, INFO, WARNING, and ERROR debug is output
typedef enum
{
	FPRO_DEBUG_NONE,
	FPRO_DEBUG_ERROR,
	FPRO_DEBUG_WARNING,
	FPRO_DEBUG_INFO,
	FPRO_DEBUG_TRACE
} FPRODBGLEVEL;


//
/// @brief Sensor Mode Name Length
///
/// Max allowed name length for Camera Modes. See #FPROSENSMODE
///
#define FPRO_SENSOR_MODE_NAME_LENGTH  (32)

///
/// @typedef sensor_mode_t FPROSENSMODE
/// @brief Sensor Modes
///
/// The FLI Camera devices support the concept of 'Modes'.  A mode is a collection
/// of settings for the camera.  As this structure illustrates, the mode has a
/// name and an index.  The name can be used primarily for a user interface so that
/// a user can see a friendly and descriptive name for the mode.  The index is used by
/// the API to set a particular mode on the camera.  See #FPROSensor_SetMode, #FPROSensor_GetMode,
/// and #FPROSensor_GetModeCount.
///
/// @var FPROSENSMODE::uiModeIndex 
/// @brief The corresponding index of the mode name.
/// @var FPROSENSMODE::wcModeName 
/// @brief A descriptive human readable name for the mode suitable for
///        a user interface.
typedef struct sensor_mode_t
{
	uint32_t uiModeIndex;
	wchar_t  wcModeName[FPRO_SENSOR_MODE_NAME_LENGTH];
} FPROSENSMODE;


///
/// @brief Gain Scale Factor
///
/// All gain table values (see #FPROGAINTABLE) returned by the API are scaled 
/// by the factor #FPRO_GAIN_SCALE_FACTOR.
#define FPRO_GAIN_SCALE_FACTOR (1000)

///
/// @enum  FPROGAINTABLE
/// @brief Gain Tables
///
/// The camera makes available specific gain values for the image sensor.
/// Each set of values is stored in a table and this enum allows you to pick
/// the desired gain table to get using the function #FPROSensor_GetGainTable.
/// The values in the table can be used as part of a user interface allowing users
/// to select  a specific gain setting. The settings are retrieved and set by index
/// in the gain table using #FPROSensor_GetGainIndex and #FPROSensor_SetGainIndex.
/// <br><br> Note that all gain table values returned by the API are scaled by the factor #FPRO_GAIN_SCALE_FACTOR.
///
/// @var FPROGAINTABLE::FPRO_GAIN_TABLE_LDR 
/// @brief Gain Table for LDR
/// @var FPROGAINTABLE::FPRO_GAIN_TABLE_HDR 
/// @brief Gain Table for HDR
/// @var FPROGAINTABLE::FPRO_GAIN_TABLE_GLOBAL 
/// @brief Gain Table for Global Sensor
/// <br>NOTE: Not supported on all devices. See your specific device documentation for details.
typedef enum
{
	FPRO_GAIN_TABLE_LDR,
	FPRO_GAIN_TABLE_HDR,
	FPRO_GAIN_TABLE_GLOBAL,
} FPROGAINTABLE;

// Camera Capabilities
#pragma pack(push, 1)
///
/// @typedef camera_capabilities_t FPROCAP
/// @brief Camera Capabilities
///
/// The camera capabilities structure is a structure provided by the camera that
/// publishes a set of camera capabilities.  The structure is retrieved using the 
/// #FPROSensor_GetCapabilities API.  The values in this structure can 
/// be used by an application to configure settings and user interfaces based
/// on the specific camera that is connected.  The uiDeviceType member is a 
/// specific device type, one of #FPRODEVICETYPE, that allows further checking
/// by an application as it can cover specific functionality not described by
/// this structure.  The user documentation for your camera will cover the details
/// on what is available for your device with respect to its uiDeviceType.
typedef struct camera_capabilities_t
{
	uint32_t uiSize;                      ///< Size of this structure (including uiSize)
	uint32_t uiCapVersion;                ///< Version of this structure
	uint32_t uiDeviceType;                ///< General device type- see documentation
	uint32_t uiMaxPixelImageWidth;        ///< Max allowed image width in pixels
	uint32_t uiMaxPixelImageHeight;       ///< max allowed image height in pixels
	uint32_t uiAvailablePixelDepths;      ///< Bit is set if pixel depth allowed (lsb= pixel depth 1)
	uint32_t uiBinningsTableSize;         ///< 0= 1:1 binning only
	uint32_t uiBlackLevelMax;             ///< Max Value Allowed (see #FPROSensor_SetBlackLevelAdjust())
	uint32_t uiBlackSunMax;               ///< Max Value Allowed (see #FPROSensor_SetBlackSunAdjust())
	uint32_t uiLDRGain;                   ///< Number of Gain Values (LDR or Top Half)
	uint32_t uiHDRGain;                   ///< Number Of Gain Values (HDR or Bottom Half)
	uint32_t uiGlobalGain;                ///< Number Of Gain Values
	uint32_t uiRowScanTime;               ///< Row Scan Time in nano secs (LDR)
	uint32_t uiDummyPixelNum;             ///< Number of Post Row Dummy Pixels when enabled. See #FPROFrame_SetDummyPixelEnable
	bool     bHorizontalScanInvertable;   ///< False= Normal scan direction only, True= Inverse Scan Available
	bool     bVerticalScanInvertable;     ///< False= Normal scan direction only, True= Inverse Scan Available
} FPROCAP;
#pragma pack(pop)


// Auxiliary I/O
/// @enum  FPROAUXIO
/// @brief Auxiliary I/O Pins
///
/// The camera makes available auxiliary I/O pins for customer defined use.  This enum simply 
/// assigns a name for each of the pins to be used in the FPROAuxIO_xxx() set of API calls.
/// <br><br> Note that different camera models can support different Aux I/O pins.  Consult your 
/// specific camera documentation for supported pins and physical pin outs.
///
/// @var FPROAUXIO::FPRO_AUXIO_1 
/// @brief Name for AUX I/O Pin 1
/// @var FPROAUXIO::FPRO_AUXIO_2 
/// @brief Name for AUX I/O Pin 2
/// @var FPROAUXIO::FPRO_AUXIO_3 
/// @brief Name for AUX I/O Pin 3
/// @var FPROAUXIO::FPRO_AUXIO_4 
/// @brief Name for AUX I/O Pin 4
///
typedef enum
{
	FPRO_AUXIO_1= 0x01,
	FPRO_AUXIO_2= 0x02,
	FPRO_AUXIO_3= 0x04,
	FPRO_AUXIO_4= 0x08,
} FPROAUXIO;

/// @enum  FPROAUXIO_DIR
/// @brief Auxiliary I/O Pin Direction
///
/// The camera makes available auxiliary I/O pins for customer defined use.  The pins can be defined
/// as inputs or ouputs.  This enum is to be used with the FPROAuxIO_xxx() set of API calls to set the
/// direction of a given AUX I/O pin.  See #FPROAUXIO for more information.
/// <br><br> Note that different camera models can support different Aux I/O pins.  Consult your 
/// specific camera documentation for supported pins and physical pin outs.
///
/// @var FPROAUXIO_DIR::FPRO_AUXIO_DIR_IN 
/// @brief Set AUX I/O pin as an input with respect to the camera.
/// @var FPROAUXIO_DIR::FPRO_AUXIO_DIR_OUT 
/// @brief Set AUX I/O pin as an output with respect to the camera.
typedef enum
{
	FPRO_AUXIO_DIR_IN= 0,
	FPRO_AUXIO_DIR_OUT,
} FPROAUXIO_DIR;

/// @enum  FPROAUXIO_STATE
/// @brief Auxiliary Output State 
///
/// The camera makes available auxiliary I/O pins for customer defined use.  The pins can be defined
/// as inputs or ouputs.  For pins defined as outputs, this enum is to be used with the FPROAuxIO_xxx() 
/// set of API calls to set the state of that pin.  See #FPROAUXIO for more information.
/// <br><br> Note that different camera models can support different Aux I/O pins.  Consult your 
/// specific camera documentation for supported pins and physical pin outs.
///
/// @var FPROAUXIO_STATE::FPRO_AUXIO_STATE_LOW 
/// @brief Pin is in the low state.
/// @var FPROAUXIO_STATE::FPRO_AUXIO_STATE_HIGH 
/// @brief Pin is in the high state.
typedef enum
{
	FPRO_AUXIO_STATE_LOW,
	FPRO_AUXIO_STATE_HIGH,
} FPROAUXIO_STATE;


/// @enum  FPROAUXIO_EXPACTIVETYPE
/// @brief Exposure Active Auxiliary Output
///
/// The camera makes available an auxiliary output pin that signals when an exposure is active.  This
/// enum defines the the set of signal types that may be configured for the output.  Consult your
/// specific camera documentation for the timing details of each of these signal types.
/// <br><br> Note that different camera models can support different Aux I/O pins.  Consult your 
/// specific camera documentation for supported pins and physical pin outs.
///
/// @var FPROAUXIO_EXPACTIVETYPE::FPRO_AUXIO_EXPTYPE_EXPOSURE_ACTIVE 
/// @brief Exposure Active- consult your camera documentation for timing details.
/// @var FPROAUXIO_EXPACTIVETYPE::FPRO_AUXIO_EXPTYPE_GLOBAL_EXPOSURE_ACTIVE 
/// @brief Global Exposure Active- consult your camera documentation for timing details.
/// @var FPROAUXIO_EXPACTIVETYPE::FPRO_AUXIO_EXPTYPE_FIRST_ROW_SYNC 
/// @brief First Row Sync- consult your camera documentation for timing details.
typedef enum
{
	FPRO_AUXIO_EXPTYPE_EXPOSURE_ACTIVE= 0,
	FPRO_AUXIO_EXPTYPE_GLOBAL_EXPOSURE_ACTIVE,
	FPRO_AUXIO_EXPTYPE_FIRST_ROW_SYNC,
	FPRO_AUXIO_EXPTYPE_RESERVED
} FPROAUXIO_EXPACTIVETYPE;

//////////////////////////////////////////
// Camera Open, Close
//////////////////////////////////////////
//////////////////////////////////////////
//////////////////////////////////////////
/// 
/// @brief FPROCam_GetCameraList
/// 
/// Returns a list of cameras detected on the host.  Most often it is the
/// first function called in the API in order to provide a list of
/// available devices to the user.  The information provided in the 
/// FPRODEVICEINFO structure allows unique names to be constructed for
/// each camera. A pointer an FPRODEVICEINFO structure corresponding to a 
/// user selected device is passed to a subsequent call to FPROCam_Open() 
/// in order to connect to the camera.
///
///	@param pDeviceInfo - Pointer to user allocated memory to hold the list of devices.
/// @param pNumDevices - On entry, the max number of devices that may be assigned to the list.
///                      Note that the pDeviceInfo pointer must point to enough memory to hold
///                      the given pNumDevices.  On exit, it contains the number of devices
///                      detected and inserted in the list.  This can be less than the requested
///                      number.  If it returns the requested number, there may be additional 
///                      devices connected.
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
///
LIBFLIPRO_API FPROCam_GetCameraList(FPRODEVICEINFO *pDeviceInfo, uint32_t *pNumDevices);


//////////////////////////////////////////
///
/// @brief Connects to the camera specified by the pDevInfo parameter.  
///
/// This call must complete successfully in order to use any calls in the API
/// that communicate with the camera.
/// The returned handle is passed to all such subsequent API calls.
///
///	@param pDevInfo - Pointer to device description as returned by FPROCam_GetCameraList().
/// @param pHandle - On successful completion, it contains the device handle to use in 
///                  subsequent API calls.
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROCam_Open(FPRODEVICEINFO *pDevInfo, int32_t *pHandle);


//////////////////////////////////////////
///
/// @brief Disconnects from the camera an releases the handle.
///
/// @param iHandle - Connected camera handle to close.
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROCam_Close(int32_t iHandle);

//////////////////////////////////////////
///
/// @brief Returns the version of this API Library.  
///
/// This function may be called
/// at any time, it does not need a device handle to report the API version.
///
///
/// @param pVersion - Buffer for returned NULL terminated version string.
/// @param uiLength - Length of supplied buffer.
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROCam_GetAPIVersion(wchar_t *pVersion, uint32_t uiLength);

//////////////////////////////////////////
///
/// @brief Returns the version information from the connected device.
///
/// @param iHandle - The handle to an open camera device returned from FPROCam_Open().
/// @param pVersion - Structure buffer to return version information.
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROCam_GetDeviceVersion(int32_t iHandle, FPRODEVICEVERS *pVersion);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Frame Data Functions
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////
///
/// @brief Aborts the active image capture.
///
/// The abort function is meant to be called to abort the current image capture.
/// It can be called from a different thread that is performing the image capture
/// as long as the recommended calling pattern for image capture is followed.
/// See FPROFrame_CaptureStart(), FPROFrame_CaptureStop() for a description of
/// the recommended image capture calling pattern.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROFrame_CaptureAbort(int32_t iHandle);

//////////////////////////////////////////
///
/// @brief Ends the active image capture.
///
/// The end function is meant to be called to be called to end the current image capture
/// and allow the capturing thread to retrieve the image data.  It is intended to be
/// be used to end a long exposure prior to the full exposure completing.  Given that,
/// it will normally be called from a different thread that is performing the image capture
/// as long as the recommended calling pattern for image capture is followed.
/// See FPROFrame_CaptureStart(), FPROFrame_CaptureStop() for a description of
/// the recommended image capture calling pattern.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROFrame_CaptureEnd(int32_t iHandle);

//////////////////////////////////////////
///
/// @brief Initiates the capture of the configured image.  
///
/// The image is retrieved using the FPROFrame_GetVideoFrame() API.
///
/// NOTE: In order to ensure data pipe integrity, FPROFrame_CaptureStart(), 
///       FPROFrame_GetVideoFrame(), and FPROFrame_CaptureStop() must be called
///	      from the same thread in a pattern similar to below:
///
/// @code
///
///     FPROFrame_CaptureStart();
///     while (frames_to_get)
///         FPROFrame_GetVideoFrame();
///	    FPROFrame_CaptureStop();
///
/// @endcode
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
/// @param uiFrameCount - Number of frames to capture. 0 == infinite stream.
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROFrame_CaptureStart(int32_t iHandle, uint32_t uiFrameCount);

//////////////////////////////////////////
///
/// @brief Stops the active image capture.
///
/// NOTE: In order to ensure data pipe integrity, FPROFrame_CaptureStart(), 
///       FPROFrame_GetVideoFrame(), and FPROFrame_CaptureStop() must be called
///	      from the same thread in a pattern similar to below:
///
/// @code
///
///     FPROFrame_CaptureStart();
///     while (frames_to_get)
///         FPROFrame_GetVideoFrame();
///	    FPROFrame_CaptureStop();
///
/// @endcode
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROFrame_CaptureStop(int32_t iHandle);

//////////////////////////////////////////
///
/// @brief Initiates the capture of a thumbnail image.  
///
/// The image is transferred over the
/// image endpoint and is retrieved using the FPROFrame_GetThumbnail() API.
/// Thumbnail images are 512x512 pixels.  No Metadata or dummy pixels are included
/// in the image.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROFrame_CaptureThumbnail(int32_t iHandle);

//////////////////////////////////////////
///
/// @brief Retrieves the dummy pixel configuration to be appended row data.  
///
/// If enabled, dummy pixels are appended to every other row of image data 
/// starting with the second row of data.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
///	@param pEnable - true: Dummy pixels will be appended to every other image row
///                  false: Dummy pixels will NOT be appended to every other image row
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROFrame_GetDummyPixelEnable(int32_t iHandle, bool *pEnable);

//////////////////////////////////////////
///
/// @brief Retrieves the dummy row configuration to be appended to frame data.  
///
/// If enabled, dummy rows are appended to image frame data.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
/// @param pNumRows - The number of dummy rows to append to the image frame data.
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROFrame_GetDummyRowPostFrameEnable(int32_t iHandle, uint32_t *pNumRows);

//////////////////////////////////////////
///
/// @brief Retrieves the dummy row configuration to be prepended to frame data.  
///
/// If enabled, dummy rows are prepended to the image frame data.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
/// @param pNumRows - The number of dummy rows to prepend to the image frame data.
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROFrame_GetDummyRowPreFrameEnable(int32_t iHandle, uint32_t *pNumRows);

//////////////////////////////////////////
///
/// @brief Enables image data imaging.
///
/// Image data may be disabled allowing only dummy pixels to be
/// produced for image frames.  If dummy pixels are enabled they are
/// prepended/appended to the image data as configure with the 
/// FPROFrame_SetDummyPixelPreRowEnable() and FPROFrame_SetDummyPixelPreRowEnable()
/// API calls.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
///	@param pEnable - true: Image data pixels enabled, false: Image data pixels disabled.
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROFrame_GetImageDataEnable(int32_t iHandle, bool *pEnable);

//////////////////////////////////////////
///
/// @brief Retrieves the test image data settings.
///
/// When enabled, the camera generates a test pattern rather than capturing image data from the
/// image sensor.  See #FPROFrame_SetTestImageEnable().
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
///	@param pEnable - true: Enable test image data
///                  false: Disables test image data- normal image data produced
/// @param pFormat - Format of the test image data to produce.
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROFrame_GetTestImageEnable(int32_t iHandle, bool *pEnable, FPROTESTIMAGETYPE *pFormat);

//////////////////////////////////////////
///
/// @brief Gets the area of the image sensor being used to produce image frame data.
///
/// Image frames are retrieved using the FPROFrame_GetVideoFrame() API.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
///	@param pColOffset - Start column (pixel) of the image frame
/// @param pRowOffset - Start row (pixel) of the image frame
/// @param pWidth - Width of the image frame in pixels.
/// @param pHeight - Height of the image frame in pixels.
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROFrame_GetImageArea(int32_t iHandle, uint32_t *pColOffset, uint32_t *pRowOffset, uint32_t *pWidth, uint32_t *pHeight);

//////////////////////////////////////////
///
/// @brief Retrieves the current pixel configuration.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
///	@param pPixelDepth - The current pixel depth.
/// @param pPixelLSB - The Pixel LSB offset.
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROFrame_GetPixelConfig(int32_t iHandle, uint32_t *pPixelDepth, uint32_t *pPixelLSB);


//////////////////////////////////////////
///
/// @brief Retrieves the thumbnail image from the camera.
///
/// The image is transferred over the
/// image endpoint and is retrieved using the FPROFrame_GetThumbnail() API.
/// Thumbnail images are 512x512 12 bit pixels in size.  That is, no Metadata 
/// or dummy pixels are included in the image.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
///	@param pFrameData - The buffer to store the frame data
/// @param pSize - Size the of the pFrameData buffer.
///                On return, the number of bytes actually received.
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROFrame_GetThumbnailFrame(int32_t iHandle, uint8_t *pFrameData, uint32_t *pSize);

//////////////////////////////////////////
///
/// @brief Retrieves an image frame from the camera.
///
/// It is important to call this function with the
/// appropriate size buffer for the frame.  That is, the buffer size should be match the
/// expected frame size.  If it is too large, the function will try and read the given size and 
/// may stall the USB connection if no more frame data is available.
///
/// NOTE: In order to ensure data pipe integrity, #FPROFrame_CaptureStart(), 
///       #FPROFrame_GetVideoFrame(), and #FPROFrame_CaptureStop() must be called
///	      from the same thread in a pattern similar to below:
///
/// @code
///
///     FPROFrame_CaptureStart();
///     while (frames_to_get)
///         FPROFrame_GetVideoFrame();
///	    FPROFrame_CaptureStop();
///
/// @endcode
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
///	@param pFrameData - The buffer to store the frame data.
/// @param pSize - Size the of the pFrameData buffer.
///                On return, the number of bytes actually received.
/// @param uiTimeoutMS - How long to wait for a frame to be available.
///                      Assuming you make this call very soon after the FPROFrame_CaptureStart()
///                      call, you should set this to the exposure time.  Internally, the API
///                      blocks (i.e. no communication with the camera) for some time less than 
///                      uiTimeoutMS and then attempts to retrieve the frame.
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROFrame_GetVideoFrame(int32_t iHandle, uint8_t *pFrameData, uint32_t *pSize, uint32_t uiTimeoutMS);

//////////////////////////////////////////
///
/// @brief Retrieves an externally triggered image frame from the camera.
///
/// This function is intended for use when using external trigger sources for image capture.
/// Unlike FPROFrame_GetVideoFrame(), no timeout is specified.  It waits forever until notification
/// of image frame data availability from the camera.  FPROFrame_CaptureAbort() and FPROFrame_CaptureEnd()
/// can be used to cancel the exposure as described in those API calls. 
/// <br>
/// FPROFrame_CaptureStart() is not expected to be called prior to this API because the External Trigger
/// will be supplying the trigger source.  However, if this call is awaiting image data, another thread may
/// call FPROFrame_CaptureStart() to force a trigger.  Once exposed, this function will return the data as
/// if an external trigger occurred. If you do call FPROFrame_CaptureStart() to force a trigger, it is important
/// to call FPROFrame_CaptureStop() after the image is retrieved.
/// <br>
/// It is important to call this function with the
/// appropriate size buffer for the frame.  That is, the buffer size should be match the
/// expected frame size.  If it is too large, the function will try and read the given size and 
/// may stall the USB connection if no more frame data is available.
///
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
///	@param pFrameData - The buffer to store the frame data.
/// @param pSize - Size the of the pFrameData buffer.
///                On return, the number of bytes actually received.
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROFrame_GetVideoFrameExt(int32_t iHandle, uint8_t *pFrameData, uint32_t *pSize);

//////////////////////////////////////////
///
/// @brief Returns whether or not Image Frame data is ready for retrieval.  
///
/// This API is primarily used in conjunction with an external trigger setup.
/// Since the external trigger is not natively synchronized with the software 
/// in any way, a method to determine when the image data is
/// available for retrieval is required.  Users can use this routine to query
/// the camera and wait for image data to be available.  When the function
/// succeeds and *pAvailable is 'true', the user can call the normal FPROFrame_GetVideoFrame()
/// API to retrieve the data.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
///	@param pAvailable - Pointer for returned query. 
///                     true: Image Frame data is available for retrieval
///                     false: Image Frame Data is not available.
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROFrame_IsAvailable(int32_t iHandle, bool *pAvailable);

//////////////////////////////////////////
///
/// @brief Sets the dummy pixel configuration to be appended row data.  
///
/// If enabled, dummy pixels are appended to every other row of image data 
/// starting with the second row of data.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
///	@param bEnable - true: Dummy pixels will be appended to every other image row
///                  false: Dummy pixels will NOT be appended to every other image row
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROFrame_SetDummyPixelEnable(int32_t iHandle, bool bEnable);

//////////////////////////////////////////
///
/// @brief Sets the dummy row configuration to be appended to frame data.  
///
/// If enabled, dummy rows are appended to image frame data.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
/// @param uiNumRows - The number of dummy rows to append to the image frame data data.
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROFrame_SetDummyRowPostFrameEnable(int32_t iHandle, uint32_t uiNumRows);

//////////////////////////////////////////
///
/// @brief Sets the dummy row configuration to be prepended to frame data.  
///
/// If enabled, dummy rows are prepended to image frame data.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
/// @param uiNumRows - The number of dummy rows to prepend to the image frame data.
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROFrame_SetDummyRowPreFrameEnable(int32_t iHandle, uint32_t uiNumRows);

//////////////////////////////////////////
///
/// @brief Enables image data imaging.
///
/// Image data may be disabled allowing only dummy pixels to be
/// produced for image frames.  If dummy pixels are enabled they are
/// prepended/appended to the image data as configure with the 
/// FPROFrame_SetDummyPixelPreRowEnable() and FPROFrame_SetDummyPixelPreRowEnable()
/// API calls.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
///	@param bEnable - true: Enables image data pixels, false: disables image data pixels.
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROFrame_SetImageDataEnable(int32_t iHandle, bool bEnable);

//////////////////////////////////////////
///
/// @brief Enables test image data to be generated rather than normal image data.
///
/// Use this to generate a test pattern rather than capturing image data from the
/// image sensor.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
///	@param bEnable - true: Enable test image data
///                  false: Disables test image data- normal image data produced
/// @param eFormat - Format of the test image data to produce. This parameter is
///                  ignored if bEnable == false.
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROFrame_SetTestImageEnable(int32_t iHandle, bool bEnable, FPROTESTIMAGETYPE eFormat);

//////////////////////////////////////////
///
/// @brief Sets the area of the image sensor to be used for Tracking Frames during image capture.
///
/// The tracking frames are retrieved as normal image frames using the 
/// FPROFrame_GetVideoFrame() API.  The image frame follow the tracking 
/// frames in the USB stream.
/// The exposure time setting set with FPROCtrl_SetExposure() applies to the
/// tracking frames. As such, the total exposure time for your image frame will
/// be exposure_time * uiNumTrackingFrames.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
///	@param uiStartRow - Start row of the tracking frame (inclusive).
/// @param uiEndRow - End row of the tracking frame (inclusive).
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROFrame_SetTrackingArea(int32_t iHandle, uint32_t uiStartRow, uint32_t uiEndRow);

//////////////////////////////////////////
///
/// @brief Enables the production of Tracking Frames by the camera.
///
/// There will be uiNumTrackingFrames produced for every image frame
/// produced.  The image frame follow the tracking frames in the USB stream.
/// The exposure time setting set with FPROCtrl_SetExposure() applies to the
/// tracking frames. As such, the total exposure time for your image frame will
/// be exposure_time * uiNumTrackingFrames.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
///	@param uiNumTrackingFrames - Number of Tracking frames to produce.  0 disables Tracking Frames.
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROFrame_SetTrackingAreaEnable (int32_t iHandle, uint32_t uiNumTrackingFrames);

//////////////////////////////////////////
///
/// @brief Sets the current pixel configuration to the specified values.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
///	@param uiPixelDepth - The current pixel depth.
/// @param uiPixelLSB - The Pixel LSB offset.
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROFrame_SetPixelConfig(int32_t iHandle, uint32_t uiPixelDepth, uint32_t uiPixelLSB);

//////////////////////////////////////////
///
/// @brief Sets the area of the image sensor to be used to produce image frame data.
///
/// Image frames are retrieved using the FPROFrame_GetVideoFrame() API.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
///	@param uiColOffset - Start column (pixel) of the image frame
/// @param uiRowOffset - Start row (pixel) of the image frame
/// @param uiWidth - Width of the image frame in pixels.
/// @param uiHeight - Height of the image frame in pixels.
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROFrame_SetImageArea(int32_t iHandle, uint32_t uiColOffset, uint32_t uiRowOffset, uint32_t uiWidth, uint32_t uiHeight);



///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
// Control Functions
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////
///
/// @brief Reads the current duty cycle of the cooler.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
/// @param pDutyCycle - Returned Cooler Duty Cycle 0-100%.
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROCtrl_GetCoolerDutyCycle(int32_t iHandle, uint32_t *pDutyCycle);

//////////////////////////////////////////
///
/// @brief Reads the exposure time of the image sensor.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
/// @param pExposureTime - Returned exposure time in nanoseconds.
/// @param pFrameDelay - Returned frame delay (end -to-start) time in nanoseconds.
///                      When performing multiple exposures with a single trigger, the frame 
///                      delay is the time from the end of expsosure of a frame
///                      to the start of exposure of the next frame.
/// @param pImmediate - This parameter affects the way exposure starts when the 
///                     FPROFrame_CaptureStart() function is called.  The camera
///                     image sensor is continually exposing its pixels on a row
///                     by row basis. When this parameter is set to true, the exposure 
///                     for the frame begins at whatever image sensor row is currently
///                     being exposed.  The raw image data returned by the FPROFrame_GetVideoFrame()
///                     call begins with this row (most likely not row 0).  The row that
///                     starts the raw video frame is recorded in the meta data for the image frame.
///
///                     When this parameter is set to false, the camera waits until row 0 is
///                     being exposed before starting the frame exposure.  In this case, the
///                     image frame data returned by the FPROFrame_GetVideoFrame() call will
///                     start at row 0.
///
/// NOTE: The Exposure Time and Frame Delay values are translated to camera specific units which
///       are in turn dependent on the current imaging mode of the camera (e.g. LDR vs HDR).
///       The API does its best to perform the necessary conversions automatically
///       when the modes are changed. It is recommended, however,  to make sure the 
///       desired exposure times are set after imaging modes have changed using this 
///       and its counterpart API function FPROCtrl_SetExposure().
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROCtrl_GetExposure(int32_t iHandle, uint64_t *pExposureTime, uint64_t *pFrameDelay, bool *pImmediate);


//////////////////////////////////////////
///
/// @brief Returns the external trigger settings of the camera.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
/// @param pEnable - true: External trigger enabled, false: External trigger disabled
/// @param pTrigType - The behavior of the external trigger signal.  See #FPROEXTTRIGTYPE.
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROCtrl_GetExternalTriggerEnable(int32_t iHandle, bool *pEnable, FPROEXTTRIGTYPE *pTrigType);

//////////////////////////////////////////
///
/// @brief Turns the Fan on or off at the specified power level.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
/// @param pOn - true: turns the fan on, false: turns the fan off
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROCtrl_GetFanEnable(int32_t iHandle, bool *pOn);

//////////////////////////////////////////
///
/// @brief Reads the current heater configuration.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
/// @param pPwrPercentage - The setting of the heater in percent.
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROCtrl_GetHeaterPower(int32_t iHandle, uint32_t *pPwrPercentage);

//////////////////////////////////////////
///
/// @brief Gets the delay between setting the Illumination on/off via FPROCtrl_SetIlluminationOn()
/// and when the illumination actually activates.  Each increment is TBD msecs.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
/// @param pOnDelay - Delay for turning the illumination on in TBD msec increments
/// @param pOffDelay - Delay for turning the illumination off in TBD msec increments
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROCtrl_GetIlluminationDelay(int32_t iHandle, uint32_t *pOnDelay, uint32_t *pOffDelay);

//////////////////////////////////////////
///
/// @brief Returns the setting of External Illumination- on or off.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
/// @param pOn - true: illumination on, false: illumination off
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROCtrl_GetIlluminationOn(int32_t iHandle, bool *pOn);

//////////////////////////////////////////
///
/// @brief Reads the internal sensor temperature of the camera.  
///
/// Note that if this
/// function is called during an exposure and 'Read During Exposure' is not
/// enabled, the internal sensor temperature is not explicitly read.  The
/// last value successfully read will be returned.  See the
/// FPROCtrl_SetSensorTemperatureReadEnable() API for more information.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
/// @param pTemp - For the returned temperature, deg C.
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROCtrl_GetSensorTemperature(int32_t iHandle, int32_t *pTemp);

//////////////////////////////////////////
///
/// @brief Returns the 'read sensor temperature during exposure' enabled flag.
///
/// See FPROCtrl_SetSensorTemperatureReadEnable() for more details.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
/// @param pEnable - true: 'read sensor temperature during exposure' Enabled
///                  false: 'read sensor temperature during exposure' Disabled
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROCtrl_GetSensorTemperatureReadEnable(int32_t iHandle, bool *pEnable);


//////////////////////////////////////////
///
/// @brief Gets the current shutter setting.
///
/// By default the camera controls the shutter during exposures.  In order for 
/// the #FPROCtrl_SetShutterOpen() API to correctly control the shutter,
/// the #FPROCtrl_SetShutterOverride() API must be called prior with the override 
/// parameter set to True.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
/// @param pOpen - true: Shutter open, false: Shutter closed
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROCtrl_GetShutterOpen(int32_t iHandle, bool *pOpen);

//////////////////////////////////////////
///
/// @brief Gets the current shutter override setting.
///
/// By default the camera controls the shutter during exposures.  In order for 
/// the #FPROCtrl_SetShutterOpen() API to correctly control the shutter,
/// the shutter override must be set to True.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
/// @param pOverride - true: Allows user control of shutter, false: Camera controls shutter
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROCtrl_GetShutterOverride(int32_t iHandle, bool *pOverride);

//////////////////////////////////////////
///
/// @brief Reads the various temperatures sensors of the camera.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
/// @param pAmbientTemp - For the returned Ambient temperature, deg C.
/// @param pBaseTemp - For the returned Base temperature, deg C.
/// @param pCoolerTemp - For the returned Cooler temperature, deg C.
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROCtrl_GetTemperatures(int32_t iHandle, double *pAmbientTemp, double *pBaseTemp, double *pCoolerTemp);

//////////////////////////////////////////
///
/// @brief Returns the Base Temperature Set Point.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
/// @param pSetPoint - The setpoint value in -75 to 70 degC.
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROCtrl_GetTemperatureSetPoint(int32_t iHandle, double *pSetPoint);

//////////////////////////////////////////
///
/// @brief Sets the exposure time of the image sensor.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
/// @param uiExposureTime - Exposure time in nanosecond increments.
/// @param uiFrameDelay - Frame delay (end - to - start) time in nanoseconds.
///                       When performing multiple exposures with a single trigger, the frame 
///                       delay is the time from the end of exposure of a frame
///                       to the start of exposure of the next frame.
/// @param bImmediate - This parameter affects the way exposure starts when the 
///                     FPROFrame_CaptureStart() function is called.  The camera
///                     image sensor is continually exposing its pixels on a row
///                     by row basis. When this parameter is set to true, the exposure 
///                     for the frame begins at whatever image sensor row is currently
///                     being exposed.  The raw image data returned by the FPROFrame_GetVideoFrame()
///                     call begins with this row (most likely not row 0).  The row that
///                     starts the raw video frame is recorded in the meta data for the image frame.
///
///                     When this parameter is set to false, the camera waits until row 0 is
///                     being exposed before starting the frame exposure.  In this case, the
///                     image frame data returned by the FPROFrame_GetVideoFrame() call will
///                     start at row 0.
///
/// NOTE: The Exposure Time and Frame Delay values are translated to camera specific units which
///       are in turn dependent on the current imaging mode of the camera (e.g. LDR vs HDR).
///       The API does its best to perform the necessary conversions automatically
///       when the modes are changed. It is recommended, however,  to make sure the 
///       desired exposure times are set after imaging modes have changed using this 
///       and its counterpart API function FPROCtrl_GetExposureTime().
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROCtrl_SetExposure(int32_t iHandle, uint64_t uiExposureTime, uint64_t uiFrameDelay, bool bImmediate);

//////////////////////////////////////////
///
/// @brief Enables or disables the external trigger of the camera.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
/// @param bEnable - true: enables the external trigger, false: disable the external trigger
/// @param eTrigType - Sets the behavior of the external trigger signal.  See #FPROEXTTRIGTYPE.
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROCtrl_SetExternalTriggerEnable(int32_t iHandle, bool bEnable, FPROEXTTRIGTYPE eTrigType);

//////////////////////////////////////////
///
/// @brief Turns the Fan on or off.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
/// @param bOn - true: turns the fan on, false: turns the fan off
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROCtrl_SetFanEnable(int32_t iHandle, bool bOn);

//////////////////////////////////////////
///
/// @brief Turns the Heater on or off at the specified power level.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
/// @param uiPwrPercentage -Specifies the power level as a percentage (0-100)
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROCtrl_SetHeaterPower(int32_t iHandle,uint32_t uiPwrPercentage);

//////////////////////////////////////////
///
/// @brief Sets the illumination delay.
///
/// The illumination delay is the time between setting the Illumination on/off via 
/// #FPROCtrl_SetIlluminationOn and when the illumination actually activates.  
/// Each increment is TBD msecs.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
/// @param uiOnDelay - Delay for turning the illumination on in TBD msec increments
/// @param uiOffDelay - Delay for turning the illumination off in TBD msec increments
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROCtrl_SetIlluminationDelay(int32_t iHandle, uint16_t uiOnDelay, uint16_t uiOffDelay);

//////////////////////////////////////////
///
/// @brief Turns External Illumination on or off.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
/// @param bOn - true: turns illumination on, false: turns illumination off
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROCtrl_SetIlluminationOn(int32_t iHandle, bool bOn);

//////////////////////////////////////////
///
/// @brief Turn the LED on or off.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
/// @param bOn - true: Turns the LED on, false: Turns the LED off
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROCtrl_SetLED(int32_t iHandle, bool bOn);

//////////////////////////////////////////
///
/// @brief Enables/disables physical reading of the image sensor temperature during exposures.  
///
/// The sensor temperature is read using the #FPROCtrl_GetSensorTemperature
/// API call.  If that call is made during an exposure, it will physically read
/// the sensor temperature only if this call was made prior to enable the reading.
/// If this 'read sensor temperature during exposure' is not enabled, then the
/// #FPROCtrl_GetSensorTemperature call will return the previous successful temperature
/// read from the imaging sensor.
/// <br>NOTE: This enable only pertains to requests during an exposure. If temperature 
/// readings are requested outside of an exposure boundary, this enable has no effect.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
/// @param bEnable - true: Enable 'read sensor temperature during exposure'.
///                  false: Disable 'read sensor temperature during exposure'.
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROCtrl_SetSensorTemperatureReadEnable(int32_t iHandle, bool bEnable);


//////////////////////////////////////////
///
/// @brief Opens/Close the shutter.
///
/// By default the camera controls the shutter during exposures.  In order for 
/// the #FPROCtrl_SetShutterOpen() API to correctly control the shutter,
/// the #FPROCtrl_SetShutterOverride() API must be called prior with the override 
/// parameter set to True.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
/// @param bOpen - true: opens the shutter, false: closes the shutter
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROCtrl_SetShutterOpen(int32_t iHandle, bool bOpen);

//////////////////////////////////////////
///
/// @brief Sets the shutter control override.
///
/// By default the camera controls the shutter during exposures.  In order for 
/// the #FPROCtrl_SetShutterOpen() API to correctly control the shutter,
/// the shutter override must be set to True.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
/// @param bOverride - true: Allows user control of shutter, false: Camera controls shutter
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROCtrl_SetShutterOverride(int32_t iHandle, bool bOverride);

//////////////////////////////////////////
///
/// @brief Sets the Base Temperature Set Point.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
/// @param dblSetPoint - The setpoint value to set in -75 to 70 degC.
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROCtrl_SetTemperatureSetPoint(int32_t iHandle, double dblSetPoint);

//////////////////////////////////////////
// Sensor Functions
//////////////////////////////////////////
//////////////////////////////////////////
///
/// @brief Retrieves the current pixel bin settings.
///
/// The bin setting for horizontal (x direction) and vertical (Y direction) binning.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
/// @param pXBin - For returned horizontal bin setting.
/// @param pYBin - For returned vertical bin setting.
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROSensor_GetBinning(int32_t iHandle, uint32_t *pXBin, uint32_t *pYBin);

//////////////////////////////////////////
///
/// @brief Retrieves the current Black Level Adjustment values.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
/// @param pAdjustValue - For returned adjustment values.
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROSensor_GetBlackLevelAdjust(int32_t iHandle, uint32_t *pAdjustValue);

//////////////////////////////////////////
///
/// @brief Retrieves the current Black Sun Adjustment values.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
/// @param pAdjustValue - For returned adjustment values.
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROSensor_GetBlackSunAdjust(int32_t iHandle, uint32_t *pAdjustValue);

//////////////////////////////////////////
///
/// @brief Retrieves the sensor capabilities structure from the camera.  
///
/// Note that you
/// may pass in a generic buffer in which to store the capabilities rather than a 
/// strict FPROCAP structure pointer.  The pCapLength parameter is the length of
/// the buffer you are passing.  It is used to determine if there us enough space in 
/// the buffer prior to reading he capabilities.
///
/// The camera is asked for the size of the capabilities prior to retrieving them.
/// If sufficient space exists (as specified but the pCapLength parameter), then
/// the capabilities are retrieved.  This sequence allows older API's to work with
/// newer cameras and different capabilities structures.  The length and version fields
/// should be parsed by the caller in order to determine the proper structure format.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
/// @param pCap - Buffer to store the capabilities structure
/// @param pCapLength - On entry the size of the buffer pointed to by pCap.  On exit,
///                     the actual size of the structure returned.
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROSensor_GetCapabilities(int32_t iHandle, FPROCAP *pCap, uint32_t *pCapLength);

//////////////////////////////////////////
///
/// @brief Retrieves the current setting for the Gain for the specified table.
///
/// Note: The index returned is the index into the table as returned by 
///       FPROSensor_GetGainTable().
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
/// @param eTable - The table to retrieve.
/// @param pGainIndex - Buffer for the returned index.
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROSensor_GetGainIndex(int32_t iHandle, FPROGAINTABLE eTable, uint32_t *pGainIndex);

//////////////////////////////////////////
///
/// @brief Retrieves the specified Gain Table. 
///
/// The pNumEntries command should be derived from the
/// uiLDRGain, uiHDRGain, or uiGlobalGain values in the #FPROCAP capabilities structure.
///
/// Note: Each gain that is returned is scaled to produce an integer number.
///       The scale factor is defined as #FPRO_GAIN_SCALE_FACTOR in the API.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
/// @param eTable - The table to retrieve.
/// @param pGainValues - Buffer for the returned values.
/// @param pNumEntries - On entry, the number of locations available in the buffer.
///                      On exit, the actual number of values inserted in the table.
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROSensor_GetGainTable(int32_t iHandle, FPROGAINTABLE eTable, uint32_t *pGainValues, uint32_t *pNumEntries);

//////////////////////////////////////////
///
/// @brief Retrieves the current setting for HDR enable.
///
/// The HDR setting is enabled/disabled by setting the appropriate mode using the #FPROSensor_SetMode
/// API.  As such there is no corresponding 'set' function for HDR enable.  This
/// API is simply a convenience function to allow a user to determine if the selected
/// mode has enabled HDR frames.  Knowing this is of course critical to determining how much
/// image data will be supplied by the camera for each exposure. When HDR is enabled, there will
/// twice the image data returned for a frame (including dummy pixels).
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
/// @param pEnable - Buffer for the returned HDR enable flag.
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROSensor_GetHDREnable(int32_t iHandle, bool *pEnable);

//////////////////////////////////////////
///
/// @brief Retrieves the current mode name for the specified index.  
///
/// The number of available modes are retrieved using the #FPROSensor_GetModeCount API.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
/// @param uiModeIndex - The index of the mode to retrieve.
/// @param pMode - Buffer for the returned mode information.
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROSensor_GetMode(int32_t iHandle, uint32_t uiModeIndex, FPROSENSMODE *pMode);

//////////////////////////////////////////
///
/// @brief Retrieves the current mode count and current mode index setting.
///
/// Mode information for a given index is retrieved using the 
/// #FPROSensor_GetMode() API.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
/// @param pCount - Buffer for the number of available modes.
/// @param pCurrentMode - Buffer for index of the currently assigned mode.
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROSensor_GetModeCount(int32_t iHandle, uint32_t *pCount,uint32_t *pCurrentMode);

//////////////////////////////////////////
///
/// @brief Retrieves the current pixel scan direction settings on the sensor.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
/// @param pHInverted - Returned horizontal scan direction: False = Normal, True = Inverted.
/// @param pVInverted - Returned vertical scan direction: False = Normal, True = Inverted.
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROSensor_GetScanDirection(int32_t iHandle, bool *pHInverted,bool *pVInverted);

//////////////////////////////////////////
///
/// @brief Sets the desired horizontal and vertical binning.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
/// @param uiXBin - Horizontal bin setting.
/// @param uiYBin - Vertical bin setting.
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROSensor_SetBinning(int32_t iHandle, uint32_t uiXBin, uint32_t uiYBin);

//////////////////////////////////////////
///
/// @brief Sets the current Black Level Adjustment values.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
/// @param uiAdjustValue - Value to set.
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROSensor_SetBlackLevelAdjust(int32_t iHandle, uint32_t uiAdjustValue);

//////////////////////////////////////////
///
/// @brief Sets the current Black Sun Adjustment values.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
/// @param uiAdjustValue - Value to set.
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROSensor_SetBlackSunAdjust(int32_t iHandle, uint32_t uiAdjustValue);

//////////////////////////////////////////
///
/// @brief Sets the current setting for the Gain for the specified table.
///
/// Note: The index is the index into the table as returned by 
///       FPROSensor_GetGainTable().
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
/// @param eTable - The table to retrieve.
/// @param uiGainIndex - Index value to set.
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROSensor_SetGainIndex(int32_t iHandle, FPROGAINTABLE eTable, uint32_t uiGainIndex);

//////////////////////////////////////////
///
/// @brief Sets the current mode specified by the given index.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
/// @param uiModeIndex - The index of the mode to set.
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROSensor_SetMode(int32_t iHandle, uint32_t uiModeIndex);

//////////////////////////////////////////
///
/// @brief Retrieves the current pixel scan direction settings on the sensor.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
/// @param bHInverted - Horizontal scan direction: False = Normal, True = Inverted.
/// @param bVInverted - Vertical scan direction: False = Normal, True = Inverted..
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROSensor_SetScanDirection(int32_t iHandle, bool bHInverted, bool bVInverted);

//////////////////////////////////////////
// Auxiliary I/O Support Functions
//////////////////////////////////////////
//////////////////////////////////////////
///
/// @brief Gets the direction and state for given Auxiliary I/O pin.
///
/// For Output pins, the state of the pin will be the value last set with the
/// #FPROAuxIO_GetPin() call.  For Input pins, the state of the pin reflects the 
/// state of the physical input signal.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
/// @param eAuxIO - Auxiliary I/O pin to retrieve.
/// @param pDirection - Pin direction.
/// @param pState - Pin state. May be NULL if you do not want the state.
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROAuxIO_GetPin(int32_t iHandle, FPROAUXIO eAuxIO, FPROAUXIO_DIR *pDirection, FPROAUXIO_STATE *pState);

//////////////////////////////////////////
///
/// @brief Get Exposure Active Type Signal.
///
/// This function gets the Exposure Type Signal for the Exposure Type Auxiliary 
/// output pin.  Consult your documentation for signal timing details.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
/// @param pType - Exposure Active Signal Type. 
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROAuxIO_GetExposureActiveType(int32_t iHandle, FPROAUXIO_EXPACTIVETYPE *pType);

//////////////////////////////////////////
///
/// @brief Sets the direction and state for given Auxiliary I/O pin.
///
/// Note that the state is only applicable to output pins.  It is ignored
/// for input pins.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
/// @param eAuxIO - Auxiliary I/O pin to configure.
/// @param eDirection - Pin direction to set.
/// @param eState - Pin state to set for output pins.
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROAuxIO_SetPin(int32_t iHandle, FPROAUXIO eAuxIO, FPROAUXIO_DIR eDirection, FPROAUXIO_STATE eState);

//////////////////////////////////////////
///
/// @brief Exposure Active Type Signal.
///
/// This function sets the Exposure Type Signal for the Exposure Type Auxiliary 
/// output pin.  Consult your documentation for signal timing details.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
/// @param eType - Exposure Active Signal Type.
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROAuxIO_SetExposureActiveType(int32_t iHandle, FPROAUXIO_EXPACTIVETYPE eType);

//////////////////////////////////////////
// Frame Acknowledgment Mode Support Functions
//////////////////////////////////////////
//////////////////////////////////////////
///
/// @brief Get Frame Acknowledgment Mode Enable.
///
/// This function gets the Frame Acknowledgment Mode Enable.  If true, Frame
/// Acknowledgment Mode is enabled.  
/// <br>
/// Frame Acknowledgment Mode is a camera mode that instructs the camera to store
/// each frame as it is exposed in an internal memory.  The frame in the memory
/// may be retransmitted to the host using the #FPROFAck_FrameResend() API.  Each frame -must- be
/// explicitly acknowledged by the user using the #FPROFAck_FrameAcknowledge() API.  This allows
/// the camera to delete the frame from its memory queue making it available for the next frame.
/// API function.
/// <br>
/// This mode is intended for users who require every image to be successully tramnsmitted to the host
/// even in the face of cable and unrecoverable transmission errors.  Because of the required acknowledgments,
/// this mode is significantly slower with respect to achievable frame rate and dependent on the host computer.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
/// @param pEnable - The Frame Acknowldgement Mode enable- true if enabled, false otherwise. 
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROFAck_GetEnable(int32_t iHandle, bool *pEnable);

//////////////////////////////////////////
///
/// @brief Set Frame Acknowledgment Mode Enable.
///
/// This function sets the Frame Acknowledgment Mode Enable.  If true, Frame
/// Acknowledgment Mode is enabled.  See #FPROFAck_GetEnable() for a description
/// of this mode.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
/// @param bEnable - The Frame Acknowldgement Mode enable- True if enabled, False otherwise. 
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROFAck_SetEnable(int32_t iHandle, bool bEnable);

//////////////////////////////////////////
///
/// @brief Acknowledge the last frame sent in Frame Acknowledgment Mode.
///
/// This function acknowledges the last from sent to the host.
/// See #FPROFAck_GetEnable() for a description of this mode.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROFAck_FrameAcknowledge(int32_t iHandle);


//////////////////////////////////////////
///
/// @brief Resend the last frame in Frame Acknowledgment Mode.
///
/// This function instructs the camera to resend the last image frame to the
/// host.  Is expected to be called by an application in the event of transmission 
/// errors or errors detected during parsing of the image data.
/// See #FPROFAck_GetEnable() for a description of this mode.
/// <br>
/// The frame data will be available immediately after this call so it is important
/// to call #FPROFrame_GetVideoFrame() with the proper parameters immediately after this call 
/// returns.
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROFAck_FrameResend(int32_t iHandle);

//////////////////////////////////////////
///
/// @brief Flush the in memory frame queue in Frame Acknowledgment Mode.
///
/// This function instructs the camera to flush all of the image frames contained
/// in its internal image frame queue.
/// See #FPROFAck_GetEnable() for a description of this mode.
/// <br>
///
///	@param iHandle - The handle to an open camera device returned from FPROCam_Open()
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API FPROFAck_FlushImageQueue(int32_t iHandle);


//////////////////////////////////////////
// Low level commands and functions
//////////////////////////////////////////
/// @private
LIBFLIPRO_API FPROCmd_SendRaw(int32_t iHandle, uint8_t *pData, uint32_t uiLength);
/// @private
LIBFLIPRO_API FPROCmd_SendRecvRaw(int32_t iHandle, uint8_t *pTxData, uint32_t uiTxLength, uint8_t *pRxData, uint32_t *pRxLength);
/// @private
LIBFLIPRO_API FPROCmd_ReadReg(int32_t iHandle, uint32_t uiReg, uint32_t *pValue);
/// @private
LIBFLIPRO_API FPROCmd_WriteReg(int32_t iHandle, uint32_t uiReg, uint32_t uiValue, uint32_t uiMask);

// Debug Functions
/// @cond DO_NOT_DOCUMENT
// Conversion strings to aid in debug printing
// The Linux part is different because of the way
// swprintf() works- when %s is used, the argument is
// assumed to be a char pointer.  Hence we do not make it wide.
#ifdef WIN32
#define STRINGIFY(x) L##x
#define MAKEWIDE(x) STRINGIFY(x)
#else
#define MAKEWIDE(x) (x)
#endif
/// @endcond

//////////////////////////////////////////
///
/// Enables the given debug level and above.
///
///	@param bEnable - Overall enable for debug output
/// @param eLevel - The level to enable if bEnable is true;
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API  FPRODebug_EnableLevel(bool bEnable, FPRODBGLEVEL eLevel);
//////////////////////////////////////////
///
/// Sets the log file path to the given folder.  The file name is auto generated.
///
///	@param pPath - Path (folder) in which to place the log file.
///
/// @return Greater than or equal to 0 on success, less than 0 on failure.
LIBFLIPRO_API  FPRODebug_SetLogPath(const wchar_t *pPath);
#ifdef WIN32 
//////////////////////////////////////////
///
/// Writes the given information to the log file if the given level is enabled.
/// <br>The parameters support the basic printf type of formatting.
///
///	@param eLevel - Debug level of the log message.
/// @param format - printf style formatting string
/// @param ...    - printf arguments for the format string
///
/// @return None.
LIBFLIPRO_VOID _cdecl FPRODebug_Write(FPRODBGLEVEL eLevel, const wchar_t *format, ...);
#else
//////////////////////////////////////////
/// FPRODebug_Write
/// Writes the given information to the log file if the given level is enabled.
/// <br>The parameters support the basic printf type of formatting.
///
///	@param eLevel - Debug level of the log message.
/// @param format - printf style formatting string
/// @param ...    - printf arguments for the format string
///
/// @return None.
LIBFLIPRO_VOID FPRODebug_Write(FPRODBGLEVEL eLevel, const wchar_t *format, ...);
//LIBFLIPRO_VOID FLIDebug(int32_t level, char *format, ...);
#endif




#endif   // _LIBFLIPRO_H_

#ifdef __cplusplus
}

#endif
