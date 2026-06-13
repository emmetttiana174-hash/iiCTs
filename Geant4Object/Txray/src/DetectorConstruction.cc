/// \file medical/GammaTherapy/src/DetectorConstruction.cc
/// \brief Implementation of the DetectorConstruction class
#include "DetectorConstruction.hh"
#include "DetectorSD.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Sphere.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4Material.hh"
#include "G4PVParameterised.hh"
#include "G4SDManager.hh"
#include "G4NistManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4RunManager.hh"
#include "G4GeometryManager.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "globals.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"
#include "DetectorConstructionMessenger.hh"
#include "DetectorMaterials.hh"

DetectorConstruction::DetectorConstruction()
 : fVerbose(false), fMaxEnergy(120.0 * keV), fRotationMatrix(new G4RotationMatrix())
{
  fNumZ    = 60;
  fNumR    = 80;
  fNumE    = 200;
  fMaxEnergy = 120.0*keV;
  DetectorMaterials materials;
  materials.DefineMaterials();
  fMessenger = new DetectorConstructionMessenger(this);

  G4NistManager* man = G4NistManager::Instance();
  fWorldMaterial   = man->FindOrBuildMaterial("G4_Galactic");
  fObjectMaterial  = man->FindOrBuildMaterial("G4_AIR");
  fDetectorMaterial = man->FindOrBuildMaterial("G4_Galactic"); 
  materials.DefineMaterials();
  
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

DetectorConstruction::~DetectorConstruction()
{
  delete fRotationMatrix;
  delete fMessenger;
}

void DetectorConstruction::InitialiseGeometryParameters()
{
  worldSize = 100. * cm;
  
  // Virtual detection plane: 5cm x 5cm square, very thin
  fDetectorX = 2 * cm;  // Half of 5cm side length
  fDetectorY = 2 * cm;  // Half of 5cm side length
  fDetectorZ = 0.00000002 * mm; // Very thin to act as a virtual plane
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
    InitialiseGeometryParameters();

    G4GeometryManager::GetInstance()->OpenGeometry();
    G4PhysicalVolumeStore::GetInstance()->Clean();
    G4LogicalVolumeStore::GetInstance()->Clean();
    G4SolidStore::GetInstance()->Clean();

    // World
    G4Box* solidWorld = new G4Box("World", worldSize, worldSize, worldSize);
    G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld, fWorldMaterial, "World");
    fPhysWorld = new G4PVPlacement(0, G4ThreeVector(), "World", logicWorld, 0, false, 0);

    // Tungsten cube (unchanged)
    G4double cubeHalfLengthXY = 6 * cm;
    G4double cubeHalfLengthZ = 2.5 * um;
    G4double targetPosX = 0 * cm;
    G4double targetPosY = 0 * cm;
    G4double targetPosZ = -0.1 * cm;
    G4Material* tungsten = G4NistManager::Instance()->FindOrBuildMaterial("G4_W");
    G4Box* solidCube = new G4Box("tungsten_cube", cubeHalfLengthXY, cubeHalfLengthXY, cubeHalfLengthZ);
    G4LogicalVolume* logicCube = new G4LogicalVolume(solidCube, tungsten, "tungsten_cube");
    G4VisAttributes* visAttCube = new G4VisAttributes(G4Colour(1.0, 1.0, 0.0));
    visAttCube->SetForceSolid(true);
    logicCube->SetVisAttributes(visAttCube);

    fRotationMatrix->rotateX(0 * deg);
    fPhysObject = new G4PVPlacement(fRotationMatrix, G4ThreeVector(targetPosX, targetPosY, targetPosZ), 
                                    logicCube, "tungsten_cube", logicWorld, false, 0, true);
    fCurrentAngle = 0 * deg;

    // Virtual detection plane
    G4Box* solidDetector = new G4Box("DetectorPlane", fDetectorX, fDetectorY, fDetectorZ);
    G4LogicalVolume* detectorLogical = new G4LogicalVolume(solidDetector, fDetectorMaterial, "DetectorPlane");
    G4VisAttributes* detectorVisAtt = new G4VisAttributes(G4Colour(0.0, 1.0, 0.0)); // Green for visibility
    detectorVisAtt->SetForceSolid(true);
    detectorLogical->SetVisAttributes(detectorVisAtt);

    // Place the detection plane at z = 5 cm
    G4double detectorPosX = 0 * cm;
    G4double detectorPosY = 0 * cm;
    G4double detectorPosZ = -4 * cm;

    G4VPhysicalVolume* physDetector = new G4PVPlacement(
        0,                              // No rotation
        G4ThreeVector(0, detectorPosY, detectorPosZ), // Centered at z = 5 cm
        detectorLogical,                // Logical volume
        "PhysDetectorPlane",            // Name
        logicWorld,                     // Mother volume
        false,                          // No boolean operations
        0,                              // Copy number
        true                            // Check overlaps
    );

    // Aluminum layer above the virtual detection plane
    G4double aluminumHalfLengthX = 2.5 * cm; // Half of 4cm
    G4double aluminumHalfLengthY = 2.5 * cm; // Half of 2mm
    G4double aluminumHalfLengthZ = 1 * mm; // Half of 4cm
    G4double aluminumPosX = 0 * cm;
    G4double aluminumPosY = 0 * cm; // Position above the detector plane
    G4double aluminumPosZ = -3* cm;

    G4Material* aluminumMaterial = G4NistManager::Instance()->FindOrBuildMaterial("G4_Al");
    G4Box* solidAluminum = new G4Box("AluminumLayer", aluminumHalfLengthX, aluminumHalfLengthY, aluminumHalfLengthZ);
    G4LogicalVolume* logicAluminum = new G4LogicalVolume(solidAluminum, aluminumMaterial, "AluminumLayer");
    G4VisAttributes* aluminumVisAtt = new G4VisAttributes(G4Colour(0.5, 0.5, 0.5)); // Grey for visibility
    aluminumVisAtt->SetForceSolid(true);
    logicAluminum->SetVisAttributes(aluminumVisAtt);

    G4VPhysicalVolume* physAluminum = new G4PVPlacement(
        0,                              // No rotation
        G4ThreeVector(aluminumPosX, aluminumPosY, aluminumPosZ), // Position above the detector plane
        logicAluminum,                  // Logical volume
        "PhysAluminumLayer",            // Name
        logicWorld,                     // Mother volume
        false,                          // No boolean operations
        0,                              // Copy number
        true                            // Check overlaps
    );

    // Store the logical volume for the sensitive detector
    cellLogical = detectorLogical;

    G4cout << "Created virtual detection plane (5cm x 5cm) at z = 5cm" << G4endl;
    G4cout << "Created aluminum layer (4cm x 4cm x 2mm) above the virtual detection plane" << G4endl;

    return fPhysWorld;
}

void DetectorConstruction::SetObjectRotationAngle(G4double angle) {
    fCurrentAngle = angle;
    if (fPhysObject) {
        *fRotationMatrix = G4RotationMatrix();
        fRotationMatrix->rotateY(angle);
        fPhysObject->SetRotation(fRotationMatrix);
        G4RunManager::GetRunManager()->GeometryHasBeenModified();
    }
}

G4double DetectorConstruction::GetCurrentAngle() const {
    return fCurrentAngle;
}

void DetectorConstruction::ConstructSDandField()
{
    G4SDManager* sdManager = G4SDManager::GetSDMpointer();
    DetectorSD* fDetectorSD = new DetectorSD("detectorSD");
    sdManager->AddNewDetector(fDetectorSD);
    cellLogical->SetSensitiveDetector(fDetectorSD);
}