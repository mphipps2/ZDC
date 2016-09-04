//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: DetectorConstruction.cc 94307 2015-11-11 13:42:46Z gcosmo $
//
/// \file DetectorConstruction.cc
/// \brief Implementation of the DetectorConstruction class

#include "DetectorConstruction.hh"
#include "SharedData.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4CSGSolid.hh"
#include "G4Box.hh"
#include "G4Para.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include <iostream>
#include <stdio.h>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
  : G4VUserDetectorConstruction(),
    m_moduleSizeX(0), m_moduleSizeY(0), m_moduleSizeZ(0),
    m_chamberSizeX(0), m_chamberSizeY(0), m_chamberSizeZ(0),
    m_scoringVolume(0),
    m_sd(NULL)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction( SharedData* sd )
  : G4VUserDetectorConstruction(),
    m_moduleSizeX(0), m_moduleSizeY(0), m_moduleSizeZ(0),
    m_chamberSizeX(0), m_chamberSizeY(0), m_chamberSizeZ(0),
    m_scoringVolume(0),
    m_sd( sd )
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::InitializeParameters()
{

  // Get Config
  TEnv* config = m_sd->GetConfig();
  
  m_moduleSizeX     = config->GetValue( "moduleSizeX", 90);
  m_moduleSizeY     = config->GetValue( "moduleSizeY", 240);
  m_moduleSizeZ     = config->GetValue( "moduleSizeZ", 150);

  G4double quartzThickness     = config->GetValue( "quartzThickness", 2);
  G4double moduleWallThickness = config->GetValue( "moduleWallThickness", 2);

  m_chamberSizeX  = m_moduleSizeX - 2*(moduleWallThickness + quartzThickness);
  m_chamberSizeY  = config->GetValue( "absorberHeight", 180 );
  m_chamberSizeZ  = m_moduleSizeZ - 2*moduleWallThickness;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  InitializeParameters();
  
  // Get Config
  TEnv* config = m_sd->GetConfig();
  
  // Get nist material manager
  G4NistManager* nist = G4NistManager::Instance();
  
  // Option to switch on/off checking of volumes overlaps
  //
  G4bool checkOverlaps = true;

  //----------------------------------------------     
  // World
  //----------------------------------------------
  G4double world_sizeX  = 1.2 * m_moduleSizeX;   // mm
  G4double world_sizeY  = 1.2 * m_moduleSizeY;   // mm
  G4double world_sizeZ  = 1.2 * m_moduleSizeZ;   // mm

  G4Material* g4Air = nist->FindOrBuildMaterial("G4_AIR");
  
  printf( "Building world with %5.1f x %5.1f x %5.1f\n",
	  world_sizeX, world_sizeY, world_sizeZ );
  
  m_solidWorld =    
    new G4Box("World",                       //its name
	      0.5*world_sizeX, 0.5*world_sizeY, 0.5*world_sizeZ);   //its size
  
  m_logicWorld =                         
    new G4LogicalVolume(m_solidWorld,        //its solid
                        g4Air,           //its material
                        "World");            //its name
                                   
  m_physWorld = 
    new G4PVPlacement(0,                     //no rotation
                      G4ThreeVector(),       //at (0,0,0)
                      m_logicWorld,          //its logical volume
                      "World",               //its name
                      0,                     //its mother  volume
                      false,                 //no boolean operation
                      0,                     //copy number
                      checkOverlaps);        //overlaps checking
                     
  //----------------------------------------------     
  // Box
  //---------------------------------------------- 
  G4Material* g4Al = nist->FindOrBuildMaterial("G4_Al");

  m_solidBox =    
    new G4Box("Box",                         //its name
	      0.5*m_moduleSizeX, 0.5*m_moduleSizeY, 0.5*m_moduleSizeZ);  //its size
  
  m_logicBox =                         
    new G4LogicalVolume(m_solidBox,          //its solid
                        g4Al,                //its material
                        "Box");              //its name
                                   
  m_physBox = 
    new G4PVPlacement(0,                     //no rotation
                      G4ThreeVector(),       //at (0,0,0)
                      m_logicBox,            //its logical volume
                      "Box",                 //its name
                      m_logicWorld,          //its mother  volume
                      false,                 //no boolean operation
                      0,                     //copy number
                      checkOverlaps);        //overlaps checking
                     
  //----------------------------------------------     
  // Quartz
  //----------------------------------------------
  G4double quartzThickness     = config->GetValue( "quartzThickness", 2);
  G4double moduleWallThickness = config->GetValue( "moduleWallThickness", 2);

  std::vector< std::pair<G4double, G4double> > quartzCenters;
  quartzCenters.emplace_back( 0,  0.5*m_moduleSizeY -
			      moduleWallThickness - 0.5*quartzThickness ); // top
  quartzCenters.emplace_back( 0, -0.5*m_moduleSizeY +
			      moduleWallThickness + 0.5*quartzThickness ); // bottom
  quartzCenters.emplace_back( -0.5*m_moduleSizeX + moduleWallThickness +
			      0.5*quartzThickness, 0 ); // left
  quartzCenters.emplace_back(  0.5*m_moduleSizeX - moduleWallThickness -
			       0.5*quartzThickness, 0 ); // right
  
  std::vector< std::pair<G4double, G4double> > quartzSizeXY;
  quartzSizeXY.emplace_back( m_moduleSizeX - 2*moduleWallThickness - 2*quartzThickness,
			     quartzThickness );          // top
  quartzSizeXY.emplace_back( m_moduleSizeX - 2*moduleWallThickness - 2*quartzThickness,
			     quartzThickness );          // bottom
  quartzSizeXY.emplace_back( quartzThickness,
			     m_moduleSizeY - 2*moduleWallThickness); // left
  quartzSizeXY.emplace_back( quartzThickness,
			     m_moduleSizeY - 2*moduleWallThickness); // right
  
  G4double quartzSizeZ = m_moduleSizeZ - 2*moduleWallThickness;

  G4Material* g4SiO2 = nist->FindOrBuildMaterial("G4_SILICON_DIOXIDE");
  
  G4VisAttributes* quartzColor = new G4VisAttributes( G4Colour::Blue() );
  
  for( unsigned int qn = 0; qn < quartzCenters.size(); qn++ ){
    m_v_solidQuartz.
      push_back( new G4Box("Quartz",            //its name
			   0.5*quartzSizeXY.at(qn).first,
			   0.5*quartzSizeXY.at(qn).second,
			   0.5*quartzSizeZ) );  //its size
    
    m_v_logicQuartz.
      push_back( new G4LogicalVolume(m_v_solidQuartz.back(), //its solid
				     g4SiO2,                 //its material
				     "Quartz") );            //its name

    m_v_logicQuartz.back()->SetVisAttributes( quartzColor );
    
    m_v_physQuartz.
      push_back( new G4PVPlacement(0,                       //no rotation
				   G4ThreeVector(quartzCenters.at(qn).first,
						 quartzCenters.at(qn).second,
						 0),
				   m_v_logicQuartz.back(),  //its logical volume
				   "Quartz",                //its name
				   m_logicBox,              //its mother  volume
				   false,                   //no boolean operation
				   qn,                      //copy number
				   checkOverlaps) );        //overlaps checking
  }
 
  //----------------------------------------------     
  // Oil
  //----------------------------------------------
  G4double oilSizeX      = m_chamberSizeX;
  G4double oilSizeY      = m_chamberSizeY;
  G4double oilSizeZ      = m_chamberSizeZ;
  G4double oilPosY       = -0.5*m_moduleSizeY + moduleWallThickness +
    quartzThickness + 0.5*oilSizeY;
  
  G4double absorberSizeY = oilSizeY;         // they should be same for now
  
  m_solidOil =    
    new G4Box("Oil",                         //its name
	      0.5*oilSizeX, 0.5*oilSizeY, 0.5*oilSizeZ);  // its size

  G4Material* g4H2O  = nist->FindOrBuildMaterial("G4_WATER");
  
  m_logicOil =                         
    new G4LogicalVolume(m_solidOil,          //its solid
                        g4H2O,               //its material
                        "Oil");              //its name
  
  m_physOil = 
    new G4PVPlacement(0,                     //no rotation
                      G4ThreeVector(0, oilPosY, 0), //at (0,oilPosY,0)
                      m_logicOil,            //its logical volume
                      "Oil",                 //its name
                      m_logicBox,            //its mother  volume
                      false,                 //no boolean operation
                      0,                     //copy number
                      checkOverlaps);        //overlaps checking
  
  G4VisAttributes* oilColor = new G4VisAttributes( G4Colour::Magenta() );
  m_logicOil->SetVisAttributes( oilColor );
  
  //----------------------------------------------     
  // Top
  //----------------------------------------------
  G4double topSizeX     = m_moduleSizeX - 2*(moduleWallThickness + quartzThickness);
  G4double topSizeY     = m_moduleSizeY - absorberSizeY - 2*(moduleWallThickness + quartzThickness);
  G4double topSizeZ     = m_moduleSizeZ - 2*moduleWallThickness;
  G4double topPosY      = 0.5*m_moduleSizeY - moduleWallThickness -
    quartzThickness - 0.5*topSizeY;
  
  m_solidTop =    
    new G4Box("Top",                         //its name
	      0.5*topSizeX, 0.5*topSizeY, 0.5*topSizeZ);  // its size

  G4Material* g4Fe  = nist->FindOrBuildMaterial("G4_Fe");
  
  m_logicTop =                         
    new G4LogicalVolume(m_solidTop,          //its solid
                        g4Fe,               //its material
                        "Top");              //its name
  
  m_physTop = 
    new G4PVPlacement(0,                     //no rotation
                      G4ThreeVector(0,topPosY,0),       //at (0,topPosY,0)
                      m_logicTop,            //its logical volume
                      "Top",                 //its name
                      m_logicBox,            //its mother  volume
                      false,                 //no boolean operation
                      0,                     //copy number
                      checkOverlaps);        //overlaps checking
  
  G4VisAttributes* topColor = new G4VisAttributes( G4Colour::Grey() );
  m_logicTop->SetVisAttributes( topColor );

  /*
  //----------------------------------------------     
  // Plates
  //---------------------------------------------- 
  std::vector< std::pair< G4double, G4double > > panelCenters;
  std::vector< G4double > panelLengths;
  
  ComputeDiagonalGeo( panelCenters, panelLengths );

  G4double theta              = config->GetValue( "absorberTheta", 0.5 );
  
  G4double reflectorThickness = config->GetValue( "reflectorThickness", 0.1 );
  G4double absorberThicknessZ = config->GetValue( "absorberThicknessZ", 1 );
  G4double absorberSizeZ      = absorberThicknessZ * TMath::Cos( theta );
 
  G4double panelSizeY         = absorberSizeY;
  G4double panelSizeZ         = absorberSizeZ + 2*reflectorThickness;
  
  G4Material* g4W  = nist->FindOrBuildMaterial("G4_W");
  G4Material* g4Au = nist->FindOrBuildMaterial("G4_Au"); 
  
  G4VisAttributes* panelColor    = new G4VisAttributes( G4Colour::Yellow() );
  G4VisAttributes* absorberColor = new G4VisAttributes( G4Colour::Red() );

  G4RotationMatrix* rotPanel = new G4RotationMatrix();
  rotPanel->rotateY( theta*rad);
  
  for( unsigned int sn = 0; sn < panelCenters.size(); sn++ ){
    m_v_solidPanel. 
      push_back( new G4Box("Panel",               //its name
			   0.5*panelLengths.at(sn),
			   0.5*panelSizeY,  
			   0.5*panelSizeZ) );     //its size
    
    m_v_solidAbsorber.
      push_back( new G4Box("Absorber",            //its name
			   0.5*panelLengths.at(sn),
			   0.5*absorberSizeY,
			   0.5*absorberSizeZ) );  //its size

    m_v_logicPanel.
      push_back( new G4LogicalVolume(m_v_solidPanel.back(), //its solid
				     g4Au,                  //its material
				     "Panel") );            //its name

    m_v_logicPanel.back()->SetVisAttributes( panelColor );

    m_v_logicAbsorber.
      push_back( new G4LogicalVolume(m_v_solidAbsorber.back(), //its solid
				     g4W,                      //its material
				     "Absorber") );            //its name

    m_v_logicAbsorber.back()->SetVisAttributes( absorberColor );
    
    m_v_physPanel.
      push_back( new G4PVPlacement(rotPanel,               //no rotation
				   G4ThreeVector(panelCenters.at(sn).first,
						 0,
						 panelCenters.at(sn).second),
				   m_v_logicPanel.back(),  //its logical volume
				   "Panel",                //its name
				   m_logicOil,             //its mother  volume
				   false,                  //no boolean operation
				   sn,                     //copy number
				   checkOverlaps) );       //overlaps checking

    m_v_physAbsorber.
      push_back( new G4PVPlacement(0,                         //no rotation
				   G4ThreeVector(),
				   m_v_logicAbsorber.back(),  //its logical volume
				   "Absorber",                //its name
				   m_v_logicPanel.back(),     //its mother  volume
				   false,                     //no boolean operation
				   sn,                        //copy number
				   checkOverlaps) );          //overlaps checking
  }
  */
  //----------------------------------------------     
  // Plates
  //---------------------------------------------- 
  std::vector< std::pair< G4double, G4double > > panelCenters;
  std::vector< G4double > panelLengths;
  
  ComputeChevronGeo( panelCenters );
  
  G4double theta               = config->GetValue( "absorberTheta", 0.5 );
  
  G4double absorberThicknessZ  = config->GetValue( "absorberThicknessZ", 1 );
  G4double absorberSizeZ       = absorberThicknessZ * TMath::Cos( theta );

  G4double reflectorThickness  = config->GetValue( "reflectorThickness", 0.1 ); 
  G4double reflectorThicknessZ = reflectorThickness / TMath::Cos( theta );

  // These are before rotation. Thats why the x<->z
  G4double panelSizeX         = absorberSizeZ + 2*reflectorThicknessZ;
  G4double panelSizeY         = absorberSizeY;
  G4double panelSizeZ         = 0.5 * oilSizeX;

  // These are before rotation. Thats why the x<->z
  G4double absorberSizeXX     = absorberSizeZ;
  G4double absorberSizeYY     = absorberSizeY;
  G4double absorberSizeZZ     = 0.5 * oilSizeX;

  
  G4Material* g4W  = nist->FindOrBuildMaterial("G4_W");
  G4Material* g4Au = nist->FindOrBuildMaterial("G4_Au"); 
  
  G4VisAttributes* panelColor    = new G4VisAttributes( G4Colour::Yellow() );
  G4VisAttributes* absorberColor = new G4VisAttributes( G4Colour::Red() );

  G4RotationMatrix* rotPanel = new G4RotationMatrix();
  rotPanel->rotateY( 90*deg );
  
  for( unsigned int sn = 0; sn < panelCenters.size(); sn++ ){
    
    for( int LR = 0; LR < 2; LR++ ){

      m_v_solidPanel. 
	push_back( new G4Para("Panel",               //its name
			      0.5 * panelSizeX,
			      0.5 * panelSizeY,
			      0.5 * panelSizeZ,
			      0,
			      TMath::Power( -1, LR + 1 ) * theta,
			      0 ) );     //its size
    
      m_v_solidAbsorber.
	push_back( new G4Para("Absorber",            //its name
			      0.5*absorberSizeXX,
			      0.5*absorberSizeYY,
			      0.5*absorberSizeZZ,
			      0,
			      TMath::Power( -1, LR + 1 ) * theta,
			      0) );  
    
      m_v_logicPanel.
	push_back( new G4LogicalVolume(m_v_solidPanel.back(), //its solid
				       g4Au,                  //its material
				       "Panel") );            //its name

      m_v_logicPanel.back()->SetVisAttributes( panelColor );

      m_v_logicAbsorber.
	push_back( new G4LogicalVolume(m_v_solidAbsorber.back(), //its solid
				       g4W,                      //its material
				       "Absorber") );            //its name

      m_v_logicAbsorber.back()->SetVisAttributes( absorberColor );
    
      m_v_physPanel.
	push_back( new G4PVPlacement(rotPanel,               //no rotation
				     G4ThreeVector( TMath::Power( -1, LR + 1 ) *
						    panelCenters.at(sn).first,
						   0,
						   panelCenters.at(sn).second),
				     m_v_logicPanel.back(),  //its logical volume
				     "Panel",                //its name
				     m_logicOil,             //its mother  volume
				     false,                  //no boolean operation
				     sn,                     //copy number
				     checkOverlaps) );       //overlaps checking

      m_v_physAbsorber.
	push_back( new G4PVPlacement(0,                         //no rotation
				     G4ThreeVector(),
				     m_v_logicAbsorber.back(),  //its logical volume
				     "Absorber",                //its name
				     m_v_logicPanel.back(),     //its mother  volume
				     false,                     //no boolean operation
				     sn,                        //copy number
				     checkOverlaps) );          //overlaps checking
    }
  }
  
  // Set Box as scoring volume
  //
  m_scoringVolume = m_logicBox;

  //
  //always return the physical World
  //
  return m_physWorld;
}

