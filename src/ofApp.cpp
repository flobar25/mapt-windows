#include "ofApp.h"
#include <chrono>

// input parameters
int SPACE_WIDTH = 10000;
int SPACE_LENGTH = 10000;
int SPACE_HEIGHT = 500;
int TOWER_COUNT = 50;
int TOWER_MIN_FLOORS = 8;
int TOWER_MAX_FLOORS = 100;
int TOWER_MIN_WIDTH = 150;
int TOWER_MAX_WIDTH = 400;
int TOWER_MIN_LENGTH = 150;
int TOWER_MAX_LENGTH = 400;
int FACES_COUNT = 10;

// faces
int currentPlayedFace = -1;
int faceDistance = 500;

bool debugMode = true;
bool recording = false;
bool kinectActive = false;
bool kinectRecordingActive = false;
bool midiRecordingActive = false;
bool midiPlayerActive = false;

int kinectCurrentFramePlayed = 0;
int kinectWidth = 640;
int kinectHeight = 480;
int kinectStep = 2;
int numberWidth = 8;
int midiFrameStart = 0;
bool started = false;





ofApp::ofApp() {
    // effects
    postGlitch.setFx(OFXPOSTGLITCH_CONVERGENCE, false);
    postGlitch.setFx(OFXPOSTGLITCH_GLOW, false);
    postGlitch.setFx(OFXPOSTGLITCH_SHAKER, false);
    postGlitch.setFx(OFXPOSTGLITCH_CUTSLIDER, false);
    postGlitch.setFx(OFXPOSTGLITCH_TWIST, false);
    postGlitch.setFx(OFXPOSTGLITCH_OUTLINE, false);
    postGlitch.setFx(OFXPOSTGLITCH_NOISE, false);
    postGlitch.setFx(OFXPOSTGLITCH_SLITSCAN, false);
    postGlitch.setFx(OFXPOSTGLITCH_SLITSCAN_ROT, false);
    postGlitch.setFx(OFXPOSTGLITCH_SWELL, false);
    postGlitch.setFx(OFXPOSTGLITCH_INVERT, false);
    postGlitch.setFx(OFXPOSTGLITCH_CR_HIGHCONTRAST, false);
    postGlitch.setFx(OFXPOSTGLITCH_CR_BLUERAISE, false);
    postGlitch.setFx(OFXPOSTGLITCH_CR_REDRAISE, false);
    postGlitch.setFx(OFXPOSTGLITCH_CR_GREENRAISE, false);
    postGlitch.setFx(OFXPOSTGLITCH_CR_REDINVERT, false);
    postGlitch.setFx(OFXPOSTGLITCH_CR_BLUEINVERT, false);
    postGlitch.setFx(OFXPOSTGLITCH_CR_GREENINVERT, false);
}

