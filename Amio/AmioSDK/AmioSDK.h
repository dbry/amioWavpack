/**************************************************************************
// ADOBE SYSTEMS INCORPORATED
// Copyright  2008 - 2011 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this 
// file in accordance with the terms of the Adobe license agreement
// accompanying it.  If you have received this file from a source
// other than Adobe, then your use, modification, or distribution
// of it requires the prior written permission of Adobe.
**************************************************************************/

#ifndef AMIO_AMIOSDK_H
#define AMIO_AMIOSDK_H

#include "AmioSDKTypes.h"

namespace amio
{
	typedef asdk::int32 AmioResult;

	// This is the type of the single interface function to an amio plug-in.
	typedef AmioResult (*AmioInterfaceFunc)(asdk::int32 inCommand, void* inState, void* inInterface);

	// These are the commands passed into the interface function as inCommand.
	enum AmioInterfaceCommand
	{
		kAmioInterfaceCommand_GetAmioInfo				= 0,	// kAmioInterfaceCommand_GetAmioInfo, The first call made to a plug-in module
																// NULL, 
																// AmioGetAmioInfoInterface.

		kAmioInterfaceCommand_Initialize				= 1,	// kAmioInterfaceCommand_Initialize, The second call made to a plug-in module
																// NULL, 
																// NULL.

		kAmioInterfaceCommand_Terminate					= 2,	// kAmioInterfaceCommand_Terminate, The last call made to a plug-in module
																// NULL, 
																// NULL.

		kAmioInterfaceCommand_PreOpen					= 3,	// kAmioInterfaceCommand_PreOpen, Determine if the current file needs a dialog before opening.
																// NULL
																// AmioPreOpenInterface.

		kAmioInterfaceCommand_RunImportSettingsDialog	= 4,	// kAmioInterfaceCommand_RunImportSettingsDialog, Display the import settings dialog.
																// NULL
																// AmioRunImportSettingsDialogInterface.

		kAmioInterfaceCommand_Open						= 5,	// kAmioInterfaceCommand_Open, Opens a file for reading.
																// NULL, 
																// AmioOpenInterface.

		kAmioInterfaceCommand_GetFormat					= 6,	// kAmioInterfaceCommand_GetFormat, Gets the format of a file open for reading.
																// the plug-in's state object set via AmioOpenInterface::SetSessionState(),
																// AmioFormatInterface.

		kAmioInterfaceCommand_ReadSamples				= 7,	// kAmioInterfaceCommand_ReadSamples, Reads samples from a file open for reading.
																// the plug-in's state object set via AmioOpenInterface::SetSessionState(),
																// AmioReadSamplesInterface.

		kAmioInterfaceCommand_ReadSamplesRaw			= 8,	// kAmioInterfaceCommand_ReadSamplesRaw, Reads samples from a file open for reading.
																// the plug-in's state object set via AmioOpenInterface::SetSessionState(),
																// AmioReadSamplesRawInterface.

		kAmioInterfaceCommand_ReadVideoFrame			= 9,	// kAmioInterfaceCommand_ReadVideoFrame, Reads a frame of video.
																// the plug-in's state object set via AmioOpenInterface::SetSessionState(),
																// AmioReadVideoFrameInterface.

		kAmioInterfaceCommand_ReadMetadata				= 10,	// kAmioInterfaceCommand_ReadMetadata, Read an item of metadata from a file open for reading.
																// the plug-in's state object returned by the Open command.
																// AmioReadMetadataInterface.

		kAmioInterfaceCommand_DoneWithAudioRead			= 11,	// kAmioInterfaceCommand_DoneWithAudioRead, All done reading audio (but maybe not video)
																// the plug-in's state object returned by the Open command.
																// NULL.

		kAmioInterfaceCommand_Close						= 12,	// kAmioInterfaceCommand_Close, Close a file open for reading.
																// the plug-in's state object returned by the Open command.
																// NULL.

		kAmioInterfaceCommand_GetDefaultExportSettings	= 13,	// kAmioInterfaceCommand_GetDefaultExportSettings, Gets the factory default supported format and settings for writing files.
																// NULL
																// AmioFormatInterface.

		kAmioInterfaceCommand_GetExportSettingsInfo		= 14,	// kAmioInterfaceCommand_GetExportSettingsInfo, Negotiate proposed settings before creating a new file for writing.
																// NULL
																// AmioGetExportSettingsInfoInterface.

		kAmioInterfaceCommand_RunExportSettingsDialog	= 15,	// kAmioInterfaceCommand_RunExportSettingsDialog, Display the export settings dialog.
																// NULL
																// AmioRunExportSettingsDialogInterface.

		kAmioInterfaceCommand_WriteStart				= 17,	// kAmioInterfaceCommand_WriteStart, Create a file for writing.
																// NULL,
																// AmioWriteStartInterface

		kAmioInterfaceCommand_WriteSamples				= 18,	// kAmioInterfaceCommand_WriteSamples,
																// the plug-in's state object set via AmioWriteStartInterface::SetSessionState(),
																// AmioWriteSamplesInterface.

		kAmioInterfaceCommand_WriteFinish				= 19,	// kAmioInterfaceCommand_WriteFinish, Finish and close the file being written.
																// the plug-in's state object set via AmioWriteStartInterface::SetSessionState(),
																// AmioWriteFinishInterface.

	};

	// an AmioResult can be one of these values, or some other value of the plug-in's own choosing.
	enum AmioInterfaceReturnCode
	{
		kAmioInterfaceReturnCode_Success				= 0,

		kAmioInterfaceReturnCode_GeneralError			= -1,	
		kAmioInterfaceReturnCode_UnknownCommand			= -2,	// The plug-in does not know about the given command.
		kAmioInterfaceReturnCode_UnsupportedCommand		= -3,	// This command is not supported by this plug-in.

		kAmioInterfaceReturnCode_CorruptOrUnsupported	= -4,	// The file is either corrupt or using an unsupported format.
		kAmioInterfaceReturnCode_NoSuchItem				= -5,	// A request was given for an unknown item. (e.g. via kAmioInterfaceCommand_ReadMetadata)
		kAmioInterfaceReturnCode_ParameterOutOfRange	= -6,	// A size or other parameter is beyond a valid range.
		kAmioInterfaceReturnCode_PlugInException		= -7,	// The plug-in generated an exception.