void DetectorConstruction ::
ComputeChevronGeo(  std::vector<std::pair< G4double, G4double > >& centers ){
  TEnv* config = m_sd->GetConfig();
  
  double absorberThicknessZ = config->GetValue( "absorberThicknessZ", 10 );
  double      gapThicknessZ = config->GetValue( "gapThicknessZ", 2 );
  double              theta = config->GetValue( "absorberTheta", 0.5 );

  double reflectorThickness  = config->GetValue( "reflectorThickness", 0.1 );
  double reflectorThicknessZ = reflectorThickness / cos( theta );

  double tanTheta   = TMath::Tan( theta );

  double totalThicknessZ = absorberThicknessZ + 2 * reflectorThicknessZ ;

  double yStep = totalThicknessZ + gapThicknessZ;
  
  double z0 = 0.5*m_chamberSizeZ - 0.5*m_chamberSizeX * tanTheta - totalThicknessZ;

  printf(" 0.5 m_chamberSizeZ = %f\n", 0.5 * m_chamberSizeZ );
  printf(" 0.5 m_chamberSizeX  = %f\n", 0.5 * m_chamberSizeX  );
  printf(" totalThick = %f\n", totalThicknessZ );
  
  while( z0 > -0.5 * m_chamberSizeZ ){
    printf("\n-----Panel-----\n");
    printf("zo=%5.1f ", z0 );
    
    double zCoord =
      z0 + 0.5*totalThicknessZ + 0.25 * m_chamberSizeX * tan(theta);

    double xCoord = 0.25 * m_chamberSizeX;

    centers.emplace_back( xCoord, zCoord);

    printf( "(x,y) = (%5.1f,%5.1f)\n",
	    centers.back().first, centers.back().second );
    
    z0 -= yStep; 
  }
}

