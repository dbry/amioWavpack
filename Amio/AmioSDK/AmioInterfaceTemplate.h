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

#ifndef AMIO_AMIOINTERFACETEMPLATE_H
#define AMIO_AMIOINTERFACETEMPLATE_H

#include "AmioSDK.h"

namespace amio
{

template<class ReadT, class WriteT>
class AmioInterfaceTemplate
{
public:
	///
	AmioInterfaceTemplate()
	{
	}

	///
	virtual ~AmioInterfaceTemplate()
	{
	}

	///
	AmioResult EntryPoint(asdk::int32 inCommand, void* inState, void* inInterface)
	{
		SetErrorInterface(NULL);

		switch (inCommand)
		{

		case kAmioInterfaceCommand_GetAmioInfo:
		{
			AmioGetAmioInfoInterface* amioInfo = (AmioGetAmioInfoInterface*)inInterface;
			if (!amioInfo)
			{
				return kAmioInterfaceReturnCode_ParameterOutOfRange;
			}
			amioInfo->SetPluginInterfaceVersion(kAmioInterfaceVersionMajor, kAmioInterfaceVersionMinor);
			return GetAmioInfo(*amioInfo);
		}

		case kAmioInterfaceCommand_Initialize:
		{
			return Initialize();
		}

		case kAmioInterfaceCommand_Terminate:
		{
			return Terminate();
		}

		case kAmioInterfaceCommand_PreOpen:
		{
			AmioPreOpenInterface* iface = (AmioPreOpenInterface*)(inInterface);
			if (!iface)
			{
				return kAmioInterfaceReturnCode_ParameterOutOfRange;
			}
			SetErrorInterface(&iface->GetErrorInterface());
			return PreOpenForRead(*iface);
		}

		case kAmioInterfaceCommand_RunImportSettingsDialog:
		{
			AmioRunImportSettingsDialogInterface* iface = (AmioRunImportSettingsDialogInterface*)(inInterface);
			if (!iface)
			{
				return kAmioInterfaceReturnCode_ParameterOutOfRange;
			}
			SetErrorInterface(&iface->GetErrorInterface());
			iface->SetCancel(false);
			AmioResult result = RunImportSettingsDialog(*iface);
			return result;
		}

		case kAmioInterfaceCommand_Open:
		{
			AmioOpenInterface* amioOpen = (AmioOpenInterface*)inInterface;
			if (!amioOpen)
			{
				return kAmioInterfaceReturnCode_ParameterOutOfRange;
			}
			SetErrorInterface(&amioOpen->GetErrorInterface());
			return OpenForRead(*amioOpen);
		}

		case kAmioInterfaceCommand_GetFormat:
		{
			ReadT* readFile = (ReadT*)inState;
			AmioFormatInterface* format = (AmioFormatInterface*)(inInterface);

			if ((!readFile) || (!format))
			{
				return kAmioInterfaceReturnCode_ParameterOutOfRange;
			}
			SetErrorInterface(&format->GetErrorInterface());
			return GetFormat(*readFile, *format);
		}

		case kAmioInterfaceCommand_ReadSamples:
		{
			ReadT* readFile = (ReadT*)inState;
			AmioReadSamplesInterface* readInterface = (AmioReadSamplesInterface*)(inInterface);

			if ((!readFile) || (!readInterface))
			{
				return kAmioInterfaceReturnCode_ParameterOutOfRange;
			}
			return ReadSamples(*readFile ,*readInterface);
		}

		case kAmioInterfaceCommand_ReadSamplesRaw:
		{
			ReadT* readFile = (ReadT*)inState;
			AmioReadSamplesRawInterface* readInterface = (AmioReadSamplesRawInterface*)(inInterface);

			if ((!readFile) || (!readInterface))
			{
				return kAmioInterfaceReturnCode_ParameterOutOfRange;
			}
			return ReadSamplesRaw(*readFile, readInterface->GetStartByteOffset(), readInterface->GetByteCount(), readInterface->GetBufferAddress());
		}

		case kAmioInterfaceCommand_ReadVideoFrame:
		{
			ReadT* readFile = (ReadT*)inState;
			AmioReadVideoFrameInterface* readInterface = (AmioReadVideoFrameInterface*)(inInterface);

			if ((!readFile) || (!readInterface))
			{
				return kAmioInterfaceReturnCode_ParameterOutOfRange;
			}
			return ReadVideoFrame(*readFile ,*readInterface);
		}

		case kAmioInterfaceCommand_ReadMetadata:
		{
			ReadT* readFile = (ReadT*)inState;
			AmioReadMetadataInterface* readInterface = (AmioReadMetadataInterface*)(inInterface);
			if ((!readFile) || (!readInterface))
			{
				return kAmioInterfaceReturnCode_ParameterOutOfRange;
			}
			return ReadMetadata(*readFile, 
				readInterface->GetInstanceID(),
				readInterface->GetStartOffset(),
				readInterface->GetByteCount(),
				readInterface->GetBufferAddress());
		}

		case kAmioInterfaceCommand_DoneWithAudioRead:
		{
			ReadT* readFile = (ReadT*)inState;
			if (!readFile)
			{
				return kAmioInterfaceReturnCode_ParameterOutOfRange;
			}
			return DoneWithAudioRead(*readFile);
		}

		case kAmioInterfaceCommand_Close:
		{
			ReadT* readFile = (ReadT*)inState;
			if (!readFile)
			{
				return kAmioInterfaceReturnCode_ParameterOutOfRange;
			}
			return CloseForRead(readFile);
		}

		case kAmioInterfaceCommand_GetDefaultExportSettings:
		{
			AmioFormatInterface* format = (AmioFormatInterface*)(inInterface);
			if (!format)
			{
				return kAmioInterfaceReturnCode_ParameterOutOfRange;
			}
			SetErrorInterface(&format->GetErrorInterface());
			return GetDefaultExportSettings(*format);
		}

		case kAmioInterfaceCommand_GetExportSettingsInfo:
		{
			AmioGetExportSettingsInfoInterface* iface = (AmioGetExportSettingsInfoInterface*)(inInterface);
			if (!iface)
			{
				return kAmioInterfaceReturnCode_ParameterOutOfRange;
			}
			SetErrorInterface(&iface->GetErrorInterface());
			return GetExportSettingsInfo(*iface);
		}

		case kAmioInterfaceCommand_RunExportSettingsDialog:
		{
			AmioRunExportSettingsDialogInterface* iface = (AmioRunExportSettingsDialogInterface*)(inInterface);
			if (!iface)
			{
				return kAmioInterfaceReturnCode_ParameterOutOfRange;
			}
			SetErrorInterface(&iface->GetErrorInterface());
			bool cancelled = false;
//dvb: I am eliminating the call to GetSettingsDialogFlags() in the following line so that we do not crash
//     on Audition 4.0 which uses version 1.0 of the AMIO interface which does not support that method.
//			AmioResult result = RunExportSettingsDialog(iface->GetFormat(), iface->GetParentWindow(), iface->GetSettingsDialogFlags(), cancelled);
			AmioResult result = RunExportSettingsDialog(iface->GetFormat(), iface->GetParentWindow(), 0, cancelled);
			if (result == kAmioInterfaceReturnCode_Success)
			{
				iface->SetCancel(cancelled);
			}
			return result;
		}

		case kAmioInterfaceCommand_WriteStart:
		{
			AmioWriteStartInterface* wsInterface = (AmioWriteStartInterface*)inInterface;
			if (!wsInterface)
			{
				return kAmioInterfaceReturnCode_ParameterOutOfRange;
			}
			SetErrorInterface(&wsInterface->GetErrorInterface());
			WriteT* writeFile = NULL;
			AmioResult result = WriteStart(*wsInterface, writeFile);
			if (result == kAmioInterfaceReturnCode_Success)
			{
				wsInterface->SetSessionState(writeFile);
			}
			return result;
		}

		case kAmioInterfaceCommand_WriteSamples:
		{
			AmioWriteSamplesInterface* writeSamples = (AmioWriteSamplesInterface*)inInterface;
			WriteT* writeFile = (WriteT*)inState;
			if ((!writeSamples) || (!writeFile))
			{
				return kAmioInterfaceReturnCode_ParameterOutOfRange;
			}
			SetErrorInterface(&writeSamples->GetErrorInterface());
			return WriteSamples(*writeFile, *writeSamples);
		}

		case kAmioInterfaceCommand_WriteFinish:
		{
			AmioWriteFinishInterface* writeFinish = (AmioWriteFinishInterface*)inInterface;
			WriteT* writeFile = (WriteT*)inState;
			if ((!writeFile) || (!writeFinish))
			{
				return kAmioInterfaceReturnCode_GeneralError;
			}
			SetErrorInterface(&writeFinish->GetErrorInterface());
			AmioResult result = FinishWrite(*writeFile, writeFinish->GetCancel(), writeFinish->GetFormat());
			return result;
		}

		default:
			return kAmioInterfaceReturnCode_UnknownCommand;

		}
	}

protected:

