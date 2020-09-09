#pragma once

#include "ofMain.h"
#include "ofxMidi.h"
#include "ofxImageSequenceRecorder.h"
#include "ofxKinectSequenceRecorder.h"
#include "ofxKinectSequencePlayer.h"
#include "ofxKinect.h"
#include "ofxOpenCv.h"
#include "ofxMidiSequenceRecorder.h"
#include "ofxMidiSequencePlayer.h"
#include "Tower.h"
#include "ofxPostGlitch.h"
#include "MyCamera.h"

class ofApp : public ofBaseApp, public ofxMidiListener {
    
public:
    ofApp();
    void setup();
    void exit();
    void update();
    void draw();
    
    // recording
    void toggleRecording();
    void captureScreen();
    
    // MIDI
    void newMidiMessage(ofxMidiMessage& eventArgs);
    void toggleMidiRecording();
    void toggleMidiPlayer();
    
    // kinect
    void initKinect();
    void toggleKinect();
    void updateKinect();
    void drawKinect();
    void toggleKinectRecording();
    
    // towers
    void updateTowers();
    void drawTowers();
    void toggleTowersMove();

    // kinect players
    void updatePlayers();
    void drawPlayers();
    void nextPlayer(int playerGroupIdx, bool reset = false);
    void setPlayerEffect(int playerGroupIdx, int playerIdx, EffectType effect);
    void setPlayerGroupEffect(int playerGroupIdx, EffectType effect);
    void resetPlayerGroup(int playerGroupIdx);
    void displayRandomFrame();
    void saveRandomFrame();
    
    void toggleDebugMode();
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    //midi handling
    void handleCamera(ofxMidiMessage& eventArgs);
    void handlePlayers(ofxMidiMessage& eventArgs);
    void handleEffects(ofxMidiMessage& eventArgs);
    void handleTowers(ofxMidiMessage& eventArgs);
    

    
private:
    // midi
    ofxMidiIn midiIn;
    ofxMidiSequenceRecorder midiRecorder;
    ofxMidiSequencePlayer midiPlayer;
    
    // camera
    MyCamera cam;
    ofEasyCam cam2;
    
    // recording
    ofxImageSequenceRecorder recorder;
    ofImage screenCapture;
    
    // kinect recording
    ofxKinectSequenceRecorder kinectRecorder;
    
    // kinect
    ofxKinect kinect;
    
    // towers
    vector<vector<Tower>> towers;
    
    // kinect players
    vector<vector<ofxKinectSequencePlayer>> players;
    
    // post procesing
    ofxPostGlitch postGlitch;
    ofFbo fbo;
    
    // debug
    string debugMessage();
};
