// Example: root -l DeadTimeCalc.C

#include "TFile.h"
#include "TTree.h"
#include <TTreeReader.h>
#include <TTreeReaderValue.h>

void DeadTimeCalc() {
	gStyle->SetOptStat(0);
	gStyle->SetOptTitle(0);
	
	TFile* f = new TFile("~/godaq_rootfiles/analysis_v2.10.0/run9.root", "read");
	TTree* t = (TTree*) f->Get("tree");
	
        TTreeReader reader(t);
        TTreeReaderValue<UInt_t> Run(reader, "Run");
        TTreeReaderValue<UInt_t> Evt(reader, "Evt");
	TTreeReaderValue<ULong64_t> TimeStamp(reader, "TimeStamp");

	TH1D* hTimeDiff = new TH1D("hTimeDiff", "hTimeDiff", 200,0.03,0.09);
	ULong64_t TimeStampPrev = 0;
        while (reader.Next()) {
		//cout << "Evt, TimeStamp = " << *Evt << "  " << *TimeStamp << endl;
		if(*Evt != 0) {
			Double_t timeDiff = (*TimeStamp - TimeStampPrev)*1/64.e6;
			//cout << "timeDiff = " << timeDiff << " seconds" << endl;
			hTimeDiff->Fill(timeDiff);
		}
		TimeStampPrev = *TimeStamp;
	}
	hTimeDiff->Draw();
	gPad->SetLogy();
	cout << "TimeDiff mean = " << hTimeDiff->GetMean() << " seconds" << endl;
	cout << "   => Max DAQ rate = " << 1/hTimeDiff->GetMean() << " Hz" << endl;
}
