#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Fbo.h"
#include "cinder/qtime/QuickTime.h"
#include "cinder/TriMesh.h"
#include "cinder/Arcball.h"
#include "cinder/Camera.h"

#include "cinder/audio/Context.h"
#include "cinder/audio/Buffer.h"
#include "cinder/audio/Source.h"
#include "cinder/audio/NodeEffects.h"
#include "cinder/audio/SamplePlayerNode.h"

#include "FourChannel3DNode.h"
#include "TwoChannelUpDownNode.h"

#include "OculusRift.h"

using namespace ci;
using namespace ci::app;
using namespace cinder::ovr;
using namespace std;

class DewOculusPlayerApp : public AppNative {
public:
    void prepareSettings(Settings* settings);
	void setup();
    void setArgs();
    void setupRift();
    void setup3d();
    void setupCamera();
    void setupAudio();
    void setupMovie();
    void keyDown(KeyEvent event);
	void update();
	void draw();
    
    void shutdown();
    
protected:
    string              mMovieUrl;
    
private:
    bool                mPlayOnHeadset = true;
    bool                mUseBarrelDistortion = true;
    
    OculusRiftRef       mOculusRift;
    
    qtime::MovieGlRef   mVideo;
    
    //3d
    gl::GlslProgRef     mShaderProg;
    gl::GlslProgRef     mBarrelProg;
    gl::Fbo             mLeftFbo, mRightFbo;
    float               mBarrelDistortionAmt;
    
    //camera
    CameraPersp         mCamera;
    Area                mEyeViewport;
    
    Vec2i               mVideoSize;
    
    //audio
    audio::BufferPlayerNodeRef      mNeswBufferPlayerNode, mSkyGroundBufferNode;
    audio::FourChannel3DNodeRef     mFourChannelNode;
//    audio::TwoChannelUpDownNodeRef  mTwoChannelNode;
    audio::GainNodeRef              mGainNode;
};

void DewOculusPlayerApp::prepareSettings(Settings* settings)
{
    vector<DisplayRef> displays = Display::getDisplays();
    console() << "num displays: " << displays.size() << endl;
    
    setArgs();
    
    if(displays.size() > 1 && mPlayOnHeadset)
    {
        //after many attempts, throughout multiple iterations of the OculusSDK, to get
        //non-jittery video, one of the major "breakthroughs" was the next line. Allowing
        //the system to just run the render/draw calls as fast as possible, as well as
        //using the quicktime gpu-accelerated video decoding built into cinder gave us smooth
        //75+fps rendering of camera movements while decoding our main video at a proper 48fps.
        //
        settings->disableFrameRate();
        //
        //after all was said and done tho, i still feel modifying the actual render code of cinder will
        //be necessary once the OculusSDK can properly handle OpenGL DirectToHMD rendering. This
        //is a nice workaround in the meantime while using Extended mode and pushing it straight
        
        settings->setDisplay(displays[1]);
        settings->setFullScreen();
    }
    else
    {
        setFrameRate(60);
    }
}

void DewOculusPlayerApp::setup()
{
    setWindowSize(1920, 1080); //will only work for non-device
    
    setupRift();
    setup3d();
    setupCamera();
    setupAudio();
    setupMovie();
}

void DewOculusPlayerApp::setArgs()
{
    //defaults
    mMovieUrl = "iceland.mp4";
    
    vector<string> args = getArgs();
    for(int i = 0; i < args.size(); i++)
    {
        if(args[i] == "-m")
            mMovieUrl = args[i+1];
        
        if(args[i] == "--onscreen")
            mPlayOnHeadset = false;
    }
}

void DewOculusPlayerApp::setupRift()
{
    //welp, not much more to do here i guess
    mOculusRift = OculusRift::create();
}

void DewOculusPlayerApp::setup3d()
{
    try {
        mShaderProg = gl::GlslProg::create(loadAsset("shaders/movie.vert"), loadAsset("shaders/movie.frag"));
        mBarrelProg = gl::GlslProg::create(loadAsset("shaders/barrel.vert"), loadAsset("shaders/barrel.frag"));
    }
    catch(gl::GlslProgCompileExc e) {
        console() << e.what() << endl;
    }
    
    //create two fbos - one for each eye, so we can independently distort each of them with barrel
    gl::Fbo::Format format;
    format.setTarget(GL_TEXTURE_RECTANGLE_ARB); //fucking quicktime - wouldnt work without rect_arb
    mLeftFbo = gl::Fbo(getWindowWidth() * 0.5f, getWindowHeight(), format);
    mRightFbo = gl::Fbo(getWindowWidth() * 0.5f, getWindowHeight(), format);
    
    //best setting for our video after testing - experiment with this
    mBarrelDistortionAmt = 1.57216f;
}

