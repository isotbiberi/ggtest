#include "ImageWriter.h"

VmbError_t GetBitsPerPixelUsed( VmbUint32_t * BitsPerPixelUsed, VmbPixelFormat_t PixelFormat)
{
    if( BitsPerPixelUsed == 0 ) return VmbErrorBadParameter;
    switch(PixelFormat)
    {
    case VmbPixelFormatBayerGR8:
    case VmbPixelFormatBayerRG8:
    case VmbPixelFormatBayerGB8:
    case VmbPixelFormatBayerBG8:
    case VmbPixelFormatMono8:
        *BitsPerPixelUsed = 8;
        return VmbErrorSuccess;
    case VmbPixelFormatBayerGR10:
    case VmbPixelFormatBayerRG10:
    case VmbPixelFormatBayerGB10:
    case VmbPixelFormatBayerBG10:
    case VmbPixelFormatBayerGR10p:
    case VmbPixelFormatBayerRG10p:
    case VmbPixelFormatBayerGB10p:
    case VmbPixelFormatBayerBG10p:
    case VmbPixelFormatMono10:
    case VmbPixelFormatMono10p:
        *BitsPerPixelUsed = 10;
        return VmbErrorSuccess;
    case VmbPixelFormatBayerGR12:
    case VmbPixelFormatBayerRG12:
    case VmbPixelFormatBayerGB12:
    case VmbPixelFormatBayerBG12:
    case VmbPixelFormatMono12:
    case VmbPixelFormatMono12Packed:
    case VmbPixelFormatMono12p:
    case VmbPixelFormatBayerGR12Packed:
    case VmbPixelFormatBayerRG12Packed:
    case VmbPixelFormatBayerGB12Packed:
    case VmbPixelFormatBayerBG12Packed:
    case VmbPixelFormatBayerGR12p:
    case VmbPixelFormatBayerRG12p:
    case VmbPixelFormatBayerGB12p:
    case VmbPixelFormatBayerBG12p:
        *BitsPerPixelUsed = 12;
        return VmbErrorSuccess;
    case VmbPixelFormatMono14:
        *BitsPerPixelUsed = 14;
        return VmbErrorSuccess;
    case VmbPixelFormatBayerGR16:
    case VmbPixelFormatBayerRG16:
    case VmbPixelFormatBayerGB16:
    case VmbPixelFormatBayerBG16:
    case VmbPixelFormatMono16:
        *BitsPerPixelUsed = 16;
        return VmbErrorSuccess;
    case VmbPixelFormatRgb8:
    case VmbPixelFormatBgr8:
        *BitsPerPixelUsed = 24;
        return VmbErrorSuccess;
    case VmbPixelFormatArgb8:
    case VmbPixelFormatBgra8:
        *BitsPerPixelUsed = 32;
        return VmbErrorSuccess;
    case VmbPixelFormatRgb10:
    case VmbPixelFormatBgr10:
        *BitsPerPixelUsed = 30;
        return VmbErrorSuccess;
    case VmbPixelFormatRgb12:
    case VmbPixelFormatBgr12:
        *BitsPerPixelUsed = 36;
        return VmbErrorSuccess;
    case VmbPixelFormatRgb14:
    case VmbPixelFormatBgr14:
        *BitsPerPixelUsed = 42;
        return VmbErrorSuccess;
    case VmbPixelFormatRgb16:
    case VmbPixelFormatBgr16:
        *BitsPerPixelUsed = 48;
        return VmbErrorSuccess;
    case VmbPixelFormatRgba10:
    case VmbPixelFormatBgra10:
        *BitsPerPixelUsed = 40;
        return VmbErrorSuccess;
    case VmbPixelFormatRgba12:
    case VmbPixelFormatBgra12:
        *BitsPerPixelUsed = 48;
        return VmbErrorSuccess;
    case VmbPixelFormatRgba14:
    case VmbPixelFormatBgra14:
        *BitsPerPixelUsed = 56;
        return VmbErrorSuccess;
    case VmbPixelFormatRgba16:
    case VmbPixelFormatBgra16:
        *BitsPerPixelUsed = 64;
        return VmbErrorSuccess;
    case VmbPixelFormatYuv411:
    case VmbPixelFormatYCbCr411_8_CbYYCrYY:
        *BitsPerPixelUsed = 12;
        return VmbErrorSuccess;
    case VmbPixelFormatYuv422:
    case VmbPixelFormatYCbCr422_8_CbYCrY:
        *BitsPerPixelUsed = 16;
        return VmbErrorSuccess;
    case VmbPixelFormatYuv444:
    case VmbPixelFormatYCbCr8_CbYCr:
        *BitsPerPixelUsed = 24;
        return VmbErrorSuccess;
    default:
        return VmbErrorBadParameter;
    }
}

