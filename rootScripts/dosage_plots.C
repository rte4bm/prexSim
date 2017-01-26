using namespace std;

void dosage_plots(){
  TFile *_file0 = TFile::Open("prex2doseNew_1.05GeV_1e6.root");
  TCanvas *c1=new TCanvas("c1","@50 cm Detectors",1800,800);
  c1->Divide(2,2);
  c1->cd(1);
  geant->Draw("y/10.:z/10. >> h1","volume==4021 && z0>-1000","zcont");
  h1->SetTitle("Beam Left");
  h1->GetXaxis()->SetTitle("z [cm]");
  h1->GetYaxis()->SetTitle("y [cm]");
  c1->cd(2);
  geant->Draw("x/10.:z/10. >> h2","volume==4022 && z0>-1000","zcont");
  h2->SetTitle("Bottom");
  h2->GetXaxis()->SetTitle("z [cm]");
  h2->GetYaxis()->SetTitle("x [cm]");
  c1->cd(3);
  geant->Draw("y/10.:z/10. >> h3","volume==4023 && z0>-1000","zcont");
  h3->SetTitle("Beam Right");
  h3->GetXaxis()->SetTitle("z [cm]");
  h3->GetYaxis()->SetTitle("y [cm]");
  c1->cd(4);
  geant->Draw("x/10.:z/10. >> h4","volume==4024 && z0>-1000","zcont");
  h4->SetTitle("Top");
  h4->GetXaxis()->SetTitle("z [cm]");
  h4->GetYaxis()->SetTitle("x [cm]");

};