void DewOculusPlayerApp::setupCamera()
{
    mCamera = CameraPersp( getWindowWidth() * .5f, getWindowHeight(), 100.0f, 0.0001f, 20000.0f );
    mCamera.setWorldUp(Vec3f(0,1.f,0));
	mCamera.lookAt( Vec3f::zero(), Vec3f( 0.0f, 0.0f, 5.0f ));
    
    mEyeViewport = Area(0, 0, getWindowWidth() * .5f, getWindowHeight());
}

void DewOculusPlayerApp::setupAudio()
{
    //the late addition of the new audio api in our 6 week development cycle proved to be a
    //god send. Prior to this, we were hooking our headset quaternions into a separate Max patch
    //through OSC. Not ideal when you want to package everything as one app for the masses.
    
    auto ctx = audio::Context::master();
    
    //gainNode for video volume control (overall)
    mGainNode = ctx->makeNode(new audio::GainNode());
    
    //nesw
	audio::SourceFileRef sourceFile = audio::load( loadAsset("iceland.wav"), ctx->getSampleRate() );
    
    audio::BufferRef neswBuffer = sourceFile->loadBuffer();
	mNeswBufferPlayerNode = ctx->makeNode( new audio::BufferPlayerNode( neswBuffer ) );
    
    audio::Node::Format neswFormat;
    neswFormat.setChannels(4);
    mFourChannelNode = ctx->makeNode(new audio::FourChannel3DNode( neswFormat ));
	mNeswBufferPlayerNode >> mFourChannelNode >> mGainNode >> ctx->getOutput();
    
    
    //updown
//    audio::SourceFileRef skyGroundSource = audio::load( loadAsset("audio/skyground.aiff"), ctx->getSampleRate() );
//    
//    audio::BufferRef skyBuffer = skyGroundSource->loadBuffer();
//    mSkyGroundBufferNode = ctx->makeNode(new audio::BufferPlayerNode(skyBuffer));
//    
//    audio::Node::Format skyFormat;
//    skyFormat.setChannels(2);
//    mTwoChannelNode = ctx->makeNode(new audio::TwoChannelUpDownNode( skyFormat ));
//    
//    mSkyGroundBufferNode >> mTwoChannelNode >> mGainNode >> ctx->getOutput();
//    
    //just a precaution to pre-load buffer like with qtime
    mNeswBufferPlayerNode->start();
//    mSkyGroundBufferNode->start();
    
    mNeswBufferPlayerNode->stop();
//    mSkyGroundBufferNode->stop();
    
    ctx->enable();
}

void DewOculusPlayerApp::setupMovie()
{
    //something ive had issues with in the past - large movies playing the first time through
    //stutter a bit here and there, but once it runs once, replays are fine
    
    mVideo = qtime::MovieGl::create(loadAsset(mMovieUrl));
    mVideoSize = Vec2i(mVideo->getWidth(), mVideo->getHeight());
    mVideo->play(); //play stop loads into memory usually, but we should let it run once
    mVideo->stop(); //first time just so we guarantee it
}

void DewOculusPlayerApp::keyDown(KeyEvent event)
{
    //change the fov
    if(event.getChar() == '+')
    {
        mCamera.setFov(mCamera.getFov() + 1);
        //console() << "fov: " << mCamera.getFov() << endl;
    }
    else if (event.getChar() == '-')
    {
        mCamera.setFov(mCamera.getFov() - 1);
        //console() << "fov: " << mCamera.getFov() << endl;
    }
    //start/restart the show!
    else if (event.getCode() == KeyEvent::KEY_RETURN)
    {
        //BUG - occasionally, probably due to aformentioned video and audio isses,
        //starting the experience would push audio and video slighty off time.
        //during the experience i would usually just hit escape twice, then return twice
        //and things played fine, it was something to do with just seeking and restarting once
        //like the buffer wouldnt fully clear or it would jump a bit, not sure.
        
        mNeswBufferPlayerNode->seekToTime(0);
//        mSkyGroundBufferNode->seekToTime(0);
        mVideo->seekToStart();
        
        if(!mVideo->isPlaying())
        {
            mNeswBufferPlayerNode->start();
//            mSkyGroundBufferNode->start();
            mVideo->play();
        }
    }
    // stop and move to first frame
    else if (event.getCode() == KeyEvent::KEY_ESCAPE)
    {
        mNeswBufferPlayerNode->stop();
//        mSkyGroundBufferNode->stop();
        
        mVideo->stop();
        mVideo->seekToStart();
    }
    //control volume of our built-in buffer, off or on, so if we need to switch to
    //maxpatch OSC we can do it on the fly
    else if(event.getChar() == 'v')
    {
        if(!mGainNode->getValue())
            mGainNode->setValue(1.f);
        else
            mGainNode->setValue(0.f);
    }
    //change barrel distortion amt
    else if(event.getChar() == '[')
    {
        mBarrelDistortionAmt -= 0.005;
        //console() << "barrelDistortion: " << mBarrelDistortionAmt << endl;
    }
    else if(event.getChar() == ']')//base 1.657155f;
    {
        mBarrelDistortionAmt += 0.005;
        //console() << "barrelDistortion: " << mBarrelDistortionAmt << endl;
    }
    
    mOculusRift->keyDown(event);
}

