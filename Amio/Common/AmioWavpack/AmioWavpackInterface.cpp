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

// Amio SDK
#include "AmioSDK.h"
#include "AmioInterfaceTemplate.h"
#include "AmioUtilities.h"

#include "AmioWavpackPrivateSettings.h"
#include "AmioWavpackSettingsDialog.h"

// Wavpack wrappers
#include "WavpackReader.h"
#include "WavpackWriter.h"

#include <string>

#ifdef AMIO_OS_MAC
#define strnicmp strncasecmp
#else if defined(AMIO_OS_WIN)
#define strnicmp _strnicmp
#endif

namespace
{
	///
	template<typename T>
	void Interleave(void* inSrc, asdk::int32* inDest,
			asdk::int32 sampleCount, asdk::int32 skip, int offset = 0)
	{
		T* src = reinterpret_cast<T*>(inSrc);
		asdk::int32* dest = inDest;

		while (sampleCount--)
		{
			*dest = *src++ + offset;
			dest += skip;
		}
	}

	///
	void Interleave24Bits(void* inSrc, asdk::int32* inDest,
			asdk::int32 sampleCount, asdk::int32 skip)
	{
		asdk::uint8* src = reinterpret_cast<asdk::uint8*>(inSrc);
		asdk::int32* dest = inDest;

		while (sampleCount--)
		{
			*dest = src [0] + (src [1] << 8) + (src [2] << 16);
            if (*dest & 0x800000) *dest |= 0xFF000000;
			dest += skip;
            src += 3;
		}
	}

	// This table maps the Amio channel identifiers (100-142) to the matching WavPack channel identifiers
	// (which, for those channels that exist in Core Audio, match the Core Audio values). Note that the
	// first 18 are the Microsoft "standard" channels, BUT ARE NOT IN MICROSOFT ORDER! Amio channels
	// 127-128 and 138-142 do not have a corresponding Core Audio number, so we use the Amio values.

    unsigned char AmioChannelTable [] = {
    // WavPack  amio::AudioChannelLabel                            WAVEFORMATEXTENSIBLE             Apple CoreAudio                                      VST 3
    // -------  -------------------------------------              -----------------------------    -----------------------------------------------      ----------------------
        1,      // kAudioChannelLabel_FrontLeft           = 100,   SPEAKER_FRONT_LEFT               kAudioChannelLabel_Left                    = 1,      kSpeakerL
        2,      // kAudioChannelLabel_FrontRight          = 101,   SPEAKER_FRONT_RIGHT              kAudioChannelLabel_Right                   = 2,      kSpeakerR
        3,      // kAudioChannelLabel_FrontCenter         = 102,   SPEAKER_FRONT_CENTER             kAudioChannelLabel_Center                  = 3,      kSpeakerC
        4,      // kAudioChannelLabel_LowFrequency        = 103,   SPEAKER_LOW_FREQUENCY            kAudioChannelLabel_LFEScreen               = 4,      kSpeakerLfe
        5,      // kAudioChannelLabel_BackLeft            = 104,   SPEAKER_BACK_LEFT                kAudioChannelLabel_LeftSurround            = 5,      kSpeakerLs
        6,      // kAudioChannelLabel_BackRight           = 105,   SPEAKER_BACK_RIGHT               kAudioChannelLabel_RightSurround           = 6,      kSpeakerRs      
        9,      // kAudioChannelLabel_BackCenter          = 106,   SPEAKER_BACK_CENTER              kAudioChannelLabel_CenterSurround          = 9,      kSpeakerCs = kSpeakerS
        7,      // kAudioChannelLabel_FrontLeftOfCenter   = 107,   SPEAKER_FRONT_LEFT_OF_CENTER     kAudioChannelLabel_LeftCenter              = 7,      kSpeakerLc
        8,      // kAudioChannelLabel_FrontRightOfCenter  = 108,   SPEAKER_FRONT_RIGHT_OF_CENTER    kAudioChannelLabel_RightCenter             = 8,      kSpeakerRc
        10,     // kAudioChannelLabel_SideLeft            = 109,   SPEAKER_SIDE_LEFT                kAudioChannelLabel_LeftSurroundDirect      = 10,     kSpeakerSl
        11,     // kAudioChannelLabel_SideRight           = 110,   SPEAKER_SIDE_RIGHT               kAudioChannelLabel_RightSurroundDirect     = 11,     kSpeakerSr
        12,     // kAudioChannelLabel_TopCenter           = 111,   SPEAKER_TOP_CENTER               kAudioChannelLabel_TopCenterSurround       = 12,     kSpeakerTm
        13,     // kAudioChannelLabel_TopFrontLeft        = 112,   SPEAKER_TOP_FRONT_LEFT           kAudioChannelLabel_VerticalHeightLeft      = 13,     kSpeakerTfl
        15,     // kAudioChannelLabel_TopFrontRight       = 113,   SPEAKER_TOP_FRONT_RIGHT          kAudioChannelLabel_VerticalHeightRight     = 15,     kSpeakerTfr
        14,     // kAudioChannelLabel_TopFrontCenter      = 114,   SPEAKER_TOP_FRONT_CENTER         kAudioChannelLabel_VerticalHeightCenter    = 14,     kSpeakerTfc
        16,     // kAudioChannelLabel_TopBackLeft         = 115,   SPEAKER_TOP_BACK_LEFT            kAudioChannelLabel_TopBackLeft             = 16,     kSpeakerTrl
        18,     // kAudioChannelLabel_TopBackRight        = 116,   SPEAKER_TOP_BACK_RIGHT           kAudioChannelLabel_TopBackRight            = 18,     kSpeakerTrr
        17,     // kAudioChannelLabel_TopBackCenter       = 117,   SPEAKER_TOP_BACK_CENTER          kAudioChannelLabel_TopBackCenter           = 17,     kSpeakerTrc

        35,     // kAudioChannelLabel_FrontLeftWide       = 118,                                    kAudioChannelLabel_LeftWide                = 35
        36,     // kAudioChannelLabel_FrontRightWide      = 119,                                    kAudioChannelLabel_RightWide               = 36
        255,    // Reserved - DO NOT USE                  = 120-126
        255,
        255,
        255,
        255,
        255,
        255,
        127,    // kAudioChannelLabel_LeftHeight          = 127,                                                                                         kSpeakerTsl
        128,    // kAudioChannelLabel_RightHeight         = 128,                                                                                         kSpeakerTsr
        33,     // kAudioChannelLabel_RearSurroundLeft    = 129,                                    kAudioChannelLabel_RearSurroundLeft        = 33,     kSpeakerLcs
        34,     // kAudioChannelLabel_RearSurroundRight   = 130,                                    kAudioChannelLabel_RearSurroundRight       = 34,     kSpeakerRcs
        37,     // kAudioChannelLabel_LFE2                = 131,                                    kAudioChannelLabel_LFE2                    = 37,     kSpeakerLfe2
        38,     // kAudioChannelLabel_LeftTotal           = 132,                                    kAudioChannelLabel_LeftTotal               = 38,
        39,     // kAudioChannelLabel_RightTotal          = 133,                                    kAudioChannelLabel_RightTotal              = 39,
        200,    // kAudioChannelLabel_Ambisonic_W         = 134,                                    kAudioChannelLabel_Ambisonic_W             = 200,    kSpeakerW
        201,    // kAudioChannelLabel_Ambisonic_X         = 135,                                    kAudioChannelLabel_Ambisonic_X             = 201,    kSpeakerX
        202,    // kAudioChannelLabel_Ambisonic_Y         = 136,                                    kAudioChannelLabel_Ambisonic_Y             = 202,    kSpeakerY
        203,    // kAudioChannelLabel_Ambisonic_Z         = 137,                                    kAudioChannelLabel_Ambisonic_Z             = 203,    kSpeakerZ
        138,    // kAudioChannelLabel_BottomFrontLeft     = 138,                                                                                         kSpeakerBfl
        139,    // kAudioChannelLabel_BottomFrontCenter   = 139,                                                                                         kSpeakerBfc
        140,    // kAudioChannelLabel_BottomFrontRight    = 140,                                                                                         kSpeakerBfr
        141,    // kAudioChannelLabel_ProximityLeft       = 141,
        142,    // kAudioChannelLabel_ProximityRight      = 142,
    };

