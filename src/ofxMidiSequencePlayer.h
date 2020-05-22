#include "ofMain.h"


class ofxMidiSequencePlayer {
public:
  
    vector<vector<ofxMidiMessage>> midiFrames;
    int currentPlayedFrame = 0;
      
    void load(string path){
        ofBuffer buffer = ofBufferFromFile(path);
        
        int currentFrame = -1;
        
        for (auto line : buffer.getLines()){
            auto split = splitLine(line);
            int lineFrame = std::stoi(split[0]);
            ofxMidiMessage midiMessage;
            midiMessage.channel = std::stoi(split[1]);
            midiMessage.status = (MidiStatus) std::stoi(split[2]);
            midiMessage.pitch = std::stoi(split[3]);
            midiMessage.velocity = std::stoi(split[4]);
            midiMessage.value = std::stoi(split[5]);
            
            while (lineFrame > currentFrame) {
                vector<ofxMidiMessage> messages;
                currentFrame++;
                midiFrames.push_back(messages);
            }
            
            midiFrames.at(lineFrame).push_back(midiMessage);
        }
    }
    
    vector<ofxMidiMessage> getMidiMessages(int frameNumber) {
        if (frameNumber >= midiFrames.size() ){
            vector<ofxMidiMessage> messages;
            return messages;
        }
        return midiFrames.at(frameNumber);
    }
    
    vector<ofxMidiMessage> getNextMidiMessages() {
        return getMidiMessages(currentPlayedFrame++);
    }
    
    vector<string> splitLine(string line){
        std::stringstream ss1(line);
        std::string item;
        std::vector<std::string> splittedLine;
        
        while (std::getline(ss1, item, ':'))
        {
           splittedLine.push_back(item);
        }
        
        std::vector<std::string> result;
        result.push_back(splittedLine[0]);
        
        std::stringstream ss2(splittedLine[1]);
        while (std::getline(ss2, item, ','))
        {
           result.push_back(item);
        }
        
        return result;
    }
};
