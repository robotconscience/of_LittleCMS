//
//  CMYKImage.h
//  ofImage_CMYK
//
//  Created by Brett Renfer on 8/21/14.
//
//

#pragma once
#include "ofMain.h"
#include "FreeImage.h"

//----------------------------------------------------------
// static variable for freeImage initialization:
void ofInitFreeImage(bool deinit=false){
	// need a new bool to avoid c++ "deinitialization order fiasco":
	// http://www.parashift.com/c++-faq-lite/ctors.html#faq-10.15
	static bool	* bFreeImageInited = new bool(false);
	if(!*bFreeImageInited && !deinit){
		FreeImage_Initialise();
		*bFreeImageInited = true;
	}
	if(*bFreeImageInited && deinit){
		FreeImage_DeInitialise();
	}
}

template<typename PixelType>
static bool loadImageCMYK(ofPixels_<PixelType> & pix, ofPixels_<PixelType> & rgbPix, string fileName, FIICCPROFILE & profile ){
	ofInitFreeImage();
	if(fileName.substr(0, 7) == "http://") {
		return ofLoadImage(pix, ofLoadURL(fileName).data);
	}
	
	fileName = ofToDataPath(fileName);
	bool bLoaded = false;
	FIBITMAP * bmp = NULL;
    
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	fif = FreeImage_GetFileType(fileName.c_str(), 0);
	if(fif == FIF_UNKNOWN) {
		// or guess via filename
		fif = FreeImage_GetFIFFromFilename(fileName.c_str());
	}
	if((fif != FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fif)) {
        switch (fif){
            case FIF_JPEG:
            case FIF_TIFF:
                bmp = FreeImage_Load(fif, fileName.c_str(), TIFF_CMYK|JPEG_CMYK);
                break;
            case FIF_PSD:
                bmp = FreeImage_Load(fif, fileName.c_str(), PSD_CMYK);
                break;
            default:
                ofLogWarning()<<"[ofImageCMYK] CMYK not supported with this format";
                bmp = FreeImage_Load(fif, fileName.c_str(), 0);
                break;
        }
        
		if (bmp != NULL){
			bLoaded = true;
		}
	}
	
	//-----------------------------
    
	if ( bLoaded ){
        profile = *FreeImage_GetICCProfile(bmp);
		putBmpIntoPixelsCMYK(bmp,pix,rgbPix);
	}
    
	if (bmp != NULL){
		FreeImage_Unload(bmp);
	}
    
	return bLoaded;
}

//----------------------------------------------------
template<typename PixelType>
void putBmpIntoPixelsCMYK(FIBITMAP * bmp, ofPixels_<PixelType> &pix, ofPixels_<PixelType> &rgbPix, bool swapForLittleEndian = true) {
	// convert to correct type depending on type of input bmp and PixelType
	FIBITMAP* bmpConverted = NULL;
	FREE_IMAGE_TYPE imgType = FreeImage_GetImageType(bmp);
    
	if(sizeof(PixelType)==1 && (FreeImage_GetColorType(bmp) == FIC_PALETTE || FreeImage_GetBPP(bmp) < 8 ||  imgType!=FIT_BITMAP)) {
       if(FreeImage_IsTransparent(bmp)) {
           bmpConverted = FreeImage_ConvertTo32Bits(bmp);
       } else {
           bmpConverted = FreeImage_ConvertTo24Bits(bmp);
       }
       bmp = bmpConverted;
   }else if(sizeof(PixelType)==2 && imgType!=FIT_UINT16 && imgType!=FIT_RGB16 && imgType!=FIT_RGBA16){
       if(FreeImage_IsTransparent(bmp)) {
           bmpConverted = FreeImage_ConvertToType(bmp,FIT_RGBA16);
       } else {
           bmpConverted = FreeImage_ConvertToType(bmp,FIT_RGB16);
       }
       bmp = bmpConverted;
   }else if(sizeof(PixelType)==4 && imgType!=FIT_FLOAT && imgType!=FIT_RGBF && imgType!=FIT_RGBAF){
       if(FreeImage_IsTransparent(bmp)) {
           bmpConverted = FreeImage_ConvertToType(bmp,FIT_RGBAF);
       } else {
           bmpConverted = FreeImage_ConvertToType(bmp,FIT_RGBF);
       }
       bmp = bmpConverted;
   }
    
	unsigned int width = FreeImage_GetWidth(bmp);
	unsigned int height = FreeImage_GetHeight(bmp);
	unsigned int bpp = FreeImage_GetBPP(bmp);
	unsigned int channels = (bpp / sizeof(PixelType)) / 8;
	unsigned int pitch = FreeImage_GetPitch(bmp);
    
	// ofPixels are top left, FIBITMAP is bottom left
	FreeImage_FlipVertical(bmp);
	
	unsigned char* bmpBits = FreeImage_GetBits(bmp);
	if(bmpBits != NULL) {
//        Red =   1 - minimum(1, Cyan * (1 - Black) + Black)
//        Green = 1 - minimum(1, Magenta * (1 - Black) + Black)
//        Blue =  1 - minimum(1, Yellow * (1 - Black) + Black)
        
		pix.setFromAlignedPixels((PixelType*) bmpBits, width, height, channels, pitch);
        rgbPix.allocate(width, height, OF_PIXELS_RGB);
        
        for ( int i = 0; i < width * height * 4; i+= 4){
            float c = ((int) pix[i + 0])/255.0;
            float m = ((int) pix[i + 1])/255.0;
            float y = ((int) pix[i + 2])/255.0;
            float k = ((int) pix[i + 3])/255.0;
            
            int index = i / 4.0 * 3.0;
            
            rgbPix[index + 0] = 255 * (1.0f - MIN(1.0f, c * (1.0f - k) + k));
            rgbPix[index + 1] = 255 * (1.0f - MIN(1.0f, m * (1.0f - k) + k));
            rgbPix[index + 2] = 255 * (1.0f - MIN(1.0f, y * (1.0f - k) + k));
        }
        
	} else {
		ofLogError("ofImage") << "putBmpIntoPixels(): unable to set ofPixels from FIBITMAP";
	}
	
	if(bmpConverted != NULL) {
		FreeImage_Unload(bmpConverted);
	}
    
#ifdef TARGET_LITTLE_ENDIAN
	if(swapForLittleEndian && sizeof(PixelType) == 1) {
		//pix.swapRgb();
        //rgbPix.swapRgb();
	}
#endif
}


