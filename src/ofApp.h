#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#import "ofxUI.h"
#import "ofxJSONElement.h"
#define PORT 12345

class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    //video and osc
    int count;
    void setupVideo();
    void setupOSC();
    ofxOscReceiver receiver;
    float position;
    
    int current_msg_string;
    
    int mouseX;
    int mouseY;
    string mouseButtonState;
    float currentMovie;
    ofVideoPlayer player;
    
    bool switchVideo;
    
    
    
    //ui
    ofxUICanvas *gui;
    ofxUIDropDownList *ddl;
    void setupUI();
    void guiEvent(ofxUIEventArgs &e);
    void updateDDL();
    bool debug;
    void drawDebug();
    
    
    //json
    ofxJSONElement response;
    std::stringstream ss;
    bool gotJSON;
    void getJSON();
    void updateJSONDebug();
    void loadJSON();
    
    void downloadVideos();
    
    ofURLFileLoader fileloader;
    void urlResponse(ofHttpResponse & httpResponse);
    
    void changeVideo(string video);
    
    int videosDownloaded;
    int numVideosToGet;
    
    bool drawLoading;
		
};
