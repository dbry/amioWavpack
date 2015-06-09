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

#ifndef AMIO_AMIOSDK_TYPES_H
#define AMIO_AMIOSDK_TYPES_H

#include "../../ASDKTypes.h"

#if defined(ASDK_OS_WIN)
	#define AMIO_OS_WIN
#elif defined(ASDK_OS_MAC)
	#define AMIO_OS_MAC
#else
	#error "unrecognized OS platform"
#endif

#ifdef AMIO_OS_WIN

	#ifndef WINVER
		#define WINVER			0x0501	// WinXP minimum
	#endif
	#define AMIO_EXPORT

#elif defined(AMIO_OS_MAC)

	#define AMIO_EXPORT		__attribute__((visibility("default")))
	#include <string>

#else
	#error "unrecognized OS platform"
#endif

#include <vector>

namespace amio
{
	typedef asdk::uint16 UTF16Char;
	typedef asdk::uint8 UTF8Char;
	
	typedef std::allocator<UTF8Char> UTF8CharAllocator;
	typedef std::allocator<UTF16Char> UTF16CharAllocator;
	
	typedef std::basic_string<UTF16Char, std::char_traits<UTF16Char>, UTF16CharAllocator> UTF16String;
	typedef std::basic_string<UTF8Char, std::char_traits<UTF8Char>, UTF8CharAllocator> UTF8String;
	
	enum AudioSampleType
	{
		kAudioSampleType_Unknown			= 0,	// Uninitialized
		kAudioSampleType_UInt8				= 10,	// 8 bit unsigned integer.
		kAudioSampleType_SInt16				= 20,	// 16 bit signed integer, little endian.
		kAudioSampleType_SInt24				= 30,	// 24 bit signed integer, little endian, packed.
		kAudioSampleType_Float32			= 40,	// 32 bit floating point, range -1.0 to 1.0 
	};
	
	typedef asdk::int32 AudioSampleRate;	// Samples per second.
	
	enum VideoPixelType
	{
		kVideoPixelType_Unknown				= 0,	// Uninitialized
		kVideoPixelType_ARGB32				= 1,	// ARGB, 8 bits per component, 32 bits per pixel
		kVideoPixelType_YUV_1				= 2,	// YUV flavor 1
	};
	
	enum AudioChannelLabel
	{
		// Normally, a channel label corresponds (roughly) to a speaker position.
		// Other more generic channel labels may be defined in the future.
		kAudioChannelLabel_Discrete					= 0,	// No particular speaker: (e.g. the channel is a discrete mono channel)