// Attempts to load the LibTiff library dynamically using Qt QLibrary 
bool ImageWriter::LoadLibTiff( void )
{
    m_LibTiffLibrary.setFileNameAndVersion("libtiff", 5);
    bool checkLoaded = m_LibTiffLibrary.load(); 

    if (!checkLoaded)
    {
        m_LibTiffLibrary.setFileNameAndVersion("libtiff", 4); 
        checkLoaded = m_LibTiffLibrary.load(); 
    }

    return(checkLoaded);
}


void ImageWriter::Setup(const tFrameInfo & inputFrame)
{
    // copy config from input frame info 
    m_ImageWidth = inputFrame.m_nFrameWidth;
    m_ImageHeight = inputFrame.m_nFrameHeight;
    m_PixelFormat = inputFrame.m_PixelFormat;
    m_ColorInterpolation = inputFrame.m_ColorInterpolation;

    // calculate image resolution 
    m_ImageSizePixels = m_ImageWidth * m_ImageHeight;    

    // calculate bit depth
    GetBitsPerPixelUsed(&m_BitsPerPixelUsed, m_PixelFormat);
    m_BitsPerSample = ( m_BitsPerPixelUsed > 8 ) ? 16 : 8;

    // calculate bytes per pixel and colors per pixel
    m_BytesPerPixel = GetBytesPerPixel(m_BitsPerSample);
    m_ColorsPerPixel = ( IsMonochrome(m_PixelFormat) || !m_ColorInterpolation ) ? 1 : 3;

    // use calculated buffer size for packed mono pixel formats
    if( IsMonochrome(m_PixelFormat) && IsPackedFormat(m_PixelFormat))
    {
         m_ImageSizeBytes = (m_BytesPerPixel * m_ImageWidth * m_ImageHeight);
    }
    else
    {
       m_ImageSizeBytes = inputFrame.m_nFrameSize;    
    }

    // allocate memory the image data
    try
    {    
        m_ImageData = QSharedPointer<unsigned char>(new unsigned char[m_ImageSizeBytes], DeleteArray<unsigned char>);
    }
    catch ( std::bad_alloc& badAlloc )
    {
        qDebug() << "bad_alloc caught: " << badAlloc.what() << '\n';
    }

     // unpack and copy image data for packed mono pixel formats
    if ( IsMonochrome(m_PixelFormat) && IsPackedFormat(m_PixelFormat) )
    {       
        UnpackToMono16((unsigned char*)inputFrame.m_pFrameData.data(), (unsigned short*)m_ImageData.data());
    }
    // make a deep copy of image data
    else
    {
        memcpy( m_ImageData.data(), inputFrame.m_pFrameData.data(), m_ImageSizeBytes );
    }
}

// Get the error message from the last QLibrary call
QString ImageWriter::GetLastError( void )
{
    return m_LibTiffLibrary.errorString();
}


