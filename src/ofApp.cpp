#include "ofApp.h"
#include <chrono>

// input parameters
int RANDOM_SEED = 67859432679;
int SPACE_WIDTH = 10000;
int SPACE_LENGTH = 10000;
int SPACE_HEIGHT = 500;
int TOWER_COUNT_1 = 50;
int TOWER_COUNT_2 = 50;
int TOWER_MIN_FLOORS = 8;
int TOWER_MAX_FLOORS = 100;
int TOWER_MIN_WIDTH = 150;
int TOWER_MAX_WIDTH = 400;
int TOWER_MIN_LENGTH = 150;
int TOWER_MAX_LENGTH = 400;
int PLAYERS1_COUNT = 3;
int PLAYERS2_COUNT = 10;
ofColor BACKGROUND_COLOR_1 = ofColor(0, 0, 100);
ofColor TOWER_COLOR_1 = ofColor(225, 202, 232);
ofColor TOWER_REFLECTION_COLOR_1 = ofColor(0, 202, 0, 0.5);
ofColor TOWER_COLOR_2 = ofColor(0, 202, 232);
ofColor TOWER_REFLECTION_COLOR_2 = ofColor(0, 0, 202, 0.5);

bool debugMode = true;
bool started = false;
bool recording = false;

// kinect cam
bool kinectActive = false;
bool kinectRecordingActive = false;
int kinectCurrentFramePlayed = 0;
int kinectWidth = 640;
int kinectHeight = 480;
int kinectStep = 2;
int numberWidth = 8;

// midi
bool midiRecordingActive = false;
bool midiPlayerActive = false;
int midiFrameStart = 0;

// kinect players
vector<int> currentPlayedIndices;
int playerDistance = 500;





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
    //camera
    cam.setNearClip(0);
    cam.setFarClip(100000);
    ofSeedRandom(RANDOM_SEED);
    
    
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
    vector<Tower> towers1;
    for (int i = 0; i < TOWER_COUNT_1; i++) {
        Tower tower;
        tower.setup(ofPoint(ofRandom(SPACE_WIDTH) - SPACE_WIDTH / 2,0,ofRandom(SPACE_LENGTH) - SPACE_WIDTH / 2), // position
                    ofRandom(TOWER_MAX_FLOORS - TOWER_MIN_FLOORS) + TOWER_MIN_FLOORS, // floors
                    200, // floor height
                    ofRandom(TOWER_MAX_LENGTH - TOWER_MIN_LENGTH) + TOWER_MIN_LENGTH, // length
                    ofRandom(TOWER_MAX_WIDTH - TOWER_MIN_WIDTH) + TOWER_MIN_WIDTH, // width
                    0.1, // displacement rate
                    ofPoint(-50,0,0), // displacement vector
                    TOWER_COLOR_1,
                    true,
                    ofVec3f(-20, -20, 0),
                    TOWER_REFLECTION_COLOR_1
                    );
        towers1.push_back(tower);
    }
    towers.push_back(towers1);
    
    vector<Tower> towers2;
    for (int i = 0; i < TOWER_COUNT_2; i++) {
        Tower tower;
        tower.setup(ofPoint(ofRandom(SPACE_WIDTH) - SPACE_WIDTH / 2,0,ofRandom(SPACE_LENGTH) - SPACE_WIDTH / 2), // position
                    ofRandom(TOWER_MAX_FLOORS - TOWER_MIN_FLOORS) + TOWER_MIN_FLOORS, // floors
                    200, // floor height
                    ofRandom(TOWER_MAX_LENGTH - TOWER_MIN_LENGTH) + TOWER_MIN_LENGTH, // length
                    ofRandom(TOWER_MAX_WIDTH - TOWER_MIN_WIDTH) + TOWER_MIN_WIDTH, // width
                    0.1, // displacement rate
                    ofPoint(50,70,0), // displacement vector
                    TOWER_COLOR_2,
                    true,
                    ofVec3f(20, 320, 40),
                    TOWER_REFLECTION_COLOR_2
                    );
        towers2.push_back(tower);
    }
    towers.push_back(towers2);
    

    vector<ofxKinectSequencePlayer> players1;
    for (int i = 0; i < PLAYERS1_COUNT; i++){
        ofxKinectSequencePlayer kinectPlayer;
        kinectPlayer.cropRight = 0;
        kinectPlayer.cropLeft = 0;
        kinectPlayer.cropUp = 0;
        kinectPlayer.cropDown = 0;
        kinectPlayer.cropNear = 100;
        kinectPlayer.cropFar = 11500;
        kinectPlayer.load(ofToDataPath("recording/1591109337000/kinect/frame_"), "png", kinectHeight, kinectWidth, 8, "images/orange1.jpg");
        players1.push_back(kinectPlayer);
    }
    players.push_back(players1);
    currentPlayedIndices.push_back(-1);
    
    vector<ofxKinectSequencePlayer> players2;
    for (int i = 0; i < PLAYERS2_COUNT; i++){
        ofxKinectSequencePlayer kinectPlayer;
        kinectPlayer.cropRight = 220;
        kinectPlayer.cropLeft = 0;
        kinectPlayer.cropUp = 0;
        kinectPlayer.cropDown = 100;
        kinectPlayer.cropNear = 200;
        kinectPlayer.cropFar = 1000;
        kinectPlayer.load(ofToDataPath("recording/kinect/frame_"), "png", kinectHeight, kinectWidth, 8, "images/blue1.jpg");
        players2.push_back(kinectPlayer);
    }
    players.push_back(players2);
    currentPlayedIndices.push_back(-1);
    
    
    // midi
    midiIn.openPort(0);
    midiIn.addListener(this);
    
    // kinect
    initKinect();
    
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
    