void DetectorConstruction ::
ComputeDiagonalGeo( std::vector<std::pair< G4double, G4double > >& centers,
		    std::vector<G4double>& lengths )
{
  TEnv* config = m_sd->GetConfig();
 
  double absorberThicknessZ = config->GetValue( "absorberThicknessZ", 5 );
  double      gapThicknessZ = config->GetValue( "gapThicknessZ", 10 );
  double              theta = config->GetValue( "absorberTheta", 0.5 );
  double             deltaX = config->GetValue( "absorberDeltaX", 10 );
  
  double reflectorThickness  = config->GetValue( "reflectorThickness", 0.1 );
  double reflectorThicknessZ = reflectorThickness / cos( theta );

  double thickness = absorberThicknessZ * cos(theta) + 2*reflectorThicknessZ;
  double       gap =      gapThicknessZ * cos(theta);

  double tanTheta   = TMath::Tan( theta );
  double m_chamberSizeXPrime = m_chamberSizeX/TMath::Cos( theta ) - thickness * tanTheta; 
  printf( "m_chamberSizeXPrime = %f\n", m_chamberSizeXPrime );
  
  double yStep = ( thickness + gap ) / TMath::Cos( theta ); 
  printf( "yStep = %f\n", yStep );
  
  double dz    = thickness * TMath::Cos( theta );
  double dx    = thickness * TMath::Sin( theta );
  printf( "dx = %f      dz = %f\n", dx, dz );
  
  double z0    = 0.5 * m_chamberSizeZ - 0.5 * tanTheta * m_chamberSizeX - deltaX;
  printf( "z0 = %f\n", z0 );
    
  while( z0 > -0.5 * m_chamberSizeZ ){
    printf("\n-----Panel-----\n");
    double x0 = -0.5 * m_chamberSizeX;
    
    double z1 = z0 + m_chamberSizeXPrime * TMath::Sin( theta );
    double x1 = 0.5 * m_chamberSizeX - dx;
    
    double z2 = z1 - dz;
    double x2 = 0.5 * m_chamberSizeX;

    double z3 = z0 - dz;
    double x3 = - 0.5 * m_chamberSizeX + dx;

    // test bad cases
    if( ( z1 > 0.5 * m_chamberSizeZ ) && ( z2 >= 0.5 * m_chamberSizeZ ) ){
      z1 = m_chamberSizeZ / 2;
      x1 = ( z1 - z0 ) / TMath::Tan( theta ) - m_chamberSizeX / 2;

      z2 = z1 - dz;
      x2 = x1 + dx;
    }
    else if( ( z1 >= 0.5 * m_chamberSizeZ ) && ( z2 < 0.5 * m_chamberSizeZ ) ){
      z1  = m_chamberSizeZ / 2;
      x1  = (m_chamberSizeZ/2 - z0)/TMath::Tan( theta ) - m_chamberSizeX/2;
      
      z2  = z1 - dz;
      x2  = m_chamberSizeX / 2;
    }
    else if( ( z0 >= -0.5 * m_chamberSizeZ ) && ( z3 < -0.5 * m_chamberSizeZ ) ){
      break;
    }

    double zPts[5];  double xPts[5];
    
    zPts[0] = z0;  xPts[0] = x0;
    zPts[1] = z1;  xPts[1] = x1;
    zPts[2] = z2;  xPts[2] = x2;
    zPts[3] = z3;  xPts[3] = x3;

    for ( int i = 0; i < 5; i++ ) {
      printf("( %f, %f )\n", xPts[i], zPts[i] );
    }


    centers.emplace_back( 0.5*(x1 + x0 + dx), 0.5*(z1 + z0 - dz) );
    // take some small fraction off due to some rounding problems
    // that create overlap later
    lengths.push_back( 0.999 * TMath::Sqrt( TMath::Power( x1 - x0, 2 ) +
				    TMath::Power( z1 - z0, 2 ) ) );

    printf( "(x,y) = (%5.1f,%5.1f)    length = %5.1f\n",
	    centers.back().first, centers.back().second, lengths.back() );
    
    // decrement z0
    z0 -= yStep;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
