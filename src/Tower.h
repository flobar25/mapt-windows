#include "ofMain.h"


class Tower  {
public:
    
    void setup(ofPoint position, int floorCount, int floorHeight, int floorLength, int floorWidth, float displacementRate, ofVec3f displacement, ofColor color, bool reflection, ofVec3f reflectionDisplacement,
               ofColor reflectionColor) {
        this->floorCount = floorCount;
        this->floorHeight = floorHeight;
        this->floorLength = floorLength;
        this->floorWidth = floorWidth;
        this->position = position;
        this->displacementRate = displacementRate;
        this->displacement = displacement;
        this->reflection = reflection;
        this->reflectionDisplacement = reflectionDisplacement;
        this->reflectionColor = reflectionColor;
        
        shader.setGeometryInputType(GL_LINES);
        shader.setGeometryOutputType(GL_LINE_STRIP);
        shader.setGeometryOutputCount(floorCount);
        shader.load("myshaders/multiplyShaderVert.c", "myshaders/multiplyShaderFrag.c", "myshaders/multiplyShaderGeo.c");
        
        mesh.setMode(ofPrimitiveMode::OF_PRIMITIVE_LINES);
        mesh.addVertex(position);
        mesh.addColor(color);
        mesh.addVertex(ofPoint(position.x + floorWidth, position.y, position.z));
        mesh.addColor(color);
        mesh.addVertex(ofPoint(position.x + floorWidth, position.y, position.z + floorLength));
        mesh.addColor(color);
        mesh.addVertex(ofPoint(position.x, position.y, position.z + floorLength));
        mesh.addColor(color);
        mesh.addIndex(0);
        mesh.addIndex(1);
        mesh.addIndex(1);
        mesh.addIndex(2);
        mesh.addIndex(2);
        mesh.addIndex(3);
        mesh.addIndex(3);
        mesh.addIndex(0);
    }
    
    void update() {
        // do nothing for now
    }
    
    void draw() {
        if (move) {
            currentMoveFrame++;
        }

        ofSetLineWidth(2);
        if (!visible) {
            return;
        }
        
        shader.begin();
        shader.setUniform1i("count", floorCount);
        shader.setUniform1i("height", floorHeight);
        shader.setUniform1i("currentMoveFrame", currentMoveFrame);
        shader.setUniform1i("currentRise", currentRise);
        shader.setUniform1f("displacementRate", displacementRate);
        shader.setUniform3f("displacement", displacement);
        shader.setUniform1i("reflection", reflection ? 1 : 0);
        shader.setUniform3f("reflectionDisplacement", reflectionDisplacement);
        shader.setUniform4f("reflectionColor", glm::vec4(reflectionColor.r/255.0, reflectionColor.g/255.0, reflectionColor.b/255.0, reflectionColor.a/255.0));
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
    
    void updateRise(int cc) {
        currentRise = cc;
    }
    
    void toggleVisible() {
        visible = !visible;
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
    bool rise = false;
    int currentRise = 0;
    bool visible = true;

    bool reflection;
    ofVec3f reflectionDisplacement;
    ofColor reflectionColor;
};
