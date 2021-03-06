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

#include "AmioSDK.h"
#include "AmioUtilities.h"

#include "windows.h"    // for OutputDebugStringA ()

#include "../Wavpack/include/wavpack.h"
#include "WavpackWriter.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

namespace {

    class write_helper {
        public:
            write_helper () : block_data (NULL) { };
            ~write_helper () { close (); }

            void close () {
                if (os.is_open ())
                    os.close ();

                if (block_data) {
                    delete [] block_data;
                    block_data = NULL;
                }
            }

#ifdef AMIO_OS_WIN
            int open (const amio::UTF16String& fname) {
                close ();
                first_block_size = error = 0;

                os.open (reinterpret_cast<const wchar_t *>(fname.c_str()),
                    ios_base::in | ios_base::out | ios_base::trunc | ios_base::binary);

                return os.is_open ();
            }
#else
            int open (const amio::UTF16String& fname) {
                close ();
                first_block_size = error = 0;

                amio::UTF8String fnameUTF8 = amio::utils::UTF16StringtoUTF8String (fname);
                os.open (reinterpret_cast<const char *>(fnameUTF8.c_str()),
                    ios_base::in | ios_base::out | ios_base::trunc | ios_base::binary);

                return os.is_open ();
            }
#endif
            int write_block (void *data, int32_t bcount) {
                if (!os.is_open () || error) return 0;

                if (data && bcount) {
                    os.write (reinterpret_cast<char*>(data), bcount);
                    error = os.bad ();

                    if (!first_block_size)
                        first_block_size = bcount;
                }

                return !error;
            }

            char *get_first_block () {
                if (!os.is_open () || error || !first_block_size) return NULL;
                if (!block_data) {
                    block_data = new char [first_block_size];
                    os.seekp (0);
                    os.read (block_data, first_block_size);
                    if (os.fail ()) {
                        delete [] block_data;
                        block_data = NULL;
                    }
                }
                return block_data;
            }

            void put_back_first_block () {
                if (os.is_open () && !error && block_data) {
                    os.seekp (0);
                    os.write (block_data, first_block_size);
                    error = os.bad ();
                }
            }

        private:
            uint32_t first_block_size;
            char *block_data;
            fstream os;
            int error;
    };

    int write_block_cb (void *id, void *data, int32_t bcount)
    {
        write_helper *whp = (write_helper *) id;
        return whp->write_block (data, bcount);
    }

    void reorder_channels (void *data, unsigned char *order, int num_chans,
        int num_samples, int bytes_per_sample)
    {
        char *temp = new char [num_chans * bytes_per_sample];
        char *src = (char *) data;

        while (num_samples--) {
            char *start = src;
            int chan;

            for (chan = 0; chan < num_chans; ++chan) {
                char *dst = temp + (order [chan] * bytes_per_sample);
                int bc = bytes_per_sample;

                while (bc--)
                    *dst++ = *src++;
            }

            memcpy (start, temp, num_chans * bytes_per_sample);
        }

        delete [] temp;
    }
} // private namespace

namespace amio
{
	class WavpackWriter::Impl
	{
	public:
		///
        Impl () : wpcx (NULL), wv_file (NULL), wvc_file (NULL), samples_requested (-1), samples_written (0), riff_metadata (0), channel_reorder (NULL)
        {
            memset (&wpconfig, 0, sizeof (wpconfig));
			mSampleRate = 0;
			mChannelCount = 0;
			mBytesPerSample = 0;
		}

		///
		~Impl()
		{
			Close();
		}

		///
		void Close()
		{
            if (wpcx) {
                WavpackCloseFile (wpcx);
                wpcx = NULL;
            }

            if (wv_file) {
                delete wv_file;
                wv_file = NULL;
            }

            if (wvc_file) {
                delete wvc_file;
                wvc_file = NULL;
            }

            if (channel_reorder) {
                delete [] channel_reorder;
                channel_reorder = NULL;
            }

			mSampleRate = 0;
			mChannelCount = 0;
			mBytesPerSample = 0;
		}

