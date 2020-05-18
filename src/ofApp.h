#pragma once

#include "ofMain.h"
#include "ofxMidi.h"
#include "ofxImageSequenceRecorder.h"
#include "ofxKinect.h"
#include "ofxOpenCv.h"

class ofApp : public ofBaseApp, public ofxMidiListener {
    
public:
    void setup();
    void exit();
    void update();
    void draw();
    
    // recording
    void toggleRecording();
    void captureScreen();
    
    // MIDI
    void newMidiMessage(ofxMidiMessage& eventArgs);
    
    // kinect
    void initKinect();
    void toggleKinectCapture();
    void updateKinect();
    void drawKinect();
    
    // Debug
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
    

    
private:
    // midi
    ofxMidiIn midiIn;
    
    // camera
    ofEasyCam cam;
    
    // recording
    ofxImageSequenceRecorder recorder;
    Boolean recording;
    ofImage screenCapture;
    
    // kinect
    ofxKinect kinect;
    int nearThreshold;
    int farThreshold;
    ofxCvGrayscaleImage grayImage; // grayscale depth image
    ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
    ofxCvGrayscaleImage grayThreshFar; // the far thresholded image
};