    #define NUM_AMIO_CHANNELS (sizeof (AmioChannelTable) / sizeof (AmioChannelTable [0]))

    void OutputDebugMemory (char *memory, int bcount)
    {
        char hexstr [256], ascstr [256];
        int i, j;

        for (j = 0; j < bcount; j += 16) {
            ascstr [0] = 0;

            sprintf (hexstr, "%04x: ", j);

            for (i = 0; i < 16; ++i)
                if (i + j < bcount) {
                    sprintf (hexstr + strlen (hexstr), "%02x ", (unsigned char) memory [i+j]);
                    sprintf (ascstr + strlen (ascstr), "%c",
                        (memory [i+j] >= ' ' && memory [i+j] <= '~') ? memory [i+j] : '.');
                }
                else {
                    strcat (hexstr, "   ");
                    strcat (ascstr, " ");
                }

            OutputDebugStringA (hexstr);
            OutputDebugStringA (ascstr);
            OutputDebugStringA ("\n");
        }
    }

} // private namespace

namespace amio
{

// these two IDs are used to pass tag metadata (from APEv2 tags in WavPack) through Audition (presumably unaltered)

const char* kAmioMetadataTypeID_BinaryTagMetadataChunk = "c29ea60a-f827-440d-bfd1-97ecc6a62e03";
const char* kAmioMetadataTypeID_TextTagMetadataChunk = "5187ccec-fcec-4407-8c8a-3cad1f83f50d";

class AmioWavpackInterface : public AmioInterfaceTemplate<WavpackReader, WavpackWriter>
{
protected:

