#include "ofApp.h"

bool debugMode = false;
bool kinectActive = false;

//--------------------------------------------------------------
void ofApp::setup(){
    // recorder
    recorder.setPrefix(ofToDataPath("recording/frame_"));
    recorder.setFormat("bmp");
    recorder.setNumberWidth(8);
    recorder.startThread();
    ofSetFrameRate(30);
    
    // midi
    midiIn.openPort(0);
    midiIn.addListener(this);
    
    // kinect
    initKinect();
}

void ofApp::exit(){
    recorder.waitForThread();
    recorder.stopThread();
}

//--------------------------------------------------------------
void ofApp::update(){
    
    if (debugMode) {
        // show the framerate on window title
        std::stringstream strm;
        strm << "fps: " << ofGetFrameRate();
        ofSetWindowTitle(strm.str());
    }
    
    updateKinect();
}

//--------------------------------------------------------------
void ofApp::draw(){
    cam.begin();
    drawKinect();
    cam.end();
    
    // capture the image if recording is started
    // this can slow down the rendering by a lot, so be aware of the framerate...
    // if frames are skipped this is going to mess up the rendering when imported as a video
    if (recording){
        screenCapture.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
        recorder.addFrame(screenCapture);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    switch (key) {
        case 'r':
            toggleRecording();
            break;
        case 'p':
            captureScreen();
            break;
        case 'k':
            toggleKinectCapture();
            break;
        default:
            break;
    }
}

void ofApp::newMidiMessage(ofxMidiMessage& eventArgs){
    
}

void ofApp::captureScreen(){
    screenCapture.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
    screenCapture.save("captures/capture.bmp");
}

void ofApp::initKinect(){
    kinect.setRegistration(true);
    kinect.init();
    kinect.open();        // opens first available kinect
    nearThreshold = 230;
    farThreshold = 70;
    kinect.setCameraTiltAngle(0);
    
    // print the intrinsic IR sensor values
    if(kinect.isConnected()) {
        ofLogNotice() << "sensor-emitter dist: " << kinect.getSensorEmitterDistance() << "cm";
        ofLogNotice() << "sensor-camera dist:  " << kinect.getSensorCameraDistance() << "cm";
        ofLogNotice() << "zero plane pixel size: " << kinect.getZeroPlanePixelSize() << "mm";
        ofLogNotice() << "zero plane dist: " << kinect.getZeroPlaneDistance() << "mm";
    }
}

void ofApp::updateKinect() {
    if (kinectActive){
        kinect.update();
        
        // there is a new frame and we are connected
        if(kinect.isFrameNew()) {
            
            // load grayscale depth image from the kinect source
            grayImage.setFromPixels(kinect.getDepthPixels());
            
            // we do two thresholds - one for the far plane and one for the near plane
            // we then do a cvAnd to get the pixels which are a union of the two thresholds
            grayThreshNear = grayImage;
            grayThreshFar = grayImage;
            grayThreshNear.threshold(nearThreshold, true);
            grayThreshFar.threshold(farThreshold);
            cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
            
            // update the cv images
            grayImage.flagImageChanged();
        }
    }
}

void ofApp::drawKinect() {
    ofSetColor(255);
    if (kinectActive){
        int w = 640;
        int h = 480;
        ofMesh mesh;
        mesh.setMode(OF_PRIMITIVE_TRIANGLES);
        int step = 2;
        for(int y = 0; y < h; y += step) {
            for(int x = 0; x < w; x += step) {
                if(kinect.getDistanceAt(x, y) > 0 && kinect.getDistanceAt(x, y) < 1000) {
                    mesh.addColor(kinect.getColorAt(x,y));
                    mesh.addVertex(kinect.getWorldCoordinateAt(x, y));
                }
            }
        }
        glPointSize(1);
        ofPushMatrix();
        // the projected points are 'upside down' and 'backwards'
        ofScale(1, -1, -1);
        ofTranslate(0, 0, -1000); // center the points a bit
        ofEnableDepthTest();
        mesh.drawVertices();
        ofDisableDepthTest();
        ofPopMatrix();
    }
}

void ofApp::toggleKinectCapture(){
    kinectActive = !kinectActive;
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
void ofApp::mouseEntered(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
    
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

void ofApp::toggleRecording(){
    recording = !recording;
}