void ofApp::setup(){
    
    using namespace std::chrono;
    milliseconds ms = duration_cast< seconds >(system_clock::now().time_since_epoch());
    
    recorder.setPrefix(ofToDataPath("recording/" + ofToString(ms.count()) + "/render/frame_"));
    recorder.setFormat("png");
    recorder.setNumberWidth(8);
    recorder.startThread();
    
    kinectRecorder.setPrefix(ofToDataPath("recording/" + ofToString(ms.count()) + "/kinect/frame_"));
    kinectRecorder.setFormat("png");
    kinectRecorder.setNumberWidth(8);
    kinectRecorder.startThread();
    
    midiRecorder.openFile(ofToDataPath(ofToDataPath("recording/" + ofToString(ms.count()) + "/midi/recording.txt")));
    midiRecorder.startThread();
    
    midiPlayer.load(ofToDataPath("recording/1590025809000/midi/recording.txt"));
    
    // towers
    for (int i = 0; i < TOWER_COUNT; i++) {
        Tower tower;
        tower.setup(ofPoint(ofRandom(SPACE_WIDTH) - SPACE_WIDTH / 2,0,ofRandom(SPACE_LENGTH) - SPACE_WIDTH / 2), // position
                    ofRandom(TOWER_MAX_FLOORS - TOWER_MIN_FLOORS) + TOWER_MIN_FLOORS, // floors
                    200, // floor height
                    ofRandom(TOWER_MAX_LENGTH - TOWER_MIN_LENGTH) + TOWER_MIN_LENGTH, // length
                    ofRandom(TOWER_MAX_WIDTH - TOWER_MIN_WIDTH) + TOWER_MIN_WIDTH, // width
                    0.1, // displacement rate
                    ofPoint(-50,0,0) // displacement vector
                    );
        towers.push_back(tower);
    }
    
    // faces
    for (int i = 0; i < FACES_COUNT; i++){
        ofxKinectSequencePlayer kinectPlayer;
        kinectPlayer.cropRight = 220;
        kinectPlayer.cropLeft = 0;
        kinectPlayer.cropUp = 0;
        kinectPlayer.cropDown = 100;
        kinectPlayer.cropNear = 200;
        kinectPlayer.cropFar = 1000;
        kinectPlayer.load(ofToDataPath("recording/kinect/frame_"), "png", kinectHeight, kinectWidth, 8, "images/orange1.jpg");
        faces.push_back(kinectPlayer);
    }
    
    
    // midi
    midiIn.openPort(0);
    midiIn.addListener(this);
    
    // kinect
    initKinect();
    
    //camera
    cam.setNearClip(0);
    cam.setFarClip(100000);
    
    // effects
    fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGB, 4);
    postGlitch.setup(&fbo);
    
    // other things
    ofEnableAlphaBlending();
    ofEnableDepthTest();
    ofSetFrameRate(30);
    ofDisableArbTex();
}

void ofApp::exit(){
    recorder.waitForThread();
    recorder.stopThread();
    kinectRecorder.waitForThread();
    kinectRecorder.stopThread();
    midiRecorder.waitForThread();
    midiRecorder.stopThread();
}

