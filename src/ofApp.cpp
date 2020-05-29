#include "ofApp.h"
#include <chrono>


bool debugMode = true;
bool recording = false;
bool kinectActive = false;
bool kinectRecordingActive = false;
bool kinectPlayerActive = false;
bool midiRecordingActive = false;
bool midiPlayerActive = false;

int kinectCurrentFramePlayed = 0;
int kinectWidth = 640;
int kinectHeight = 480;
int kinectStep = 2;
int numberWidth = 8;
int midiFrameStart = 0;
bool started = false;

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
int KINECT_PLAYERS_COUNT = 50;


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
    
    kinectPlayer1.cropRight = 220;
    kinectPlayer1.cropLeft = 0;
    kinectPlayer1.cropUp = 0;
    kinectPlayer1.cropDown = 100;
    kinectPlayer1.cropNear = 200;
    kinectPlayer1.cropFar = 1000;
    kinectPlayer1.load(ofToDataPath("recording/kinect/frame_"), "png", kinectHeight, kinectWidth, 8, "images/orange1.jpg");
    
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
        kinectPlayer1.update();
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
   
    if (debugMode) {
        ofDrawAxis(200);
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
        case 'd': toggleKinectPlayer(); break;
        case 'z': toggleDebugMode(); break;
        case 'i': toggleMidiRecording(); break;
        case 'o': toggleMidiPlayer(); break;
        case '`': kinectPlayer1.toggleNoEffect(); break;
        case '-': started = !started; break;
        case 'j': cam.slowMoveToRandomPosition(); break;
        case 'k': cam.fastMoveToRandomPosition(); break;
        case '1': kinectPlayer1.toggleStretch(); break;
        case '2': kinectPlayer1.toggleStrips(); break;
        case '3': kinectPlayer1.toggleExplosion(); break;
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
    ofSetBackgroundColor(0);
    ofSetColor(255);
    
    ofMesh mesh;
    if (kinectActive){
        mesh = kinectPlayer1.convertToMesh(kinect.getRawDepthPixels());
        glPointSize(1);
        ofPushMatrix();
        // the projected points are 'upside down' and 'backwards'
        ofScale(1, -1, -1);
        ofTranslate(0, 0, -1000); // center the points a bit
        ofEnableDepthTest();
        mesh.drawVertices();
        ofDisableDepthTest();
        ofPopMatrix();
    } else if (kinectPlayerActive) {
        kinectPlayer1.draw();
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
    << "d: activate kinect player " << ofToString(kinectPlayerActive) << endl
    << "i: toggle midi recording :  " << ofToString(midiRecordingActive) << endl
    << "o: toggle midi player :  " << ofToString(midiPlayerActive) << endl
    << "z: toggle debug :  " << ofToString(debugMode) << endl;
    
    return reportStream.str();
}