	///
	/// This override is called first and gets basic information about the plug-in.
	/// Please use a new guidID and not this one from the sample code.
	///
	virtual AmioResult GetAmioInfo(AmioGetAmioInfoInterface& amioInfo)
	{
		//////////////////////////////////////////////////////////////////////////////////
		//
		// IMPORTANT NOTE!!!!!!!!!!
		//
		// You must create a new guid to pass into SetPluginID.
		// Each plug-in MUST have a different ID.
		// We're begging you: please don't let two plug-ins exist with the same ID.
		// Change it NOW.
		// 
		// IMPORTANT NOTE!!!!!!!!!!
		//
		//////////////////////////////////////////////////////////////////////////////////
		amioInfo.SetPluginID("6b524afb-c4f0-476e-bfc7-9c2ef8ead405", amio::utils::AsciiToUTF16("WavPack Amio plug-in").c_str());
		amio::UTF16String filterName = amio::utils::AsciiToUTF16("WavPack");
		amio::ExtensionList extList;
		extList.push_back("wv");										// A list of supported file extensions
		amioInfo.AddInputFormat(filterName.c_str(), NULL, extList);		// The extensions that are supported for file reading
		amioInfo.AddOutputFormat(filterName.c_str(), NULL, extList);	// The extensions that are supported for file writing

		char msg [128];
		sprintf (msg, "AmioWavpack::GetAmioInfo(): host interface version from template = %c.%c\n",
			'0'+mHostInterfaceVersionMajor, '0'+mHostInterfaceVersionMinor);
		OutputDebugStringA (msg);

		return kAmioInterfaceReturnCode_Success;
	}

	///
	/// This override is the first call when attempting to open an existing file for reading.
	///
	virtual AmioResult OpenForRead(AmioOpenInterface& amioOpen)
	{
		UTF16String fileName(amioOpen.GetFileName());
		if (!amio::utils::FileExists(fileName))
		{
			SetErrorString(amio::utils::AsciiToUTF16("File does not exist.").c_str());
			return kAmioInterfaceReturnCode_GeneralError;
		}

		// This reader object is the "SessionState" object that will be passed back to 
		// subsequent read-related calls.
		std::auto_ptr<WavpackReader> reader(new WavpackReader());

		WavpackReader::Error decoderResult = reader->Initialize(fileName);

		switch (decoderResult)
		{
		case WavpackReader::kError_NoError:
			break;
		default:
		case WavpackReader::kError_General:
            OutputDebugStringA ("OpenForRead() threw the error!\n");
			SetErrorString(amio::utils::AsciiToUTF16("Invalid or unsupported file.").c_str());
			return kAmioInterfaceReturnCode_GeneralError;
		case WavpackReader::kError_UnsupportedBitDepth:
			SetErrorString(amio::utils::AsciiToUTF16("Unsupported bit depth.").c_str());
			return kAmioInterfaceReturnCode_GeneralError;
		}

		asdk::int32 bytesPerSample = reader->GetBytesPerSample();
		if ((bytesPerSample < 1) || (bytesPerSample > 4))
		{
			SetErrorString(amio::utils::AsciiToUTF16("Unsupported bit depth.").c_str());
			return kAmioInterfaceReturnCode_GeneralError;
		}

		AmioWavpackPrivateSettings privateSettings;
		privateSettings.SetCompressionLevel(reader->GetCompressionLevel());

		asdk::int32 bitRate = reader->GetBitRate();
		char asciiFormat[256];
		sprintf(asciiFormat,"WavPack %s %ld kbps",
			amio::utils::UTF16StringtoUTF8String(privateSettings.GetCompressionQualityString()).c_str(),
			bitRate/1000);
		amio::UTF16String formatDescription = amio::utils::AsciiToUTF16(asciiFormat);

		amioOpen.SetAudioFormatDescriptionString(formatDescription.c_str()); 
		amioOpen.SetAudioBitrate(bitRate);
		amioOpen.SetSessionState(reader.release());
		amioOpen.SetFileFlags(kAmioFileFlag_XmpSupportThroughPluginOnly);
		return kAmioInterfaceReturnCode_Success;
	}