		// Amio												   WAVEFORMATEXTENSIBLE				Apple CoreAudio								VST 3
		// ----------------------------------------------	   -----------------------------	---------------------------------------		------------------
		kAudioChannelLabel_FrontLeft				= 100,	// SPEAKER_FRONT_LEFT				kAudioChannelLabel_Left						kSpeakerL
		kAudioChannelLabel_FrontRight				= 101,	// SPEAKER_FRONT_RIGHT				kAudioChannelLabel_Right					kSpeakerR
		kAudioChannelLabel_FrontCenter				= 102,	// SPEAKER_FRONT_CENTER				kAudioChannelLabel_Center					kSpeakerC
		kAudioChannelLabel_LowFrequency				= 103,	// SPEAKER_LOW_FREQUENCY			kAudioChannelLabel_LFEScreen				kSpeakerLfe
		kAudioChannelLabel_BackLeft					= 104,	// SPEAKER_BACK_LEFT				kAudioChannelLabel_LeftSurround				kSpeakerLs
		kAudioChannelLabel_BackRight				= 105,	// SPEAKER_BACK_RIGHT				kAudioChannelLabel_RightSurround			kSpeakerRs		
		kAudioChannelLabel_BackCenter				= 106,	// SPEAKER_BACK_CENTER				kAudioChannelLabel_CenterSurround			kSpeakerCs = kSpeakerS
		kAudioChannelLabel_FrontLeftOfCenter		= 107,	// SPEAKER_FRONT_LEFT_OF_CENTER		kAudioChannelLabel_LeftCenter				kSpeakerLc
		kAudioChannelLabel_FrontRightOfCenter		= 108,	// SPEAKER_FRONT_RIGHT_OF_CENTER	kAudioChannelLabel_RightCenter				kSpeakerRc
		kAudioChannelLabel_SideLeft					= 109,	// SPEAKER_SIDE_LEFT				kAudioChannelLabel_LeftSurroundDirect		kSpeakerSl
		kAudioChannelLabel_SideRight				= 110,	// SPEAKER_SIDE_RIGHT				kAudioChannelLabel_RightSurroundDirect		kSpeakerSr
		kAudioChannelLabel_TopCenter				= 111,	// SPEAKER_TOP_CENTER				kAudioChannelLabel_TopCenterSurround		kSpeakerTm
		kAudioChannelLabel_TopFrontLeft				= 112,	// SPEAKER_TOP_FRONT_LEFT			kAudioChannelLabel_VerticalHeightLeft		kSpeakerTfl
		kAudioChannelLabel_TopFrontRight			= 113,	// SPEAKER_TOP_FRONT_RIGHT			kAudioChannelLabel_VerticalHeightRight		kSpeakerTfr
		kAudioChannelLabel_TopFrontCenter			= 114,	// SPEAKER_TOP_FRONT_CENTER			kAudioChannelLabel_VerticalHeightCenter		kSpeakerTfc
		kAudioChannelLabel_TopBackLeft				= 115,	// SPEAKER_TOP_BACK_LEFT			kAudioChannelLabel_TopBackLeft				kSpeakerTrl
		kAudioChannelLabel_TopBackRight				= 116,	// SPEAKER_TOP_BACK_RIGHT			kAudioChannelLabel_TopBackRight				kSpeakerTrr
		kAudioChannelLabel_TopBackCenter			= 117,	// SPEAKER_TOP_BACK_CENTER			kAudioChannelLabel_TopBackCenter			kSpeakerTrc
		kAudioChannelLabel_FrontLeftWide			= 118,	//									kAudioChannelLabel_LeftWide 
		kAudioChannelLabel_FrontRightWide			= 119,	//									kAudioChannelLabel_RightWide
		// Reserved - DO NOT USE					  120-126
		kAudioChannelLabel_LeftHeight				= 127,	//																				kSpeakerTsl
		kAudioChannelLabel_RightHeight				= 128,	//																				kSpeakerTsr
		kAudioChannelLabel_RearSurroundLeft 		= 129,	//									kAudioChannelLabel_RearSurroundLeft			kSpeakerLcs
		kAudioChannelLabel_RearSurroundRight		= 130, 	//									kAudioChannelLabel_RearSurroundRight		kSpeakerRcs
		kAudioChannelLabel_LFE2						= 131,	//									kAudioChannelLabel_LFE2						kSpeakerLfe2
		kAudioChannelLabel_LeftTotal				= 132,	//									kAudioChannelLabel_LeftTotal 
		kAudioChannelLabel_RightTotal				= 133,	//									kAudioChannelLabel_RightTotal 
		kAudioChannelLabel_Ambisonic_W				= 134,	//									kAudioChannelLabel_Ambisonic_W				kSpeakerW
		kAudioChannelLabel_Ambisonic_X				= 135,	//									kAudioChannelLabel_Ambisonic_X				kSpeakerX
		kAudioChannelLabel_Ambisonic_Y				= 136,	//									kAudioChannelLabel_Ambisonic_Y				kSpeakerY
		kAudioChannelLabel_Ambisonic_Z				= 137,	//									kAudioChannelLabel_Ambisonic_Z				kSpeakerZ
	};
	
	struct AudioChannelDescription
	{
		AudioChannelDescription(AudioChannelLabel inLabel)
		: mChannelLabel(inLabel)
		{
		}
		inline bool operator == (const AudioChannelDescription& inRHS) const
		{
			return mChannelLabel == inRHS.mChannelLabel;
		}
		inline bool operator != (const AudioChannelDescription& inRHS) const
		{
			return mChannelLabel != inRHS.mChannelLabel;
		}
		inline bool operator < (const AudioChannelDescription& inRHS) const
		{
			return mChannelLabel < inRHS.mChannelLabel;
		}
		AudioChannelLabel mChannelLabel;
		// More advanced positioning and description of the speaker placement will appear here in the future.
	};
	
	typedef size_t AudioChannelCount;	// The total number of channels.
	typedef size_t AudioChannelIndex;	// A zero-based index into the total number of channels.
	
	// This channel layout describes the assignment of channels by giving an AudioChannelDescription for each AudioChannelIndex.
	typedef std::vector<AudioChannelDescription> AudioChannelLayout;
	
	/// File format constants for use with XMPFiles.
	enum {
		
		// --------------------
		// Public file formats.
		
		/// Public file format constant: 'PDF '
		kXMP_PDFFile             = 0x50444620UL,
		/// Public file format constant: 'PS  ', general PostScript following DSC conventions
		kXMP_PostScriptFile      = 0x50532020UL,
		/// Public file format constant: 'EPS ', encapsulated PostScript
		kXMP_EPSFile             = 0x45505320UL,
		
		/// Public file format constant: 'JPEG'
		kXMP_JPEGFile            = 0x4A504547UL,
		/// Public file format constant: 'JPX ', JPEG 2000, ISO 15444-1
		kXMP_JPEG2KFile          = 0x4A505820UL,
		/// Public file format constant: 'TIFF'
		kXMP_TIFFFile            = 0x54494646UL,
		/// Public file format constant: 'GIF '
		kXMP_GIFFile             = 0x47494620UL,
		/// Public file format constant: 'PNG '
		kXMP_PNGFile             = 0x504E4720UL,
		