//--------------------------------------------------------------
void ofApp::update(){
    
    if (debugMode) {
        // show the framerate on window title
        std::stringstream strm;
        strm << "fps: " << ofGetFrameRate();
        ofSetWindowTitle(strm.str());
    }
    
    if (started) {
        cam.update();
        updateKinect();
        updateFaces();
        updateTowers();
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    if (midiPlayerActive) {
        for (auto midiMessage : midiPlayer.getNextMidiMessages()) {
            newMidiMessage(midiMessage);
        }
    }
    
    ofEnableDepthTest();
    
    fbo.begin();
    cam.begin();
    ofClear(0,0,0,255);
    
    drawKinect();
    drawTowers();
    drawFaces();
    
    if (debugMode) {
        ofDrawAxis(200);
        ofSetColor(200,0,0);
        ofDrawBox(cam.getCurrentLookAt().x, cam.getCurrentLookAt().y, cam.getCurrentLookAt().z, 20);
    }
    cam.end();
    fbo.end();
    
    postGlitch.generateFx();
    ofSetColor(255);
    fbo.draw(0,0);
    
    
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
    
    if (debugMode) {
        ofDrawBitmapString(debugMessage(), 20, 652);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    switch (key) {
        case 'q': toggleRecording(); break;
        case 'w': captureScreen(); break;
        case 'a': toggleKinect(); break;
        case 's': toggleKinectRecording(); break;
        case 'd': nextFace(); break;
        case 'z': toggleDebugMode(); break;
        case 'i': toggleMidiRecording(); break;
        case 'o': toggleMidiPlayer(); break;
        case '-': started = !started; break;
        case 'j': cam.slowMoveToRandomPosition(); break;
        case 'k': cam.fastMoveToRandomPosition(); break;
        case '`': faces[currentPlayedFace].setEffect(EffectType::NONE); break;
        case '1': faces[currentPlayedFace].setEffect(EffectType::STRETCH); break;
        case '2': faces[currentPlayedFace].setEffect(EffectType::STRIPS); break;
        case '3': faces[currentPlayedFace].setEffect(EffectType::EXPLOSION); break;
        case '4': toggleTowersMove(); break;
        default: break;
    }
    
    if (key == 'Q') postGlitch.setFx(OFXPOSTGLITCH_CONVERGENCE, true);
    if (key == 'W') postGlitch.setFx(OFXPOSTGLITCH_GLOW, true);
    if (key == 'E') postGlitch.setFx(OFXPOSTGLITCH_SHAKER, true);
    if (key == 'R') postGlitch.setFx(OFXPOSTGLITCH_CUTSLIDER, true);
    if (key == 'T') postGlitch.setFx(OFXPOSTGLITCH_TWIST, true);
    if (key == 'Y') postGlitch.setFx(OFXPOSTGLITCH_OUTLINE, true);
    if (key == 'U') postGlitch.setFx(OFXPOSTGLITCH_NOISE, true);
    if (key == 'I') postGlitch.setFx(OFXPOSTGLITCH_SLITSCAN, true);
    if (key == 'O') postGlitch.setFx(OFXPOSTGLITCH_SLITSCAN_ROT, true);
    if (key == 'P') postGlitch.setFx(OFXPOSTGLITCH_SWELL, true);
    if (key == 'A') postGlitch.setFx(OFXPOSTGLITCH_INVERT, true);
    if (key == 'S') postGlitch.setFx(OFXPOSTGLITCH_CR_HIGHCONTRAST, true);
    if (key == 'D') postGlitch.setFx(OFXPOSTGLITCH_CR_BLUERAISE, true);
    if (key == 'F') postGlitch.setFx(OFXPOSTGLITCH_CR_REDRAISE, true);
    if (key == 'G') postGlitch.setFx(OFXPOSTGLITCH_CR_GREENRAISE, true);
    if (key == 'H') postGlitch.setFx(OFXPOSTGLITCH_CR_REDINVERT, true);
    if (key == 'J') postGlitch.setFx(OFXPOSTGLITCH_CR_BLUEINVERT, true);
    if (key == 'K') postGlitch.setFx(OFXPOSTGLITCH_CR_GREENINVERT, true);
    
}

void ofApp::newMidiMessage(ofxMidiMessage& midiMessage){
    ofLog(ofLogLevel::OF_LOG_NOTICE, "" + ofToString(midiMessage.channel) + ","
          + ofToString(midiMessage.status) + ","
          + ofToString(midiMessage.pitch) + ","
          + ofToString(midiMessage.velocity) + ","
          + ofToString(midiMessage.value) + ",") ;
    
    if (midiRecordingActive){
        midiRecorder.addMidiFrame(ofGetFrameNum() - midiFrameStart, midiMessage);
    }
}

void ofApp::captureScreen(){
    screenCapture.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
    screenCapture.save("captures/capture.bmp");
}

void ofApp::initKinect(){
    // kinect.setRegistration(true); -> supposed to slow down perf, so only activate if needed
    kinect.init();
    kinect.open();
    kinect.setCameraTiltAngle(0);
}

void ofApp::updateKinect() {
    if (kinectActive){
        kinect.update();
    }
}

void ofApp::drawKinect() {

    if (kinectActive){
        ofSetBackgroundColor(0);
        ofSetColor(255);
        auto mesh = kinectPlayer.convertToMesh(kinect.getRawDepthPixels());
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


void ofApp::updateTowers(){
    for (auto it = towers.begin(); it != towers.end(); it++){
        it->update();
    }
}

void ofApp::drawTowers(){
    for (auto it = towers.begin(); it != towers.end(); it++){
        it->draw();
    }
}

void ofApp::toggleTowersMove(){
    for (auto it = towers.begin(); it != towers.end(); it++){
        it->toggleMove();
    }
}

void ofApp::updateFaces(){
    for (auto it = faces.begin(); it != faces.end(); it++){
        it->update();
    }
}
void ofApp::drawFaces(){
    for (auto it = faces.begin(); it != faces.end(); it++){
        it->draw();
    }
}
void ofApp::nextFace(){
    if (currentPlayedFace >= 0) {
        setFaceEffect(currentPlayedFace, EffectType::EXPLOSION);
    }
    
    currentPlayedFace++;
    if (currentPlayedFace == FACES_COUNT) {
        currentPlayedFace = 0;
    }
    
    setFaceEffect(currentPlayedFace, EffectType::NONE);
    auto pos = (cam.getCurrentLookAt() - cam.getPosition()).normalize() * faceDistance + cam.getPosition();
    //cam.getOrientationEulerDeg()
    
    faces[currentPlayedFace].setPosition(pos.x, pos.y, pos.z);
}

void ofApp::setFaceEffect(int faceIdx, EffectType effect){
    faces[faceIdx].setEffect(effect);
}	


void ofApp::toggleDebugMode() {
    debugMode = !debugMode;
}

void ofApp::toggleMidiRecording(){
    midiRecordingActive = !midiRecordingActive;
    midiFrameStart = ofGetFrameNum();
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if (key == 'Q') postGlitch.setFx(OFXPOSTGLITCH_CONVERGENCE, false);
    if (key == 'W') postGlitch.setFx(OFXPOSTGLITCH_GLOW, false);
    if (key == 'E') postGlitch.setFx(OFXPOSTGLITCH_SHAKER, false);
    if (key == 'R') postGlitch.setFx(OFXPOSTGLITCH_CUTSLIDER, false);
    if (key == 'T') postGlitch.setFx(OFXPOSTGLITCH_TWIST, false);
    if (key == 'Y') postGlitch.setFx(OFXPOSTGLITCH_OUTLINE, false);
    if (key == 'U') postGlitch.setFx(OFXPOSTGLITCH_NOISE, false);
    if (key == 'I') postGlitch.setFx(OFXPOSTGLITCH_SLITSCAN, false);
    if (key == 'O') postGlitch.setFx(OFXPOSTGLITCH_SLITSCAN_ROT, false);
    if (key == 'P') postGlitch.setFx(OFXPOSTGLITCH_SWELL, false);
    if (key == 'A') postGlitch.setFx(OFXPOSTGLITCH_INVERT, false);
    if (key == 'S') postGlitch.setFx(OFXPOSTGLITCH_CR_HIGHCONTRAST, false);
    if (key == 'D') postGlitch.setFx(OFXPOSTGLITCH_CR_BLUERAISE, false);
    if (key == 'F') postGlitch.setFx(OFXPOSTGLITCH_CR_REDRAISE, false);
    if (key == 'G') postGlitch.setFx(OFXPOSTGLITCH_CR_GREENRAISE, false);
    if (key == 'H') postGlitch.setFx(OFXPOSTGLITCH_CR_REDINVERT, false);
    if (key == 'J') postGlitch.setFx(OFXPOSTGLITCH_CR_BLUEINVERT, false);
    if (key == 'K') postGlitch.setFx(OFXPOSTGLITCH_CR_GREENINVERT, false);
    
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

void ofApp::toggleMidiPlayer() {
    midiPlayerActive = !midiPlayerActive;
}

string ofApp::debugMessage() {
    stringstream reportStream;
    reportStream << "q: trigger recording : " << ofToString(recording) << endl
    << "w: screenshot" << endl
    << "a: activate kinect : " << ofToString(kinectActive) << endl
    << "s: trigger kinect recording " << ofToString(kinectRecordingActive) << endl
    << "i: toggle midi recording :  " << ofToString(midiRecordingActive) << endl
    << "o: toggle midi player :  " << ofToString(midiPlayerActive) << endl
    << "z: toggle debug :  " << ofToString(debugMode) << endl;
    
    return reportStream.str();
}
