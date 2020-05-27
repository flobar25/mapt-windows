#include "ofMain.h"


class Tower  {
public:
    
    void setup(ofPoint position, int floorCount, int floorHeight, int floorLength, int floorWidth, float displacementRate, ofVec3f displacement) {
        this->floorCount = floorCount;
        this->floorHeight = floorHeight;
        this->floorLength = floorLength;
        this->floorWidth = floorWidth;
        this->position = position;
        this->displacementRate = displacementRate;
        this->displacement = displacement;
        
        shader.setGeometryInputType(GL_LINES);
        shader.setGeometryOutputType(GL_LINE_STRIP);
        shader.setGeometryOutputCount(floorCount);
        shader.load("shaders/multiplyShaderVert.c", "shaders/multiplyShaderFrag.c", "shaders/multiplyShaderGeo.c");
        
        mesh.setMode(ofPrimitiveMode::OF_PRIMITIVE_LINES);
        mesh.addVertex(position);
        mesh.addColor(ofColor(200));
        mesh.addVertex(ofPoint(position.x + floorWidth, position.y, position.z));
        mesh.addColor(ofColor(200));
        mesh.addVertex(ofPoint(position.x + floorWidth, position.y, position.z + floorLength));
        mesh.addColor(ofColor(200));
        mesh.addVertex(ofPoint(position.x, position.y, position.z + floorLength));
        mesh.addColor(ofColor(200));
        mesh.addIndex(0);
        mesh.addIndex(1);
        mesh.addIndex(1);
        mesh.addIndex(2);
        mesh.addIndex(2);
        mesh.addIndex(3);
        mesh.addIndex(3);
        mesh.addIndex(0);

    }
    
    void draw() {
        if (move) {
            currentMoveFrame++;
        }
        ofSetLineWidth(2);
        shader.begin();
        shader.setUniform1i("count", floorCount);
        shader.setUniform1i("height", floorHeight);
        shader.setUniform1i("currentFrame", currentMoveFrame);
        shader.setUniform1f("displacementRate", displacementRate);
        shader.setUniform3f("displacement", displacement);
        mesh.draw();
        shader.end();
    }
    
    void toggleMove() {
        if (move == 0) {
            move = 1;
        } else {
            move = 0;
        }
    }
    
private:
    int floorCount;
    int floorHeight;
    int floorLength;
    int floorWidth;
    float displacementRate;
    ofVec3f displacement;
    ofShader shader;
    ofPoint position;
    ofMesh mesh;
    int move = 0;
    int currentMoveFrame = 0;
};
