/*
    Standard image processing plug-ins of Computer Vision Sandbox

    Copyright (C) 2011-2018, cvsandbox
    http://www.cvsandbox.com/contacts.html

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/

#include <stdlib.h>
#include <ximaging.h>
#include "DiffImagesThresholdedPlugin.hpp"

// Supported pixel formats of input/output images
const XPixelFormat DiffImagesThresholdedPlugin::supportedFormats[] =
{
    XPixelFormatGrayscale8, XPixelFormatRGB24, XPixelFormatRGBA32
};

DiffImagesThresholdedPlugin::DiffImagesThresholdedPlugin( ) :
    threshold( 30 ), hiColor( { 0xFFFFFFFF } ), lowColor( { 0xFF000000 } ), diffPixels( 0 )
{

}

void DiffImagesThresholdedPlugin::Dispose( )
{
    delete this;
}

// The plug-in can process image in-place without creating new image as a result
bool DiffImagesThresholdedPlugin::CanProcessInPlace( )
{
    return true;
}

// Provide supported pixel formats
XErrorCode DiffImagesThresholdedPlugin::GetPixelFormatTranslations( XPixelFormat* inputFormats, XPixelFormat* outputFormats, int32_t* count )
{
    return GetPixelFormatTranslationsImpl( inputFormats, outputFormats, count, supportedFormats, supportedFormats,
        sizeof( supportedFormats ) / sizeof( XPixelFormat ) );
}

xstring DiffImagesThresholdedPlugin::GetSecondImageDescription( )
{
    return XStringAlloc( "An image to calculate thresholded absolute difference with" );
}

XSupportedImageSize DiffImagesThresholdedPlugin::GetSecondImageSupportedSize( )
{
    return XImageSizeEqual;
}

XPixelFormat DiffImagesThresholdedPlugin::GetSecondImageSupportedFormat( XPixelFormat inputPixelFormat )
{
    return ( IsPixelFormatSupportedImpl( supportedFormats, sizeof( supportedFormats ) / sizeof( XPixelFormat ), inputPixelFormat ) == true ) ?
    inputPixelFormat : XPixelFormatUnknown;
}

// Process the specified source image and return new as a result
XErrorCode DiffImagesThresholdedPlugin::ProcessImage( const ximage* src, const ximage* src2, ximage** dst )
{
    XErrorCode ret = XImageClone( src, dst );

    if ( ret == SuccessCode )
    {
        ret = ProcessImageInPlace( *dst, src2 );

        if ( ret != SuccessCode )
        {
            XImageFree( dst );
        }
    }

    return ret;
}

// Process the specified source image by changing it
XErrorCode DiffImagesThresholdedPlugin::ProcessImageInPlace( ximage* src, const ximage* src2 )
{
    return DiffImagesThresholded( src, src2, threshold, &diffPixels, hiColor, lowColor );
}

// Get specified property value of the plug-in
XErrorCode DiffImagesThresholdedPlugin::GetProperty( int32_t id, xvariant* value ) const
{
    XErrorCode ret = SuccessCode;

    switch ( id )
    {
    case 0:
        value->type       = XVT_I2;
        value->value.sVal = threshold;
        break;

    case 1:
        value->type          = XVT_ARGB;
        value->value.argbVal = hiColor;
        break;

    case 2:
        value->type          = XVT_ARGB;
        value->value.argbVal = lowColor;
        break;

    case 3:
        value->type        = XVT_U4;
        value->value.uiVal = diffPixels;
        break;

    default:
        ret = ErrorInvalidProperty;
    }

    return ret;
}

// Set specified property value of the plug-in
XErrorCode DiffImagesThresholdedPlugin::SetProperty( int32_t id, const xvariant* value )
{
    XErrorCode ret = SuccessCode;

    xvariant convertedValue;
    XVariantInit( &convertedValue );

    // make sure property value has expected type
    ret = PropertyChangeTypeHelper( id, value, propertiesDescription, 4, &convertedValue );

    if ( ret == SuccessCode )
    {
        switch ( id )
        {
        case 0:
            threshold = convertedValue.value.sVal;
            break;

        case 1:
            hiColor = convertedValue.value.argbVal;
            break;

        case 2:
            lowColor = convertedValue.value.argbVal;
            break;

        default:
            ret = ErrorReadOnlyProperty;
            break;
        }
    }

    XVariantClear( &convertedValue );

    return ret;
}
