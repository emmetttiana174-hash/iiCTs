## Background

CT simulation is indispensable in both algorithm R&D and manufacturing. Analytic methods run quickly but often mis-model key physics, leading to discrepancies with measurements. Monte Carlo simulation fills that gap, powering critical tasks like scatter correction and dose evaluation. The Integrated Intelligent CT Simulation System (CTISS) addresses these needs through a modular, user-centric design that unifies detector modeling, X-ray spectrum simulation, scan trajectory planning, and image reconstruction into a cohesive workflow.


## Major Features

- **Comprehensive Physics Modeling**
  Builds full 3D spatial models of X-ray focal spots, computes voxel-wise spectral curves, and integrates multi-component physical properties (detector response, tube spectrum, and phantom interactions) across all modules.

- **Unified Ecosystem with Five Core Modules**
  A cohesive framework connecting key components of CT simulation:
  - `Home`: Manages project information, system paths, and global configurations.
  - `Detector Model`: Configures detector geometry, pixel parameters (size, count, thickness), and material properties.
  - `Tube Spectrum`: Models X-ray tube spectra, including anode/cathode materials and energy distribution.
  - `CT Scan Simulation`: Sets up scan protocols (helical/spiral/static), photon counts, fan angles, and phantom positioning.
  - `CT Reconstruction`: Defines image dimensions (pixel size/count), reconstruction algorithms, kernels, and output settings.

- **Multi-Scenario Support**
  Out-of-the-box workflows for diverse CT modalities:
  * Helical (spiral) CT
  * Dental CBCT
  * Linear static CT
  * Hexagonal static CT

- **Dual-Mode Operation**
  Lowers entry barriers while supporting advanced users:
  + **GUI Interface**: Visual parameter editors with unit guidance (e.g., mm for dimensions, keV for energy thresholds) and one-click loading of scanner templates.
  + **Script-Based Editing**: Text-mode template customization for complex parameter tuning, with `Save`/`Save As` for version control of user-defined protocols.


## v1.0 Beta Highlights

1. **Full Five-Module Integration**
   End-to-end connectivity across `Home`, detector, tube spectrum, scan simulation, and reconstruction modules, enabling seamless parameter propagation and cross-validation.

2. **Enhanced Physical Modeling**
   - Detailed detector configuration via `Detector Model` (e.g., photon counting thresholds PCCTTh1-7).
   - Accurate X-ray spectrum simulation in `Tube Spectrum`, including material-dependent energy distribution.

3. **Flexible Scan & Reconstruction Workflows**
   - Configurable scan parameters in `CT Scan Simulation` (fan angle, bed direction, phantom/tube offsets).
   - Fine-grained reconstruction controls in `CT Reconstruction` (pixel dimensions, layer thickness, algorithm/kernel selection).

4. **User-Centric Interface**
   Unified GUI with view toggling (UI/Det/AI) across all modules, plus intuitive template management (system-provided and user-created files) and real-time progress tracking via terminal integration.


## Developers

- Yi Guo
- Yankun Wang
- Yule Li


## Release Information

- **Version**: 1.0 Beta
- **Date**: 2025-07-27
