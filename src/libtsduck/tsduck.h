//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2020, Thierry Lelegard
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
// THE POSSIBILITY OF SUCH DAMAGE.
//
//----------------------------------------------------------------------------

#pragma once
#include "tsAACDescriptor.h"
#include "tsAbortInterface.h"
#include "tsAbstractAudioVideoAttributes.h"
#include "tsAbstractAVCAccessUnit.h"
#include "tsAbstractAVCData.h"
#include "tsAbstractAVCStructure.h"
#include "tsAbstractDefinedByStandards.h"
#include "tsAbstractDeliverySystemDescriptor.h"
#include "tsAbstractDemux.h"
#include "tsAbstractDescrambler.h"
#include "tsAbstractDescriptor.h"
#include "tsAbstractDescriptorsTable.h"
#include "tsAbstractDuplicateRemapPlugin.h"
#include "tsAbstractHTTPInputPlugin.h"
#include "tsAbstractLongTable.h"
#include "tsAbstractMultilingualDescriptor.h"
#include "tsAbstractOutputStream.h"
#include "tsAbstractSignalization.h"
#include "tsAbstractTable.h"
#include "tsAbstractTablePlugin.h"
#include "tsAbstractTransportListTable.h"
#include "tsAC3Attributes.h"
#include "tsAdaptationFieldDataDescriptor.h"
#include "tsAES.h"
#include "tsAFExtensionsDescriptor.h"
#include "tsAIT.h"
#include "tsAlgorithm.h"
#include "tsAncillaryDataDescriptor.h"
#include "tsAnnouncementSupportDescriptor.h"
#include "tsApplicationDescriptor.h"
#include "tsApplicationIconsDescriptor.h"
#include "tsApplicationIdentifier.h"
#include "tsApplicationNameDescriptor.h"
#include "tsApplicationRecordingDescriptor.h"
#include "tsApplicationSharedLibrary.h"
#include "tsApplicationSignallingDescriptor.h"
#include "tsApplicationStorageDescriptor.h"
#include "tsApplicationUsageDescriptor.h"
#include "tsArgMix.h"
#include "tsArgs.h"
#include "tsArgsSupplierInterface.h"
#include "tsArgsWithPlugins.h"
#include "tsAssociationTagDescriptor.h"
#include "tsAsyncReport.h"
#include "tsATSCAC3AudioStreamDescriptor.h"
#include "tsATSCEAC3AudioDescriptor.h"
#include "tsATSCEIT.h"
#include "tsATSCMultipleString.h"
#include "tsATSCStuffingDescriptor.h"
#include "tsATSCTimeShiftedServiceDescriptor.h"
#include "tsAudioAttributes.h"
#include "tsAudioLanguageOptions.h"
#include "tsAudioPreselectionDescriptor.h"
#include "tsAudioStreamDescriptor.h"
#include "tsAVCAttributes.h"
#include "tsAVCHRDParameters.h"
#include "tsAVCParser.h"
#include "tsAVCSequenceParameterSet.h"
#include "tsAVCTimingAndHRDDescriptor.h"
#include "tsAVCVideoDescriptor.h"
#include "tsAVCVUIParameters.h"
#include "tsBAT.h"
#include "tsBCD.h"
#include "tsBetterSystemRandomGenerator.h"
#include "tsBinaryTable.h"
#include "tsBitrateDifferenceDVBT.h"
#include "tsBitRateRegulator.h"
#include "tsBitStream.h"
#include "tsBlockCipher.h"
#include "tsBlockCipherAlertInterface.h"
#include "tsBouquetNameDescriptor.h"
#include "tsByteBlock.h"
#include "tsC2BundleDeliverySystemDescriptor.h"
#include "tsC2DeliverySystemDescriptor.h"
#include "tsCableDeliverySystemDescriptor.h"
#include "tsCableEmergencyAlertTable.h"
#include "tsCADescriptor.h"
#include "tsCAIdentifierDescriptor.h"
#include "tsCaptionServiceDescriptor.h"
#include "tsCarouselIdentifierDescriptor.h"
#include "tsCASDate.h"
#include "tsCASFamily.h"
#include "tsCASMapper.h"
#include "tsCASSelectionArgs.h"
#include "tsCAT.h"
#include "tsCBC.h"
#include "tsCellFrequencyLinkDescriptor.h"
#include "tsCellListDescriptor.h"
#include "tsCerrReport.h"
#include "tsChannelFile.h"
#include "tsCIAncillaryDataDescriptor.h"
#include "tsCipherChaining.h"
#include "tsCOM.h"
#include "tsComponentDescriptor.h"
#include "tsComponentNameDescriptor.h"
#include "tsCondition.h"
#include "tsConfigFile.h"
#include "tsConfigSection.h"
#include "tsContentAdvisoryDescriptor.h"
#include "tsContentDescriptor.h"
#include "tsContinuityAnalyzer.h"
#include "tsCopyrightDescriptor.h"
#include "tsCountryAvailabilityDescriptor.h"
#include "tsCPDescriptor.h"
#include "tsCPIdentifierDescriptor.h"
#include "tsCRC32.h"
#include "tsCTR.h"
#include "tsCTS1.h"
#include "tsCTS2.h"
#include "tsCTS3.h"
#include "tsCTS4.h"
#include "tsCueIdentifierDescriptor.h"
#include "tsCVCT.h"
#include "tsCyclingPacketizer.h"
#include "tsDataBroadcastDescriptor.h"
#include "tsDataBroadcastIdDescriptor.h"
#include "tsDataStreamAlignmentDescriptor.h"
#include "tsDCCArrivingRequestDescriptor.h"
#include "tsDCCDepartingRequestDescriptor.h"
#include "tsDCCSCT.h"
#include "tsDCCT.h"
#include "tsDeferredAssociationTagsDescriptor.h"
#include "tsDektecControl.h"
#include "tsDektecDeviceInfo.h"
#include "tsDektecInputPlugin.h"
#include "tsDektecOutputPlugin.h"
#include "tsDektecUtils.h"
#include "tsDeliverySystem.h"
#include "tsDES.h"
#include "tsDescriptor.h"
#include "tsDescriptorList.h"
#include "tsDIILocationDescriptor.h"
#include "tsDiscontinuityInformationTable.h"
#include "tsDisplayInterface.h"
#include "tsDoubleCheckLock.h"
#include "tsDSMCCStreamDescriptorsTable.h"
#include "tsDSNGDescriptor.h"
#include "tsDTSDescriptor.h"
#include "tsDTSHDDescriptor.h"
#include "tsDTSNeuralDescriptor.h"
#include "tsDuckConfigFile.h"
#include "tsDuckContext.h"
#include "tsDuckExtension.h"
#include "tsDuckExtensionRepository.h"
#include "tsDuckProtocol.h"
#include "tsDVBAC3Descriptor.h"
#include "tsDVBAC4Descriptor.h"
#include "tsDVBCharset.h"
#include "tsDVBCharsetSingleByte.h"
#include "tsDVBCharsetUTF16.h"
#include "tsDVBCharsetUTF8.h"
#include "tsDVBCISSA.h"
#include "tsDVBCSA2.h"
#include "tsDVBEnhancedAC3Descriptor.h"
#include "tsDVBHTMLApplicationBoundaryDescriptor.h"
#include "tsDVBHTMLApplicationDescriptor.h"
#include "tsDVBHTMLApplicationLocationDescriptor.h"
#include "tsDVBJApplicationDescriptor.h"
#include "tsDVBJApplicationLocationDescriptor.h"
#include "tsDVBStuffingDescriptor.h"
#include "tsDVBTimeShiftedServiceDescriptor.h"
#include "tsDVS042.h"
#include "tsEacemPreferredNameIdentifierDescriptor.h"
#include "tsEacemPreferredNameListDescriptor.h"
#include "tsEacemStreamIdentifierDescriptor.h"
#include "tsEASAudioFileDescriptor.h"
#include "tsEASInbandDetailsChannelDescriptor.h"
#include "tsEASInbandExceptionChannelsDescriptor.h"
#include "tsEASMetadataDescriptor.h"
#include "tsECB.h"
#include "tsECMGClient.h"
#include "tsECMGClientArgs.h"
#include "tsECMGClientHandlerInterface.h"
#include "tsECMGSCS.h"
#include "tsECMRepetitionRateDescriptor.h"
#include "tsEDID.h"
#include "tsEIT.h"
#include "tsEITProcessor.h"
#include "tsEMMGClient.h"
#include "tsEMMGMUX.h"
#include "tsEnumeration.h"
#include "tsETID.h"
#include "tsETT.h"
#include "tsEutelsatChannelNumberDescriptor.h"
#include "tsException.h"
#include "tsExtendedChannelNameDescriptor.h"
#include "tsExtendedEventDescriptor.h"
#include "tsExternalApplicationAuthorizationDescriptor.h"
#include "tsExternalESIdDescriptor.h"
#include "tsFatal.h"
#include "tsFileInputPlugin.h"
#include "tsFileNameRate.h"
#include "tsFileOutputPlugin.h"
#include "tsFilePacketPlugin.h"
#include "tsFlexMuxTimingDescriptor.h"
#include "tsFMCDescriptor.h"
#include "tsForkPipe.h"
#include "tsFrequencyListDescriptor.h"
#include "tsFTAContentManagementDescriptor.h"
#include "tsGenreDescriptor.h"
#include "tsGitHubRelease.h"
#include "tsGraphicsConstraintsDescriptor.h"
#include "tsGrid.h"
#include "tsGuard.h"
#include "tsGuardCondition.h"
#include "tsHash.h"
#include "tsHDSimulcastLogicalChannelDescriptor.h"
#include "tsHEVCTimingAndHRDDescriptor.h"
#include "tsHEVCVideoDescriptor.h"
#include "tsHFBand.h"
#include "tsHiDesDevice.h"
#include "tsHiDesDeviceInfo.h"
#include "tsHierarchyDescriptor.h"
#include "tshls.h"
#include "tshlsInputPlugin.h"
#include "tshlsMediaPlayList.h"
#include "tshlsMediaSegment.h"
#include "tshlsOutputPlugin.h"
#include "tshlsPlayList.h"
#include "tshlsTagAttributes.h"
#include "tsIBPDescriptor.h"
#include "tsIDSA.h"
#include "tsImageIconDescriptor.h"
#include "tsInputRedirector.h"
#include "tsINT.h"
#include "tsIntegerUtils.h"
#include "tsInterruptHandler.h"
#include "tsIPAddress.h"
#include "tsIPAddressMask.h"
#include "tsIPInputPlugin.h"
#include "tsIPMACGenericStreamLocationDescriptor.h"
#include "tsIPMACPlatformNameDescriptor.h"
#include "tsIPMACPlatformProviderNameDescriptor.h"
#include "tsIPMACStreamLocationDescriptor.h"
#include "tsIPOutputPlugin.h"
#include "tsIPSignallingDescriptor.h"
#include "tsIPUtils.h"
#include "tsIPv6Address.h"
#include "tsISO639LanguageDescriptor.h"
#include "tsISPAccessModeDescriptor.h"
#include "tsjson.h"
#include "tsjsonArray.h"
#include "tsjsonFalse.h"
#include "tsjsonNull.h"
#include "tsjsonNumber.h"
#include "tsjsonObject.h"
#include "tsjsonString.h"
#include "tsjsonTrue.h"
#include "tsjsonValue.h"
#include "tsKeyTable.h"
#include "tsLinkageDescriptor.h"
#include "tsLNB.h"
#include "tsLocalTimeOffsetDescriptor.h"
#include "tsLogicalChannelNumberDescriptor.h"
#include "tsMACAddress.h"
#include "tsMain.h"
#include "tsMaximumBitrateDescriptor.h"
#include "tsMD5.h"
#include "tsMemory.h"
#include "tsMessageDescriptor.h"
#include "tsMessagePriorityQueue.h"
#include "tsMessageQueue.h"
#include "tsMetadataSTDDescriptor.h"
#include "tsMGT.h"
#include "tsMJD.h"
#include "tsModulation.h"
#include "tsModulationArgs.h"
#include "tsMonotonic.h"
#include "tsMosaicDescriptor.h"
#include "tsMPEDemux.h"
#include "tsMPEG.h"
#include "tsMPEG2AACAudioDescriptor.h"
#include "tsMPEG4AudioDescriptor.h"
#include "tsMPEG4VideoDescriptor.h"
#include "tsMPEHandlerInterface.h"
#include "tsMPEPacket.h"
#include "tsMultilingualBouquetNameDescriptor.h"
#include "tsMultilingualComponentDescriptor.h"
#include "tsMultilingualNetworkNameDescriptor.h"
#include "tsMultilingualServiceNameDescriptor.h"
#include "tsMultiplexBufferDescriptor.h"
#include "tsMultiplexBufferUtilizationDescriptor.h"
#include "tsMutex.h"
#include "tsMutexInterface.h"
#include "tsMVCExtensionDescriptor.h"
#include "tsNames.h"
#include "tsNetworkChangeNotifyDescriptor.h"
#include "tsNetworkNameDescriptor.h"
#include "tsNIT.h"
#include "tsNorDigLogicalChannelDescriptorV1.h"
#include "tsNorDigLogicalChannelDescriptorV2.h"
#include "tsNPTEndpointDescriptor.h"
#include "tsNPTReferenceDescriptor.h"
#include "tsNullMutex.h"
#include "tsNullReport.h"
#include "tsNVODReferenceDescriptor.h"
#include "tsObject.h"
#include "tsOneShotPacketizer.h"
#include "tsOutputPager.h"
#include "tsOutputRedirector.h"
#include "tsPacketDecapsulation.h"
#include "tsPacketEncapsulation.h"
#include "tsPacketizer.h"
#include "tsPagerArgs.h"
#include "tsParentalRatingDescriptor.h"
#include "tsPartialTransportStreamDescriptor.h"
#include "tsPAT.h"
#include "tsPCR.h"
#include "tsPCRAnalyzer.h"
#include "tsPCRRegulator.h"
#include "tsPCSC.h"
#include "tsPDCDescriptor.h"
#include "tsPESDemux.h"
#include "tsPESHandlerInterface.h"
#include "tsPESPacket.h"
#include "tsPIDOperator.h"
#include "tsPlatform.h"
#include "tsPlugin.h"
#include "tsPluginOptions.h"
#include "tsPluginRepository.h"
#include "tsPluginSharedLibrary.h"
#include "tsPluginThread.h"
#include "tsPMT.h"
#include "tsPMTHandlerInterface.h"
#include "tsPolledFile.h"
#include "tsPollFiles.h"
#include "tsPollFilesListener.h"
#include "tsPrefetchDescriptor.h"
#include "tsPrivateDataIndicatorDescriptor.h"
#include "tsPrivateDataSpecifierDescriptor.h"
#include "tsProtectionMessageDescriptor.h"
#include "tsPSILogger.h"
#include "tsPSIMerger.h"
#include "tsPushInputPlugin.h"
#include "tsRandomGenerator.h"
#include "tsRedistributionControlDescriptor.h"
#include "tsRegistrationDescriptor.h"
#include "tsRegistry.h"
#include "tsReport.h"
#include "tsReportBuffer.h"
#include "tsReportFile.h"
#include "tsReportHandler.h"
#include "tsReportWithPrefix.h"
#include "tsResidentBuffer.h"
#include "tsRingNode.h"
#include "tsRRT.h"
#include "tsRST.h"
#include "tsS2SatelliteDeliverySystemDescriptor.h"
#include "tsS2XSatelliteDeliverySystemDescriptor.h"
#include "tsSafePtr.h"
#include "tsSatelliteDeliverySystemDescriptor.h"
#include "tsScramblingDescriptor.h"
#include "tsSCTE35.h"
#include "tsSCTE52.h"
#include "tsSDT.h"
#include "tsSection.h"
#include "tsSectionDemux.h"
#include "tsSectionFile.h"
#include "tsSectionHandlerInterface.h"
#include "tsSectionProviderInterface.h"
#include "tsSelectionInformationTable.h"
#include "tsService.h"
#include "tsServiceAvailabilityDescriptor.h"
#include "tsServiceDescriptor.h"
#include "tsServiceDiscovery.h"
#include "tsServiceIdentifierDescriptor.h"
#include "tsServiceListDescriptor.h"
#include "tsServiceLocationDescriptor.h"
#include "tsServiceMoveDescriptor.h"
#include "tsServiceRelocatedDescriptor.h"
#include "tsSHA1.h"
#include "tsSHA256.h"
#include "tsSHA512.h"
#include "tsSharedLibrary.h"
#include "tsSHDeliverySystemDescriptor.h"
#include "tsShortEventDescriptor.h"
#include "tsShortSmoothingBufferDescriptor.h"
#include "tsSimpleApplicationBoundaryDescriptor.h"
#include "tsSimpleApplicationLocationDescriptor.h"
#include "tsSimulCryptDate.h"
#include "tsSingletonManager.h"
#include "tsSkyLogicalChannelNumberDescriptor.h"
#include "tsSLDescriptor.h"
#include "tsSmoothingBufferDescriptor.h"
#include "tsSocket.h"
#include "tsSocketAddress.h"
#include "tsSpliceAvailDescriptor.h"
#include "tsSpliceDTMFDescriptor.h"
#include "tsSpliceInformationTable.h"
#include "tsSpliceInsert.h"
#include "tsSpliceSchedule.h"
#include "tsSpliceSegmentationDescriptor.h"
#include "tsSpliceTimeDescriptor.h"
#include "tsSRTInputPlugin.h"
#include "tsSRTOutputPlugin.h"
#include "tsSRTSocket.h"
#include "tsSSUDataBroadcastIdDescriptor.h"
#include "tsSSULinkageDescriptor.h"
#include "tsStandaloneTableDemux.h"
#include "tsStaticInstance.h"
#include "tsSTDDescriptor.h"
#include "tsStreamEventDescriptor.h"
#include "tsStreamIdentifierDescriptor.h"
#include "tsStreamModeDescriptor.h"
#include "tsStringifyInterface.h"
#include "tsSTT.h"
#include "tsSubRipGenerator.h"
#include "tsSubtitlingDescriptor.h"
#include "tsSupplementaryAudioDescriptor.h"
#include "tsSVCExtensionDescriptor.h"
#include "tsSwitchableReport.h"
#include "tsSysInfo.h"
#include "tsSystemClockDescriptor.h"
#include "tsSystemMonitor.h"
#include "tsSystemRandomGenerator.h"
#include "tsSysUtils.h"
#include "tsT2DeliverySystemDescriptor.h"
#include "tsT2MIDemux.h"
#include "tsT2MIDescriptor.h"
#include "tsT2MIHandlerInterface.h"
#include "tsT2MIPacket.h"
#include "tsTableHandlerInterface.h"
#include "tsTables.h"
#include "tsTablesDisplay.h"
#include "tsTablesFactory.h"
#include "tsTablesLogger.h"
#include "tsTablesLoggerFilter.h"
#include "tsTablesLoggerFilterInterface.h"
#include "tsTablesLoggerFilterRepository.h"
#include "tsTablesPtr.h"
#include "tsTargetBackgroundGridDescriptor.h"
#include "tsTargetIPAddressDescriptor.h"
#include "tsTargetIPSlashDescriptor.h"
#include "tsTargetIPSourceSlashDescriptor.h"
#include "tsTargetIPv6AddressDescriptor.h"
#include "tsTargetIPv6SlashDescriptor.h"
#include "tsTargetIPv6SourceSlashDescriptor.h"
#include "tsTargetMACAddressDescriptor.h"
#include "tsTargetMACAddressRangeDescriptor.h"
#include "tsTargetRegionDescriptor.h"
#include "tsTargetRegionNameDescriptor.h"
#include "tsTargetSerialNumberDescriptor.h"
#include "tsTargetSmartcardDescriptor.h"
#include "tsTCPConnection.h"
#include "tsTCPServer.h"
#include "tsTCPSocket.h"
#include "tsTDES.h"
#include "tsTDT.h"
#include "tsTelephoneDescriptor.h"
#include "tsTeletextCharset.h"
#include "tsTeletextDemux.h"
#include "tsTeletextDescriptor.h"
#include "tsTeletextFrame.h"
#include "tsTeletextHandlerInterface.h"
#include "tsTelnetConnection.h"
#include "tsTerrestrialDeliverySystemDescriptor.h"
#include "tsTextFormatter.h"
#include "tsTextParser.h"
#include "tsThread.h"
#include "tsThreadAttributes.h"
#include "tsTime.h"
#include "tsTimeShiftBuffer.h"
#include "tsTimeShiftedEventDescriptor.h"
#include "tsTimeSliceFECIdentifierDescriptor.h"
#include "tsTimeTrackerDemux.h"
#include "tstlv.h"
#include "tstlvAnalyzer.h"
#include "tstlvChannelMessage.h"
#include "tstlvConnection.h"
#include "tstlvLogger.h"
#include "tstlvMessage.h"
#include "tstlvMessageFactory.h"
#include "tstlvProtocol.h"
#include "tstlvSerializer.h"
#include "tstlvStreamMessage.h"
#include "tsTLVSyntax.h"
#include "tsTOT.h"
#include "tsTransportProtocolDescriptor.h"
#include "tsTransportStreamDescriptor.h"
#include "tsTransportStreamId.h"
#include "tsTSAnalyzer.h"
#include "tsTSAnalyzerOptions.h"
#include "tsTSAnalyzerReport.h"
#include "tsTSDT.h"
#include "tsTSFile.h"
#include "tsTSFileInputBuffered.h"
#include "tsTSFileOutputResync.h"
#include "tstsp.h"
#include "tsTSPacket.h"
#include "tsTSPacketMetadata.h"
#include "tsTSPacketQueue.h"
#include "tstspControlCommandLine.h"
#include "tsTSScanner.h"
#include "tsTSScrambling.h"
#include "tsTSSpeedMetrics.h"
#include "tsTuner.h"
#include "tsTunerArgs.h"
#include "tsTVCT.h"
#include "tsUChar.h"
#include "tsUDPReceiver.h"
#include "tsUDPSocket.h"
#include "tsUID.h"
#include "tsURILinkageDescriptor.h"
#include "tsURL.h"
#include "tsUserInterrupt.h"
#include "tsUString.h"
#include "tsVariable.h"
#include "tsVBIDataDescriptor.h"
#include "tsVBITeletextDescriptor.h"
#include "tsVCT.h"
#include "tsVersion.h"
#include "tsVersionInfo.h"
#include "tsVersionString.h"
#include "tsVideoAttributes.h"
#include "tsVideoDepthRangeDescriptor.h"
#include "tsVideoStreamDescriptor.h"
#include "tsVideoWindowDescriptor.h"
#include "tsWatchDog.h"
#include "tsWatchDogHandlerInterface.h"
#include "tsWebRequest.h"
#include "tsWebRequestArgs.h"
#include "tsWebRequestHandlerInterface.h"
#include "tsxml.h"
#include "tsxmlAttribute.h"
#include "tsxmlComment.h"
#include "tsxmlDeclaration.h"
#include "tsxmlDocument.h"
#include "tsxmlElement.h"
#include "tsxmlNode.h"
#include "tsxmlText.h"
#include "tsxmlTweaks.h"
#include "tsxmlUnknown.h"

#if defined(TS_LINUX)
#include "tsDTVProperties.h"
#include "tsSignalAllocator.h"
#endif

#if defined(TS_MAC)
#include "tsMacPList.h"
#endif

#if defined(TS_WINDOWS)
#include "tsComIds.h"
#include "tsComPtr.h"
#include "tsDirectShowFilterCategory.h"
#include "tsDirectShowGraph.h"
#include "tsDirectShowNetworkType.h"
#include "tsDirectShowTest.h"
#include "tsDirectShowUtils.h"
#include "tsMediaTypeUtils.h"
#include "tsSinkFilter.h"
#include "tsTunerGraph.h"
#include "tsWinUtils.h"
#endif
