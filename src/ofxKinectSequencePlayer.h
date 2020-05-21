#include "ofMain.h"


class ofxKinectSequencePlayer  {
public:
  
    vector<ofShortPixels> frames;
    int currentFrame = 0;
      
    void load(string prefix, string format, int numberWidth){
        int frameNumber = 0;
        string fileName = prefix + ofToString(frameNumber, numberWidth, '0') + "." + format;
        ofShortImage load;
        while (ofFile::doesFileExist(fileName)){
            load.load(fileName);
            frames.push_back(load.getPixels());
            frameNumber++;
            fileName = prefix + ofToString(frameNumber, numberWidth, '0') + "." + format;
        }
    }
    
    ofShortPixels getImage(int frameNumber) {
        return frames.at(frameNumber % frames.size());
    }
    
    ofShortPixels getNextImage() {
        return frames.at(currentFrame++ % frames.size());
    }
    

};