// Write the frame to the specified file 
bool ImageWriter::WriteTiff( const char* filename )
{
    bool result = false;

    // check if function symbol could be resolved (using QLibrary) 
    TIFFOpenPrototype dynamicTIFFOpen = (TIFFOpenPrototype) m_LibTiffLibrary.resolve("TIFFOpen");    
    if( dynamicTIFFOpen )
    {
        // Create a new TIFF file by constructing a Tiff object
        m_TiffFile = dynamicTIFFOpen(filename, "w");

        if( NULL == m_TiffFile )
            result = false;
        else
        {
            // Set the TIFF tags
            result = WriteStandardTags();

            if( true == result)
            {
                // Write the image data 
                result = WriteFrameBuffer();
            }
        }
    }

    // Close connection to the file
    TIFFClosePrototype dynamicTIFFClose = (TIFFClosePrototype) m_LibTiffLibrary.resolve("TIFFClose");
    if( dynamicTIFFClose)
    {
        dynamicTIFFClose(m_TiffFile);
    }
    else
    {
        qDebug() << "Error! Could not locate TIFFClose function. TIFFClose not called.";
    }

    return result;
}


// Helper function to fill in the TIFF file parameters 
bool ImageWriter::WriteStandardTags( void )
{
    int    result = 0;
    
    // check if function symbol could be resolved (using QLibrary) 
    TIFFSetFieldProtoype dynamicTIFFSetField = (TIFFSetFieldProtoype) m_LibTiffLibrary.resolve("TIFFSetField");
    if( dynamicTIFFSetField )
    {
        result = dynamicTIFFSetField(m_TiffFile, IMGWRITER_IMAGEWIDTH, static_cast<unsigned long>(m_ImageWidth));
        if (result == 0)
            return false;
        result = dynamicTIFFSetField(m_TiffFile, IMGWRITER_IMAGELENGTH, static_cast<unsigned long>(m_ImageHeight));
        if (result == 0)
            return false;
        result = dynamicTIFFSetField(m_TiffFile, IMGWRITER_COMPRESSION, static_cast<unsigned short>(1)); // No compression
        if (result == 0)
            return false;
        result = dynamicTIFFSetField(m_TiffFile, IMGWRITER_XRESOLUTION, static_cast<float>(1.0F));
        if (result == 0)
            return false;
        result = dynamicTIFFSetField(m_TiffFile, IMGWRITER_YRESOLUTION, static_cast<float>(1.0F));
        if (result == 0)
            return false;
        result = dynamicTIFFSetField(m_TiffFile, IMGWRITER_RESOLUTIONUNIT, static_cast<unsigned short>(1)); // No units
        if (result == 0)
            return false;
        result = dynamicTIFFSetField(m_TiffFile, IMGWRITER_PLANARCONFIG, static_cast<unsigned short>(1));
        if (result == 0)
            return false;

        // raw image data, bayer or mono
        if ( IsMonochrome(m_PixelFormat) || !m_ColorInterpolation) 
        {
            // Monochrome
            result = dynamicTIFFSetField(m_TiffFile, IMGWRITER_BITSPERSAMPLE, static_cast<unsigned short>(m_BitsPerSample));
            if (result == 0)
                return false;
            result = dynamicTIFFSetField(m_TiffFile, IMGWRITER_PHOTOMETRIC, static_cast<unsigned short>(1)); // Monochrome, black=0
            if (result == 0)
                return false;
        }
        else // bayer format && color interpolation on
        {
            // Color
            result = dynamicTIFFSetField(m_TiffFile, IMGWRITER_BITSPERSAMPLE, static_cast<unsigned short>(m_BitsPerSample),
                static_cast<unsigned short>(m_BitsPerSample), static_cast<unsigned short> (m_BitsPerSample));
            if (result == 0)
                return false;
            result = dynamicTIFFSetField(m_TiffFile, IMGWRITER_SAMPLESPERPIXEL, static_cast<unsigned short>(3));
            if (result == 0)
                return false;
            result = dynamicTIFFSetField(m_TiffFile, IMGWRITER_PHOTOMETRIC, static_cast<unsigned short>(2)); // RGB color
            if (result == 0)
                return false;
        }
    }
    return true;
}