		bool Initialize(const amio::UTF16String& inFileName, 
			asdk::int32 inSampleRate,
			asdk::int32 inNumChannels,
			asdk::int32 inBytesPerSample,
			asdk::int64 inSampleCount,
			asdk::int32 inChannelMask,
            unsigned char *inChannelIdentities,
            unsigned char *inChannelReorder,
			asdk::int32 inCompressionMode,
			double inHybridBitsPerSample,
			ExtendedError& outExtendedError)
		{
			if (((inCompressionMode % 1000) - (inCompressionMode % 100)) &&	// if hybrid and wvc check
				((inCompressionMode % 100) - (inCompressionMode % 10))) {		// for tempfile use
					int fni, fnlen = (int) inFileName.length();

					for (fni = fnlen; fni > 0 && inFileName [fni-1] != '\\'; fni--);

					if (fnlen-fni >= 7 && inFileName[fni]=='~' && inFileName[fni+1]=='$' && isdigit (inFileName[fnlen-4])) {
						amio::UTF16String strippedFileName = inFileName;
						strippedFileName.erase(fnlen-4,1);
						strippedFileName.erase(fni,2);

						if (ifstream (strippedFileName.c_str()).good()) {
							OutputDebugStringA ("*** detected temporary file use ***\n");
							wvcFinalName = strippedFileName + L"c";
						}
					}

					wvcFileName = inFileName + L"c";
			}

            wv_file = new write_helper;

            if (!wv_file->open (inFileName)) {
                Close ();
                return 1;
            }

			if ((inCompressionMode % 1000) - (inCompressionMode % 100)) {		// if hybrid...
				wpconfig.bitrate = (float) inHybridBitsPerSample;
				wpconfig.flags |= CONFIG_HYBRID_FLAG;

				if ((inCompressionMode % 100) - (inCompressionMode % 10)) {	// if wvc...
					wpconfig.flags |= CONFIG_CREATE_WVC;
					wvc_file = new write_helper;

					if (!wvc_file->open (wvcFileName)) {
						Close ();
						return 1;
					}
				}
			}

            wpcx = WavpackOpenFileOutput (write_block_cb, wv_file, wvc_file);

            wpconfig.num_channels = inNumChannels;
            wpconfig.channel_mask = inChannelMask;

            if (inChannelReorder) {
                channel_reorder = new unsigned char [inNumChannels];
                memcpy (channel_reorder, inChannelReorder, inNumChannels);
            }

            wpconfig.sample_rate = inSampleRate;
            wpconfig.bytes_per_sample = inBytesPerSample;
            wpconfig.bits_per_sample = inBytesPerSample*8;

            if (inBytesPerSample == 4)
                wpconfig.float_norm_exp = 127;

            if (inCompressionMode / 1000 == 1)
                wpconfig.flags |= CONFIG_FAST_FLAG;
            else if (inCompressionMode / 1000 == 3)
                wpconfig.flags |= CONFIG_HIGH_FLAG;
            else if (inCompressionMode / 1000 == 4)
                wpconfig.flags |= CONFIG_VERY_HIGH_FLAG;

			if (inCompressionMode % 10) {										// if extra processing...
				wpconfig.xmode = inCompressionMode % 10;
				wpconfig.flags |= CONFIG_EXTRA_MODE;
			}

            WavpackSetConfiguration64 (wpcx, &wpconfig, inSampleCount, inChannelIdentities);
            WavpackPackInit (wpcx);

			mSampleRate = inSampleRate;
			mChannelCount = inNumChannels;
			mBytesPerSample = inBytesPerSample;
			samples_requested = inSampleCount;

			outExtendedError = kExtendedError_NoError;
			return true;
		}

		bool WriteSampleBytes(asdk::int64 inStartByte, asdk::int32 inSampleCount, void *inBuffer)
		{
            if (!wpcx)
                return false;

            if (channel_reorder)
                reorder_channels (inBuffer, channel_reorder, mChannelCount, inSampleCount, 4);

            samples_written += inSampleCount;
            return WavpackPackSamples (wpcx, reinterpret_cast<int32_t *>(inBuffer), inSampleCount) ? true : false;
		}

		/// Flush file being written. Use after all samples have been written or after RIFF data has been written
		bool FlushWrite(void)
		{
            if (!wpcx)
                return false;

			return WavpackFlushSamples (wpcx) ? true : false;
		}

		/// Add specified RIFF metadata to file as trailing "wrapper"
		bool AddRiffMetadata(void* inMetadataBuffer, int inMetadataBytes)
		{
            if (!wpcx)
                return false;

            if (inMetadataBuffer && inMetadataBytes) {
                char msg [256];
                sprintf (msg, "WavpackWriter::AddRiffMetadata() got %u bytes of metadata\n", inMetadataBytes);
                OutputDebugStringA (msg);

				riff_metadata += inMetadataBytes;
                return WavpackAddWrapper (wpcx, inMetadataBuffer, inMetadataBytes) ? true : false;
            }

			return true;
		}

		/// Add specified tag metadata to the file (either text or binary; goes in APEv2 tag)
		bool AddTagMetadata(void* inMetadataBuffer, int inMetadataBytes, bool inIsBinary)
		{
            if (!wpcx)
                return false;

            if (inMetadataBuffer && inMetadataBytes) {
				int item_len = (int) strlen ((char *) inMetadataBuffer);
                char msg [256];
                sprintf (msg, "WavpackWriter::AddTagMetadata() got %u bytes of %s metadata\n", inMetadataBytes, inIsBinary ? "binary" : "text");
                OutputDebugStringA (msg);

				if (inIsBinary)
					return WavpackAppendBinaryTagItem (wpcx, (char *) inMetadataBuffer, (char *) inMetadataBuffer + item_len + 1,
						inMetadataBytes - item_len - 1) ? true : false;
				else
					return WavpackAppendTagItem (wpcx, (char *) inMetadataBuffer, (char *) inMetadataBuffer + item_len + 1,
						inMetadataBytes - item_len - 1) ? true : false;
            }

			return true;
		}

