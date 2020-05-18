#include "ofApp.h"

bool debugMode = false;
bool kinectActive = false;
bool kinectRecordingActive = false;
bool kinectPlayerActive;
int kinectWidth = 640;
int kinectHeight = 480;

//--------------------------------------------------------------
void ofApp::setup(){
    // recorder
    recorder.setPrefix(ofToDataPath("recording/render/frame_"));
    recorder.setFormat("bmp");
    recorder.setNumberWidth(8);
    recorder.startThread();
    
    kinectRecorder.setPrefix(ofToDataPath("recording/kinect/frame_"));
    kinectRecorder.setFormat("png");
    kinectRecorder.setNumberWidth(8);
    kinectRecorder.startThread();

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
    kinectRecorder.waitForThread();
    kinectRecorder.stopThread();
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
    
    if (kinectRecordingActive) {
        ofShortPixels pixelsRaw = kinect.getRawDepthPixels();
        ofShortImage image;
        image.setFromPixels(kinect.getRawDepthPixels());
        image.save(ofToDataPath("recording/kinect/frame_00000000.bmp"));
        //kinectRecorder.addFrame(image);
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
            toggleKinect();
            break;
        case 'l':
            toggleKinectRecording();
            break;
        case 'm':
            toggleKinectPlayer();
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
    
    if (kinectPlayerActive) {
        ofShortPixels pixels;
        ofLoadImage(pixels, ofToDataPath("recording/kinect/frame_00000000.bmp"));
        int w = 640;
        int h = 480;
        ofMesh mesh;
        mesh.setMode(OF_PRIMITIVE_TRIANGLES);
        int step = 2;
        for(int y = 0; y < h; y += step) {
            for(int x = 0; x < w; x += step) {
                auto distance = pixels[y * w + x];
                if((float) distance > 0 && (float) distance < 1000) {
//                    mesh.addColor(kinect.getColorAt(x,y));
                    mesh.addVertex(ofPoint(x, y, distance));
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

void ofApp::toggleKinect(){
    kinectActive = !kinectActive;
}

void ofApp::toggleKinectRecording(){
    kinectRecordingActive = !kinectRecordingActive;
}

void ofApp::toggleKinectPlayer(){
    kinectPlayerActive = !kinectPlayerActive;
    if (kinectPlayerActive){
        kinectActive = false;
        kinectRecordingActive = false;
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