		kAmioInterfaceReturnCode_UnsupportedFormat		= -8,	// The given format is not supported.

		kAmioInterfaceReturnCode_DecoderError			= -9,	// There was an error during the decode process.
		kAmioInterfaceReturnCode_EncoderError			= -10,	// There was an error during the encode process.	
	};


	// Some interface definitions


	// Used for commands for which extended error information needs to be returned.
	class AmioErrorInterface
	{
	public:
		virtual ~AmioErrorInterface(){}

		//////////////////////////////////////////////////////////////////////////////
		// Set a human-readable error string using a UTF16 string.
		virtual void SetErrorString(const asdk::uint16* inString) = 0;

		//////////////////////////////////////////////////////////////////////////////
		// Get the human-readable error string in UTF16 format.
		virtual const asdk::uint16* GetErrorString() const = 0;

		//////////////////////////////////////////////////////////////////////////////
		// Find out if an error string has been set.
		virtual bool HasErrorString() const = 0;

		//////////////////////////////////////////////////////////////////////////////
		// Set a human-readable warning string using a UTF16 string.
		virtual void AddWarning(const asdk::uint16* inWarning) = 0;

		//////////////////////////////////////////////////////////////////////////////
		// Get a previously added human-readable warning string in UTF16 format.
		// The warning index should be from 0 to GetWarningCount() - 1.
		virtual const asdk::uint16* GetWarningText(asdk::int32 inWarningIndex) const = 0;

		//////////////////////////////////////////////////////////////////////////////
		// Gets the count of warnings that have been added.
		virtual asdk::int32 GetWarningCount() const = 0;

		//////////////////////////////////////////////////////////////////////////////
		// Clear all previously added warnings.
		virtual void ClearWarnings() = 0;
	};


	enum AmioFileFlag
	{
		kAmioFileFlag_None								= 0,

		kAmioFileFlag_ReadSamplesRaw					= 0x00000001,	// Reading of the file is done only through the AmioReadSamplesRawInterface.
																		// Data requested is a byte offset into the uncompressed sequence of
																		// interleaved channels.  Therefore, this reading method can request
																		// data this is not aligned to an interleaved block, or even sample boundaries.
																		// The amount of data available must be the number of channels *
																		// the sample total * the number of bytes per sample.
																		// If real-time playback is to be supported from the plug-in, it must 
																		// provide data through this interface.
																		
		kAmioFileFlag_RealTimeSupport					= 0x00000002,	// This file can be played in real-time.  That means:
																		// 1) The interface is through AmioReadSamplesRawInterface (i.e. the 
																		//    kAmioFileFlag_ReadSamplesRaw flag is set).
																		// 2) Seeking to arbitrary or random blocks of data is possible 																		
																		//    without (appreciable) delay.
																		// 3) The processing of the samples can be done using minimal
																		//    CPU processing.
																		// If this flag is not set, the host will read and cache all
																		// audio data, and use its internal cache for real-time playback.
																		// A plug-in may choose to use the ReadSamplesRaw interface without 
																		// supporting real-time performance.  
																		// When in doubt, DO NOT set this flag.

		kAmioFileFlag_SampleTotalInexact				= 0x00000004,	// When opening a file for reading, not enough information is known
																		// to determine an exact sample count.  The count initially returned 
																		// via AmioFormatInterface is an estimate.  The host should 
																		// call AmioReadSamplesInterface until an end of file condition is encountered
																		// at which point the exact total sample count will be known.
																		// This flag cannot be used in conjunction with kAmioFileFlag_ReadSamplesRaw.

		kAmioFileFlag_XmpSupportThroughPluginOnly		= 0x00000008,	// This flag alerts the host that the plug-in wants to override all XMP metadata
																		// support.  This flag would be set only if the Adobe XMP handler does not
																		// support the plug-in's file type, or does not acceptably reconcile XMP 
																		// metadata with native metadata stored in the file.  
																		// If XMP does not natively support the media type, pass kXMP_UnknownFile into the
																		// SetXmpHandlerId method of the AmioFormatInterface.
																		// See additional comments with kAmioFileFlag_WriteXmpMetadataBeforeSamples

		kAmioFileFlag_WriteXmpMetadataBeforeSamples		= 0x00000010,	// When kAmioFileFlag_XmpSupportThroughPluginOnly is NOT set, this flag alerts the
																		// host that it should attempt to use its XMP subsystem to write XMP data before 
																		// supplying any samples.
																		// Otherwise if XMP is written after all the samples, all the sample data may
																		// have to be shifted because the XMP data is written to the start of the file.
																		// When kAmioFileFlag_XmpSupportThroughPluginOnly IS set, you would expect
																		// this flag to have no effect, but at this writing, a quirk 
																		// in Audition means that XMP metadata will not be provided to the plug-in
																		// at all unless this flag is set.

		kAmioFileFlag_SupportsVideoScaling				= 0x00000020,	// When this flag is set, the application can expect to read video at any arbitrary scaling.
	};

	const asdk::int32 kAmioInterfaceVersionMajor = 1;					// Pass this value in AmioGetAmioInfoInterface::SetInterfaceVersion()
	const asdk::int32 kAmioInterfaceVersionMinor = 1;					// Pass this value in AmioGetAmioInfoInterface::SetInterfaceVersion()
																		// 1.0 was the first release with Audition CS5.5, that is Audition 4.0
																		// 1.1 is a version with some extended interfaces but should be compatible with 1.0 plug-ins.

	// Used for kAmioInterfaceCommand_GetAmioInfo, Getting information about this plug-in.
	class AmioGetAmioInfoInterface
	{
	public:
		virtual ~AmioGetAmioInfoInterface(){}