	///
	virtual AmioResult GetAmioInfo(AmioGetAmioInfoInterface&)
	{
		return kAmioInterfaceReturnCode_Success;
	}

	///
	virtual AmioResult Initialize()
	{
		return kAmioInterfaceReturnCode_Success;
	}

	///
	virtual AmioResult Terminate()
	{
		return kAmioInterfaceReturnCode_Success;
	}

	///
	virtual AmioResult PreOpenForRead(AmioPreOpenInterface& inInterface)
	{
		return kAmioInterfaceReturnCode_Success;
	}

	///
	virtual AmioResult RunImportSettingsDialog(AmioRunImportSettingsDialogInterface& inImportSettings)
	{
		return kAmioInterfaceReturnCode_UnsupportedCommand;
	}

	///
	virtual AmioResult OpenForRead(AmioOpenInterface&)
	{
		return kAmioInterfaceReturnCode_UnsupportedCommand;
	}

	///
	virtual AmioResult GetFormat(ReadT&, AmioFormatInterface&)
	{
		return kAmioInterfaceReturnCode_UnsupportedCommand;
	}

	///
	virtual AmioResult ReadSamples(ReadT& inReadFile, AmioReadSamplesInterface& inReadInterface)
	{
		return kAmioInterfaceReturnCode_UnsupportedCommand;
	}