		/// Finish and close the file being written, or cancel writing.
		bool FinishWrite(bool inCancelMode)
		{
            if (!wpcx)
                return false;

			if (inCancelMode) {
				OutputDebugStringA ("*** WavpackWriter::FinishWrite() called with cancel!! ***\n");
				Close ();

				// attempt to delete the wvc file (if we were writing one)

				if (wvcFileName.length())
					_wunlink (wvcFileName.c_str());

				return true;
			}

            WavpackFlushSamples (wpcx);
            WavpackWriteTag (wpcx);

            if (samples_requested != samples_written || riff_metadata) {
                if (wv_file && wv_file->get_first_block ()) {
                    WavpackUpdateNumSamples (wpcx, wv_file->get_first_block ());
                    wv_file->put_back_first_block ();
                }

                if (wvc_file && wvc_file->get_first_block ()) {
                    WavpackUpdateNumSamples (wpcx, wvc_file->get_first_block ());
                    wvc_file->put_back_first_block ();
                }
            }

			Close();

			// if we were writing a wvc file, and determined earlier that it must be a temporary file name
			// (i.e., "~$something1.wvc") then we must do the rename/overwrite here (because Audition won't).

			if (wvcFinalName.length()) {
				OutputDebugStringA ("*** WavpackWriter::FinishWrite() renaming wvc file ***\n");
				_wunlink (wvcFinalName.c_str());

				if (ifstream (wvcFinalName.c_str()).good())
					OutputDebugStringA ("*** could not delete wvc file to overwrite!! ***\n");
				else if (_wrename (wvcFileName.c_str(), wvcFinalName.c_str()))
					OutputDebugStringA ("*** error in renaming wvc file!! ***\n");
			}

			return true;
		}

	protected:
        WavpackContext *wpcx;
        write_helper *wv_file, *wvc_file;
        WavpackConfig wpconfig;
        asdk::int64 samples_requested, samples_written, riff_metadata;
		amio::UTF16String wvcFileName, wvcFinalName;
        unsigned char *channel_reorder;

	public:
		asdk::int32 mSampleRate;
		asdk::int32 mChannelCount;
		asdk::int32 mBytesPerSample;
	};


	///
	WavpackWriter::WavpackWriter()
	{
		mImpl = new WavpackWriter::Impl();
	}

	///
	WavpackWriter::~WavpackWriter()
	{
		delete mImpl;
	}


	bool WavpackWriter::Initialize(const amio::UTF16String& inFileName, 
		asdk::int32 inSampleRate,
		asdk::int32 inNumChannels,
		asdk::int32 inBytesPerSample,
		asdk::int64 inSampleCount,
		asdk::int32 inChannelMask,
		unsigned char *inChannelIdentities,
        unsigned char *inChannelReorder,
		asdk::int32 inCompressionMode,
		double inHybridBitsPerSample,
		ExtendedError& outExtendedError)
	{
		return mImpl->Initialize(inFileName, 
			inSampleRate, 
			inNumChannels, 
			inBytesPerSample, 
			inSampleCount, 
			inChannelMask,
            inChannelIdentities,
            inChannelReorder,
            inCompressionMode,
			inHybridBitsPerSample,
			outExtendedError);
	}

	///
	asdk::int32 WavpackWriter::GetSampleRate() const
	{
		return mImpl->mSampleRate;
	}

	///
	asdk::int32 WavpackWriter::GetNumChannels() const
	{
		return mImpl->mChannelCount;
	}

	///
	asdk::int32 WavpackWriter::GetBytesPerSample() const
	{
		return mImpl->mBytesPerSample;
	}

	/// The byte offset should be the total number of bytes provided prior to this call.
	bool WavpackWriter::WriteSampleBytes(asdk::int64 inStartByte, asdk::int32 inByteCount, void *inBuffer)
	{
		return mImpl->WriteSampleBytes(inStartByte, inByteCount, inBuffer);
	}

	/// Flush file being written. Use after all samples have been written or after RIFF data has been written
	bool WavpackWriter::FlushWrite(void)
	{
		return mImpl->FlushWrite();
	}

	/// Add specified data to the RIFF trailing wrapper written to file
	bool WavpackWriter::AddRiffMetadata(void* inMetadataBuffer, int inMetadataBytes)
	{
		return mImpl->AddRiffMetadata(inMetadataBuffer, inMetadataBytes);
	}

	/// Add specified tag metadata to the file (either text or binary; goes in APEv2 tag)
	bool WavpackWriter::AddTagMetadata(void* inMetadataBuffer, int inMetadataBytes, bool inIsBinary)
	{
		return mImpl->AddTagMetadata(inMetadataBuffer, inMetadataBytes, inIsBinary);
	}

	/// Finish and close the file being written, or cancel writing.
	bool WavpackWriter::FinishWrite(bool inCancelMode)
	{
		return mImpl->FinishWrite(inCancelMode);
	}

} //namespace amio