		//////////////////////////////////////////////////////////////////////////////
		// Set the version of this interface used by this plug-in.
		// Pass kAmioInterfaceVersionMajor and kAmioInterfaceVersionMinor so the host knows which version of the
		// amio interface this plug-in is using.
		virtual void SetPluginInterfaceVersion(asdk::int32 inAmioInterfaceVersionMajor, asdk::int32 inAmioInterfaceVersionMinor) = 0;

		//////////////////////////////////////////////////////////////////////////////
		// Get the version of the amio interface used by the host.
		virtual void GetHostInterfaceVersion(asdk::int32& outAmioInterfaceVersionMajor, asdk::int32& outAmioInterfaceVersionMinor) const = 0;

		//////////////////////////////////////////////////////////////////////////////
		// Set a unique GUID string, formatted like "3D3B5917-EA4E-463E-B79A-1F6F1CED323E" to identify this plug-in,
		// And give a description of the overall plug-in.  (Individual format descriptions are set using AddInputFormat and AddOutputFormat.)
		// PLEASE, make the ID unique.  
		virtual void SetPluginID(const char* inAmioPluginID, const asdk::uint16* inPluginDescription) = 0;

		//////////////////////////////////////////////////////////////////////////////
		// Set the description and unique ID of a format supported for reading, with its file extensions such as "wav", "aiff", etc.
		// If the list of extensions is long, use inExtensionListUiOverride to provide a description used in the file UI.
		// Call this for each input format supported.
		// Use inFormatID if the plug-in supports multiple different formats
		virtual void AddInputFormat(const asdk::uint16* inFormatDescription, 
				const asdk::uint16* inFormatID,
				const ExtensionList& inExtensions,
				const asdk::uint16* inExtensionListUiOverride = NULL) = 0;

		//////////////////////////////////////////////////////////////////////////////
		// Set the description and unique ID of a format supported for writing, with its file extensions such as "wav", "aiff", etc.
		// If the list of extensions is long, use inExtensionListUiOverride to provide a description used in the file UI.
		// Call this for each output format supported.
		// Use inFormatID if the plug-in supports multiple different formats
		virtual void AddOutputFormat(const asdk::uint16* inFormatDescription,
				const asdk::uint16* inFormatID,
				const ExtensionList& inExtensions,
				const asdk::uint16* inExtensionListUiOverride = NULL) = 0;
	};

	
	//////////////////////////////////////////////////////////////////////////////
	// An interface that specifies video format attributes
	class AmioVideoFormatInterface
	{
	public:
		virtual ~AmioVideoFormatInterface(){}

		//////////////////////////////////////////////////////////////////////////////
		/// Get / Set the number of video streams.  The inStreamIndex parameter on all methods
		/// ranges from 0 to GetStreamCount()-1.  Behavior is undefined for other values.
		virtual asdk::int32 GetStreamCount() const = 0;
		virtual void SetStreamCount(asdk::int32 inStreamCount) = 0;

		//////////////////////////////////////////////////////////////////////////////
		/// Get / Set the frame count for the given stream of video.
		virtual asdk::int32 GetFrameCount(asdk::int32 inStreamIndex) const = 0;
		virtual void SetFrameCount(asdk::int32 inStreamIndex, asdk::int32 inFrameCount) = 0;

		//////////////////////////////////////////////////////////////////////////////
		/// Get / Set the pixel type of the video.  The value represents one of the VideoPixelType values.
		virtual VideoPixelType GetPixelType(asdk::int32 inStreamIndex) const = 0;
		virtual void SetPixelType(asdk::int32 inStreamIndex, VideoPixelType inPixelType) = 0;

		//////////////////////////////////////////////////////////////////////////////
		/// Get / Set the frame rate of the video in numerator / denominator frames per second.
		/// For example, 30000 / 1001 for 29.97 fps.
		virtual void GetFrameRate(asdk::int32 inStreamIndex, asdk::int64& outNumerator, asdk::int64& outDenominator) const = 0;
		virtual void SetFrameRate(asdk::int32 inStreamIndex, asdk::int64 inNumerator, asdk::int64 inDenominator) = 0;

		//////////////////////////////////////////////////////////////////////////////
		/// Get / Set the frame rate of the video in a scalar double.
		/// Do not use this interface if it is possible to specify the rate exactly a ratio.
		virtual double GetFrameRate(asdk::int32 inStreamIndex) const = 0;
		virtual void SetFrameRate(asdk::int32 inStreamIndex, double inFrameRate) = 0;

		//////////////////////////////////////////////////////////////////////////////
		/// Get / Set width and height of the video frame in pixels.
		virtual asdk::int32 GetWidth(asdk::int32 inStreamIndex) const = 0;
		virtual void SetWidth(asdk::int32 inStreamIndex, asdk::int32 inWidth) = 0;
		virtual asdk::int32 GetHeight(asdk::int32 inStreamIndex) const = 0;
		virtual void SetHeight(asdk::int32 inStreamIndex, asdk::int32 inHeight) = 0;

		//////////////////////////////////////////////////////////////////////////////
		/// Get / Set pixel aspect ratio.
		virtual double GetPixelAspectRatio(asdk::int32 inStreamIndex) const = 0;
		virtual void SetPixelAspectRatio(asdk::int32 inStreamIndex, double inPixelAspectRatio) = 0;

		//////////////////////////////////////////////////////////////////////////////
		/// Get / Set a UTF16 string representing the video format of the given stream of the file
		virtual const asdk::uint16* GetFormatDescriptionString(asdk::int32 inStreamIndex) const = 0;
		virtual void SetFormatDescriptionString(asdk::int32 inStreamIndex, const asdk::uint16* inString) = 0;
	};			


	// These GUID strings, formatted like "3D3B5917-EA4E-463E-B79A-1F6F1CED323E" are for 
	// specific types of metadata recognized by the host.
	// If you want to pass other types of metadata through the host, use your own GUID(s).
	extern const char* kAmioMetadataTypeID_XMP;							// Adobe Extensible Metadata Platform (XMP) metadata
	extern const char* kAmioMetadataTypeID_BWF_bext;					// Broadcast Wave Format Audio Extension chunk including the chunk ID and size.
	extern const char* kAmioMetadataTypeID_GenericWaveMetadataChunk;	// Generic .wav file riff chunks.
	extern const char* kAmioMetadataTypeID_GenericAiffMetadataChunk;	// Generic .aiff / .aifc file chunks. 

