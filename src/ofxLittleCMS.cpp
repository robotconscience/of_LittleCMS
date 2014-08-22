//
//  ofxLittleCMS.cpp
//  example_cmyk
//
//  Created by Brett Renfer on 8/22/14.
//  Copyright (c) 2014 __MyCompanyName__. All rights reserved.
//

#include "ofxLittleCMS.h"
//
//template <typename PixelType>
//cmsUInt32Number ofxLittleCMS::getTypeFromPixels( PixelType & pix ){

cmsUInt32Number ofxLittleCMS::getTypeFromPixels( ofPixels & pix ){
    switch (ofGetGlInternalFormat(pix)) {
            // ofPixels
#ifndef TARGET_OPENGLES
            case GL_RGB8:
            return TYPE_RGB_8;
            break;
            
            case GL_RGBA8:
            return TYPE_RGBA_8;
            break;
            
            case GL_RG8:
            case GL_LUMINANCE_ALPHA:
            return TYPE_GRAYA_8;
            break;
            
            case GL_R8:
            case GL_LUMINANCE8:
            return TYPE_GRAY_8;
            break;
            
#else
            case GL_RGB:
            return TYPE_RGB_8;
            break;
            
            case GL_RGBA:
            return TYPE_RGBA_8;
            break;
            
            case GL_LUMINANCE_ALPHA:
            return TYPE_GRAYA_8;
            break;
            
            case GL_LUMINANCE:
            return TYPE_GRAY_8;
            break;
#endif
            
            // ofShortPixels
#ifndef TARGET_OPENGLES
            case GL_RGB16:
            return TYPE_RGB_16;
            break;
            
            case GL_RGBA16:
            return TYPE_RGBA_16;
            break;
            
            case GL_RG16:
            case GL_LUMINANCE16_ALPHA16:
            return TYPE_GRAYA_16;
            break;
            
            case GL_R16:
            case GL_LUMINANCE16:
            return TYPE_GRAY_16;
            break;
            
        // ofFloatPixels
            case GL_RGB32F:
            return TYPE_RGB_FLT;
            break;
            
            case GL_RGBA32F:
            return TYPE_RGBA_FLT;
            break;
            
            case GL_RG32F:
            case GL_LUMINANCE_ALPHA32F_ARB:
            ofLogWarning("ofxLittleCMS", "No Gray+A float :(");
            return TYPE_GRAY_FLT;
            break;
            
            case GL_R32F:
            case GL_LUMINANCE32F_ARB:
            return TYPE_GRAY_FLT;
            break;
#endif
            
        default:
            return TYPE_RGB_8;
            break;
    }
}


ofPixelFormat   ofxLittleCMS::getOFTypeFromFormat( cmsUInt32Number fmt ){
    switch (fmt) {
            case TYPE_RGB_8:
            case TYPE_RGB_16:
            return OF_PIXELS_RGB;
            break;
            
            case TYPE_RGBA_8:
            case TYPE_RGBA_16:
            case TYPE_CMYK_8:
            case TYPE_CMYK_16:
            cout <<"RGBA"<<endl;
            return OF_PIXELS_RGBA;
            break;
            
            case TYPE_GRAY_8:
            case TYPE_GRAY_16:
            return OF_PIXELS_MONO;
        default:
            // ah geez
            break;
    }
}