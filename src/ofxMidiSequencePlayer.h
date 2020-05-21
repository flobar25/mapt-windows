#include "ofMain.h"


class ofxMidiSequenceRecorder : public ofThread {
public:
    
    queue<string> q;
    ofBuffer buffer;
    string filePath;
    
    
    ~ofxMidiSequenceRecorder(){
        ofBufferToFile(filePath, buffer);
    }
    
    void openFile(std::string path){
        filePath =path;
    }
    
    void threadedFunction() {
        while(isThreadRunning()) {
            if(!q.empty()){
                buffer.append(q.front());
                q.pop();
            }
        }
    }
    
    
    void addMidiFrame(int frameNumber, ofxMidiMessage& midi) {
        string input = ofToString(frameNumber) + ":" + ofToString(midi.channel) + ","
            + ofToString(midi.status) + ","
            + ofToString(midi.pitch) + ","
            + ofToString(midi.velocity) + ","
            + ofToString(midi.value) + "\n";
        q.push(input);
    }
};
