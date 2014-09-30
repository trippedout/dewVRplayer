//
//  FourChannel3DNode.cpp
//  AudioTest
//
//  Created by Anthony Tripaldi on 9/1/14.
//
//
#include "FourChannel3DNode.h"

using namespace ci::app;
using namespace std;

namespace cinder { namespace audio {

    FourChannel3DNode::FourChannel3DNode( const Format &format )
	: Node( format ),mParam( this, 0.f )
    {
    }
    
    void FourChannel3DNode::process(Buffer *buffer)
    {
        if( mParam.eval() ) {
            //this was the primary audio processing area of other examples in the
            //cinder audio code - but for my purposes never fired at all.
            
            console() << "process: " << mParam.getValueArray() << endl;
        }
        else
        {
            //guarantee that we have all 4 channels to work with
            if(buffer->getNumChannels() == 4)
            {
                //get all channels
                float *channel0 = buffer->getChannel( 0 );
                float *channel1 = buffer->getChannel( 1 );
                float *channel2 = buffer->getChannel( 2 );
                float *channel3 = buffer->getChannel( 3 );
                
                //shouldnt vary much, if ever
                size_t numFrames = buffer->getNumFrames();
                
                //make copy - only did this because using the same channels twice was giving
                //me errors on second pass
                float *origChannel0 = new float[numFrames]();
                std::copy(channel0, channel0 + numFrames, origChannel0);
                
                float *origChannel1 = new float[numFrames]();
                std::copy(channel1, channel1 + numFrames, origChannel1);
                
                //get the starting values for your ears
                float leftVal = fmod(mParam.getValue() + .75f, 1.f);
                float rightVal = fmod(mParam.getValue() + .25f, 1.f);
                
                //console() << leftVal << ", " << rightVal << " :: " << endl;
                
                //left ear processing first, pushing results into channel0
                if( leftVal <= 0.25f)
                {
                    float newVal1 = lmap(leftVal, 0.f, 0.25f, 1.f, 0.f);
                    float newVal2 = lmap(leftVal, 0.f, 0.25f, 0.f, 1.f);
                    
                    float* result0 = new float[numFrames]();
                    float* result1 = new float[numFrames]();
                    
                    dsp::mul(channel0, newVal1, result0, numFrames);
                    dsp::mul(channel1, newVal2, result1, numFrames);
                    dsp::add(result0, result1, channel0, numFrames);
                }
                else if( leftVal > 0.25f && leftVal <= .5f)
                {
                    float newVal1 = lmap(leftVal, 0.25f, .5f, 1.f, 0.f);
                    float newVal2 = lmap(leftVal, 0.25f, .5f, 0.f, 1.f);
                    
                    float* result0 = new float[numFrames]();
                    float* result1 = new float[numFrames]();
                    
                    dsp::mul(channel1, newVal1, result0, numFrames);
                    dsp::mul(channel2, newVal2, result1, numFrames);
                    dsp::add(result0, result1, channel0, numFrames);
                }
                else if(leftVal > .5f && leftVal <= .75f)
                {
                    float newVal1 = lmap(leftVal, .5f, .75f, 1.f, 0.f);
                    float newVal2 = lmap(leftVal, .5f, .75f, 0.f, 1.f);
                    
                    float* result0 = new float[numFrames]();
                    float* result1 = new float[numFrames]();
                    
                    dsp::mul(channel2, newVal1, result0, numFrames);
                    dsp::mul(channel3, newVal2, result1, numFrames);
                    dsp::add(result0, result1, channel0, numFrames);
                }
                else
                {
                    float newVal1 = lmap(leftVal, .75f, 1.f, 1.f, 0.f);
                    float newVal2 = lmap(leftVal, .75f, 1.f, 0.f, 1.f);
                    
                    float* result0 = new float[numFrames]();
                    float* result1 = new float[numFrames]();
                    
                    dsp::mul(channel3, newVal1, result0, numFrames);
                    dsp::mul(channel0, newVal2, result1, numFrames);
                    dsp::add(result0, result1, channel0, numFrames);
                }
                
                //then right ear, pushing results into channel1
                if( rightVal <= 0.25f)
                {
                    float newVal1 = lmap(rightVal, 0.f, 0.25f, 1.f, 0.f);
                    float newVal2 = lmap(rightVal, 0.f, 0.25f, 0.f, 1.f);
                    
                    float* result0 = new float[numFrames]();
                    float* result1 = new float[numFrames]();
                    
                    dsp::mul(origChannel0, newVal1, result0, numFrames);
                    dsp::mul(channel1, newVal2, result1, numFrames);
                    dsp::add(result0, result1, channel1, numFrames);
                }
                else if( rightVal > 0.25f && rightVal <= .5f)
                {
                    float newVal1 = lmap(rightVal, 0.25f, .5f, 1.f, 0.f);
                    float newVal2 = lmap(rightVal, 0.25f, .5f, 0.f, 1.f);
                    
                    float* result0 = new float[numFrames]();
                    float* result1 = new float[numFrames]();
                    
                    dsp::mul(channel1, newVal1, result0, numFrames);
                    dsp::mul(channel2, newVal2, result1, numFrames);
                    dsp::add(result0, result1, channel1, numFrames);
                }
                else if(rightVal > .5f && rightVal <= .75f)
                {
                    float newVal1 = lmap(rightVal, .5f, .75f, 1.f, 0.f);
                    float newVal2 = lmap(rightVal, .5f, .75f, 0.f, 1.f);
                    
                    float* result0 = new float[numFrames]();
                    float* result1 = new float[numFrames]();
                    
                    dsp::mul(channel2, newVal1, result0, numFrames);
                    dsp::mul(channel3, newVal2, result1, numFrames);
                    dsp::add(result0, result1, channel1, numFrames);
                }
                else
                {
                    float newVal1 = lmap(rightVal, .75f, 1.f, 1.f, 0.f);
                    float newVal2 = lmap(rightVal, .75f, 1.f, 0.f, 1.f);
                    
                    float* result0 = new float[numFrames]();
                    float* result1 = new float[numFrames]();
                    
                    dsp::mul(channel3, newVal1, result0, numFrames);
                    dsp::mul(origChannel0, newVal2, result1, numFrames);
                    dsp::add(result0, result1, channel1, numFrames);
                }
            }
            
        }
        
    };

} } //cinder::audio