	///
	/// After opening a file for reading, Audition wants to know more about its format and the metadata it contains.
	///
	virtual AmioResult GetFormat(WavpackReader& inReadFile, AmioFormatInterface& inFormat)
	{
		// Set the Audio format.
		inFormat.SetChannelCount(inReadFile.GetNumChannels());
		inFormat.SetSampleRate(inReadFile.GetSampleRate());
		switch (inReadFile.GetBytesPerSample())
		{
		case 1: inFormat.SetSampleType(kAudioSampleType_UInt8); break;
		case 2: inFormat.SetSampleType(kAudioSampleType_SInt16); break;
		case 3: inFormat.SetSampleType(kAudioSampleType_SInt24); break;
		case 4: inFormat.SetSampleType(kAudioSampleType_Float32); break;
		default:
			return kAmioInterfaceReturnCode_GeneralError;
		}		
		inFormat.SetSampleTotal(inReadFile.GetSampleCount());

        int channelAssignment = 0;
        int channelCount = inReadFile.GetNumChannels();
        asdk::int32 channelMask = inReadFile.GetChannelMask();

		// with the new WavpackGetChannelIdentities(), obtaining channel information is easy

		unsigned char *channel_identities = new unsigned char [channelCount+1];
		inReadFile.GetChannelIdentities (channel_identities);

		for (int i = 0; i < channelCount; ++i) {
			if (channel_identities [i] == 255)
                inFormat.SetSpeakerAssignment (i, kAudioChannelLabel_Discrete);
			else {
				int j;

				for (j = 0; j < NUM_AMIO_CHANNELS; ++j)
					if (channel_identities [i] == AmioChannelTable [j]) {
						inFormat.SetSpeakerAssignment (i, (amio::AudioChannelLabel) (kAudioChannelLabel_FrontLeft + j));
						break;
					}

				if (j == NUM_AMIO_CHANNELS)
					inFormat.SetSpeakerAssignment (i, kAudioChannelLabel_Discrete);
			}
		}

		delete [] channel_identities;

		// Set our private settings.
		AmioWavpackPrivateSettings privateSettings;
		privateSettings.SetCompressionLevel(inReadFile.GetCompressionLevel());
		inFormat.SetPrivateFormatData(privateSettings.GetSerialized().c_str());

		// Pass along information about the metadata we found in the file.
		for (int metadataIndex = 0; metadataIndex < inReadFile.GetRiffMetadataItemCount(); metadataIndex++)
		{
			asdk::int32 size = 0;
			const asdk::uint8* buffer = inReadFile.GetRiffMetadataItem(metadataIndex, size);
			if (size < 8)
			{
				continue;	// Sanity check.
			}
			if ((buffer[3] == 'X') &&
				(buffer[2] == 'M') &&
				(buffer[1] == 'P') &&
				(buffer[0] == '_'))
			{
				// Audition will be very happy to be supplied with xmp metadata.
				// Here we strip the RIFF header as it appears in the WavPack file to obtain the size of the raw XMP metadata.
				size -= 8;
				inFormat.AddMetadataItem(metadataIndex, kAmioMetadataTypeID_XMP, size, 1.0);
			}
			else if ((buffer[0] == 'b') &&
				     (buffer[1] == 'e') &&
				     (buffer[2] == 'x') &&
				     (buffer[3] == 't'))
			{
				inFormat.AddMetadataItem(metadataIndex, kAmioMetadataTypeID_BWF_bext, size, 1.0);
			}
			else
			{
				//
				// NOTE: if you have your own metadata types that you want to flow through Audition untouched, and
				//       they are not one of the defined types, you must define your own GUID to describe them.
				//       If you claim that a blob of metadata is of type kAmioMetadataTypeID_GenericWaveMetadataChunk
				//       It may get written to file types where it is not appropriate.  
				//       For this particular plug-in, kAmioMetadataTypeID_GenericWaveMetadataChunk is the correct
				//       ID because WavPack actually stores its metadata as RIFF chunks just like a Wave file.
				// NOTE: We mark all of these location hints as 1.0 since the WavPack file format has all the RIFF metadata at the end.
				//       Other formats may have metadata at different places throughout the file, and this value is your hint
				//       about where a particular metadata item should be written.  This value will be passed back 
				//		 along with the metadata item when written during a save operation.
				//
				inFormat.AddMetadataItem(metadataIndex, kAmioMetadataTypeID_GenericWaveMetadataChunk, size, 1.0);
			}
		}

		// Pass along information about the tag metadata we found in the file.
		for (int metadataIndex = 0; metadataIndex < inReadFile.GetTagMetadataItemCount(); metadataIndex++)
		{
			bool isBinary = 0;
			asdk::int32 size = 0;
			inReadFile.GetTagMetadataItem(metadataIndex, size, isBinary);

			if (isBinary)
				inFormat.AddMetadataItem(metadataIndex + inReadFile.GetRiffMetadataItemCount(),
					kAmioMetadataTypeID_BinaryTagMetadataChunk, size, 1.0);
			else
				inFormat.AddMetadataItem(metadataIndex + inReadFile.GetRiffMetadataItemCount(),
					kAmioMetadataTypeID_TextTagMetadataChunk, size, 1.0);
		}

		return kAmioInterfaceReturnCode_Success;
	}

	///
	/// Here is where the actual audio samples are read.
	///
	virtual AmioResult ReadSamples(WavpackReader& inReadFile, AmioReadSamplesInterface& inReadInterface)
	{
		asdk::int64 samplesDesired = inReadInterface.GetSampleCount();

		inReadInterface.SetReturnedSampleCount(0);

		int channelCount = inReadFile.GetNumChannels();
		for (int channelIndex = 0; channelIndex < channelCount; channelIndex++)
		{
			if (!inReadFile.ReadSamples(
					inReadInterface.GetStartSample(), 
					static_cast<asdk::int32>(samplesDesired),
					channelIndex, 
					inReadInterface.GetBufferAddress(channelIndex)))
			{
				return kAmioInterfaceReturnCode_DecoderError;
			}
		}
		inReadInterface.SetReturnedSampleCount(samplesDesired);
		return kAmioInterfaceReturnCode_Success;
	}