	// Used for kAmioInterfaceCommand_GetFormat, to provide the format of the open file,
	// and by other commands to read the format of a contemplated, or newly created file.
	class AmioFormatInterface
	{
	public:
		virtual ~AmioFormatInterface(){}

		//////////////////////////////////////////////////////////////////////////////
		// Get an interface for setting extended error information.
		virtual AmioErrorInterface& GetErrorInterface() = 0;

		//////////////////////////////////////////////////////////////////////////////
		/// Get / Set the sample rate of the audio in samples per second.
		/// All channels must have the same sample rate.
		virtual AudioSampleRate GetSampleRate() const = 0;
		virtual void SetSampleRate(AudioSampleRate inSampleRate) = 0;

		//////////////////////////////////////////////////////////////////////////////
		/// Get / Set the sample type of the audio.  The value represents one of the kAudioSampleType values.
		/// All channels must have the same sample type.
		virtual AudioSampleType GetSampleType() const = 0;
		virtual void SetSampleType(AudioSampleType inSampleType) = 0;

		//////////////////////////////////////////////////////////////////////////////
		/// Get / Set the channel count.  The channel count should be set prior to setting any speaker assignments.
		virtual AudioChannelCount GetChannelCount() const = 0;
		virtual void SetChannelCount(AudioChannelCount inChannelCount) = 0;

		//////////////////////////////////////////////////////////////////////////////
		/// Get / Set the speaker assignment for the given channel.  The value 
		/// represents one of the kAudioChannelLabel values.  The channel count 
		/// must be set prior to setting the speaker assignments.  Out of range, or 
		/// uninitialized values will return kAudioChannelLabel_Discrete.
		/// @param inChannelIndex	The channel index from 0 to GetChannelCount()-1.
		virtual AudioChannelLabel GetSpeakerAssignment(AudioChannelIndex inChannelIndex) const = 0;
		virtual void SetSpeakerAssignment(AudioChannelIndex inChannelIndex, AudioChannelLabel inSpeakerAssignment) = 0;

		//////////////////////////////////////////////////////////////////////////////
		/// Get an interface to set video format information
		virtual AmioVideoFormatInterface& GetVideoFormatInterface() = 0;

		//////////////////////////////////////////////////////////////////////////////
		// For AmioInterfaceVersion 1.1, renamed from SetFileTypeFormatDescription / GetFileTypeFormatDescription
		// If multiple formats were registered with AddInputFormat and/or AddOutputFormat with different FormatIDs, 
		// get or set the FormatID that corresponds to the format this interface is describing.
		virtual void SetFormatID(const asdk::uint16* inFormatID) = 0; 
		virtual const asdk::uint16* GetFormatID() const = 0;

		//////////////////////////////////////////////////////////////////////////////
		/// Get the serialized private settings data.  This data is in the form of a null terminated UTF16 string.
		virtual const asdk::uint16* GetPrivateFormatData() const = 0;

		//////////////////////////////////////////////////////////////////////////////
		/// Set the serialized private settings data.  This data must be in the form of a null terminated UTF16 string.
		virtual void SetPrivateFormatData(const asdk::uint16* inSettings) = 0;

		//////////////////////////////////////////////////////////////////////////////
		/// Set / Get the mime type of the media file.
		virtual void SetMimeType(const char* inMimeType) = 0;
		virtual const char* GetMimeType() const = 0;

		//////////////////////////////////////////////////////////////////////////////
		/// Set / Get the ID of the XMP handler that can be used to open this file, or kXMP_UnknownFile
		virtual void SetXmpHandlerId(asdk::uint32 inXmpHandlerId) = 0;
		virtual asdk::uint32 GetXmpHandlerId() const = 0;

		//////////////////////////////////////////////////////////////////////////////
		// Get / Set flags, a bitwise OR combination of AmioFileFlag values.
		virtual void SetFileFlags(asdk::uint32 inFlags) = 0;
		virtual asdk::uint32 GetFileFlags() const = 0;

		//////////////////////////////////////////////////////////////////////////////
		// When providing information about metadata, call this method one time for each metadata object in the file in order to report its characteristics.
		// The app can read this data via kAmioInterfaceCommand_ReadMetadata.
		// inInstanceID - An identifier used by the plug-in to identify the particular instance of metadata described.
		//                Metadata will be read from the plug-in by referencing this instance ID.
		// inAmioMetadataTypeID - One of the kAmioMetadataTypeID_ defines, or a plug-in defined GUID string to identify the type of the metadata item.
		// inSize - The size in bytes of this metadata item.
		// inLocationHint - A value between 0.0 and 1.0 expressing approximately where in the file the expression of the metadata was found or should be placed.
		// returns true on success.
		virtual bool AddMetadataItem(asdk::int64 inInstanceID, const char* inAmioMetadataTypeID, asdk::int64 inSize, double inLocationHint) = 0;

		//////////////////////////////////////////////////////////////////////////////
		// When reading metadata through this interface, use GetMetadataItemCount, GetMetadataItemInfo, and GetMetadataItem.
		// First, find out how many metadata items are in present.
		virtual asdk::int32 GetMetadataItemCount() const = 0;

		//////////////////////////////////////////////////////////////////////////////
		// Get information about a metadata item.  The inIndex should be from 0 up to GetMetadataItemCount()-1.
		virtual bool GetMetadataItemInfo(asdk::int32 inIndex, AsciiString& outAmioMetadataTypeID, asdk::int64& outSize, double& outLocationHint) const = 0;

		//////////////////////////////////////////////////////////////////////////////
		// Copy the data (or a portion of it, up to GetSize() bytes) from the given metadata item to the supplied buffer.
		// Returns true on success.  inStartByte offsets into the data, and the first byte 
		// at that offset will be copied into the first byte of the inDestinationBuffer buffer.
		// The inIndex should be from 0 up to GetMetadataItemCount()-1, as in GetMetadataItemInfo
		virtual bool GetMetadataItem(asdk::int32 inIndex, void* inDestinationBuffer, asdk::int64 inStartByte, asdk::int64 inByteCount) const = 0;

