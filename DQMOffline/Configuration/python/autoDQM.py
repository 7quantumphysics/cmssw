autoDQM = { 'common': ['@dcs+DQMMessageLoggerSeq+@strip+@pixel+@tracking+@L1TMon+@hlt+@beam+@castor+@physics+@tau',
                        'PostDQMOffline',
                        '@dcs+DQMMessageLoggerSeq+@strip+@pixel+@tracking+@L1TMon+@hlt+@beam+@fed+@tau+dqmFastTimerServiceClient'],

            'commonFakeHLT': ['@dcs+DQMMessageLoggerSeq+@strip+@pixel+@tracking+@L1TMon+@beam+@castor+@physics+@tau',
                        'PostDQMOffline',
                        '@dcs+DQMMessageLoggerSeq+@strip+@pixel+@tracking+@L1TMon+@beam+@fed+@tau+dqmFastTimerServiceClient'],

            'commonSiStripZeroBias': ['@dcs+DQMMessageLoggerSeq+@stripZeroBias+@pixel+@tracking+@L1TMon+@hlt+@beam+@castor+@physics',
                                      'PostDQMOffline',
                                      '@dcs+DQMMessageLoggerSeq+@strip+@pixel+@tracking+@L1TMon+@hlt+@beam+@fed+dqmFastTimerServiceClient'],

            'commonSiStripZeroBiasFakeHLT': ['@dcs+DQMMessageLoggerSeq+@stripZeroBias+@pixel+@tracking+@L1TMon+@beam+@castor+@physics',
                                      'PostDQMOffline',
                                      '@dcs+DQMMessageLoggerSeq+@strip+@pixel+@tracking+@L1TMon+@beam+@fed+dqmFastTimerServiceClient'],

            'trackingOnlyDQM': ['DQMOfflineTracking',
                                'PostDQMOffline',
                                'DQMHarvestTracking'],

            'pixelTrackingOnlyDQM': ['DQMOfflinePixelTracking',
                                     'PostDQMOffline',
                                     'DQMHarvestPixelTracking'],

            'outerTracker': ['DQMOuterTracker',
                             'PostDQMOffline',
                             'DQMHarvestOuterTracker'],

	    'dcs': ['DQMOfflineDCS',
		    'PostDQMOffline',
		    'DQMHarvestDCS'],

	    'strip': ['DQMOfflineTrackerStrip',
		      'PostDQMOffline',
		      'DQMHarvestTrackerStrip+DQMCertTrackerStrip'],

	    'stripZeroBias': ['DQMOfflineTrackerStripMinBias',
			      'PostDQMOffline',
                              'DQMHarvestTrackerStrip+DQMCertTrackerStrip'],

	    'pixel': ['DQMOfflineTrackerPixel',
		      'PostDQMOffline',
                      'DQMHarvestTrackerPixel+DQMCertTrackerPixel'],

	    'castor': ['DQMOfflineCASTOR',
		       'PostDQMOffline',
		       'DQMNone'],

	    'tracking': ['DQMOfflineTracking',
			 'PostDQMOffline',
                         'DQMHarvestTracking+DQMCertTracking'],

	    'hlt': ['DQMOfflineTrigger',
		    'PostDQMOffline',
                    'DQMHarvestTrigger+DQMCertTrigger'],

	    'fed': ['DQMNone',
		    'PostDQMOffline',
		    'DQMHarvestFED'],

	    'tau': ['DQMOfflineTAU',
		    'PostDQMOffline',
		    'DQMHarvestTAU'],

	    'beam': ['DQMOfflineBeam',
		     'PostDQMOffline',
		     'DQMHarvestBeam'],

            'lumi': ['DQMOfflineLumi',
                     'PostDQMOffline',
                     'DQMNone'],

            'muon': ['DQMOfflineMuon',
                     'PostDQMOffline',
                     'DQMHarvestMuon+DQMCertMuon'],

            'hcal': ['DQMOfflineHcal',
                     'PostDQMOffline',
                     'DQMHarvestHcal'],

            'hcal2': ['DQMOfflineHcal2',
                      'PostDQMOffline',
                      'DQMHarvestHcal2'],

            'jetmet': ['DQMOfflineJetMET',
                       'PostDQMOffline',
                       'DQMHarvestJetMET+DQMCertJetMET'],

            'ecal': ['DQMOfflineEcal',
                     'PostDQMOffline',
                     'DQMHarvestEcal+DQMCertEcal'],

            'egamma': ['DQMOfflineEGamma',
                       'PostDQMOffline',
                       'DQMHarvestEGamma+DQMCertEGamma'],

            'ctpps': ['DQMOfflineCTPPS',
                      'PostDQMOffline',
                      'DQMHarvestCTPPS'],
            
            'btag': ['DQMOfflineBTag',
                     'PostDQMOffline',
                     'DQMHarvestBTag'],

	    'physics': ['DQMOfflinePhysics',
			'PostDQMOffline',
			'DQMNone'],

            'L1TMon': ['DQMOfflineL1T',
                       'PostDQMOffline',
                       'DQMHarvestL1T'],

            'L1TEgamma': ['DQMOfflineL1TEgamma',
                          'PostDQMOffline',
                          'DQMHarvestL1TEgamma'],

            'L1TMuon': ['DQMOfflineL1TMuon',
                        'PostDQMOffline',
                        'DQMHarvestL1TMuon'],

            'HLTMon': ['HLTMonitoring',
                       'PostDQMOffline',
                       'HLTMonitoringClient'],

            'HLTMonPA': ['HLTMonitoringPA', 
			 'PostDQMOffline', 
			 'HLTMonitoringClientPA'],

            'express': ['@commonSiStripZeroBias+@muon+@hcal+@jetmet+@ecal',
                        'PostDQMOffline',
                        '@commonSiStripZeroBias+@muon+@hcal+@jetmet+@ecal'],

            'allForPrompt': ['@common+@muon+@hcal+@jetmet+@ecal+@egamma',
                             'PostDQMOffline',
                             '@common+@muon+@hcal+@jetmet+@ecal+@egamma'],

            'rerecoCommon': ['@common+@muon+@hcal+@jetmet+@ecal+@egamma+@L1TMuon+@L1TEgamma+@ctpps',
                             'PostDQMOffline',
                             '@common+@muon+@hcal+@jetmet+@ecal+@egamma+@L1TMuon+@L1TEgamma+@ctpps'],

            'rerecoSingleMuon': ['@common+@muon+@hcal+@jetmet+@ecal+@egamma+@lumi+@L1TMuon+@L1TEgamma+@ctpps',
                                 'PostDQMOffline',
                                 '@common+@muon+@hcal+@jetmet+@ecal+@egamma+@lumi+@L1TMuon+@L1TEgamma+@ctpps'],

            'rerecoZeroBias' : ['@commonSiStripZeroBias+@muon+@hcal+@hcal2+@jetmet+@ecal+@egamma+@L1TMuon+@L1TEgamma+@ctpps',
                                'PostDQMOffline',
                                '@commonSiStripZeroBias+@muon+@hcal+@hcal2+@jetmet+@ecal+@egamma+@L1TMuon+@L1TEgamma+@ctpps'],

            'rerecoZeroBiasFakeHLT' : ['@commonSiStripZeroBiasFakeHLT+@muon+@hcal+@hcal2+@jetmet+@ecal+@egamma+@L1TMuon+@L1TEgamma+@ctpps',
                                       'PostDQMOffline',
                                       '@commonSiStripZeroBiasFakeHLT+@muon+@hcal+@hcal2+@jetmet+@ecal+@egamma+@L1TMuon+@L1TEgamma+@ctpps'],

            'miniAODDQM': ['DQMOfflineMiniAOD',
                           'PostDQMOfflineMiniAOD',
                           'DQMHarvestMiniAOD'],

            'nanoAODDQM': ['DQMOfflineNanoAOD',
                           'PostDQMOffline',
                           'DQMHarvestNanoAOD'],

            'pfDQM': ['DQMOfflinePF',
                      'PostDQMOffline',
                      'DQMHarvestPF'],

             'standardDQM': ['DQMOffline',
                            'PostDQMOffline',
                            'dqmHarvesting'],
#            'standardDQM': ['@dcs+DQMMessageLoggerSeq+@ecal+@hcal+@hcal2+@strip+@pixel+@castor+@ctpps+@muon+@tracking+@jetmet+@egamma+@L1TMon+@hlt+@btag+@beam+@physics+@HLTMon',
#                            'PostDQMOffline',
#                            '@dcs+DQMMessageLoggerSeq+@ecal+@hcal+@hcal2+@strip+@pixel+@fed+@muon+@tracking+@jetmet+@egamma+@L1TMon+@hlt+@btag+@beam+@HLTMon'],

            'ExtraHLT': ['DQMOfflineExtraHLT',
                         'PostDQMOffline',
                         'dqmHarvestingExtraHLT'],

            'standardDQMFakeHLT': ['DQMOfflineFakeHLT',
                                   'PostDQMOffline',
                                   'dqmHarvestingFakeHLT'],
#            'standardDQMFakeHLT': ['@dcs+DQMMessageLoggerSeq+@ecal+@hcal+@hcal2+@strip+@pixel+@castor+@ctpps+@muon+@tracking+@jetmet+@egamma+@L1TMon+@btag+@beam+@physics',
#                                   'PostDQMOffline',
#                                   '@dcs+DQMMessageLoggerSeq+@ecal+@hcal+@hcal2+@strip+@pixel+@fed+@muon+@tracking+@jetmet+@egamma+@L1TMon+@btag+@beam'],

            'standardDQMHIFakeHLT': ['DQMOfflineHeavyIonsFakeHLT',
                                   'PostDQMOfflineHI',
                                   'dqmHarvestingFakeHLT'],

            'liteDQMHI': ['liteDQMOfflineHeavyIons',
                          'PostDQMOfflineHI',
                          'dqmHarvesting'],

            'none': ['DQMNone',
                     'PostDQMOffline',
                     'DQMNone'],
            }

_phase2_allowed = ['trackingOnlyDQM','outerTracker','muon','hcal','hcal2','egamma']
autoDQM['phase2'] = ['','','']
for i in [0,2]:
    autoDQM['phase2'][i] = '+'.join([autoDQM[m][i] for m in _phase2_allowed])
autoDQM['phase2'][1] = 'PostDQMOffline'

