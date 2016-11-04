void cdf2(){
  TFile *fin=TFile::Open("crex4Crex5highEHistsRicky.root","READ");
  TH1F *h4=(TH1F*)fin->Get("crex_4");
  TH1F *h5=(TH1F*)fin->Get("crex_5");

  h4->SetTitle("Front Coll: r>1.133; energy [MeV]; N*KE [MeV]");
  h5->SetTitle("Front Coll: r>1.133; energy [MeV]; N*KE [MeV]");


  
  TH1F *c4=(TH1F*)h4->Clone("c4");
  TH1F *c5=(TH1F*)h4->Clone("c5");
  TH1F *c6=(TH1F*)h4->Clone("c6");

  makeCdf(h4,c4,(double)h4->Integral());
  makeCdf(h5,c5,(double)h5->Integral());
  makeCdf(h5,c6,(double)h4->Integral());

  gStyle->SetOptStat(0);
  TCanvas *c1=new TCanvas("c1","c1",1800,800);
  c1->Divide(3);
  c1->cd(1);
  h5->SetLineColor(4);
  h5->SetMaximum(100000000);
  h5->DrawCopy();
  h4->SetLineColor(2);
  h4->DrawCopy("same");
  gPad->SetGridy(1);
  gPad->SetGridx(1);
  gPad->SetLogy();
  
  leg = new TLegend(0.1,0.7,0.48,0.9);
  leg->AddEntry("crex_4","Crex 4","l");
  leg->AddEntry("crex_5","Crex 5 2.2 Gev","l");
  leg->Draw();
  
  c1->cd(2);
  c4->SetLineColor(2);
  c4->DrawCopy();
  c5->SetLineColor(4);
  c5->DrawCopy("same");
  gPad->SetGridy(1);
  gPad->SetGridx(1);
  c1->cd(3);
  c4->SetLineColor(2);
  c4->DrawCopy();
  c6->SetLineColor(4);
  c6->DrawCopy("same");
  gPad->SetGridy(1);
  gPad->SetGridx(1);

  TCanvas *c2=new TCanvas("c2","c2",1200,800);
  c2->Divide(2);
  c2->cd(1);
  TH1F *d4=(TH1F*)c4->Clone("d4");
  d4->SetTitle("ratio of CDF distributions: C5highE/C4");
  d4->Divide(c5,c4);
  d4->SetLineColor(4);
  d4->DrawCopy();
  gPad->SetGridy(1);
  gPad->SetGridx(1);
  c2->cd(2);
  TH1F *d4=(TH1F*)c5->Clone("d5");
  d5->SetTitle("C5highE - C4");
  d5->Add(c5,c4,1,-1);
  d5->SetLineColor(4);
  d5->DrawCopy();
  gPad->SetGridy(1);
  gPad->SetGridx(1);
  
  fin->Close();

}

void makeCdf(TH1F *h,TH1F *c,double integ){
  int nbin=h->GetXaxis()->GetNbins();
  double sum(0);
  for(int i=1;i<=nbin;i++){
    sum+=h->GetBinContent(i);
    c->SetBinContent(i,sum/integ);
  }  
}