		//////////////////////////////////////////////////////////////////////////////
		// Set the number of samples of audio data available from this file (on read),
		// or expected to be written (on write).  Set to zero if the number of samples to
		// be written is unknown.  If the number of samples to be read is unknown, 
		// set this to an estimate, and use the kAmioFileFlag_SampleTotalInexact flag.
		virtual void SetSampleTotal(asdk::int64 inSampleCount) = 0;
		virtual asdk::int64 GetSampleTotal() const = 0;

		//////////////////////////////////////////////////////////////////////////////
		// New for AmioInterfaceVersion 1.1
		// If the particular format described should have a restricted set of allowed file extensions, specify them here.
		// For example, a plug-in may register support for extensions "aiff", and "aifc", but given the current settings,
		// wish for the app to allow only the "aiff" extension only.  If no allowed extensions are specified, all
		// registered extensions will be considered valid.
		virtual void SetAllowedExtensions(const ExtensionList& inAllowedExtensions) = 0;
		virtual const ExtensionList& GetAllowedExtensions() const = 0;

		//////////////////////////////////////////////////////////////////////////////
		// New for AmioInterfaceVersion 1.1
		// If a file is being written based on settings from another file, (i.e. a save, or save-as of a modified file), 		
		// those settings are accessed via GetPrivateFormatData.  Private data only makes sense to the plug-in that
		// created the data.  If private data were provided from some other plug-in, the format would not be understood,
		// and so private data from other plug-ins is not provided via GetPrivateFormatData.  However, if there is particular
		// knowledge of another plug-in's private data, and useful settings can be gleaned from it, use
		// GetOriginalMediaPrivateSettings to obtain the settings, if available, of the original media file, no matter what
		// plug-in originally imported the file.
		//      outOriginalMediaPlugInGuid - the GUID string of the plug-in that imported the original media as 
		//                                   set in AmioGetAmioInfoInterface::SetPluginID.
		//      outOriginalMediaFormatID - the formatID of the plug-in that imported the original media as set
		//                                 in AmioGetAmioInfoInterface::AddInputFormat.
		//      outOriginalMediaPrivateSettings - the private settings from the plug-in that imported the original 
		//                                        media as set in AmioFormatInterface::SetPrivateFormatData
		//                                        at the time the file was opened.
		virtual void GetOriginalMediaPrivateSettings(const char*& outOriginalMediaPlugInGuid, 
				const asdk::uint16*& outOriginalMediaFormatID, 
				const asdk::uint16*& outOriginalMediaPrivateSettings) const = 0;
	};


	// Used for kAmioInterfaceCommand_Open, opening an existing file for reading.
	class AmioOpenInterface
	{
	public:
		virtual ~AmioOpenInterface(){}

		//////////////////////////////////////////////////////////////////////////////
		// Get an interface for setting extended error information.
		virtual AmioErrorInterface& GetErrorInterface() = 0;

		//////////////////////////////////////////////////////////////////////////////
		// Get the name of the file to open using either a UTF16 or UTF8 path.
		virtual const asdk::uint16* GetFileName() const = 0;
		virtual const char* GetFileNameUTF8() const = 0;

		//////////////////////////////////////////////////////////////////////////////
		// The app will not rely on this interface, but rather use the
		// kAmioInterfaceCommand_GetFormat command to get the format of a file
		// just after it has been opened.  The interface here is used only to get the
		// settings used to retrieve the plug-in's private settings that may have
		// been set in the Import Settings Dialog.   Most plug-ins will not need
		// to use this.  See the kAmioInterfaceCommand_PreOpen flag.
		virtual AmioFormatInterface& GetFormat() = 0;

		//////////////////////////////////////////////////////////////////////////////
		// Set a state object that represents the file opened by the plug-in.
		// This state is defined by the plug-in and not interpreted by the host.
		// It will be passed to the plug-in for subsequent calls.
		virtual void SetSessionState(void* inState) = 0;
		virtual void* GetSessionState() const = 0;

		//////////////////////////////////////////////////////////////////////////////
		// Set flags about this particular file, a bitwise OR combination of AmioFileFlag values.
		// If the Get method is called without calling Set, it will return the defaults that would be used 
		// by the host for plug-ins not calling the Set method.
		virtual void SetFileFlags(asdk::uint32 inFlags) = 0;
		virtual asdk::uint32 GetFileFlags() const = 0;

		//////////////////////////////////////////////////////////////////////////////
		// Set / Get a UTF16 string representing the audio format of the file, for example "Waveform Audio - ADPCM"
		virtual void SetAudioFormatDescriptionString(const asdk::uint16* inString) = 0;
		virtual const asdk::uint16* GetAudioFormatDescriptionString() const = 0;

		//////////////////////////////////////////////////////////////////////////////
		// Report the native or compressed audio bitrate of this file for all channels collectively in bits per second.
		virtual void SetAudioBitrate(asdk::int32 inAudioBitrate) = 0;
		virtual asdk::int32 GetAudioBitrate() const = 0;

		//////////////////////////////////////////////////////////////////////////////
		// Set the size, as a sample count, that the plug-in would prefer for passing data to the host.
		// If the Get method is called without calling Set, it will return the default that would be used 
		// by the host for plug-ins not calling the Set method.
		// Setting a particular preferred size does not guarantee that samples will always be read
		// in chunks of that size.  The intention is to alert the host to what 
		// chunk size would result in the best performance.
		virtual void SetPreferredChunkSize(asdk::int64 inChunkSampleCount) = 0;
		virtual asdk::int64 GetPreferredChunkSize() const = 0;
	};


	// Used for kAmioInterfaceCommand_ReadSamples, reading samples from an open file.
	// This interface is never used if kAmioFileFlag_ReadSamplesRaw has been set.
	// For additional information about AmioReadSamplesInterface, see the kAmioFileFlag_SampleTotalInexact flag.
	class AmioReadSamplesInterface
	{
	public:
		virtual ~AmioReadSamplesInterface(){}