	///
	/// After opening, GetFormat returned information about the metadata in the file,
	/// but here is where the actual metadata items are passed to Audition.
	///
	virtual AmioResult ReadMetadata(WavpackReader& inReadFile, 
		asdk::int64 inInstanceID,
		asdk::int64 inStartOffset,
		asdk::int64 inByteCount,
		void *inDestinationBuffer)
	{
		int instanceID = static_cast<int>(inInstanceID);
		asdk::int32 size = 0;
		bool isBinary;
//		char msg [256];

		if (instanceID >= inReadFile.GetRiffMetadataItemCount ()) {
			const asdk::uint8* sourceBuffer = inReadFile.GetTagMetadataItem(instanceID - inReadFile.GetRiffMetadataItemCount (), size, isBinary);

			if (!sourceBuffer)
				return kAmioInterfaceReturnCode_NoSuchItem;

			if (inByteCount + inStartOffset > size)
				return kAmioInterfaceReturnCode_ParameterOutOfRange;

			memcpy(inDestinationBuffer, &sourceBuffer[inStartOffset], static_cast<size_t>(inByteCount));
//			sprintf (msg, "ReadMetadata: %d bytes read from tag item \"%s\"\n", inByteCount, (char *) sourceBuffer);
//			OutputDebugStringA (msg);

			return kAmioInterfaceReturnCode_Success;
		}

		const asdk::uint8* sourceBuffer = inReadFile.GetRiffMetadataItem(instanceID, size);
		if (!sourceBuffer)
		{
			return kAmioInterfaceReturnCode_NoSuchItem;
		}
		if ((sourceBuffer[3] == 'X') &&
			(sourceBuffer[2] == 'M') &&
			(sourceBuffer[1] == 'P') &&
			(sourceBuffer[0] == '_'))
		{
			// Strip the RIFF header and deliver the raw XMP data.
			size -= 8;
			sourceBuffer += 8;
//			sprintf (msg, "ReadMetadata: _PMX of %d bytes\n", size);
//			OutputDebugStringA (msg);
		}
//		else {
//			sprintf (msg, "ReadMetadata: %c%c%c%c of %d bytes\n", sourceBuffer [0], sourceBuffer [1], sourceBuffer [2], sourceBuffer [3], size);
//			OutputDebugStringA (msg);
//		}

		if (inByteCount + inStartOffset > size)
		{			
			return kAmioInterfaceReturnCode_ParameterOutOfRange;
		}
		memcpy(inDestinationBuffer, &sourceBuffer[inStartOffset], static_cast<size_t>(inByteCount));
		return kAmioInterfaceReturnCode_Success;
	}

	///
	/// For file writing, provide a format that is always going to be valid for this plug-in.
	///
	virtual AmioResult GetDefaultExportSettings(AmioFormatInterface& outSettings)
	{
		// These are just about the most plain vanilla settings possible.
		outSettings.SetChannelCount(2);
		outSettings.SetSampleRate(44100);
		outSettings.SetSampleType(kAudioSampleType_SInt16);
		return kAmioInterfaceReturnCode_Success;
	}

	///
	/// Before a file is opened for writing, we have to negotiate about the settings that will be used.
	/// If the given format is acceptable without modification, return kAmioInterfaceReturnCode_Success.
	/// If the plug-in was able to change the given format to something acceptable, then:
	///		if GetFixFormatParameter() == kFixFormatParameter_FixAll return kAmioInterfaceReturnCode_UnsupportedFormat
	///		otherwise return kAmioInterfaceReturnCode_Success, because the format could be changed successfully.
	/// If nothing can be done to make the format acceptable, return some other error code, such as kAmioInterfaceReturnCode_ParameterOutOfRange.
	///
	virtual AmioResult GetExportSettingsInfo(AmioGetExportSettingsInfoInterface& inSettingsInfo)
	{
		inSettingsInfo.SetHasExportSettingsDialog(true);
		inSettingsInfo.SetIsLossy(false);

		AmioFormatInterface& audioFormat = inSettingsInfo.GetFormat();
		AmioWavpackPrivateSettings privateSettings;
		amio::UTF16String privateSettingsString(audioFormat.GetPrivateFormatData());
		privateSettings.InitializeFromSerialized(privateSettingsString);

		bool isGivenFormatOK = true;

		int bytesPerSample = 0;
		UTF16String sampleDepth;
		switch (audioFormat.GetSampleType())
		{
		case kAudioSampleType_UInt8:
			bytesPerSample = 1;
			sampleDepth = amio::utils::AsciiToUTF16("8 bit");
			break;
		case kAudioSampleType_SInt16:
			bytesPerSample = 2;
			sampleDepth = amio::utils::AsciiToUTF16("16 bit");
			break;
		case kAudioSampleType_SInt24:
			bytesPerSample = 3;
			sampleDepth = amio::utils::AsciiToUTF16("24 bit");
			break;
		case kAudioSampleType_Float32:
			bytesPerSample = 4;
			sampleDepth = amio::utils::AsciiToUTF16("32 bit float");
			break;
		default:
			// Wavpack supports only 8, 16, and 24 bit integer and 32 bit float samples.
			isGivenFormatOK = false;
			audioFormat.SetSampleType(kAudioSampleType_SInt24);
			break;
		}

		asdk::int64 totalInputBytes = audioFormat.GetSampleTotal() * bytesPerSample * audioFormat.GetChannelCount();
		double sizeFactorGuess = privateSettings.GetEstimatedCompressionFactor();
		asdk::int64 estimatedSize = static_cast<asdk::int64>(sizeFactorGuess * totalInputBytes);

		// When estimating the size, take into consideration the metadata.
		for(asdk::int32 metadataIndex = 0; metadataIndex < audioFormat.GetMetadataItemCount(); metadataIndex++)
		{
			amio::AsciiString metadataTypeID;
			asdk::int64 metadataSize = 0;
			double locationHint;

			if (audioFormat.GetMetadataItemInfo(metadataIndex, metadataTypeID, metadataSize, locationHint))
			{
				estimatedSize += metadataSize;
			}
		}

		inSettingsInfo.SetEstimatedFileSize(estimatedSize);
		// audioFormat.SetMimeType("audio/x-ape");	// Since these files will probably most often be used in their decompressed form, 
		// and the decompressor is not smart enough by itself to change the mime type, let's call it wav rather than ape.
		audioFormat.SetMimeType("audio/x-wav");

		audioFormat.SetXmpHandlerId(kXMP_UnknownFile);
		asdk::uint32 fileFlags = amio::kAmioFileFlag_XmpSupportThroughPluginOnly | amio::kAmioFileFlag_WriteXmpMetadataBeforeSamples;

		// kAmioFileFlag_DisableSaveOptimization was introduced in AmioInterfaceVersion 1.2 (and we want to use it)

		if (mHostInterfaceVersionMajor > 1 || mHostInterfaceVersionMinor >= 2)
			fileFlags |= amio::kAmioFileFlag_DisableSaveOptimization;

		audioFormat.SetFileFlags(fileFlags);

		amio::UTF16String desc = amio::utils::AsciiToUTF16("WavPack ") + sampleDepth + amio::utils::AsciiToUTF16("\n") + 
				privateSettings.GetCompressionQualityString();
					
		inSettingsInfo.SetSettingsDescriptionString(desc.c_str());

		if (isGivenFormatOK || (inSettingsInfo.GetFixFormatParameter() != AmioGetExportSettingsInfoInterface::kFixFormatParameter_FixAll))
		{
			return kAmioInterfaceReturnCode_Success;
		}
		return kAmioInterfaceReturnCode_UnsupportedFormat;
	}

