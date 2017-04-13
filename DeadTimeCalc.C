// Example: root -l DeadTimeCalc.C

#include "TFile.h"
#include "TTree.h"
#include <TTreeReader.h>
#include <TTreeReaderValue.h>

double DeadTimeFromTrueAndMeasRates(double r, double m) 
{
	return 1/m - 1/r;
}

void DeadTimeCalc() {
	gStyle->SetOptStat(0);
	gStyle->SetOptTitle(0);
	
	TFile* f = new TFile("~/godaq_rootfiles/analysis_v2.10.0/run9.root", "read");
	//TFile* f = new TFile("~/godaq_rootfiles/analysis_v2.10.0/run78.root", "read");
	TTree* t = (TTree*) f->Get("tree");
	
        TTreeReader reader(t);
        TTreeReaderValue<UInt_t> Run(reader, "Run");
        TTreeReaderValue<UInt_t> Evt(reader, "Evt");
	TTreeReaderValue<ULong64_t> TimeStamp(reader, "TimeStamp");
	TTreeReaderValue<Double_t> RateLvsR4(reader, "RateLvsR4");
	TTreeReaderValue<Double_t> RateLvsR5(reader, "RateLvsR5");

	TH1D* hTimeDiff = new TH1D("hTimeDiff", "hTimeDiff", 200,0.03,0.09);
	TH1D* hRateLvsR4 = new TH1D("hRateLvsR4", "hRateLvsR4", 200,0,20e3);
	TH1D* hRateLvsR5 = new TH1D("hRateLvsR5", "hRateLvsR5", 200,0,20e3);
	hRateLvsR5->SetLineColor(kRed);
	
	ULong64_t TimeStampPrev = 0;
        while (reader.Next()) {
		//cout << "Evt, TimeStamp = " << *Evt << "  " << *TimeStamp << endl;
		if(*Evt != 0) {
			Double_t timeDiff = (*TimeStamp - TimeStampPrev)*1/64.e6;
			//cout << "timeDiff = " << timeDiff << " seconds" << endl;
			hTimeDiff->Fill(timeDiff);
		}
		TimeStampPrev = *TimeStamp;
		hRateLvsR4->Fill(*RateLvsR4);
		hRateLvsR5->Fill(*RateLvsR5);
	}
	
	cout << "--> Method 1 (directly from DAQ rate, assumes we are on the asymptot)" << endl;
	TCanvas* c1 = new TCanvas("c1", "c1", 600, 400);
	hTimeDiff->Draw();
	gPad->SetLogy();
	double timeDiffMean = hTimeDiff->GetMean();
	double DAQrate = 1/timeDiffMean;
	cout << "TimeDiff mean = " << timeDiffMean << " seconds" << endl;
	cout << "   => DAQ rate = " << DAQrate << " Hz" << endl;
		
	cout << "--> Method 2 (computes dead time from r and m)" << endl;
	TCanvas* c2 = new TCanvas("c2", "c2", 600, 400);
	hRateLvsR4->Draw();
	hRateLvsR5->Draw("same");
	double r_4 = hRateLvsR4->GetMean();
	double r_5 = hRateLvsR5->GetMean();
	cout << "RateLvsR4 mean = " << r_4 << " Hz" << endl;
	cout << "RateLvsR5 mean = " << r_5 << " Hz" << endl;
	cout << "Dead time with coinc window 4 = " << DeadTimeFromTrueAndMeasRates(r_4, DAQrate) << endl;
	cout << "Dead time with coinc window 5 = " << DeadTimeFromTrueAndMeasRates(r_5, DAQrate) << endl;
}