		//////////////////////////////////////////////////////////////////////////////
		// Get the first sample that should be returned in the given buffer, relative to the start of the file.
		virtual asdk::int64 GetStartSample() const = 0;

		//////////////////////////////////////////////////////////////////////////////
		// Get the number of samples that should be returned.
		virtual asdk::int64 GetSampleCount() const = 0;

		//////////////////////////////////////////////////////////////////////////////
		// Get the address of the buffer into which the sample data for the given channel must be copied.
		// If the value is NULL, no sample data is being requested for that channel.
		virtual void* GetBufferAddress(asdk::int32 inChannelIndex) const = 0;

		//////////////////////////////////////////////////////////////////////////////
		// Get the size of the buffers pointed to by GetBufferAddress().
		// This is for sanity checking, because the size will always be the sample count times
		// the size of the sample.
		virtual asdk::int64 GetBufferSize() const = 0;

		//////////////////////////////////////////////////////////////////////////////
		// Set the number of samples that were successfully returned.
		// (That is, successfully returned for all channels.)  If this value is 
		// less than GetSampleCount() then an error code should be returned unless
		// it is an EndOfFile condition.  See comments on kAmioFileFlag_SampleTotalInexact.
		virtual void SetReturnedSampleCount(asdk::int64 inReturnedSampleCount) = 0;

		//////////////////////////////////////////////////////////////////////////////
		// If the kAmioFileFlag_SampleTotalInexact flag was set, the host will attempt to read samples
		// until the plug-in signals with this call that it is done.  In addition to SetReturnedSampleCount() above, 
		// the plug-in should use this method (only during the buffer of samples that runs into the end 
		// of the file) to communicate that no more samples will be returned, and what the final duration of the file is.
		virtual void SetEndOfFile(asdk::int64 inFileSampleTotal) = 0;
	};
	

	// Used for kAmioInterfaceCommand_ReadSamplesRaw, reading raw bytes of sample data from an open file.
	// This interface is only used if kAmioFileFlag_ReadSamplesRaw has been set.
	class AmioReadSamplesRawInterface
	{
	public:
		virtual ~AmioReadSamplesRawInterface(){}

		//////////////////////////////////////////////////////////////////////////////
		// Get the offset of the first byte that should be returned.  0 is the offset of the first byte of 
		// the first channel of interleaved sample data.
		virtual asdk::int64 GetStartByteOffset() const = 0;

		//////////////////////////////////////////////////////////////////////////////
		// Get the number of bytes of interleaved sample data that should be returned.
		virtual asdk::int64 GetByteCount() const = 0;

		//////////////////////////////////////////////////////////////////////////////
		// Get the address of the buffer into which the interleaved sample data must be copied.
		virtual void* GetBufferAddress() const = 0;
	};


	// Used for kAmioInterfaceCommand_ReadVideoFrame, reading a frame of video.
	class AmioReadVideoFrameInterface
	{
	public:
		virtual ~AmioReadVideoFrameInterface(){}

		//////////////////////////////////////////////////////////////////////////////
		// Get the 0-based index of the frame that is to be returned.
		virtual asdk::int32 GetFrameNumber() const = 0;

		//////////////////////////////////////////////////////////////////////////////
		// Get the 0-based index of the stream of video to be accessed if there is more than one stream.
		virtual asdk::int32 GetStreamIndex() const = 0;

		//////////////////////////////////////////////////////////////////////////////
		// Get an interface for getting the format of the video that is being requested.
		// If the format supplied is not suitable, modify the format to something that 
		// would be suitable and return kAmioInterfaceReturnCode_UnsupportedFormat.
		virtual AmioVideoFormatInterface& GetRequestedVideoFormat() = 0;

		enum AmioReadVideoFlags
		{
			AmioReadVideoFlag_None					= 0,
			AmioReadVideoFlag_HighQuality			= 1<<1,		// Higher than normal quality -- may be slower
			AmioReadVideoFlag_LowQuality			= 1<<2,		// Lower than normal quality -- should be faster.	AmioReadVideoFlag_LowQuality | AmioReadVideoFlag_HighQuality not defined.

			AmioReadVideoFlag_SwizzlePixels			= 1<<5,		// Switch the endianness of the pixel data, please. (e.g. ARGB data should be returned as BGRA)
			AmioReadVideoFlag_BottomUp				= 1<<6,		// Supply the bottom left rather than the top left pixel as the first in the returned buffer, please.
			AmioReadVideoFlag_ForceFormatToARGB		= 1<<7,		// Only 32 bit ARGB data is supported for display, so no matter the video pixel type, return kVideoPixelType_ARGB32 data.
		};
		//////////////////////////////////////////////////////////////////////////////
		// Get AmioReadVideoFlag flags for this video read request.
		virtual asdk::uint32 GetFlags() const = 0;

		//////////////////////////////////////////////////////////////////////////////
		// Get the address of the buffer into which the video frame data is to be copied.
		// The size of the buffer will correspond to the format specified by GetRequestedVideoFormat().
		virtual void* GetBufferAddress() const = 0;
	};


	// Used for kAmioInterfaceCommand_ReadMetadata, to read a buffer of metadata
	// that is present in the open file.
	class AmioReadMetadataInterface
	{
	public:
		virtual ~AmioReadMetadataInterface(){}

		//////////////////////////////////////////////////////////////////////////////
		// This is the instance ID of the metadata the host wants to read.  The plug-in 
		// set this previously via MetadataReadItem::SetInstanceID().
		virtual asdk::int64 GetInstanceID() const = 0;	

		//////////////////////////////////////////////////////////////////////////////
		// Get the byte offset into the metadata item from which data should be supplied.
		virtual asdk::int64 GetStartOffset() const = 0;

		//////////////////////////////////////////////////////////////////////////////
		// Get the number of bytes that should be returned in the given buffer
		virtual asdk::int64 GetByteCount() const = 0;

		//////////////////////////////////////////////////////////////////////////////
		// Get the address of the buffer into which the metadata described should be returned.
		virtual void* GetBufferAddress() const = 0;
	};