	///
	/// There may be settings that are specific to this file type.  Here the user can select them before the file is written.
	///
	virtual AmioResult RunExportSettingsDialog(AmioFormatInterface& inFormat, void* inParentWindow, asdk::int64 inFlags, bool &outCancelled)
	{
		AmioWavpackPrivateSettings privateSettings;
		amio::UTF16String privateSettingsString(inFormat.GetPrivateFormatData());
		privateSettings.InitializeFromSerialized(privateSettingsString);

		AmioWavpackSettingsDialog dialog(privateSettings);
		outCancelled = true;
		if (dialog.RunModal(reinterpret_cast<AmioWavpackSettingsDialog::PlatformWindow>(inParentWindow)))
		{
			inFormat.SetPrivateFormatData(privateSettings.GetSerialized().c_str());
			outCancelled = false;
		}
		return kAmioInterfaceReturnCode_Success;
	}

	///
	/// Now that all the settings to be used for writing have been decided, we actually begin the write operation.
	///
	virtual AmioResult WriteStart(AmioWriteStartInterface& inWriteStart, WavpackWriter*& outWriteFile)
	{
		amio::UTF16String fileName = inWriteStart.GetFileName();
		const AmioFormatInterface& amioFormat = inWriteStart.GetFormat();
		outWriteFile = NULL;

		AmioWavpackPrivateSettings privateSettings;
		amio::UTF16String privateSettingsString(amioFormat.GetPrivateFormatData());
		privateSettings.InitializeFromSerialized(privateSettingsString);

		int bytesPerSample = 0;
		switch (amioFormat.GetSampleType())
		{
		case kAudioSampleType_UInt8:
			bytesPerSample = 1;
			break;
		case kAudioSampleType_SInt16:
			bytesPerSample = 2;
			break;
		case kAudioSampleType_SInt24:
			bytesPerSample = 3;
			break;
		case kAudioSampleType_Float32:
			bytesPerSample = 4;
			break;
		default:
			return kAmioInterfaceReturnCode_UnsupportedFormat;
		}

        int numChannels = (int) amioFormat.GetChannelCount();
        int channelMask = 0, definedChannels = 0, i;

        // First, go though the channels and determine how many are MS defined (and not duplicates)
        // and create the channelMask that we will use

        for (i = 0; i < numChannels; ++i) {
            char msg [80];
            AudioChannelLabel speaker = amioFormat.GetSpeakerAssignment (i);
            sprintf (msg, "GetSpeakerAssignment (%d) = %d\n", i, speaker);
            OutputDebugStringA (msg);

			if (speaker >= kAudioChannelLabel_FrontLeft && speaker <= kAudioChannelLabel_TopBackCenter) {
				unsigned int mask_bit = 1 << (AmioChannelTable [speaker - kAudioChannelLabel_FrontLeft] - 1);

				if (!(channelMask & mask_bit)) {
					channelMask |= mask_bit;
					definedChannels++;
				}
			}
        }

        // These arrays will specify the destination position for each of the input channels,
		// and the final identities of the [possibly] re-ordered channels. Note that the
		// definedChannels that we counted above come first (channel-mask, first-occurrence),
		// then any others (duplicates and non-channel-mask) come after that.

		unsigned char *channelIdentities = new unsigned char [numChannels+1];
        unsigned char *channelOrder = new unsigned char [numChannels];
        int channelMask2ndPass = 0;

        // Loop through the channels once more to determine where each of them will go when we save

        for (i = 0; i < numChannels; ++i) {
            AudioChannelLabel speaker = amioFormat.GetSpeakerAssignment (i);

			if (speaker >= kAudioChannelLabel_FrontLeft && speaker <= kAudioChannelLabel_TopBackCenter) {
				unsigned int mask_bit = 1 << (AmioChannelTable [speaker - kAudioChannelLabel_FrontLeft] - 1);

				// channel is Microsoft, but make sure it's not a duplicate

				if (channelMask2ndPass & mask_bit) {
					channelIdentities [definedChannels] = AmioChannelTable [speaker - kAudioChannelLabel_FrontLeft];
					channelOrder [i] = definedChannels++;
				}
				else {
					channelMask2ndPass |= mask_bit;		// this is to detect duplicates
                    channelOrder [i] = 0;

					while (mask_bit >>= 1)				// count earlier masked channels to determine order
						if (channelMask & mask_bit)
                            channelOrder [i]++;

					// now we know where it's going, so we can store the identity
					channelIdentities [channelOrder [i]] = AmioChannelTable [speaker - kAudioChannelLabel_FrontLeft];
				}
			}
			else {	// channel is non-Microsoft, so we know it's going at the end with the "undefind's"
				if (speaker < kAudioChannelLabel_FrontLeft + NUM_AMIO_CHANNELS)
					channelIdentities [definedChannels] = AmioChannelTable [speaker - kAudioChannelLabel_FrontLeft];
				else
					channelIdentities [definedChannels] = 255;	// outside of our table (really should not happen)

				channelOrder [i] = definedChannels++;
			}
        }

		if (definedChannels != numChannels)
			OutputDebugStringA ("*** definedChannels != numChannels ***\n");

		channelIdentities [definedChannels] = 0;	// channelIdentities is NULL terminated

        // Finally, if there's actually no re-ordering to do (vast majority of cases), then pass a NULL
        // array to Initialize() so we don't waste a lot of time doing nothing.

        for (i = 0; i < numChannels; ++i)
            if (channelOrder [i] != i)
                break;

        if (i == numChannels) {
            delete [] channelOrder;
            channelOrder = NULL;
            OutputDebugStringA ("WriteStart: channels are NOT being re-ordered\n");
        }
        else
            OutputDebugStringA ("WriteStart: channels ARE being re-ordered\n");

		WavpackWriter::ExtendedError extError;
		WavpackWriter *theFile = new WavpackWriter;
		if (theFile->Initialize(fileName,
			amioFormat.GetSampleRate(),
			numChannels,
			bytesPerSample,
			amioFormat.GetSampleTotal(),
			channelMask, channelIdentities, channelOrder,
            privateSettings.GetCompressionLevel (),
			extError))
                {
                    // These things must be set because when doing a save as, Audition will display and use this information just as if the file were freshly opened.
                    inWriteStart.SetFileFlags(amio::kAmioFileFlag_XmpSupportThroughPluginOnly | amio::kAmioFileFlag_WriteXmpMetadataBeforeSamples);
                    amio::UTF16String formatDescription = amio::utils::AsciiToUTF16("WavPack ") + privateSettings.GetCompressionQualityString();
                    inWriteStart.SetAudioFormatDescriptionString(formatDescription.c_str());

                    outWriteFile = theFile;
                    delete [] channelOrder;
                    return kAmioInterfaceReturnCode_Success;
                }

        delete theFile;
        delete [] channelIdentities;
        if (channelOrder) delete [] channelOrder;

		amio::UTF16String errorText;
		switch (extError)
		{
		case WavpackWriter::kExtendedError_FileCreate:
			errorText = amio::utils::AsciiToUTF16("Unable to create ") + fileName;
			SetErrorString(errorText.c_str());
			break;
		default:
			errorText = amio::utils::AsciiToUTF16("Error creating WavPack file ") + fileName;
			SetErrorString(errorText.c_str());
			break;
		}
		return kAmioInterfaceReturnCode_GeneralError;
	}