//    if (debugMode) {
        // show the framerate on window title
        std::stringstream strm;
        strm << "fps: " << ofGetFrameRate();
        ofSetWindowTitle(strm.str());
//    }
    
    if (kinectActive) {
        updateKinect();
        return;
    }
    
    if (started) {
        cam.update();
        updatePlayers();
        updateTowers();
    }
}

//--------------------------------------------------------------
void ofApp::draw(){

    
    if (kinectActive){
        drawKinect();
        if (kinectRecordingActive) {
            kinectRecorder.addFrame(kinect.getRawDepthPixels());
        }
        return;
    }
    
    if (midiPlayerActive) {
        for (auto midiMessage : midiPlayer.getNextMidiMessages()) {
            newMidiMessage(midiMessage);
        }
    }
    
    ofEnableDepthTest();
    
    fbo.begin();
    cam.begin();
    ofClear(0,0,0,255);
    ofBackground(BACKGROUND_COLOR_1);

    drawTowers();
    drawPlayers();
    
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
        case 'd': nextPlayer(0); break;
        case 'f': nextPlayer(1); break;
        case 'z': toggleDebugMode(); break;
        case 'i': toggleMidiRecording(); break;
        case 'o': toggleMidiPlayer(); break;
        case '-': started = !started; break;
        case 'j': cam.slowMoveToRandomPosition(); break;
        case 'k': cam.fastMoveToRandomPosition(); break;
//        case '`': sePlayertEffect(EffectType::NONE); break;
//        case '1': players1[currentPlayedFace].setEffect(EffectType::STRETCH); break;
//        case '2': players1[currentPlayedFace].setEffect(EffectType::STRIPS); break;
//        case '3': players1[currentPlayedFace].setEffect(EffectType::EXPLOSION); break;
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
    
    switch (midiMessage.channel) {
        case 1:
            //record
            if (midiMessage.status == MIDI_NOTE_ON){
                toggleKinect();
                toggleKinectRecording();
            }
            break;
        default:
            break;
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
        ofBackground(0, 0, 0);
        kinect.update();
    }
}

void ofApp::drawKinect() {

    
    if (kinectActive){
        cam2.begin();
        ofSetBackgroundColor(0);
        ofSetColor(255);
        auto mesh = ofxKinectSequencePlayer::convertToMeshWithoutCropping(kinect.getRawDepthPixels(), kinectWidth, kinectHeight);
        glPointSize(1);
        ofPushMatrix();
        // the projected points are 'upside down' and 'backwards'
        ofScale(1, -1, -1);
        ofTranslate(0, 0, -1000); // center the points a bit
        ofEnableDepthTest();
        mesh.drawVertices();
        ofDisableDepthTest();
        ofPopMatrix();
        cam2.end();
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
        for (auto subIt = it->begin(); subIt != it->end(); subIt++){
            subIt->update();
        }
    }
}

void ofApp::drawTowers(){
    for (auto it = towers.begin(); it != towers.end(); it++){
        for (auto subIt = it->begin(); subIt != it->end(); subIt++){
            subIt->draw();
        }
    }
}

void ofApp::toggleTowersMove(){
    for (auto it = towers.begin(); it != towers.end(); it++){
        for (auto subIt = it->begin(); subIt != it->end(); subIt++){
            subIt->toggleMove();
        }
    }
}

void ofApp::updatePlayers(){
    for (auto it = players.begin(); it != players.end(); it++){
        for (auto subIt = it->begin(); subIt != it->end(); subIt++){
            subIt->update();
        }
    }
}
void ofApp::drawPlayers(){
    for (auto it = players.begin(); it != players.end(); it++){
        for (auto subIt = it->begin(); subIt != it->end(); subIt++){
            subIt->draw();
        }
    }
}


void ofApp::nextPlayer(int playerGroupIdx){
    int currentPlayerIdx = currentPlayedIndices[playerGroupIdx];
    
    if (currentPlayerIdx >= 0) {
        setPlayerEffect(playerGroupIdx, currentPlayerIdx, EffectType::EXPLOSION);
    }
    
    currentPlayerIdx++;
    if (currentPlayerIdx == players[playerGroupIdx].size()) {
        currentPlayerIdx = 0;
    }
    
    setPlayerEffect(playerGroupIdx, currentPlayerIdx, EffectType::NONE);
    auto pos = (cam.getCurrentLookAt() - cam.getPosition()).normalize() * playerDistance + cam.getPosition();
    players[playerGroupIdx][currentPlayerIdx].setQuaternion(cam.getOrientationQuat());
    players[playerGroupIdx][currentPlayerIdx].setPosition(pos.x, pos.y, pos.z);
    currentPlayedIndices[playerGroupIdx] = currentPlayerIdx;
}

void ofApp::setPlayerEffect(int playerGroupIdx, int playerIdx, EffectType effect){
    players[playerGroupIdx][playerIdx].setEffect(effect);
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
