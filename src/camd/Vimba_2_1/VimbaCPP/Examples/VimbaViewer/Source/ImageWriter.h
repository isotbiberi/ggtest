#ifndef IMAGEWRITER_H
#define IMAGEWRITER_H

#include <QLibrary>
#include <QString>
#include <QSharedPointer>
#include <QDebug>
#include <stdint.h>
#include "Helper.h"
#include "VmbTransform.h"

VmbError_t GetBitsPerPixelUsed( VmbUint32_t * BitsPerPixelUsed, VmbPixelFormat_t PixelFormat);

typedef struct tiff TIFF;

// constants for configuring Tiff tag data
#define    IMGWRITER_IMAGEWIDTH         256    /* image width in pixels (TIFFTAG_IMAGEWIDTH) */
#define    IMGWRITER_IMAGELENGTH        257    /* image height in pixels (TIFFTAG_IMAGELENGTH) */
#define    IMGWRITER_BITSPERSAMPLE      258    /* bits per channel (sample) (TIFFTAG_BITSPERSAMPLE) */
#define    IMGWRITER_COMPRESSION        259    /* data compression technique (TIFFTAG_COMPRESSION) */
#define    IMGWRITER_PHOTOMETRIC        262    /* photometric interpretation (TIFFTAG_PHOTOMETRIC) */
#define    IMGWRITER_XRESOLUTION        282    /* pixels/resolution in x (TIFFTAG_XRESOLUTION) */
#define    IMGWRITER_YRESOLUTION        283    /* pixels/resolution in y (TIFFTAG_YRESOLUTION) */
#define    IMGWRITER_RESOLUTIONUNIT     296    /* units of resolutions (TIFFTAG_RESOLUTIONUNIT) */
#define    IMGWRITER_PLANARCONFIG       284    /* storage organization (TIFFTAG_PLANARCONFIG) */
#define    IMGWRITER_SAMPLESPERPIXEL    277    /* samples per pixel (TIFFTAG_SAMPLESPERPIXEL) */


class ImageWriter
{
public:
    bool LoadLibTiff        ( void );
    void Setup              ( const tFrameInfo & inputFrame );
    QString GetLastError    ( void );
    bool WriteTiff          ( const char* filename );

private:
    bool WriteStandardTags  ( void );
    bool WriteFrameBuffer   ( void );
    bool IsMonochrome       ( VmbPixelFormat_t format );
    bool IsPackedFormat     ( VmbPixelFormat_t format );
    int GetBytesPerPixel    ( unsigned int allocatedBits );
    bool UnpackToMono16     ( unsigned char *pInBuffer, unsigned short *pOutBuffer);

    TIFF * m_TiffFile;
    QLibrary m_LibTiffLibrary;

    QSharedPointer<unsigned char> m_ImageData;
    VmbPixelFormat_t m_PixelFormat;
    bool m_ColorInterpolation;
    VmbUint32_t m_ImageWidth;
    VmbUint32_t m_ImageHeight;
    VmbUint32_t m_ImageSizePixels;
    VmbUint32_t m_ImageSizeBytes; 
    VmbUint32_t m_BitsPerSample;
    VmbUint32_t m_BitsPerPixelUsed;
    VmbUint32_t m_BytesPerPixel;
    VmbUint32_t m_ColorsPerPixel;

    // function pointer typedefs
    typedef TIFF* (*TIFFOpenPrototype)( const char*, const char* );
    typedef void (*TIFFClosePrototype)( TIFF* );
    typedef int (*TIFFSetFieldProtoype)( TIFF*, uint32_t, ... );
    typedef int (*TIFFWriteScanlinePrototype)( TIFF*, void*, uint32_t, uint16_t );
};

// byte alignment for 12bit packet formats
typedef struct 
{
    unsigned char LByte;
    unsigned char MByte;
    unsigned char UByte;

} Packed12BitsPixel_t;

#endif // IMAGEWRITER_H