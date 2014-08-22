//
//  ofxLittleCMS.h
//  example_cmyk
//
//  Created by Brett Renfer on 8/22/14.
//  Copyright (c) 2014 Brett Renfer. All rights reserved.
//

#pragma once

#include "ofMain.h"
#include "lcms2.h"
#include "ofxCMYKImage.h"

static void ofxLCMSError(cmsContext ContextID, cmsUInt32Number ErrorCode, const char *Text)
{
    ofLogError("ofxLittleCMS", Text);
}

class ofxLittleCMS
{
public:
    
    ofxLittleCMS() :
    in_profile(NULL),
    out_profile(NULL)
    {
        cmsSetLogErrorHandler(ofxLCMSError);
    }
    
    ~ofxLittleCMS(){
    }
    
    // load in and out profile for transform
    void setupProfiles( string input_profile, string output_profile ){
        // setup profiles
        string path = ofToDataPath(input_profile, true);
        in_profile = cmsOpenProfileFromFile(path.c_str(), "r");
        
        path = ofToDataPath(output_profile, true);
        out_profile = cmsOpenProfileFromFile(path.c_str(), "r");
    }
    
    void setupInputProfile( string input_profile ){
        string path = ofToDataPath(input_profile, true);
        in_profile = cmsOpenProfileFromFile(path.c_str(), "r");
    }
    
    void setupInputProfile( FIICCPROFILE * profile ){
        in_profile = cmsOpenProfileFromMem(profile->data, profile->size);
    }
    
    void setupOutputProfile( string output_profile ){
        string path = ofToDataPath(output_profile, true);
        out_profile = cmsOpenProfileFromFile(path.c_str(), "r");
    }
    
    void setupOutputProfile( FIICCPROFILE * profile ){
        out_profile = cmsOpenProfileFromMem(profile->data, profile->size);
    }
    
    // intent == INTENT_PERCEPTUAL, INTENT_RELATIVE_COLORIMETRIC, INTENT_SATURATION or INTENT_ABSOLUTE_COLORIMETRIC
    template <typename PixelTypeIn, typename PixelTypeOut>
    void transformPixels( PixelTypeIn & pixels, cmsUInt32Number input_format, PixelTypeOut & out_pixels,
                         cmsUInt32Number output_format, cmsUInt32Number intent = INTENT_PERCEPTUAL )
    {
        if ( !pixels.isAllocated() ){
            ofLogError("ofxLittleCMS", "Both sets of pixels must be pre-allocated!");
            return;
        }
        
        // build transform from profiles
        cmsHTRANSFORM transform = cmsCreateTransform(in_profile, input_format, out_profile, output_format, intent, 0);
        
        out_pixels.allocate( pixels.getWidth(), pixels.getHeight(), getOFTypeFromFormat( output_format ));
        ofImageType original = pixels.getImageType();
        cmsDoTransform( transform, pixels.getPixels(), out_pixels.getPixels(), out_pixels.getWidth() * out_pixels   .getHeight() );
    }
    
    
protected:
    
    cmsUInt32Number getTypeFromPixels( ofPixels & pix );
    ofPixelFormat   getOFTypeFromFormat( cmsUInt32Number fmt );
    
    cmsHPROFILE in_profile;
    cmsHPROFILE out_profile;
};

/*
 
 FIBITMAP *in_bitmap = FreeImage_Load(FIF_TIFF, "icc.tif");
 int width = FreeImage_GetWidth(in_bitmap);
 int height = FreeImage_GetHeight(in_bitmap);
 int bpp = FreeImage_GetBPP(in_bitmap);
 
 FIICCPROFILE *profile = FreeImage_GetICCProfile(in_bitmap);
 
 if (profile) {
 cmsHPROFILE in_profile = cmsOpenProfileFromMem(profile->data, profile->size);
 cmsHPROFILE out_profile = cmsOpenProfileFromFile("USWebUncoated.icc", "r");
 cmsHTRANSFORM transform = cmsCreateTransform(in_profile, TYPE_BGR_8, out_profile, TYPE_CMYK_8, INTENT_PERCEPTUAL, 0);
 
 FIBITMAP *out_bitmap = FreeImage_Allocate(width, height, 32);
 FreeImage_GetICCProfile(out_bitmap)->flags |= FIICC_COLOR_IS_CMYK;
 
 for (int i = 0; i < height; ++i) {
 BYTE *in_line = FreeImage_GetScanLine(in_bitmap, i);
 BYTE *out_line = FreeImage_GetScanLine(out_bitmap, i);
 
 cmsDoTransform(transform, in_line, out_line, width);
 }
 
 cmsDeleteTransform(transform);
 cmsCloseProfile(in_profile);
 cmsCloseProfile(out_profile);
 
 FreeImage_Save(FIF_TIFF, out_bitmap, "test2.tif", TIFF_CMYK);
 FreeImage_Unload(out_bitmap);
 }
 
 FreeImage_Unload(in_bitmap);
 
 */
