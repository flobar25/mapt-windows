#include "ofMain.h"


class ofxKinectSequenceRecorder : public ofThread {
public:
  
    int counter;
    queue<QueuedImage> q;
    string prefix;
    string format;
    int numberWidth;
      
    ofxKinectSequenceRecorder(){
        counter=0;
        numberWidth=4;
    }
    
    void setPrefix(string pre){
        prefix = pre;
    }
    
    void setFormat(string fmt){
        format = fmt;
    }
       
    void setCounter(int count){
        counter = count;
    }
       
    void setNumberWidth(int nbwidth){
        numberWidth = nbwidth;
    }
       
    void threadedFunction() {
        while(isThreadRunning()) {
            if(!q.empty()){
                QueuedImage i = q.front();
                ofSaveImage(i.image, i.fileName);
                q.pop();
            }
        }
    }

        
    void addFrame(ofShortPixels imageToSave) {
        string fileName = prefix + ofToString(counter, numberWidth, '0') + "." + format;
        counter++;
        
        QueuedImage qImage;
        qImage.fileName = fileName;
        qImage.image = imageToSave;
        
        q.push(qImage);
    }
};
