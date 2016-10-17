AmioWavpack - Input/Output Plugin for Adobe Audition
====================================================

This plugin is based on
the amioSDK provided by Adobe. The latest version of the amioSDK and
its EULA are located at the link below, and it is important that
you read the EULA before downloading this project. The WavPack-specific
code, and the resulting plugins, are licensed under the 3-clause BSD.

[Adobe amioSDK EULA And Latest Version](http://www.adobe.com/devnet/audition/AuditionSDKEULA.html)

Note that the SDK comes with a Monkey's Audio plugin sample and the required Monkey's
Audio library, and that this project was based on that sample. However, all Monkey's Audio
references have been deleted and replaced with WavPack in this repository (although some of
the original sample code was not modified). Many thanks to the Audition team for making this
SDK available to me early on and for incorporating some of my suggestions.

## Features

* Support for all Audition audio formats, including the native 32-bit float format
* Support of all the Audition metadata allowed with WAV files (cues, markers, etc.)
* Support for the Audition maximum channel count (32)
* Fast operation

## Coming soon...

* Support for "realtime" mode so you don't have to wait for a file scan every load
* Retention of all APEv2 tag information when editing existing WavPack files
* Support for the Audition maximum file size (no more 2 GB limit)
* Support for all amio channel identities

## Building

This project uses a static copy of libwavpack and builds on Visual Studio 2008 and there
are both 32-bit and 64-bit versions of the project. The 32-bit version is for Audition 4 and 5
(CS5.5 and CS6) and the 64-bit version is for Audition 6 and newer (CC).

## Download

The latest releases of the plugins can be found on [the WavPack downloads page](http://www.wavpack.com/downloads.html).
