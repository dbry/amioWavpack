******************************************************************
 Adobe Audition CC Version File Filter for WavPack     2016-03-28
 Copyright (c) 2016 David Bryant.  All Rights Reserved.
******************************************************************

                   ---- INTRODUCTION ----

This plugin allows you to load and save files in the WavPack format.
It will only work with Adobe Audition versions 6 and 7 (also known as
CC and CC 2014). For earlier versions of Adobe Audition or Cool Edit,
other plugins are available on the WavPack website.

It supports reading both pure lossless and hybrid WavPack files (with
correction files) and all bitdepths, including Audition's native 32-bit
floating point. It supports any number of channels and all sampling
rates. Also, extra information like cue and play lists, artist/title
information, EBU extensions and even bitmaps can be stored and
retrieved in WavPack files.

All WavPack files can be loaded with the filter except "raw" files
created by WavPack versions prior to 4.0 and any "correction" files
are always automatically used if found.

Currently, it is only possible to create lossless WavPack files
(although others can be loaded). Also, ID3v1 or APEv2 tags stored at
the end of WavPack files will be lost when you save them (the metadata
information in Adobe is saved as RIFF data). Also, the plugin is
currently limited to writing 2GB of uncompressed equivalent data (like
WAV files). All of these limitation are targeted for fixing in the next
version of this plugin.


                   ---- INSTALLATION ----

To install the filter, copy (or extract) the file "AmioWavpack.amio"
into the "Plug-ins/Amio" directory starting at the Audition executable,
and then restart Audition. Normally, this would be something like this:

C:\Program Files\Adobe\Adobe Audition CC 2014\Plug-ins\Amio\


                  ---- DIALOG OPTIONS ----

When you save WavPack files you can use the dialog box for selecting the compression mode:

 o Fast      -> This mode provides faster operation with somewhat
             less compression than the other modes.

 o Normal    -> This is the default mode and provides a decent
             compromise between compression ratio and speed.

 o High      -> This mode provides better compression, but is somewhat
             slower than the default mode both in saving and loading
             files.

 o Very High -> This mode provides the highest possible compression
             in WavPack, but is significantly slower than the default
             mode both in saving and loading files and is NOT
             recommended for file to be played on hardware devices.


WavPack and this plugin are free programs; feel free to give them to
anyone who may find them useful. There is no warrantee provided and you
agree to use them completely at your own risk. If you have any questions
or problems please let me know at david@wavpack.com and be sure to visit
www.wavpack.com for the latest versions of WavPack and this plugin.

                     ---- HISTORY ----

Version 1.0 - Initial release
Version 1.1 - Fix crash on invoking compression dialog (Audition CS5.5)
Version 1.2 - Enhancements and fixes to saving of multichannel files (beyond 5.1)
Version 1.3 - Performance improvements from assembly optimizations (WavPack 4.75)
Version 1.4 - Fixes from WavPack 4.75.2 library code (creating corrupt files)
Version 1.5 - Update to WavPack 4.80.0 library code and Amio SDK revision 1.2

Many thanks to the team at Adobe for inviting me to participate!
