Dew VR Player for Oculus Rift
=============================

This is the source code of the app created specifically for [Dew Tour Brooklyn](http://www.dewtour.com/actionsports/event/toyota-city-championships-brooklyn/info), where we
had over 800 people come and try out the [Dew VR Skate Experience](http://www.mountaindew.com/vrskate/). We recorded a stereoscopic,
360 degree video using a 3d printed rig and 12 GoPro cameras. We also recorded 6 channels 
of audio, using two binaural-audio rigs and two direcitonal mics, as well as extra mics
attached to the athletes that were placed in the scene during post production. 

Thanks to the good folks at Mountain Dew for allowing me to put this online. 
Any feedback, questions, comments, etc, feel free to hit me up [@trippedout](http://www.twitter.com/trippedout).

Behind the scenes video here: 

http://www.mountaindew.com/vrskate/

Running/Compiling
-----------------

Open project in Xcode. 

Make sure to edit your build settings to point to your local Cinder repo as well as
proper OculusSDK includes and libraries.

![user defined](http://www.trippedout.net/vrskate/user_defined.png)

![search paths](http://www.trippedout.net/vrskate/search_paths.png)

![linking the library](http://www.trippedout.net/vrskate/linking.png)

Then add the necessary sound and video files for testing. 

You can find those files *coming soon*.

You're video files should live in the /assets/ folder, and audio goes in /assets/audio/. 
The app is expecting a square, top-over-bottom video file, and two audio files (which could technically eventually
be merged into one). The first audio file is a 4 channel north-east-south-west recording, 
the second is your 2-channel up down audio recording.

Build and run as normal.

The app will determine whether or not you have your Oculus attached and render as necessary. 

If it is attached, it will push the app over to the second monitor (your rift) and go fullscreen, 
so you won't necessarily see anything happen on your current screen, but put on the rift and check it out.

Notes
=====

LOTS of comments in the code, and its really not as big of a project as we had originally planned, so give it a read 
through if you have any questions as the answer might lie inside.

I started R&D with a DK1 and OculusSDK version 3.0, and we got our DK2 units about 10 days before the event,
so things were changing rapidly and didn't get all the attention and love they should have. The Oculus SDK wrapper I 
wrote for Cinder is super simple because of this timeline/rapid sdk changes. It sets up the device and really 
only cares about getting the rotational data from the headset for now.

Once the SDK can handle OpenGL Direct To HMD rendering, I plan on continuing the other lib I started writing
which will be much more in depth, and will probably end up being a fork of Cinder itself, rather than just an add-on.

Audio
-----

Again, lots of comments in there, but briefly - this wouldn't have be possible without the excellent work
by the Cinder team and the 0.8.6 release and its new audio APIs. We had previously wrote a custom Max patch
and was throwing the rotational data from this app to it over OSC, which worked, but was not the all-in-one
solution we had hoped for from the beginning. I had attempted re-writing it myself (attempting with FMOD whyyyyy then
Obj-C libs) about a day or two before 0.8.6's release, and am thankful it dropped when it did.

Cross-Platform
--------------

We originally intended to write the app solely for Windows based on the open source project [VR Player](vrplayer.codeplex.com), but its
usage of VLC for decoding proved to be too slow, as jitterying was extremely headache inducing and it wasn't 
the solution we were looking for.

While this project WILL run on windows in its current state (albeit i did not include a VS2013 project, 
using Tinderbox and just copying source over should be easy enough), once we swapped to Cinder and its use of
hardware-accelerated quicktime, windows simply cannot handle the large videos we were using since quicktime's impl
on windows is a joke. Would LOVE to look more into Cinder's usage of DirectX and the new video-acceleration in DX11
for a port that can work proper on all systems.

Previous Attempts
-----------------

Direct To HMD was the main priority for about 3 weeks, with plenty of topics popping up on the oculus dev forums 
as to why some people could get it to work, and some people were failing miserably. As of this writing, OpenGL 
DTH still doesn't work, DirectX seems like it does but I couldn't learn DirectX fast enough to be sure that we could
get it to run on Windows prior to the event. Also attempted to modify Cinder's low level graphics classes to help this 
cause all of which proved completely pointless except for making sure that I didn't sleep for about a week.


This was an excellent, albeit stressful, few weeks, and I really hope this code will benefit someone somewhere.

@trippedout
Sept 30, 2014