		/// Public file format constant: 'SWF '
		kXMP_SWFFile             = 0x53574620UL,
		/// Public file format constant: 'FLA '
		kXMP_FLAFile             = 0x464C4120UL,
		/// Public file format constant: 'FLV '
		kXMP_FLVFile             = 0x464C5620UL,
		
		/// Public file format constant: 'MOV ', Quicktime
		kXMP_MOVFile             = 0x4D4F5620UL,
		/// Public file format constant: 'AVI '
		kXMP_AVIFile             = 0x41564920UL,
		/// Public file format constant: 'CIN ', Cineon
		kXMP_CINFile             = 0x43494E20UL,
 		/// Public file format constant: 'WAV '
		kXMP_WAVFile             = 0x57415620UL,
		/// Public file format constant: 'MP3 '
		kXMP_MP3File             = 0x4D503320UL,
		/// Public file format constant: 'SES ', Audition session
		kXMP_SESFile             = 0x53455320UL,
		/// Public file format constant: 'CEL ', Audition loop
		kXMP_CELFile             = 0x43454C20UL,
		/// Public file format constant: 'MPEG'
		kXMP_MPEGFile            = 0x4D504547UL,
		/// Public file format constant: 'MP2 '
		kXMP_MPEG2File           = 0x4D503220UL,
		/// Public file format constant: 'MP4 ', ISO 14494-12 and -14
		kXMP_MPEG4File           = 0x4D503420UL,
		/// Public file format constant: 'WMAV', Windows Media Audio and Video
		kXMP_WMAVFile            = 0x574D4156UL,
		/// Public file format constant:  'AIFF'
		kXMP_AIFFFile            = 0x41494646UL,
		/// Public file format constant:  'P2  ', a collection not really a single file
		kXMP_P2File              = 0x50322020UL,
		/// Public file format constant:  'XDCF', a collection not really a single file
		kXMP_XDCAM_FAMFile       = 0x58444346UL,
		/// Public file format constant:  'XDCS', a collection not really a single file
		kXMP_XDCAM_SAMFile       = 0x58444353UL,
		/// Public file format constant:  'XDCX', a collection not really a single file
		kXMP_XDCAM_EXFile        = 0x58444358UL,
		/// Public file format constant:  'AVHD', a collection not really a single file
		kXMP_AVCHDFile           = 0x41564844UL,
		/// Public file format constant:  'SHDV', a collection not really a single file
		kXMP_SonyHDVFile         = 0x53484456UL,
		/// Public file format constant:  'CNXF', a collection not really a single file
		kXMP_CanonXFFile         = 0x434E5846UL,
		
		/// Public file format constant: 'HTML'
		kXMP_HTMLFile            = 0x48544D4CUL,
		/// Public file format constant: 'XML '
		kXMP_XMLFile             = 0x584D4C20UL,
		/// Public file format constant:  'text'
		kXMP_TextFile            = 0x74657874UL,
		
		// -------------------------------
		// Adobe application file formats.
		
		/// Adobe application file format constant: 'PSD '
		kXMP_PhotoshopFile       = 0x50534420UL,
		/// Adobe application file format constant: 'AI  '
		kXMP_IllustratorFile     = 0x41492020UL,
		/// Adobe application file format constant: 'INDD'
		kXMP_InDesignFile        = 0x494E4444UL,
		/// Adobe application file format constant: 'AEP '
		kXMP_AEProjectFile       = 0x41455020UL,
		/// Adobe application file format constant: 'AET ', After Effects Project Template
		kXMP_AEProjTemplateFile  = 0x41455420UL,
		/// Adobe application file format constant: 'FFX '
		kXMP_AEFilterPresetFile  = 0x46465820UL,
		/// Adobe application file format constant: 'NCOR'
		kXMP_EncoreProjectFile   = 0x4E434F52UL,
		/// Adobe application file format constant: 'PRPJ'
		kXMP_PremiereProjectFile = 0x5052504AUL,
		/// Adobe application file format constant: 'PRTL'
		kXMP_PremiereTitleFile   = 0x5052544CUL,
		/// Adobe application file format constant: 'UCF ', Universal Container Format
		kXMP_UCFFile             = 0x55434620UL,
		
		// -------
		// Others.
		
		/// Unknown file format constant: '    '
		kXMP_UnknownFile         = 0x20202020UL
		
	};
	
