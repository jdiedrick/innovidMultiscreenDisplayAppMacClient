#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    //hide cursor
    CGDisplayHideCursor(NULL);
    
    cout << "listening for osc messages on port " << PORT << "\n";
    
    receiver.setup( PORT );
    
    //player.loadMovie("movies/tv.mov");
    //player.play();
    
    //set count
    count = 0;
    
    //set debug and sendosc
    debug = false;
    
    //setup ui and our first video
    setupUI();
    setupVideo();
    
    //setup osc
    setupOSC();
    
    //set gotjson
    gotJSON = false;
    
    //set ss
    ss << "No video information received yet" << endl;
    
    //load json
    loadJSON();
    
    ofRegisterURLNotification(this);
    videosDownloaded = 0;


}

//--------------------------------------------------------------
void ofApp::update(){
    
    player.update();
    
    if (!debug) CGDisplayHideCursor(NULL);

    
    while( receiver.hasWaitingMessages() ){
        // get the next message
        ofxOscMessage m;
        receiver.getNextMessage( &m );
        
        
        //get movie position
        
        if ( m.getAddress() == "/movie/position" )
        {
            position = m.getArgAsFloat(0);
            
        }
        // Set the position
        player.setPosition(position);
        
        cout << "position: " << position << endl;
        
    }
    

}

//--------------------------------------------------------------
void ofApp::draw(){
    player.draw(0, 0, 1920, 1080);
    if(debug){
        CGDisplayShowCursor(NULL);
       // drawDebug();
    }
    
    if(drawLoading) {
        ofPushStyle();
        ofSetColor(255, 255, 255, 127);
        ofRect(0,0,ofGetWidth(),ofGetHeight());
        ofPopStyle();
        string vid_dl_string = "Downloaded " + ofToString(videosDownloaded, 2) + " of " + ofToString(numVideosToGet, 2)  + " videos. Please wait!";
        ofDrawBitmapString(vid_dl_string, 0 ,ofGetHeight()/2);
    }
    //cout << "draw loading? : " << drawLoading << endl;



}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key=='f'){
        ofToggleFullscreen();
    }
    
    if(key=='d'){
        debug = !debug;
        gui->toggleVisible();
    }

}

# pragma mark - Video and OSC

//--------------------------------------------------------------
void ofApp::setupOSC(){
    //set up osc and counter
    receiver.setup( PORT );
}

//--------------------------------------------------------------
void ofApp::setupVideo(){
    //load movie and start playing
    player.loadMovie("movies/tv.mov");
    player.play();
    player.setLoopState(OF_LOOP_NORMAL);
    
}

# pragma mark - setup UI

void ofApp::setupUI()
{
    gui = new ofxUICanvas(0, 0, 320, 320);
    //gui->setColorBack(ofColor(127));
    gui->setFont("GUI/NewMedia Fett.ttf");
    //gui->addButton("Download videos", false, 44, 44);
    gui->addButton("Update videos", false, 44, 44);
    vector<string> names;
    names.push_back("-");
    ddl = gui->addDropDownList("SELECT A VIDEO", names);
    ddl->addToggle(ofGetTimestampString());
    ddl->setAutoClose(true);
    ofAddListener(gui->newGUIEvent, this, &ofApp::guiEvent);
    gui->toggleVisible();
    gui->loadSettings("GUI/guiSettings.xml");
}

//--------------------------------------------------------------
void ofApp::guiEvent(ofxUIEventArgs &e)
{
    string name = e.widget->getName();
	//int kind = e.widget->getKind();
	cout << "got event from: " << name << endl;
	
	if(name == "Download videos")
    {
        ofxUIButton *button = (ofxUIButton *) e.widget;
        //setupOSC();
        //sendOSC = toggle->getValue();
        if(button->getValue() == 1){
            //downloadVideos();
            cout << "trying to dl videos" << endl;
        }
        //cout << name << "\t value: " << button->getValue() << endl;
    }
    
    else if(name == "Update videos")
    {
        ofxUIButton *button = (ofxUIButton *) e.widget;
        //setupOSC();
        //sendOSC = toggle->getValue();
        if(button->getValue() == 1){
            getJSON();
        }
        //cout << name << "\t value: " << button->getValue() << endl;
    }
    
    else if(name == "SELECT A VIDEO")
    {
        ofxUIDropDownList *ddlist = (ofxUIDropDownList *) e.widget;
        vector<ofxUIWidget *> &selected = ddlist->getSelected();
        for(int i = 0; i < selected.size(); i++)
        {
            cout << "SELECTED: " << selected[i]->getName() << endl;
            
            if (selected[i]->getName() == "iphone.mov") { // special case for prebundled video
                player.stop();
                player.loadMovie("movies/iphone.mov");
                player.play();
            }else if (selected[i]->getName() == "-") { // special case for prebundled video
                continue;
            }else{
                changeVideo(selected[i]->getName());
            }
            
        }
    }
}

//--------------------------------------------------------------
void ofApp::drawDebug(){
    
    ofDrawBitmapString("frame: " + ofToString(player.getCurrentFrame()) + "/"+ofToString(player.getTotalNumFrames()),0,ofGetHeight()/2);
    ofDrawBitmapString("position: " + ofToString(player.getPosition()),50,ofGetHeight()/2 + 20);
    ofDrawBitmapString(ss.str(), 0, ofGetHeight()/2 + 40); // draw json on right side of screen
    
    
}

