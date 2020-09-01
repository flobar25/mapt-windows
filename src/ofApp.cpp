#include "ofApp.h"
#include <chrono>

// input parameters
int FRAME_RATE = 6;
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
int PLAYERS2_COUNT = 3;
ofColor BACKGROUND_COLOR_1 = ofColor(0, 0, 50);
ofColor TOWER_COLOR_1 = ofColor(213, 216, 247);
ofColor TOWER_REFLECTION_COLOR_1 = ofColor(213, 216, 247, 30);
ofColor TOWER_COLOR_2 = ofColor(135, 202, 232);
ofColor TOWER_REFLECTION_COLOR_2 = ofColor(135, 202, 232, 30);



bool debugMode = true;
bool started = false;
bool recording = false;
bool randomMode = false;

// kinect cam
bool kinectActive = false;
bool kinectRecordingActive = false;
int kinectCurrentFramePlayed = 0;
int kinectWidth = 640;
int kinectHeight = 480;
int kinectStep = 2;
int numberWidth = 8;
ofxKinectSequencePlayer* randomPlayer;

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
    
    
    // kinect players
    vector<ofxKinectSequencePlayer> players1;
    ofxKinectSequencePlayer kinectPlayer1;
    kinectPlayer1.cropRight = 0;
    kinectPlayer1.cropLeft = 0;
    kinectPlayer1.cropUp = 0;
    kinectPlayer1.cropDown = 0;
    kinectPlayer1.cropNear = 100;
    kinectPlayer1.cropFar = 2000;
    kinectPlayer1.load(ofToDataPath("recording/dancesing2/kinect/frame_"), "png", kinectHeight, kinectWidth, 8, "images/orange1.jpg", 9, 800, ofVec3f(-200, -300, -900));
    players1.push_back(kinectPlayer1);
    for (int i = 0; i < PLAYERS1_COUNT-1; i++){
        ofxKinectSequencePlayer* copiedPlayer = new ofxKinectSequencePlayer();
        copiedPlayer->loadFromPlayer(kinectPlayer1);
        players1.push_back(*copiedPlayer);
    }
    players.push_back(players1);
    currentPlayedIndices.push_back(-1);
    
    vector<ofxKinectSequencePlayer> players2;
    ofxKinectSequencePlayer kinectPlayer2;
    kinectPlayer2.cropRight = 220;
    kinectPlayer2.cropLeft = 0;
    kinectPlayer2.cropUp = 0;
    kinectPlayer2.cropDown = 100;
    kinectPlayer2.cropNear = 200;
    kinectPlayer2.cropFar = 1000;
    kinectPlayer2.load(ofToDataPath("recording/face1/kinect/frame_"), "png", kinectHeight, kinectWidth, 8, "images/blue1.jpg", 9, 800, ofVec3f(-400, -300, -1000));
    players2.push_back(kinectPlayer2);
    for (int i = 0; i < PLAYERS2_COUNT; i++){
        ofxKinectSequencePlayer* copiedPlayer = new ofxKinectSequencePlayer();
        copiedPlayer->loadFromPlayer(kinectPlayer2);
        players2.push_back(*copiedPlayer);
    }
    players.push_back(players2);
    currentPlayedIndices.push_back(-1);
    
    
    vector<ofxKinectSequencePlayer> players3;
    ofxKinectSequencePlayer kinectPlayer3;
    kinectPlayer3.cropRight = 0;
    kinectPlayer3.cropLeft = 0;
    kinectPlayer3.cropUp = 0;
    kinectPlayer3.cropDown = 0;
    kinectPlayer3.cropNear = 0;
    kinectPlayer3.cropFar = 2000;
    kinectPlayer3.intensityThreshold = 50;
    kinectPlayer3.load(ofToDataPath("recording/dog2/kinect/frame_"), "png", kinectHeight, kinectWidth, 8, "images/blue2.jpg", 1920, 2020);
    players3.push_back(kinectPlayer3);
    for (int i = 0; i < 1; i++){
        ofxKinectSequencePlayer* copiedPlayer = new ofxKinectSequencePlayer();
        copiedPlayer->loadFromPlayer(kinectPlayer3);
        players3.push_back(*copiedPlayer);
    }
    players.push_back(players3);
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
    ofEnableSmoothing();
    ofEnableDepthTest();
    ofSetFrameRate(FRAME_RATE);
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
    
    if (started && !randomMode) {
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

    //ofClear(0,0,0,255);
    ofEnableAlphaBlending();
    ofBackground(BACKGROUND_COLOR_1);
    
    if (randomMode) {
        randomPlayer->drawRandom(cam);
        
    } else {
        drawTowers();
        drawPlayers();
    }
    
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
        case 'g': nextPlayer(2); break;
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
        case 'b': displayRandomFrame(); break;
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
            started = true;
            break;
        case 2:
            handleCamera(midiMessage);
            break;
        case 3:
            handlePlayers(midiMessage);
            break;
        case 4:
            handleEffects(midiMessage);
            break;
        case 5:
            handleTowers(midiMessage);
            break;
        case 16:
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

void ofApp::handleTowers(ofxMidiMessage &midiMessage){
    if (midiMessage.status == MIDI_CONTROL_CHANGE){
        switch (midiMessage.control) {
            case 14:
                for (auto it = towers[0].begin(); it!=towers[0].end(); it++){
                    it->updateRise(midiMessage.value);
                }
                break;
            case 15:
                for (auto it = towers[1].begin(); it!=towers[1].end(); it++){
                    it->updateRise(midiMessage.value);
                }
                break;
            default:
                break;
        }
    }
    if (midiMessage.status == MIDI_NOTE_ON){
        switch (midiMessage.pitch) {
            case 0:
                for (auto it = towers.begin(); it != towers.end(); it++){
                    for (auto subIt = it->begin(); subIt != it->end(); subIt++){
                        subIt->toggleVisible();
                    }
                }
                break;
            default:
                break;
        }
    }
}

void ofApp::handlePlayers(ofxMidiMessage &midiMessage){
    if (midiMessage.status == MIDI_NOTE_ON){
        switch (midiMessage.pitch) {
            case 0:
                nextPlayer(0);
                break;
            case 1:
                setPlayerEffect(0, currentPlayedIndices[0], EffectType::INVISIBLE);
                break;
            case 2:
                setPlayerEffect(0, currentPlayedIndices[0], EffectType::STRIPS);
                break;
            case 3:
                setPlayerEffect(0, currentPlayedIndices[0], EffectType::STRETCH);
                break;
            case 4:
                setPlayerEffect(0, currentPlayedIndices[0], EffectType::EXPLOSION);
                break;
            case 5:
                setPlayerEffect(0, currentPlayedIndices[0], EffectType::NONE);
                break;
            case 6:
                setPlayerGroupEffect(0, EffectType::INVISIBLE);
                break;
            case 7:
                resetPlayerGroup(0);
                break;
            case 12:
                nextPlayer(1);
                break;
            case 13:
                setPlayerEffect(1, currentPlayedIndices[1], EffectType::INVISIBLE);
                break;
            case 14:
                setPlayerEffect(1, currentPlayedIndices[1], EffectType::STRIPS);
                break;
            case 15:
                setPlayerEffect(1, currentPlayedIndices[1], EffectType::STRETCH);
                break;
            case 16:
                setPlayerEffect(1, currentPlayedIndices[1], EffectType::EXPLOSION);
                break;
            case 17:
                setPlayerEffect(1, currentPlayedIndices[1], EffectType::NONE);
                break;
            case 18:
                setPlayerGroupEffect(1, EffectType::INVISIBLE);
                break;
            case 19:
                resetPlayerGroup(1);
                break;
            case 24:
                nextPlayer(2, true); // specific case for the basset
                break;
            case 25:
                setPlayerEffect(2, currentPlayedIndices[2], EffectType::INVISIBLE);
                break;
            case 26:
                setPlayerEffect(2, currentPlayedIndices[2], EffectType::STRIPS);
                break;
            case 27:
                setPlayerEffect(2, currentPlayedIndices[2], EffectType::STRETCH);
                break;
            case 28:
                setPlayerEffect(2, currentPlayedIndices[2], EffectType::EXPLOSION);
                break;
            case 29:
                setPlayerEffect(2, currentPlayedIndices[2], EffectType::NONE);
                break;
            case 30:
                setPlayerGroupEffect(2, EffectType::INVISIBLE);
                break;
            case 31:
                resetPlayerGroup(2);
                break;
            default:
                break;
        }
    }
}

void ofApp::handleCamera(ofxMidiMessage &midiMessage){
    if (midiMessage.status == MIDI_NOTE_ON){
        switch (midiMessage.pitch) {
            case 0:
                cam.slowMoveToRandomPosition();
                break;
            case 1:
                cam.fastMoveToRandomPosition();
                break;
            default:
                break;
        }
    }
}

void ofApp::handleEffects(ofxMidiMessage &midiMessage){
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
    
    if (midiMessage.status == MIDI_NOTE_ON){
        postGlitch.setFx(static_cast<ofxPostGlitchType>(midiMessage.pitch), true);
    }  else if (midiMessage.status == MIDI_NOTE_OFF){
        postGlitch.setFx(static_cast<ofxPostGlitchType>(midiMessage.pitch), false);
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

void ofApp::displayRandomFrame(){
    randomMode = true;
    int playerGrpIdx = ofRandom(players.size() + 0.1);
    int playerIdx = ofRandom(players[playerGrpIdx].size() + 0.1);
    
    randomPlayer = &(players[playerGrpIdx][playerIdx]);
    randomPlayer->setPosition(0,0,0);
    randomPlayer->setEffect(EffectType::STRIPS);
    int frameIdx = ofRandom(randomPlayer->size() + 0.1);
    randomPlayer->setCurrentFrame(frameIdx);
}

void ofApp::nextPlayer(int playerGroupIdx, bool reset ){
    int currentPlayerIdx = currentPlayedIndices[playerGroupIdx];
    
//    if (currentPlayerIdx >= 0) {
//        setPlayerEffect(playerGroupIdx, currentPlayerIdx, EffectType::EXPLOSION);
//    }
    
    currentPlayerIdx++;
    if (currentPlayerIdx == players[playerGroupIdx].size()) {
        currentPlayerIdx = 0;
    }
    
    if (reset) {
        players[playerGroupIdx][currentPlayerIdx].reset();
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

void ofApp::setPlayerGroupEffect(int playerGroupIdx, EffectType effect){
    for (auto it = players[playerGroupIdx].begin(); it != players[playerGroupIdx].end(); it++){
        it->setEffect(effect);
    }
}

void ofApp::resetPlayerGroup(int playerGroupIdx){
    for (auto it = players[playerGroupIdx].begin(); it != players[playerGroupIdx].end(); it++){
        it->reset();
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
    << "i: toggle midi recording :  " << ofToString(midiRecordingActive) << endl
    << "o: toggle midi player :  " << ofToString(midiPlayerActive) << endl
    << "z: toggle debug :  " << ofToString(debugMode) << endl;
    
    return reportStream.str();
}