//----------------------------------------------------
template<typename PixelType>
FIBITMAP* getBmpFromPixels(ofPixels_<PixelType> &pix){
	PixelType* pixels = pix.getPixels();
	unsigned int width = pix.getWidth();
	unsigned int height = pix.getHeight();
	unsigned int bpp = pix.getBitsPerPixel();
	
	FREE_IMAGE_TYPE freeImageType = FIT_BITMAP;// getFreeImageType(pix);
	FIBITMAP* bmp = FreeImage_AllocateT(freeImageType, width, height, bpp);
	unsigned char* bmpBits = FreeImage_GetBits(bmp);
	if(bmpBits != NULL) {
		int srcStride = width * pix.getBytesPerPixel();
		int dstStride = FreeImage_GetPitch(bmp);
		unsigned char* src = (unsigned char*) pixels;
		unsigned char* dst = bmpBits;
		for(int i = 0; i < (int)height; i++) {
			memcpy(dst, src, srcStride);
			src += srcStride;
			dst += dstStride;
		}
	} else {
		ofLogError("ofImage") << "getBmpFromPixels(): unable to get FIBITMAP from ofPixels";
	}
	
	// ofPixels are top left, FIBITMAP is bottom left
	FreeImage_FlipVertical(bmp);
	
	return bmp;
}

template<typename PixelType>
static void saveImageCMYK(ofPixels_<PixelType> & pix, string fileName, ofImageQualityType qualityLevel) {
	ofInitFreeImage();
	if (pix.isAllocated() == false){
		ofLogError("ofImage") << "saveImage(): couldn't save \"" << fileName << "\", pixels are not allocated";
		return;
	}
    
#ifdef TARGET_LITTLE_ENDIAN
	if(sizeof(PixelType) == 1) {
		//pix.swapRgb();
	}
#endif
    
	FIBITMAP * bmp	= getBmpFromPixels(pix);
    
#ifdef TARGET_LITTLE_ENDIAN
	if(sizeof(PixelType) == 1) {
		//pix.swapRgb();
	}
#endif
	
	ofFilePath::createEnclosingDirectory(fileName);
	fileName = ofToDataPath(fileName);
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	fif = FreeImage_GetFileType(fileName.c_str(), 0);
	if(fif == FIF_UNKNOWN) {
		// or guess via filename
		fif = FreeImage_GetFIFFromFilename(fileName.c_str());
	}
	if((fif != FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fif)) {
		if(fif == FIF_JPEG) {
			int quality = JPEG_QUALITYSUPERB;
			switch(qualityLevel) {
				case OF_IMAGE_QUALITY_WORST: quality = JPEG_QUALITYBAD; break;
				case OF_IMAGE_QUALITY_LOW: quality = JPEG_QUALITYAVERAGE; break;
				case OF_IMAGE_QUALITY_MEDIUM: quality = JPEG_QUALITYNORMAL; break;
				case OF_IMAGE_QUALITY_HIGH: quality = JPEG_QUALITYGOOD; break;
				case OF_IMAGE_QUALITY_BEST: quality = JPEG_QUALITYSUPERB; break;
			}
			FreeImage_Save(fif, bmp, fileName.c_str(), quality | JPEG_CMYK);
		} else {
			if(qualityLevel != OF_IMAGE_QUALITY_BEST) {
				ofLogWarning("ofImage") << "saveImage(): ofImageCompressionType only applies to JPEGs,"
                << " ignoring value for \" "<< fileName << "\"";
			}
			
			if (fif == FIF_GIF) {
				FIBITMAP* convertedBmp;
				if(pix.getImageType() == OF_IMAGE_COLOR_ALPHA) {
					// this just converts the image to grayscale so it can save something
					convertedBmp = FreeImage_ConvertTo8Bits(bmp);
				} else {
					// this will create a 256-color palette from the image
					convertedBmp = FreeImage_ColorQuantize(bmp, FIQ_NNQUANT);
				}
				FreeImage_Save(fif, convertedBmp, fileName.c_str());
				if (convertedBmp != NULL){
					FreeImage_Unload(convertedBmp);
				}
			} else {
				FreeImage_Save(fif, bmp, fileName.c_str(), TIFF_CMYK );
			}
		}
	}
    
	if (bmp != NULL){
		FreeImage_Unload(bmp);
	}
}

