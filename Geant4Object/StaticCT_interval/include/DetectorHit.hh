#ifndef DetectorHit_h
#define DetectorHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4Step.hh"
#include "G4VProcess.hh"



class DetectorHit : public G4VHit {
    public:
        //DetectorHit() : fEdep(0.), fHasPostStep(false), fDetectorID(0), fProcessName(""), fParticleName("") {}
        DetectorHit(); 
        ~DetectorHit(); 
        DetectorHit(const DetectorHit& right);                   
        const DetectorHit& operator=(const DetectorHit& right); 
        // 移动Inline方法实现到DetectorHit.cc
        void AddEnergy(G4double edep); 
        //inline G4double GetEdep() const { return fEdep; }

        G4Step* GetStep() const;
        void SetStep(G4Step* step);

        void SetPosition(const G4ThreeVector& pos) { fPosition = pos; }
        G4ThreeVector GetPosition() const { return fPosition; }

        //void SetEdep(G4double edep) { fEdep = edep; }
        void SetEdep(G4double edep) ;
        
        G4double GetEdep() const { return fEdep; }
        //G4double GetEdep() const ;

        void SetPos(const G4ThreeVector& pos);
        G4ThreeVector GetPos() const;

        //xintianjiade
        void SetCopyNo(G4int no) { fCopyNo = no; }
        void SetWorldPos(const G4ThreeVector& pos) { fWorldPos = pos; }
        void SetLocalPos(const G4ThreeVector& pos) { fLocalPos = pos; }
        //void AddEnergy(G4double e) { fEnergy += e; }  // 确保已有 AddEnergy 方法
        // Getter 方法（若未定义也需要补充）
        G4int GetCopyNo() const { return fCopyNo; }
        G4ThreeVector GetWorldPos() const { return fWorldPos; }
        G4ThreeVector GetLocalPos() const { return fLocalPos; }
        G4double GetEnergy() const { return fEnergy; }

        void SetDetectorID(G4int id){fDetectorID = id;}
        G4int GetDetectorID() const { return fDetectorID; }

        void RecordStepInfo(const G4Step* step);
        ///
        G4String GetProcessName() const { return fProcessName; }
        G4String GetParticleName() const { return fParticleName; }
        void SetProcessName(G4String name) { fProcessName = name; }
        void SetParticleName(G4String name) { fParticleName = name; }
        ///
        const G4ThreeVector& GetPostStepPosition() const { return fPostStepPos; }
        G4bool IsPostStepValid() const { return fHasPostStep; }
        ///
        void SetScattered(bool scat) { fScattered = scat; }
        bool IsScattered() const { return fScattered; }
        void SetPrimary(bool prim) { fPrimary = prim; }
        bool IsPrimary() const { return fPrimary; }
        ///
        // 必须实现的虚函数
        virtual void Draw() override;
        virtual void Print() override;
    private:
        G4int fCopyNo = -1;           // 单元唯一标识
        G4ThreeVector fWorldPos; // 世界坐标
        G4ThreeVector fLocalPos; // 本地坐标
        G4double fEnergy = 0.0;        // 总沉积能   

        G4ThreeVector fPos;  // 声明成员变量
        G4double      fEdep;
        G4Step*       fStep;
        G4ThreeVector pos;
        G4ThreeVector fPostStepPos;  // 直接保存位置而非整个 Step
        G4bool fHasPostStep = false;
        G4int fDetectorID;
        G4ThreeVector fPosition;
        G4String fProcessName;    // 新增
        G4String fParticleName;   // 新增
        bool fScattered;
        bool fPrimary;
};


#include <G4THitsCollection.hh>  // 必须包含此头文件
using DetectorHitsCollection = G4THitsCollection<DetectorHit>;  // 
#endif