// Helper function to write the data buffer to the TIFF file
bool ImageWriter::WriteFrameBuffer( void )
{
    int    result;
    //unsigned char* tmpBuffer = NULL;
    unsigned char* tmpBufferEnd = NULL;
    QSharedPointer<unsigned char> tmpBuffer;

    // calculate image size to check against image size from frame info
    VmbUint32_t destLineSize = m_BytesPerPixel * m_ImageWidth * m_ColorsPerPixel; // in bytes
    VmbUint32_t tmpBufferSize = destLineSize * m_ImageHeight;
    
    if ( tmpBufferSize < m_ImageSizeBytes )
    {
        qDebug() << "Error: wrong image size info!";
        return false;
    }
    
    // allocate the temporary buffer used to write to the file
    try
    {
        tmpBuffer = QSharedPointer<unsigned char>(new unsigned char[tmpBufferSize], DeleteArray<unsigned char>);
    }
    catch (std::bad_alloc& badAlloc)
    {
        qDebug() << "bad_alloc caught: " << badAlloc.what() << '\n';
        return false;
    }
    catch(...)
    {
        return false;
    }
    if(tmpBuffer.isNull())
    {
        return false;
    }

    // calculate boundary of temporary buffer
    tmpBufferEnd  = tmpBuffer.data() + tmpBufferSize;

    // copy or manipulate data into temporary buffer
    if ( (IsMonochrome(m_PixelFormat)) || (!m_ColorInterpolation) ) // raw mono or bayer (no color interpolation)
    {       
        if( (m_BytesPerPixel == 1) && (m_BytesPerPixel * 8 == m_BitsPerPixelUsed) ) // raw data 8
        {
            memcpy(tmpBuffer.data(), m_ImageData.data(), m_ImageSizeBytes);
        }
        else if( (m_BytesPerPixel == 2) && (m_BytesPerPixel * 8 >= m_BitsPerPixelUsed) ) // raw data 16
        {   
            UnpackToMono16((unsigned char*)m_ImageData.data(), (unsigned short*)tmpBuffer.data());
            const unsigned short*        pSrc = (unsigned short*)m_ImageData.data();
            const unsigned short* const    pSrcEnd = pSrc + m_ImageSizePixels;
            unsigned short*                pDest = (unsigned short*)tmpBuffer.data();
            const unsigned short        bitshift = 16 - m_BitsPerPixelUsed;

            while (pSrc < pSrcEnd)
            {
                *(pDest++) = *(pSrc++) << bitshift;
            }
        }
    }
    else // bayer && color interpolation on
    {
        if( (m_BytesPerPixel == 1) && (m_BytesPerPixel * 8 == m_BitsPerPixelUsed) ) // bayer8
        {
            unsigned char*                pDest = (unsigned char*)tmpBuffer.data();

            VmbImage            sourceImage, destinationImage;
            VmbError_t          transformResult;

            sourceImage.Size = sizeof( destinationImage );
            sourceImage.Data = m_ImageData.data();
            VmbSetImageInfoFromPixelFormat( m_PixelFormat, m_ImageWidth, m_ImageHeight, &sourceImage );

            destinationImage.Size = sizeof( destinationImage );
            destinationImage.Data = pDest;

            VmbSetImageInfoFromInputImage(&sourceImage, VmbPixelLayoutRGB, 8, &destinationImage);

            transformResult = VmbImageTransform( &sourceImage, &destinationImage, NULL, 0);

        }
        else if( (m_BytesPerPixel == 2) && (m_BytesPerPixel * 8 >= m_BitsPerPixelUsed) ) // bayer16
        {
            unsigned short*        pSrc = (unsigned short*)m_ImageData.data();
            unsigned short*                pDest = (unsigned short*)tmpBuffer.data();
            const unsigned short* const    pDestEnd = pDest + m_ImageSizePixels * 3;
            const unsigned short        bitshift = 16 - m_BitsPerPixelUsed;
            
            VmbError_t transformResult;

            VmbImage sourceImage;
            sourceImage.Size = sizeof( sourceImage );
            sourceImage.Data = pSrc;
            VmbSetImageInfoFromPixelFormat( m_PixelFormat, m_ImageWidth, m_ImageHeight, &sourceImage );

            VmbImage destinationImage;            
            destinationImage.Size = sizeof( destinationImage );
            destinationImage.Data = pDest;
            VmbSetImageInfoFromInputImage(&sourceImage, VmbPixelLayoutRGB, 16, &destinationImage);
                        
            transformResult = VmbImageTransform( &sourceImage, &destinationImage, NULL, 0);

            while (pDest < pDestEnd)
                *(pDest++) <<= bitshift;
        }
    }

    // Write data into TIFF file.
    TIFFWriteScanlinePrototype dynamicTIFFWriteScanline = (TIFFWriteScanlinePrototype) m_LibTiffLibrary.resolve("TIFFWriteScanline");

    if(dynamicTIFFWriteScanline)
    {
        unsigned char* pScanLine = tmpBuffer.data();
        for (unsigned long i = 0; i < m_ImageHeight; i++)
        {
            result = dynamicTIFFWriteScanline(m_TiffFile, pScanLine, i, 0);
            if (result == 0)
                return false;

            pScanLine += destLineSize; 
        }
    }

    return true;
}