	///
	/// Here is where the audio samples are passed in from Audition for writing.
	///
	virtual AmioResult WriteSamples(WavpackWriter& inWriteFile, AmioWriteSamplesInterface& inWriteInterface)
	{
		asdk::int32 channelCount = inWriteFile.GetNumChannels();
		asdk::int32 bytesPerSample = inWriteFile.GetBytesPerSample();
		asdk::int32 sampleCount = static_cast<asdk::int32>(inWriteInterface.GetSampleCount());
		asdk::int64 bufferSize = static_cast<asdk::int64>(sampleCount) * channelCount * 4;

		if ((bufferSize >= 0x80000000) || (channelCount < 1))
		{
			return kAmioInterfaceReturnCode_ParameterOutOfRange;
		}
		std::auto_ptr<asdk::int32> processedBuffer;
		processedBuffer.reset(new asdk::int32[sampleCount * channelCount]);

		// Process and interleave.
		for (asdk::int32 channelIndex = 0; channelIndex < channelCount; channelIndex++)
		{
			void *writeBuffer = inWriteInterface.GetBufferAddress(channelIndex);

			switch(inWriteFile.GetBytesPerSample())
			{
			case 1:
				Interleave<asdk::uint8>(writeBuffer, processedBuffer.get()+channelIndex, static_cast<asdk::uint32>(inWriteInterface.GetSampleCount()), channelCount, -128);
				break;
			case 2:
				Interleave<asdk::int16>(writeBuffer, processedBuffer.get()+channelIndex, static_cast<asdk::uint32>(inWriteInterface.GetSampleCount()), channelCount);
				break;
			case 3:
				Interleave24Bits(writeBuffer, processedBuffer.get()+channelIndex, static_cast<asdk::uint32>(inWriteInterface.GetSampleCount()), channelCount);
				break;
			case 4:
				Interleave<asdk::int32>(writeBuffer, processedBuffer.get()+channelIndex, static_cast<asdk::uint32>(inWriteInterface.GetSampleCount()), channelCount);
				break;
			default:
				return kAmioInterfaceReturnCode_UnsupportedFormat;
			}
		}

		// Write to the file
		asdk::int64 startByte = inWriteInterface.GetStartSample() * bytesPerSample * channelCount;
		if (inWriteFile.WriteSampleBytes(startByte, sampleCount, processedBuffer.get()))
		{
			return kAmioInterfaceReturnCode_Success;
		}

		SetErrorString(amio::utils::AsciiToUTF16("Error writing samples to the file").c_str());

		return kAmioInterfaceReturnCode_GeneralError;
	}

