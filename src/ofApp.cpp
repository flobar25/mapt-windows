#include "ofApp.h"
#include <chrono>

bool debugMode = false;
bool kinectActive = false;
bool kinectRecordingActive = false;
bool kinectPlayerActive;
int kinectCurrentFramePlayed = 0;
int kinectWidth = 640;
int kinectHeight = 480;
int kinectStep = 2;
int numberWidth = 8;

//--------------------------------------------------------------
void ofApp::setup(){
    
    using namespace std::chrono;
    milliseconds ms = duration_cast< seconds >(system_clock::now().time_since_epoch());
    
    // recorder
    recorder.setPrefix(ofToDataPath("recording/" + ofToString(ms.count()) + "/render/frame_"));
    recorder.setFormat("png");
    recorder.setNumberWidth(8);
    recorder.startThread();
    
    kinectRecorder.setPrefix(ofToDataPath("recording/" + ofToString(ms.count()) + "/kinect/frame_"));
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
        kinectRecorder.addFrame(kinect.getRawDepthPixels());
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
    // kinect.setRegistration(true); -> supposed to slow down perf, so only activate if needed
    kinect.init();
    kinect.open();        // opens first available kinect
//    nearThreshold = 230;
//    farThreshold = 70;
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
//        if (kinectRecordingActive) {
//            auto rawDepthPixels = kinect.getRawDepthPixels();
//
//            ofShortImage image;
//            image.setFromPixels(rawDepthPixels);
//            image.save(ofToDataPath("recording/kinect/frame_00000000.png"));
//
//            ofShortImage loadedImage;
//            loadedImage.load("recording/kinect/frame_00000000.png");
//            auto loadedImagePixels = loadedImage.getPixels();
//            for(int y = 0; y < kinectHeight; y++) {
//                for(int x = 0; x < kinectWidth; x++) {
//                    auto value0 = image.getPixels()[y * kinectWidth + x];
//                    auto value1 = loadedImagePixels[y * kinectWidth + x];
//                    auto value2 = rawDepthPixels[y * kinectWidth + x];
//                    auto value3 = kinect.getDistanceAt(x, y);
//                    ofLog(ofLogLevel::OF_LOG_NOTICE, ofToString(value0) + " : " +ofToString(value1) + " : " + ofToString(value2) + " : " + ofToString(value3));
//                }
//            }
//        }
        
        
        ofMesh mesh;
        mesh.setMode(OF_PRIMITIVE_TRIANGLES);
        for(int y = 0; y < kinectHeight; y += kinectStep) {
            for(int x = 0; x < kinectWidth; x += kinectStep) {
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
        // TODO load that in memory at the beginning
        auto path = ofToDataPath("recording/kinect/frame_" + ofToString(kinectCurrentFramePlayed++, numberWidth, '0') + ".png");
        ofShortImage load;
        if (!ofFile::doesFileExist(path)){
            kinectCurrentFramePlayed = 0;
            path = ofToDataPath("recording/kinect/frame_" + ofToString(kinectCurrentFramePlayed++, numberWidth, '0') + ".png");
        }
        load.load(path);

        ofMesh mesh;
        mesh.setMode(OF_PRIMITIVE_TRIANGLES);
        int step = 2;
        for(int y = 0; y < kinectHeight; y += step) {
            for(int x = 0; x < kinectWidth; x += step) {
                auto distance = load.getPixels()[y * kinectWidth + x];
                if(distance > 0 && distance < 1000) {
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