class ofxCMYKImage : public ofImage {
public:
    
    ofxCMYKImage(){
        
    }
    
    void allocate(int w, int h){
        drawPixels.allocate(w, h, OF_PIXELS_RGB);
        pixels.allocate(w, h, OF_PIXELS_RGBA);
        
        // take care of texture allocation --
        if (drawPixels.isAllocated() && bUseTexture){
            tex.allocate(drawPixels.getWidth(), drawPixels.getHeight(), ofGetGlInternalFormat(drawPixels));
            if(ofGetGLProgrammableRenderer() && (drawPixels.getNumChannels()==1 || pixels.getNumChannels()==2)){
                tex.setRGToRGBASwizzles(true);
            }
        }
        
        width	= pixels.getWidth();
        height	= pixels.getHeight();
        bpp		= pixels.getBitsPerPixel();
        type	= pixels.getImageType();
        
        updateDrawPixels();
    }
    
    bool load(string fileName){
        bool bLoadedOk = loadImageCMYK(pixels, drawPixels, fileName, profile);
        if (!bLoadedOk) {
            ofLogError("ofImage") << "loadImage(): couldn't load image from \"" << fileName << "\"";
            clear();
            return false;
        }
        if (bLoadedOk && drawPixels.isAllocated() && bUseTexture){
            tex.allocate(drawPixels.getWidth(), drawPixels.getHeight(), ofGetGlInternalFormat(drawPixels));
            if(ofGetGLProgrammableRenderer() && (pixels.getNumChannels()==1 || pixels.getNumChannels()==2)){
                tex.setRGToRGBASwizzles(true);
            }
        }
        update();
        return bLoadedOk;
    }
    
    void update(){
        width = pixels.getWidth();
        height = pixels.getHeight();
        bpp = pixels.getBitsPerPixel();
        type = pixels.getImageType();
        
        if (drawPixels.isAllocated() && bUseTexture){
            int glTypeInternal = ofGetGlInternalFormat(drawPixels);
            if(!tex.isAllocated() || tex.getWidth() != width || tex.getHeight() != height || tex.getTextureData().glTypeInternal != glTypeInternal){
                tex.allocate(drawPixels.getWidth(), drawPixels.getHeight(), glTypeInternal);
                if(ofGetGLProgrammableRenderer() && (drawPixels.getNumChannels()==1 || drawPixels.getNumChannels()==2)){
                    tex.setRGToRGBASwizzles(true);
                }
            }
            tex.loadData(drawPixels);
        }
    }
    
    void save( string dest ){
        saveImageCMYK(getPixelsRef(), dest, OF_IMAGE_QUALITY_BEST );
    }
    
    FIICCPROFILE * getProfile(){
        return &profile;
    }
    
protected:
    ofPixels drawPixels;
    FIICCPROFILE profile;
    
    void updateDrawPixels(){
        for ( int i = 0; i < width * height * 4; i+= 4){
            float c = ((int) pixels[i + 0])/255.0;
            float m = ((int) pixels[i + 1])/255.0;
            float y = ((int) pixels[i + 2])/255.0;
            float k = ((int) pixels[i + 3])/255.0;
            
            int index = i / 4.0 * 3.0;
            
            drawPixels[index + 0] = 255 * (1.0f - MIN(1.0f, c * (1.0f - k) + k));
            drawPixels[index + 1] = 255 * (1.0f - MIN(1.0f, m * (1.0f - k) + k));
            drawPixels[index + 2] = 255 * (1.0f - MIN(1.0f, y * (1.0f - k) + k));
        }
    }
};