	// Used for kAmioInterfaceCommand_PreOpen, to see if a dialog is needed before opening a file.
	// Most plug-ins will not need to implement this as they probably won't need additional data from the host or user
	// to determine if the file is openable or not.  This allows the host to determine whether or not it should ask
	// the plug-in to display a dialog to the user before fully committing to open the file.
	class AmioPreOpenInterface
	{
	public:
		virtual ~AmioPreOpenInterface(){}

		//////////////////////////////////////////////////////////////////////////////
		// Get an interface for setting extended error information.
		virtual AmioErrorInterface& GetErrorInterface() = 0;

		//////////////////////////////////////////////////////////////////////////////
		// Set true if based on the given format, a dialog for additional info might
		// be displayed upon opening a file, for example to get info on how to interpret a file.
		// See AmioRunImportSettingsDialogInterface.
		virtual void SetNeedToShowDialogForThisFile(bool inNeedsImportDialog) = 0;
		virtual bool GetNeedToShowDialogForThisFile() const = 0;

		//////////////////////////////////////////////////////////////////////////////
		// Get the name of the file to open using either a UTF16 or UTF8 path.
		virtual const asdk::uint16* GetFileName() const = 0;
		virtual const char* GetFileNameUTF8() const = 0;
	};

	// Used for kAmioInterfaceCommand_RunImportSettingsDialog.  Most plug-ins won't need to
	// implement this as usually no user-intervention is needed to open a set of files.
	class AmioRunImportSettingsDialogInterface
	{
	public:
		virtual ~AmioRunImportSettingsDialogInterface(){}

		//////////////////////////////////////////////////////////////////////////////
		// Get an interface for setting extended error information.
		virtual AmioErrorInterface& GetErrorInterface() = 0;

		//////////////////////////////////////////////////////////////////////////////
		// Get the name of the file to open using either a UTF16 or UTF8 path.
		virtual const asdk::uint16* GetFileName() const = 0;
		virtual const char* GetFileNameUTF8() const = 0;

		//////////////////////////////////////////////////////////////////////////////
		// Get the settings used to populate the dialog from here, and write the settings 
		// back through this interface when finished.
		virtual AmioFormatInterface& GetFormat() = 0;

		//////////////////////////////////////////////////////////////////////////////
		// Get a pointer to a platform specific type expressing the parent window for the dialog.
		virtual void* GetParentWindow() const = 0;

		//////////////////////////////////////////////////////////////////////////////
		// set to true if the modified settings should be ignored.
		virtual void SetCancel(bool inCancelled) = 0;
		virtual bool GetCancel() const = 0;
	};


	// Used for kAmioInterfaceCommand_GetExportSettingsInfo, to validate settings for file creation
	// and get information about them.
	// If the given format is acceptable without modification, return kAmioInterfaceReturnCode_Success.
	// If the plug-in was able to change the given format to something acceptable, then:
	//		if GetFixFormatParameter() == kFixFormatParameter_FixAll return kAmioInterfaceReturnCode_UnsupportedFormat
	//		otherwise return kAmioInterfaceReturnCode_Success, because the format could be changed successfully.
	// If nothing can be done to make the format acceptable, return some other error code, such as kAmioInterfaceReturnCode_ParameterOutOfRange.
	class AmioGetExportSettingsInfoInterface
	{
	public:
		virtual ~AmioGetExportSettingsInfoInterface(){}

		//////////////////////////////////////////////////////////////////////////////
		// Get an interface for setting extended error information.
		virtual AmioErrorInterface& GetErrorInterface() = 0;

		//////////////////////////////////////////////////////////////////////////////
		// Use this interface to get the settings under consideration for creating a new file. 
		// If they are not acceptable as given, modify them according to the FixFormatParameter, or return kAmioInterfaceReturnCode_UnsupportedFormat.
		// Use the modified settings to return the rest of the information, such as estimated file size, etc.
		virtual AmioFormatInterface& GetFormat() = 0;

		enum FixFormatParameter {
			kFixFormatParameter_FixAll = 0,				// Is the format acceptable exactly as given?
			kFixFormatParameter_FavorInputFormat,		// Modify the format as best you can to something that is supported, but favor the input format.
			kFixFormatParameter_FavorPrivateSettings,	// Modify the input format as needed to preserve the given private settings, but favor the private format settings.
// At this point it seems like we want to fix or not fix only private format settings.
//			kFixFormatParameter_FixSampleRate,			// Is the sample rate acceptable, even if other parameters need to change?
///			kFixFormatParameter_FixSampleType,			// Is the sample type acceptable, even if other parameters need to change?
//			kFixFormatParameter_FixChannelCount,		// Is the number of channels acceptable, even if the layout or other parameters need to change?
//			kFixFormatParameter_FixChannelLayout,		// Is the exact channels layout acceptable, even if other parameters need to change?
		};
		//////////////////////////////////////////////////////////////////////////////
		// Get instructions on how to modify the given settings.  See GetFormat() above.
		virtual FixFormatParameter GetFixFormatParameter() const  = 0;

		//////////////////////////////////////////////////////////////////////////////
		// Set the estimated file size based on these settings (if the expected number of samples is greater than zero).
		virtual void SetEstimatedFileSize(asdk::int64 inFileSize) = 0;
		virtual asdk::int64 GetEstimatedFileSize() const = 0;

		//////////////////////////////////////////////////////////////////////////////
		// Set true if based on the given format, a dialog of additional export settings can be posted.
		// See AmioRunExportSettingsDialogInterface.
		virtual void SetHasExportSettingsDialog(bool inHasExportDialog) = 0;
		virtual bool GetHasExportSettingsDialog() const = 0;

		//////////////////////////////////////////////////////////////////////////////
		/// Do these settings represent a lossy audio compression format?
		virtual void SetIsLossy(bool inIsLossy) = 0;
		virtual bool GetIsLossy() const = 0;