	// List template designed to be binary-compatible with those used by Audition CS 5.5 Amio plug-ins.
	template<class T> class LegacyList
	{	
	public:
		
		struct Node
		{
			struct Node* mNext;
			struct Node* mPrev;
			T mVal;
		};
		
		class const_iterator
		{
		public:
			///
			const_iterator(const struct Node* inPtr)
			{
				mPtr = inPtr;
			}
			///
			const T& operator*() const
			{
				return mPtr->mVal;
			}
			///
			bool operator!=(const const_iterator& inRight) const
			{
				return (mPtr != inRight.mPtr);
			}
			/// preincrement
			const_iterator& operator++()
			{
				mPtr = mPtr->mNext;
				return *this;
			}
			
		protected:
			const struct Node* mPtr;
		};
		
		///
		LegacyList()
		{
#ifdef AMIO_OS_WIN
			mBase = new struct Node;
			reserved = 0;
			mSize = 0;
#endif			
			GetBase()->mNext = GetBase()->mPrev = GetBase();
		}
		//
		~LegacyList()
		{
			clear();
#ifdef AMIO_OS_WIN
			delete mBase;
#endif
		}
		///
		void clear()
		{
			for (struct Node* it = GetBase()->mNext; it != GetBase();)
			{
				struct Node* next = it->mNext;
				delete it;
				it = next;
			}
#ifdef AMIO_OS_WIN
			mSize = 0;
#endif
		}
		///
		void push_back(const T& inVal)
		{
			struct Node* oldEnd = GetBase()->mPrev;
			struct Node* newEnd = new struct Node;
			newEnd->mNext = GetBase();
			newEnd->mPrev = oldEnd;
			newEnd->mVal = inVal;
			oldEnd->mNext = newEnd;
			GetBase()->mPrev = newEnd;
#ifdef AMIO_OS_WIN
			mSize++;
#endif
		}
		///
		const_iterator begin() const
		{
			return (const_iterator(GetFirstNode()));
		}
		///
		const_iterator end() const
		{
			return (const_iterator(GetBaseConst()));
		}
		///
		asdk::int32 size() const
		{
#ifdef AMIO_OS_WIN
			return mSize;
#elif defined(ASDK_OS_MAC)
			asdk::int32 sizeValue = 0;
			for (struct Node* it = GetBase()->mNext; it != GetBase(); sizeValue++)
				;
			return sizeValue;	
#endif			
		}
		///
		bool empty() const
		{
			return GetBase() == GetBase()->mNext;
		}
		
	protected:
#ifdef AMIO_OS_WIN
		asdk::int32 reserved;	// For binary compatibility.  Do not remove.
		struct Node* mBase;
		asdk::int32 mSize;
		inline struct Node* GetBase() { return mBase; }
		inline const struct Node* GetBaseConst() const { return mBase; }
		inline const struct Node* GetFirstNode() const { return mBase->mNext; }
#elif defined(ASDK_OS_MAC)
		struct Node mBase;
		inline struct Node* GetBase() { return &mBase; }
		inline const struct Node* GetBaseConst() const { return &mBase; }
		inline const struct Node* GetFirstNode() const { return mBase.mNext; }
#endif		
	};
	typedef LegacyList<const char*> ExtensionList;
	
	
#ifdef AMIO_OS_WIN
	// string holder designed to be binary-compatible with the one used by Audition CS 5.5 Amio plug-ins.
	template<class T> class LegacyString
	{
	public:
		///
		LegacyString()
		{
			initialize();
		}
		///
		LegacyString(const T *inStr)
		{
			initialize();
			assign(inStr);
		}
		///
		~LegacyString()
		{
			delete mBuffer;
		}
		///
		void clear()
		{
			mStringSize = 0;
		}
		///
		bool empty()
		{
			return mStringSize == 0;
		}
		///
		asdk::int32 size()
		{
			return mStringSize;
		}
		///
		const T* c_str() const
		{
			return mBuffer;
		}
		///
		void operator=(const LegacyString& inRight)
		{
			assign(inRight.c_str());
		}
		///
		void assign(const T* inStr)
		{
			const T* in = inStr;
			T* out = mBuffer;
			for (mStringSize = 0; mStringSize < mBufferSize; mStringSize++)
			{
				if (!(*out++ = *in++))
					return;
			}
			// Have to allocate more memory.
			for (;*in++ != 0; mStringSize++)
			{
			}
			mBuffer = new T[mBufferSize = mStringSize + 1];
			assign(inStr);
		}
	protected:
		void initialize()
		{
			reserved1 = reserved2[0] = reserved2[1] = reserved2[3] = 0;
			mBuffer = new T[mBufferSize = 256];
			mBuffer[0] = 0;
			mStringSize = 0;
		}
		
		asdk::int32 reserved1;
		T* mBuffer;
		asdk::int32 reserved2[3];
		asdk::int32 mStringSize;
		asdk::int32 mBufferSize;
	};
	typedef LegacyString<char> AsciiString;
#else
	typedef std::string AsciiString;	
#endif
} // namespace amio

#endif // AMIO_AMIOSDK_TYPES_H