//--------------------------------------------------------------
void ofApp::updateDDL(){
    cout << "updating ddl" << endl;
    //clear toggles
    ddl->clearToggles();
    //ddl->addToggle("iphone.mov");
    //update our dropdown box with the videos
    for (int i=0; i<response["videos"].size(); i++){
        if (response["videos"][i]["tag"].asString() == "Mini") {
            ddl->addToggle(response["videos"][i]["filename"].asString());
        }
    }
}

#pragma mark - JSON
//--------------------------------------------------------------
void ofApp::getJSON(){
    cout << "Getting JSON..." << endl;
    ss.str(std::string()); // clear string stream
    ss << "Getting JSON..." << endl;
    
    //get json
    std::string url = "http://stormy-badlands-2316.herokuapp.com/data";
    
    if (!response.open(url)) {
		cout  << "Failed to parse JSON\n" << endl;
        ss.str(std::string()); // clear string stream
        ss << "Failed to parse JSON\n" << endl;
	}else{
        gotJSON = true;
        ss.str(std::string()); // clear string stream
        cout << "Got JSON..." << endl;
        
        ofFile file(ofToDataPath("innovid_videos.json"),ofFile::WriteOnly);
        file << response.getRawString();
        file.close();
        
        //updateJSONDebug();
        string json_file_path = ofToDataPath("innovid_videos.json");
        std::string filePath = json_file_path;
        bool parsingSuccessful = response.open(filePath);
        
        if (parsingSuccessful) {
            cout  << "Parsing successful" << endl;
            
            //updateJSONDebug();
            updateDDL();
            downloadVideos();
        } else {
            cout  << "Failed to parse JSON" << endl;
        }
        
        
        //updateDDL();
        //downloadVideos();
    }
}

//--------------------------------------------------------------
void ofApp::updateJSONDebug(){
    //clear the stringstream
    ss.str(std::string());
    
    //update string stream w json data
    ss<< "number of videos in backend: " << response["videos"].size() << "\n" << endl;
    for (int i=0; i<response["videos"].size(); i++){
        ss << "video " << i+ 1 // +1 for pretty, "non-coder" numbers
        << "\n"
        << "title: "
        << response["videos"][i]["title"].asString()
        << "\n"
        << "filename: "
        << response["videos"][i]["filename"].asString()
        << "\n"
        << "timestamp: "
        <<  response["videos"][i]["timestamp"].asString()
        << "\n"
        << endl;
    }
}

//--------------------------------------------------------------
void ofApp::loadJSON(){
    
    //if json isn't there
    ofFile json;
    string json_file_path = ofToDataPath("innovid_videos.json");
    if ( !json.doesFileExist(json_file_path, false) ) {
        
        //download, save and load json
        //this is done on the ui now by hitting the update json button.
        //could do automatically, but maybe not good if you're not online
        
        cout << "json file doesn't exist, lets try to download it" << endl;
        getJSON();
        
        
    }else if( json.doesFileExist(json_file_path, false) ){
        
        //else, load saved json
        
        cout << "json file exists, lets load it!!!" << endl;
        std::string file = json_file_path;
        
        // Now parse the JSON
        bool parsingSuccessful = response.open(file);
        
        if (parsingSuccessful) {
            updateJSONDebug();
            updateDDL();
        } else {
            cout  << "Failed to parse JSON" << endl;
        }
    }
}

//--------------------------------------------------------------
void ofApp::downloadVideos(){
    
    for (int i=0; i<response["videos"].size(); i++){
        
        if (response["videos"][i]["tag"].asString() == "Mini") {
            ofFile video;
            string video_url = response["videos"][i]["link"].asString();
            string video_filename = response["videos"][i]["filename"].asString();
            string video_final_path = ofToDataPath(video_filename);
            
            if (!video.doesFileExist(video_final_path)) {
                //[indicator startAnimating];
                drawLoading = true;
                numVideosToGet++;
                fileloader.saveAsync(video_url, video_final_path);
                cout << "downloading video number: " << i  << " url: " << video_url << " final path: " << video_final_path << " num vids to get " << numVideosToGet << endl;
                cout << "draw loading? : " << drawLoading << endl;
            }
        }
    }
    
}

//---

void ofApp::urlResponse(ofHttpResponse & httpResponse) {
    if (httpResponse.status==200) {
        cout << "good response" << endl;
        //[indicator stopAnimating];
        videosDownloaded++;
        if(videosDownloaded == numVideosToGet){
            cout << "we have downloaded all of our videos!" << endl;
            drawLoading = false;
            //[indicator stopAnimating];
            // [indicator removeFromSuperview];
        }
        
        cout << "videos downloaded: " << videosDownloaded  << " num videos to get " << numVideosToGet << endl;
        //img.loadImage(response.data);
        //loading = false;
    } else {
        cout << httpResponse.status << " " << httpResponse.error << endl;
        //if (response.status != -1) loading = false;
    }
    
    
}

//--------------------------------------------------------------
void ofApp::changeVideo(string video){
    string video_path = ofToDataPath(video);
    player.stop();
    player.loadMovie(video_path);
    player.play();
}


//--------------------------------------------------------------
void ofApp::keyReleased(int key){

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
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