		//////////////////////////////////////////////////////////////////////////////
		// Set a human-readable description of the private format settings using a UTF16 string.
		// This description can include newline characters, but should conform to the format
		// commonly seen in the export settings dialog, such as:
		//		Wave Uncompressed
		//		16-bit Integer
		// or
		//		AIFF Uncompressed
		//		24-bit Integer
		//		Big-Ending (PowerPC Byte Order)
		// or
		//		MP3 VBR
		//		Highest Quality (44100 Hz), quality similar to 224 Kbps CBR.
		// or
		//		Monkey's Audio 24-bit
		//		Fast Compression
		// etc.
		// See AmioFormatInterface::GetPrivateFormatData.
		virtual void SetSettingsDescriptionString(const asdk::uint16* inString) = 0;
		virtual const asdk::uint16* GetSettingsDescriptionString() const = 0;
	};


	// Used for kAmioInterfaceCommand_RunExportSettingsDialog, 
	class AmioRunExportSettingsDialogInterface
	{
	public:
		virtual ~AmioRunExportSettingsDialogInterface(){}

		//////////////////////////////////////////////////////////////////////////////
		// Get an interface for setting extended error information.
		virtual AmioErrorInterface& GetErrorInterface() = 0;

		//////////////////////////////////////////////////////////////////////////////
		// Get the settings used to populate the dialog from here, and write the settings 
		// back through this interface when finished.
		virtual AmioFormatInterface& GetFormat() = 0;

		//////////////////////////////////////////////////////////////////////////////
		// Get a pointer to a platform specific type expressing the parent window for the dialog.
		virtual void* GetParentWindow() const = 0;

		//////////////////////////////////////////////////////////////////////////////
		// set to true if the modified settings should be ignored.
		virtual void SetCancel(bool inCancelled) = 0;
		virtual bool GetCancel() const = 0;

		enum SettingsDialogFlag
		{
			kSettingsDialogFlag_None				= 0,
			kSettingsDialogFlag_FixSampleRate		= 0x00000001,	// Please do not show options in the settings dialog that would require a change in the sample rate.
			kSettingsDialogFlag_FixSampleType		= 0x00000002,	// Please do not show options in the settings dialog that would require a change in the sample type.
			kSettingsDialogFlag_FixChannelCount		= 0x00000004,	// Please do not show options in the settings dialog that would require a change in the channel count.
		};
		//////////////////////////////////////////////////////////////////////////////
		// New for AmioInterfaceVersion 1.1
		// Get SettingsDialogFlag flags that may affect how the settings dialog should behave.
		virtual asdk::int64 GetSettingsDialogFlags() const = 0;
	};


	// Used for kAmioInterfaceCommand_WriteStart, Create a file for writing.
	class AmioWriteStartInterface
	{
	public:
		virtual ~AmioWriteStartInterface(){}

		//////////////////////////////////////////////////////////////////////////////
		// Get an interface for setting extended error information.
		virtual AmioErrorInterface& GetErrorInterface() = 0;

		//////////////////////////////////////////////////////////////////////////////
		// Get the name of the file to create using either a UTF16 or UTF8 path.
		virtual const asdk::uint16* GetFileName() const = 0;
		virtual const char* GetFileNameUTF8() const = 0;

		//////////////////////////////////////////////////////////////////////////////
		// Get the format of the file to be written. 
		virtual const AmioFormatInterface& GetFormat() const = 0;

		//////////////////////////////////////////////////////////////////////////////
		// Get / Set flags about this particular file, a bitwise OR combination of AmioFileFlag values.
		// If flags are known previously, they should also be set during parameter negotiation
		// using AmioFormatInterface::SetFileFlags().
		virtual void SetFileFlags(asdk::uint32 inFlags) = 0;
		virtual asdk::uint32 GetFileFlags() const = 0;

		//////////////////////////////////////////////////////////////////////////////
		// Set / Get a UTF16 string representing the audio format of the file, for example "Waveform Audio - ADPCM"
		virtual void SetAudioFormatDescriptionString(const asdk::uint16* inString) = 0;
		virtual const asdk::uint16* GetAudioFormatDescriptionString() const = 0;

		//////////////////////////////////////////////////////////////////////////////
		// Set a state object that represents the file opened by the plug-in.
		// This state is defined by the plug-in and not interpreted by the host.
		// It will be passed to the plug-in for subsequent calls.
		virtual void SetSessionState(void* inState) = 0;
		virtual void* GetSessionState() const = 0;
	};


	// Used for kAmioInterfaceCommand_WriteSamples, writing samples to a newly created file.
	class AmioWriteSamplesInterface
	{
	public:
		virtual ~AmioWriteSamplesInterface(){}

		//////////////////////////////////////////////////////////////////////////////
		// Get an interface for setting extended error information.
		virtual AmioErrorInterface& GetErrorInterface() = 0;

		//////////////////////////////////////////////////////////////////////////////
		// Get 0-based index of the first sample supplied in the given buffer, relative to the start of the file.
		virtual asdk::int64 GetStartSample() const = 0;

		//////////////////////////////////////////////////////////////////////////////
		// Get the number of samples provided.
		virtual asdk::int64 GetSampleCount() const = 0;

		//////////////////////////////////////////////////////////////////////////////
		// Get the address of the buffer supplying sample data for the given channel.
		// The size will always be the sample count times the size of one sample.
		virtual void* GetBufferAddress(asdk::int32 inChannelIndex) const = 0;

		//////////////////////////////////////////////////////////////////////////////
		// for convenience, the format and metadata information used in the AmioWriteStartInterface
		virtual const AmioFormatInterface& GetFormat() const = 0;
	};


	// Used for kAmioInterfaceCommand_WriteFinish, Finish and close the file being written.
	class AmioWriteFinishInterface
	{
	public:
		virtual ~AmioWriteFinishInterface(){}

		//////////////////////////////////////////////////////////////////////////////
		// Get an interface for setting extended error information.
		virtual AmioErrorInterface& GetErrorInterface() = 0;

		//////////////////////////////////////////////////////////////////////////////
		// returns true if instead of finishing the file normally, the write operation should be cleanly canceled.
		virtual bool GetCancel() const = 0;

		//////////////////////////////////////////////////////////////////////////////
		// for convenience, the format and metadata information used in the AmioWriteStartInterface
		virtual const AmioFormatInterface& GetFormat() const = 0;
	};

} // namespace amio

#endif // AMIO_AMIOSDK_H