void DewOculusPlayerApp::update()
{
    mOculusRift->update();
    
    //rotate our camera to match the users head movement
    mCamera.setOrientation(mOculusRift->getCurrentHeadPose());
    
    //values for channel mixers
    float rotation = toDegrees(atan2(mCamera.getViewDirection().x, mCamera.getViewDirection().z)- M_PI);
    float upDown = mCamera.getViewDirection().y;
    
    //remap for channel mixer
    //TODO bother with double remap? or is this 100% necessary?
    //change volume code to deal with original values maybe?
    if(rotation > 0)
        rotation = lmap(rotation, 0.f, 180.f, -360.f, -180.f);
    rotation = lmap(rotation, 0.f, -360.f, 0.f, 1.f);
    
    //update audio channels
    mFourChannelNode->setValue(rotation);
//    mTwoChannelNode->setValue(upDown);
}

void DewOculusPlayerApp::draw()
{
    gl::setViewport( mEyeViewport );
    gl::setMatrices( mCamera );
    gl::clear( ColorAf::black() );
    
    gl::pushMatrices();
    {
        if(mVideo->getTexture())
        {
            try {
                gl::rotate(Vec3f(0.,-90., 0.));
                
                //TODO - make only one draw call and use vertex shader to
                //handle the spherical distortion maybe...
                
                //left eye
                mShaderProg->bind();
                mShaderProg->uniform("tex0", 0);
                mShaderProg->uniform("videoSize", mVideoSize);
                mShaderProg->uniform("isLeftEye", true);
                mVideo->getTexture().enableAndBind();
                
                //draw left eye into fbo
                mLeftFbo.bindFramebuffer();
                {
                    gl::drawSphere(Vec3f(0,0,0), 10.f, 64);
                }
                mLeftFbo.unbindFramebuffer();
                
                mVideo->getTexture().unbind();
                mShaderProg->unbind(); //do i need to unbind/rebind for second eye?
                
                //right eye
                mShaderProg->bind();
                mShaderProg->uniform("tex0", 0);
                mShaderProg->uniform("videoSize", mVideoSize);
                mShaderProg->uniform("isLeftEye", false);
                mVideo->getTexture().enableAndBind();
                
                //draw right eye into fbo
                mRightFbo.bindFramebuffer();
                {
                    gl::drawSphere(Vec3f(0,0,0), 10.f, 64);
                }
                mRightFbo.unbindFramebuffer();
                
                mVideo->getTexture().unbind();
                mShaderProg->unbind();
                
            }
            catch(std::exception e) {
                console() << e.what() << endl;
            }
            
        }
    }
    gl::popMatrices();
    
    //reset matrices to window for 2d drawing
    gl::setMatricesWindow(getWindowSize());
    gl::setViewport(getWindowBounds());
    gl::clear( ColorAf::black() );
    
    if(mUseBarrelDistortion)
    {
        mBarrelProg->bind();
        mBarrelProg->uniform("factor", mBarrelDistortionAmt);
    }
    
    gl::draw(mLeftFbo.getTexture());
    gl::translate(Vec2f(getWindowWidth()/2, 0));
    gl::draw(mRightFbo.getTexture());
    
    if(mUseBarrelDistortion) mBarrelProg->unbind();
}

void DewOculusPlayerApp::shutdown()
{
    mOculusRift->shutdown();
}

CINDER_APP_NATIVE( DewOculusPlayerApp, RendererGl )