	///
	virtual AmioResult ReadSamplesRaw(ReadT& inReadFile,
		asdk::int64 inStartOffset,
		asdk::int64 inByteCount,
		void *inDestinationBuffer)
	{
		return kAmioInterfaceReturnCode_UnsupportedCommand;
	}

	///
	virtual AmioResult ReadVideoFrame(ReadT& inReadFile, AmioReadVideoFrameInterface& inReadInterface)
	{
		return kAmioInterfaceReturnCode_UnsupportedCommand;
	}

	///
	virtual AmioResult ReadMetadata(ReadT& inReadFile, 
		asdk::int64 inInstanceID,
		asdk::int64 inStartOffset,
		asdk::int64 inByteCount,
		void *inDestinationBuffer)
	{
		return kAmioInterfaceReturnCode_UnsupportedCommand;
	}

	///
	virtual AmioResult DoneWithAudioRead(ReadT& inReadFile)
	{
		return kAmioInterfaceReturnCode_UnsupportedCommand;
	}

	///
	virtual AmioResult CloseForRead(ReadT* inReadFile)
	{
		delete inReadFile;
		return kAmioInterfaceReturnCode_Success;
	}

	///
	virtual AmioResult GetDefaultExportSettings(AmioFormatInterface& outSettings)
	{
		return kAmioInterfaceReturnCode_UnsupportedCommand;
	}

	///
	virtual AmioResult GetExportSettingsInfo(AmioGetExportSettingsInfoInterface& inInterface)
	{
		return kAmioInterfaceReturnCode_UnsupportedCommand;
	}

	///
	virtual AmioResult RunExportSettingsDialog(AmioFormatInterface& inFormat, void* inParentWindow, asdk::int64 inFlags, bool &outCancelled)
	{
		return kAmioInterfaceReturnCode_UnsupportedCommand;
	}

	///
	virtual AmioResult WriteStart(AmioWriteStartInterface& inInterface, WriteT*& outWriteFile)
	{
		return kAmioInterfaceReturnCode_UnsupportedCommand;
	}

	///
	virtual AmioResult WriteSamples(WriteT& inWriteFile, AmioWriteSamplesInterface& inWriteInterface)
	{
		return kAmioInterfaceReturnCode_UnsupportedCommand;
	}

	///
	virtual AmioResult FinishWrite(WriteT& inWriteFile, bool inIsCancel, const AmioFormatInterface& inFormat)
	{
		return kAmioInterfaceReturnCode_UnsupportedCommand;
	}

protected:
	///
	void SetErrorInterface(AmioErrorInterface *inErrorInterface)
	{
		mErrorInterface = inErrorInterface;
	}

	/// Return extended error information if we can.
	void SetErrorString(const UTF16Char* inErrorString)
	{
		if (mErrorInterface)
			mErrorInterface->SetErrorString(inErrorString);
	}

	void AddWarning(const UTF16Char* inWarningText)
	{
		if (mErrorInterface)
			mErrorInterface->AddWarning(inWarningText);
	}

	AmioErrorInterface *mErrorInterface;
};

} // namespace amio

#endif // AMIO_AMIOINTERFACETEMPLATE_H