// helper functions to test if pixel format is of type Mono
bool ImageWriter::IsMonochrome( VmbPixelFormat_t format )
{
    bool isMono;

    switch(format)
    {
    case VmbPixelFormatMono8:            
    case VmbPixelFormatMono10:
    case VmbPixelFormatMono10p:
    case VmbPixelFormatMono12:
    case VmbPixelFormatMono12Packed:
    case VmbPixelFormatMono12p:
    case VmbPixelFormatMono14:
    case VmbPixelFormatMono16:
        isMono = true;
        break;
    default:
        isMono = false;
    }

    return isMono;
}


// helper functions to test if pixel format is packed
bool ImageWriter::IsPackedFormat( VmbPixelFormat_t format )
{
    bool isPacked;

    switch(format)
    {
    case VmbPixelFormatMono10p:
    case VmbPixelFormatMono12Packed:
    case VmbPixelFormatMono12p:
    case VmbPixelFormatBayerGR12Packed:
    case VmbPixelFormatBayerRG12Packed:
    case VmbPixelFormatBayerGB12Packed:
    case VmbPixelFormatBayerBG12Packed:
    case VmbPixelFormatBayerGR10p:
    case VmbPixelFormatBayerRG10p:
    case VmbPixelFormatBayerGB10p:
    case VmbPixelFormatBayerBG10p:
    case VmbPixelFormatBayerGR12p:
    case VmbPixelFormatBayerRG12p:
    case VmbPixelFormatBayerGB12p:
    case VmbPixelFormatBayerBG12p:
        isPacked = true;
        break;
    default:
        isPacked = false;
    }

    return isPacked;
}


// helper functions to get the bytes used per pixel
int ImageWriter::GetBytesPerPixel( unsigned int allocatedBits )
{
    int bytesPerPixel;
    
    bytesPerPixel = (allocatedBits / 8);
    if( (allocatedBits % 8) != 0 )
    {    
        ++bytesPerPixel;
    }

    return bytesPerPixel;
}

// helper function to unpack packed mono pixel formats
bool ImageWriter::UnpackToMono16(unsigned char *pInBuffer, unsigned short *pOutBuffer)
{
    bool result = false;
    VmbImage sourceImage;
    VmbImage destinationImage;
    VmbTransformInfo vmbInfo;

    // set size member for verification inside API
    sourceImage.Size = sizeof ( sourceImage );
    destinationImage.Size = sizeof ( destinationImage );

    // attach the data buffers
    sourceImage.Data = pInBuffer;
    destinationImage.Data = pOutBuffer;

    // fill image info from pixel format string
    VmbSetImageInfoFromPixelFormat ( m_PixelFormat, m_ImageWidth, m_ImageHeight, &sourceImage );

    destinationImage.ImageInfo.PixelInfo.Alignment = VmbAlignmentLSB;

    // fill image info from pixel format
    VmbSetImageInfoFromInputImage(&sourceImage, VmbPixelLayoutMono, 16, &destinationImage);

    // perform the transformation
    if ( VmbErrorSuccess == VmbImageTransform ( &sourceImage , &destinationImage , &vmbInfo , 0 ) )
    {
        result = true;
    }

    return result;
}