	///
	/// This is the completion of a file being written.
	///
	virtual AmioResult FinishWrite(WavpackWriter& inWriteFile, bool inIsCancel, const AmioFormatInterface& inFormat)
	{
		asdk::int32 result = kAmioInterfaceReturnCode_Success;

		if (!inWriteFile.FlushWrite())
			result = kAmioInterfaceReturnCode_GeneralError;
			
		// loop through all the metadata items, passing them to the riff "wrapper" destination, or to APEv2 tags

		for(asdk::int32 metadataIndex = 0; metadataIndex < inFormat.GetMetadataItemCount(); metadataIndex++)
		{
			amio::AsciiString metadataTypeID;
			asdk::int64 metadataSize = 0;
			double locationHint;

			if (result != kAmioInterfaceReturnCode_Success)
				break;

			if (inFormat.GetMetadataItemInfo(metadataIndex, metadataTypeID, metadataSize, locationHint))
			{
				bool isXMP = false, isGenericWave = false, isTextTag = false, isBinaryTag = false;

				// use the GUID to figure out what kinf of metadata it is

				if (!strnicmp(kAmioMetadataTypeID_XMP, metadataTypeID.c_str(), metadataTypeID.size()))
				{
					isXMP = true;
				}
				else if (!strnicmp(kAmioMetadataTypeID_BinaryTagMetadataChunk, metadataTypeID.c_str(), metadataTypeID.size()))
				{
					isBinaryTag = true;
				}
				else if (!strnicmp(kAmioMetadataTypeID_TextTagMetadataChunk, metadataTypeID.c_str(), metadataTypeID.size()))
				{
					isTextTag = true;
				}
				else if (!strnicmp(kAmioMetadataTypeID_GenericWaveMetadataChunk, metadataTypeID.c_str(), metadataTypeID.size()))
				{
					isGenericWave = true;
				}
				else
				{
					char msg [256];

					sprintf (msg, "FinishWrite: unknown ID %s, %d bytes, skipping\n", metadataTypeID.c_str(), metadataSize);
					OutputDebugStringA (msg);
					continue;
				}

				if (isXMP)		// for XMP_, we must send an appropriate RIFF header first
				{
					char xmp_header [] = "_PMX\0\0\0\0";

					xmp_header [4] = (char) metadataSize;
					xmp_header [5] = (char) (metadataSize >> 8);
					xmp_header [6] = (char) (metadataSize >> 16);
					xmp_header [7] = (char) (metadataSize >> 24);

					if (!inWriteFile.AddRiffMetadata (xmp_header, 8))
						result = kAmioInterfaceReturnCode_GeneralError;
				}

				// read the metadata and pad it with a zero byte (for odd riff sizes)

				char *metadataBuffer = new char [(unsigned int) metadataSize + 1];
				inFormat.GetMetadataItem (metadataIndex, metadataBuffer, 0, metadataSize);
				metadataBuffer [metadataSize] = 0;

				if (isXMP || isGenericWave) {
					if (!inWriteFile.AddRiffMetadata (metadataBuffer, (int) (metadataSize + (metadataSize & 1))))
						result = kAmioInterfaceReturnCode_GeneralError;
				}

				if (isTextTag || isBinaryTag) {
					if (!inWriteFile.AddTagMetadata (metadataBuffer, (int) metadataSize, isBinaryTag))
						result = kAmioInterfaceReturnCode_GeneralError;
				}

				delete metadataBuffer;
			}
		}

		if (!inWriteFile.FinishWrite(inIsCancel))
			result = kAmioInterfaceReturnCode_GeneralError;

		delete &inWriteFile;

		if (result != kAmioInterfaceReturnCode_Success)
		{
			SetErrorString(amio::utils::AsciiToUTF16("Error completing file writing").c_str());
		}
		return result;
	}
};

///
/// This is actually the only connection point between the plug-in and Audition.
/// All commands come through this one entry point.
///
extern "C" AMIO_EXPORT AmioResult AmioInterface(asdk::int32 inCommand, void* inState, void* inInterface)
{
	AmioWavpackInterface wavpackInterface;
	return wavpackInterface.EntryPoint(inCommand, inState, inInterface);
}

} // namespace amio
