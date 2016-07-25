//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
              
#include "MollerGlobalMagnetField.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


MollerGlobalMagnetField::MollerGlobalMagnetField() 
{	
    
  G4cout << G4endl << "###### Calling MollerGlobalMagnetField::MollerGlobalMagnetField() " << G4endl << G4endl;

  fMagneticField_MainMagnet = NULL;
  fMagneticField_MiniMagnet = NULL;

  fieldMessenger = new MollerGlobalMagnetFieldMessenger(this);  
  //=======================================================
  //    configuration values of the QTOR field map
  //=======================================================

  BFieldScalingFactor_MainMagnet =   0.0;    // according to Juliette Mammei
  fMagneticField_MainMagnet = new MollerMagnetFieldMap();


  if(fMagneticField_MainMagnet){
    BFieldScalingFactor_MainMagnet = 0.0;
    fMagneticField_MainMagnet->SetFieldMap_RMin(0.015*m);      
    fMagneticField_MainMagnet->SetFieldMap_RMax(0.375*m); 
    fMagneticField_MainMagnet->SetFieldMap_RStepsize(0.002*m);
    
    fMagneticField_MainMagnet->SetFieldMap_ZMin(9.5*m); 
    fMagneticField_MainMagnet->SetFieldMap_ZMax(17.0*m);
    fMagneticField_MainMagnet->SetFieldMap_ZStepsize(0.1*m);
    
    fMagneticField_MainMagnet->SetFieldMap_PhiMin(-25*degree);
    fMagneticField_MainMagnet->SetFieldMap_PhiMax(25*degree);
    fMagneticField_MainMagnet->SetFieldMap_PhiStepsize(1*degree);
    
    fMagneticField_MainMagnet->InitializeGrid();
    
  }
  
  //=======================================================
  //    configuration values of the MiniTorus field map
  //=======================================================
  
  BFieldScalingFactor_MiniMagnet =  0.0;
  fMagneticField_MiniMagnet = new MollerMagnetFieldMap();
  
  if(fMagneticField_MiniMagnet){
    // 100% of maxiumum current density (560 A/cm^2)
    BFieldScalingFactor_MiniMagnet =  0.0; // according to Juliette Mammei: 84% (470 A/cm^2)
    
    fMagneticField_MiniMagnet->SetFieldMap_RMin(0.015*m);      
    fMagneticField_MiniMagnet->SetFieldMap_RMax(0.375*m); 
    fMagneticField_MiniMagnet->SetFieldMap_RStepsize(0.002*m);
    
    fMagneticField_MiniMagnet->SetFieldMap_ZMin(6.0*m); 
    fMagneticField_MiniMagnet->SetFieldMap_ZMax(8.0*m);
    fMagneticField_MiniMagnet->SetFieldMap_ZStepsize(0.1*m);
    
    fMagneticField_MiniMagnet->SetFieldMap_PhiMin(-25*degree);
    fMagneticField_MiniMagnet->SetFieldMap_PhiMax(25*degree);
    fMagneticField_MiniMagnet->SetFieldMap_PhiStepsize(1*degree);
    
    fMagneticField_MiniMagnet->InitializeGrid();
    
  }

  G4cout << G4endl
	 << "Magnetic Shielding low limit= " << mg_field_low
	 << "   Magnetic Shielding high limit= " << mg_field_high << G4endl;  
  G4cout << G4endl << "###### Leaving MollerGlobalMagnetField::MollerGlobalMagnetField " << G4endl << G4endl;
}		
    
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
MollerGlobalMagnetField::~MollerGlobalMagnetField()
{  
    G4cout << G4endl << "###### Calling MollerGlobalMagnetField::~MollerGlobalMagnetField() " << G4endl << G4endl;
    if (fMagneticField_MainMagnet) delete fMagneticField_MainMagnet;	
    if (fMagneticField_MiniMagnet) delete fMagneticField_MiniMagnet;
    delete fieldMessenger;    
    G4cout << G4endl << "###### Leaving MollerGlobalMagnetField::~MollerGlobalMagnetField() " << G4endl << G4endl;
}	
    
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void MollerGlobalMagnetField::GetFieldValue(const G4double Point[4], G4double *Bfield ) const
{
  
  G4double myLocalPointInMainMagnet[4];
  G4double myLocalPointInMiniMagnet[4];  
  G4double myLocalBfieldInMainMagnet[3];
  G4double myLocalBfieldInMiniMagnet[3];

  G4bool readfrommap = false;
  G4double dBxdy = 0.*gauss/cm;
  G4double dBydx = 0.*gauss/cm;

  // gives integral 7003.4 G @ 1 cm
  G4double a = magScaleFactor*158.*gauss/cm;
  G4double shielded = magScaleFactor*0.78 *gauss/cm;
  // bu and bd define center of gaussian approximations of fringe field along z-axis
  G4double bu = -44*cm;
  G4double bd = 44*cm;
  // c1 is gaussian width inside the shield
  G4double c1 = 4.2*cm;
  // c2 is gaussian width outside the shield
  G4double c2 = 14.15*cm;
  
  // Worksite for new gaussian - Ricky
  // Calculating the new peaks au and ad
  G4double au = a*(exp(-((pow(mg_field_low-bu,2))/(2*pow(c2,2)))));
  G4double ad = a*(exp(-((pow(mg_field_high-bd,2))/(2*pow(c2,2)))));
  G4double z_intercept_low = mg_field_low + pow( (2*pow(c1,2)*log(a*exp(-(pow(mg_field_low-bu,2)/(2*pow(c2,2))))/shielded)),0.5);
  G4double z_intercept_high =  mg_field_high - pow( (2*pow(c1,2)*log(a*exp(-(pow(mg_field_high-bd,2)/(2*pow(c2,2))))/shielded)),0.5);

  //---------------------------------------------------------------
  // translation from global Point[4] into local magnet coordinates
  //---------------------------------------------------------------
  myLocalPointInMainMagnet[0] = Point[0];             // x-pos
  myLocalPointInMainMagnet[1] = Point[1];             // y-pos
  myLocalPointInMainMagnet[2] = Point[2]-69.91*cm;    // z-pos
  myLocalPointInMainMagnet[3] = Point[3];             // time
  //---------------------------------------------------------------
  // (2) transformation into local MiniMagnet coordinate is easy
  //     since MiniMagnet is shifted by 505*cm in z
  myLocalPointInMiniMagnet[0] = Point[0];             // x-pos
  myLocalPointInMiniMagnet[1] = Point[1];             // y-pos
  myLocalPointInMiniMagnet[2] = Point[2];             // z-pos
  myLocalPointInMiniMagnet[3] = Point[3];             // time


  //  G4cout << "reading septum magnetic field" << G4endl;
  // Parameterizing the gradient as two gaussians at each end of the septum, defined for different z values
  // within the radius of the beampipe through the septum, r = 4.128 cm.
  if( !readfrommap && sqrt(pow(myLocalPointInMainMagnet[0],2)+pow(myLocalPointInMainMagnet[1],2))<4.128*cm){    
    if ((myLocalPointInMainMagnet[2]>-100*cm)&&(myLocalPointInMainMagnet[2]<100*cm)){
      if (myLocalPointInMainMagnet[2]<mg_field_low){ 
	dBxdy = a*(exp(-((pow(myLocalPointInMainMagnet[2]-bu,2))/(2*pow(c2,2)))));
      } else if ((myLocalPointInMainMagnet[2]>=mg_field_low)&&(myLocalPointInMainMagnet[2]<z_intercept_low)){
	dBxdy = a*(exp(-(pow(mg_field_low-bu                         ,2)/(2*pow(c2,2)))))*
	          (exp(-(pow(myLocalPointInMainMagnet[2]-mg_field_low,2)/(2*pow(c1,2)))));
      } else if ((myLocalPointInMainMagnet[2]>=z_intercept_low)&&(myLocalPointInMainMagnet[2]<=z_intercept_high)){
	dBxdy = shielded;
      } else if ((myLocalPointInMainMagnet[2]<=mg_field_high)&&(myLocalPointInMainMagnet[2]>z_intercept_high)){
	dBxdy = a*(exp(-(pow(mg_field_high-bd                         ,2)/(2*pow(c2,2)))))*
	          (exp(-(pow(myLocalPointInMainMagnet[2]-mg_field_high,2)/(2*pow(c1,2)))));
      } else if ((myLocalPointInMainMagnet[2]>mg_field_high)){ 
	dBxdy = a*(exp(-((pow(myLocalPointInMainMagnet[2]-bd,2))/(2*pow(c2,2)))));
      } else {
	dBxdy=0;
      }     	
      dBydx=dBxdy;      
      Bfield[0] = -myLocalPointInMainMagnet[1]*dBxdy;
      Bfield[1] = -myLocalPointInMainMagnet[0]*dBydx;
      Bfield[2] = 0.;      
    } else {	
      Bfield[0] = 0.;
      Bfield[1] = 0.;
      Bfield[2] = 0.;
    }
  } else {	
      Bfield[0] = 0.;
      Bfield[1] = 0.;
      Bfield[2] = 0.;
  }
  
  if(readfrommap){
    if(fMagneticField_MiniMagnet && fMagneticField_MainMagnet){
      fMagneticField_MiniMagnet->GetFieldValue( myLocalPointInMiniMagnet, myLocalBfieldInMiniMagnet );
      fMagneticField_MainMagnet->GetFieldValue( myLocalPointInMainMagnet, myLocalBfieldInMainMagnet );
      
      Bfield[0] = myLocalBfieldInMainMagnet[0]*BFieldScalingFactor_MainMagnet + myLocalBfieldInMiniMagnet[0]*BFieldScalingFactor_MiniMagnet;
      Bfield[1] = myLocalBfieldInMainMagnet[1]*BFieldScalingFactor_MainMagnet + myLocalBfieldInMiniMagnet[1]*BFieldScalingFactor_MiniMagnet;
      Bfield[2] = myLocalBfieldInMainMagnet[2]*BFieldScalingFactor_MainMagnet + myLocalBfieldInMiniMagnet[2]*BFieldScalingFactor_MiniMagnet;
    }
    else if(fMagneticField_MainMagnet && !fMagneticField_MiniMagnet){
      fMagneticField_MainMagnet->GetFieldValue( myLocalPointInMainMagnet, myLocalBfieldInMainMagnet );
    
      Bfield[0] = myLocalBfieldInMainMagnet[0]*BFieldScalingFactor_MainMagnet;
      Bfield[1] = myLocalBfieldInMainMagnet[1]*BFieldScalingFactor_MainMagnet;
      Bfield[2] = myLocalBfieldInMainMagnet[2]*BFieldScalingFactor_MainMagnet;
    }
    else if(!fMagneticField_MainMagnet && fMagneticField_MiniMagnet){
      fMagneticField_MiniMagnet->GetFieldValue( myLocalPointInMiniMagnet, myLocalBfieldInMiniMagnet );

      Bfield[0] =  myLocalBfieldInMiniMagnet[0]*BFieldScalingFactor_MiniMagnet;
      Bfield[1] =  myLocalBfieldInMiniMagnet[1]*BFieldScalingFactor_MiniMagnet;
      Bfield[2] =  myLocalBfieldInMiniMagnet[2]*BFieldScalingFactor_MiniMagnet;    
    }else{
      Bfield[0] = 0.0*kilogauss;
      Bfield[1] = 0.0*kilogauss;
      Bfield[2] = 0.0*kilogauss;
    }
  }
  
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void MollerGlobalMagnetField::WriteMagField()
{
    G4cout << G4endl
	   << "###### Printing Septum field on file ######"
	   << G4endl << G4endl;
    G4cout << G4endl
	   << "Magnetic Shielding low limit= " << mg_field_low
	   << "   Magnetic Shielding high limit= " << mg_field_high << G4endl;

    double z[201];
    double grB[201][3];
    double grB2[201][3];
    double gr_dBxdy[201];
    G4double g4grB[201][3];
    G4double point[4];
    point[0] = 1.*cm;
    point[1] = 0.*cm;
    point[3] = 0.*ns;
    G4double point2[4];
    double gr_dy = 0.001;
    point2[0] = -1.*cm;
    point2[1] = gr_dy *cm;
    point2[3] = 0.*ns;
    for (int i=0; i<201; i++) {
      z[i] = 175.211-105.3+double(i-100);
      point[2] = z[i] *cm;
      point2[2] = z[i] *cm;
      GetFieldValue(point,g4grB[i]);
      for (int j=0; j<3; j++) grB[i][j]=(double)g4grB[i][j]/gauss;
      GetFieldValue(point2,g4grB[i]);
      for (int j=0; j<3; j++) grB2[i][j]=(double)g4grB[i][j]/gauss;
      gr_dBxdy[i] = (grB2[i][0]- grB[i][0])/gr_dy;
      G4cout << z[i] << " " << point[2]/cm << " " << gr_dBxdy[i] << G4endl;
    }
    TGraph * graph_Mag_field = new TGraph(201,z,gr_dBxdy);
    graph_Mag_field->SetTitle("Magnetic Field #frac{dBx}{dy} at (-1cm,0cm,z);z(cm);gauss/cm");
    graph_Mag_field->Write();
}
