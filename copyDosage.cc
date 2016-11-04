#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <math.h>

#include <TH1F.h>
#include <TH2F.h>
#include <TH3F.h>
#include <TTree.h>
#include <TF1.h>
#include <TFile.h>
#include <TChain.h>
#include <TDirectory.h>

using namespace std;

int SensVolume_v;

bool ScanFamily(int parent, std::vector<int>& family);

int main(int argc,char** argv){

  if(argc != 5){
    cout<<"Usage: ./build/anaDose <outputFile> <det ID#> <Neutron cut> <eM cut>"<<endl<<"If you don't want any MeV cuts, just put in 0's"<<endl;
    return 1;
  }
  

  // First we read in the input file and which volume to look at. Volumes will be the plastic detectors (plasDets 3001-3006). More may be put in later, this is just an initial macro. 
  string ifnm=argv[1];
  SensVolume_v = atoi(argv[2]);
  Float_t N_cut = atof(argv[3]);
  Float_t eM_cut = atof(argv[4]);
  
  Float_t type, volume, event, track, parent;
  Int_t event_n=0;
  Float_t Edeposit, kineE;
  Float_t neutron_E=0., eM_E=0., other_E=0.;
  Float_t neutronCut_E=0., eMCut_E=0.;
  Float_t x_0,y_0,z_0,xd,yd,zd;
  
  TChain *t = new TChain("geant");
  t->Add(ifnm.c_str());
  t->SetBranchAddress("type",&type);
  t->SetBranchAddress("volume",&volume);
  t->SetBranchAddress("x",&xd);  
  t->SetBranchAddress("y",&yd);  
  t->SetBranchAddress("z",&zd);  
  t->SetBranchAddress("x0",&x_0);
  t->SetBranchAddress("y0",&y_0);
  t->SetBranchAddress("z0",&z_0);
  t->SetBranchAddress("event",&event);
  t->SetBranchAddress("parent",&parent);
  t->SetBranchAddress("track",&track);
  t->SetBranchAddress("kineE",&kineE);
  t->SetBranchAddress("Edeposit",&Edeposit);

  //These are 'catalogs', or what I'll call 'families', which will store the ancestors of a particle. Say we see and proton, but its the daugther of an icident neutron. Then that proton's track will be filed under the neutron family and its damaging energy will be associated with neutron radiation.
  std::vector<int> neutron_Fam;
  std::vector<int> neutronCut_Fam;
  std::vector<int> eM_Fam;
  std::vector<int> eMCut_Fam;
  std::vector<int> other_Fam;
  
  Int_t nentries = (Int_t)t->GetEntries();
  for (int i=0; i<nentries ; i++) {
    if (i % 1000000 == 0 )
      cout<<" processed: "<< i <<"\t"
	  << neutron_E <<"\t"
	  << event <<"\t"
	  << event_n <<endl;

    t->GetEntry(i);
    if ((int)event != event_n) {
      neutron_Fam.clear();
      eM_Fam.clear();
      other_Fam.clear();
      event_n = event;

      neutronCut_Fam.clear();
      eMCut_Fam.clear();
      
    }

    // This portion marks particles coming in with an incident energy greater than the user prescribed cutoff by using surrounding vacuum detectors
    if ((int)volume == SensVolume_v - 10 || (int)volume == SensVolume_v - 20){
      if (type == 5 && kineE > N_cut) {
	neutronCut_Fam.push_back(track);
      }
      if ((type == 0 || type == 1 || type ==4) && kineE > eM_cut){
	eMCut_Fam.push_back(track);
      }
    } 

    // Now we start looking at when the particles actually hit the detector
    if ((int)volume == SensVolume_v) {
      
      // This is the part where we will see if this is a daughter particle or whether it is an incident particle, and what 'family' to put its energy in 
      if (ScanFamily(parent,neutron_Fam)) {
	neutron_Fam.push_back(track);
	neutron_E += Edeposit;
	// We also check if this is a daughter particle of an incident neutron that was greater than the neutron energy cut off
	if (ScanFamily(parent,neutronCut_Fam)) {
	  neutronCut_Fam.push_back(track);
	  neutronCut_E += Edeposit;
	} 
      }
      else if (ScanFamily(parent,eM_Fam)) {
	eM_Fam.push_back(track);
	eM_E += Edeposit;
	if (ScanFamily(parent,eMCut_Fam)) {
	  eMCut_Fam.push_back(track);
	  eMCut_E += Edeposit;
	}
      }
      else if (ScanFamily(parent,other_Fam)) {
	other_Fam.push_back(track);
	other_E += Edeposit;
      }
    
      // If the particle has no family history, its an incident particle, and is filed away in the family vector corresponding to its type. In addition, we include a statement to scan to see if the particle was recording as coming in with an energy above the cutoff. 
      else {
	if (type == 5){
	  neutron_Fam.push_back(track);
	  neutron_E += Edeposit;
	  if (ScanFamily(track,neutronCut_Fam)) {
	    neutronCut_Fam.push_back(track);
	    neutronCut_E += Edeposit;
	  }
	}
	else if (type == 0 || type == 1 || type == 4) {
	  eM_Fam.push_back(track);
	  eM_E += Edeposit;
	  if (ScanFamily(track,eMCut_Fam)) {
	    eMCut_Fam.push_back(track);
	    eMCut_E += Edeposit;
	  }
	}
	else {
	  other_Fam.push_back(track);
	  other_E += Edeposit;
	}
      }      
    }
  }
  cout<<"Neutron Energy: "<<neutron_E<<" [MeV]"<<endl;
  if (N_cut > 0) cout<<">"<<N_cut<<"MeV Neutron Energy "<<neutronCut_E<<" [MeV]"<<endl;
  cout<<"eM Energy: "<<eM_E<<" [MeV]"<<endl;
  if (N_cut > 0) cout<<">"<<eM_cut<<"MeV eM Energy "<<eMCut_E<<" [MeV]"<<endl;
  cout<<"Other Energy: "<<other_E<<" [MeV]"<<endl;
  
  return 0;
}


// This function will check the family vectors to see if the parent (or track) of the particle of interest was recorded.

bool ScanFamily(int parent, std::vector<int>& family){
  for (int i=0; i<(int)family.size(); i++){
    if (family.at(i) == parent) {
      return true;
    }
  }
  return false;
}
