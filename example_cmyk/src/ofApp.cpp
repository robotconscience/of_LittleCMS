#include "ofApp.h"

bool bNeedToUpdate = false;
int intent = INTENT_PERCEPTUAL;

//--------------------------------------------------------------
void ofApp::setup(){
    cmykImage.load("test.tif");
    
    cmsManager.setupInputProfile( cmykImage.getProfile() );
    cmsManager.setupOutputProfile("AdobeRGB1998.icc");
    
    ofPixels pix;
    
    cmsManager.transformPixels(cmykImage.getPixelsRef(), TYPE_CMYK_8, cmykTranformedImage.getPixelsRef(), TYPE_RGB_8);
    cmykTranformedImage.update();
    
    cmykTranformedImage.saveImage("tight.tif");
}

//--------------------------------------------------------------
void ofApp::update(){
    if (bNeedToUpdate){
        cout << intent << endl;
        bNeedToUpdate = false;
        cmsManager.transformPixels(cmykImage.getPixelsRef(), TYPE_CMYK_8, cmykTranformedImage.getPixelsRef(), TYPE_RGB_8, intent);
        cmykTranformedImage.update();
        cmykTranformedImage.saveImage("tight.tif");
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    cmykImage.draw(0,0);
    cmykTranformedImage.draw(0, cmykImage.height);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if ( key == '0'){
        cmsManager.setupOutputProfile("AdobeRGB1998.icc");
        bNeedToUpdate = true;
    } else if ( key == '1'){
        cmsManager.setupOutputProfile("AppleRGB.icc");
        bNeedToUpdate = true;
    } else if ( key == '2'){
        cmsManager.setupOutputProfile("ColorMatchRGB.icc");
        bNeedToUpdate = true;
    } else if ( key == '+' || key == '=' ){
        intent++;
        if ( intent > INTENT_ABSOLUTE_COLORIMETRIC ){
            intent = 0;
        }
        bNeedToUpdate = true;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
