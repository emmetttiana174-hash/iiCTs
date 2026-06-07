# CT Reconstruction Module

## 1. Overview
The `CT Reconstruction` module is a critical component within the CTISS software suite, designed to configure and manage parameters for CT image reconstruction. It allows users to define reconstruction-specific properties (e.g., pixel dimensions, layer thickness, offsets), select reconstruction algorithms and kernels, edit template files in both graphical and text modes, and initiate CT reconstruction simulations. This module integrates with project information to ensure consistent parameter management and seamless workflow execution.


## 2. Module Content
The `CT Reconstruction` module includes several key components organized to support comprehensive CT reconstruction configuration:

### 2.1 View Toggle Buttons
Located in the top-right corner, these buttons switch between different operational views:
- **UI**: Graphical interface for parameter editing (default view).
- **Det (Detailed)**: Text-based editor for direct modification of template files.
- **AI**: AI-assisted panel (`ChatPanel`) for guidance and automation (if available).


### 2.2 Main Operation Controls
- **Simulation Button**: A prominent button (middle-upper section) to initiate the CT reconstruction process using current parameters and selected target files.
- **Target Path Selector**: A dropdown menu (`TargetCombo`) to choose the script file (.gwl) that defines the reconstruction workflow. Displays user-created and system-provided scripts with labels (`(user)` or `(system)`).


### 2.3 Parameter Configuration Panels
#### 2.3.1 UI View Panel
A structured interface for editing CT reconstruction parameters, divided into:
- **Parameter Settings**: Input fields for numerical parameters with units:
  - Image dimensions: `PixelCountX` (no unit), `PixelCountY` (no unit), `PixelSizeX` (mm), `PixelSizeY` (mm).
  - Layer properties: `LayerThickness` (mm).
  - Position offsets: `XOffset` (mm), `YOffset` (mm), `ZOffset` (mm).

- **Path Settings**: Fields for algorithms, kernels, and file/directory paths:
  - `ReconAlgorithm`: Reconstruction algorithm (text input, e.g., "FBP").
  - `ReconKernel`: Reconstruction kernel (text input, e.g., "Standard").
  - `DetectorGeometry`: Path to detector geometry files (with a browse button `...` to select files).
  - `CTGeometry`: Path to CT system geometry files (with a browse button `...` to select files).
  - `InputPath`: Directory for input projection data (with a browse button `...` to select folders).
  - `OutputPath`: Directory for reconstruction results (with a browse button `...` to select folders).

- **Template File Controls**: A dropdown (`fileUICombo`) to load system/user templates, plus `Save` and `Save As` buttons to store modified parameters.


#### 2.3.2 Detailed View Panel
A text editor for direct modification of template files:
- **Template File Selector**: A dropdown (`fileCombo`) to load templates (same as `fileUICombo` in UI view).
- **Text Editor**: A large input area to edit the raw content of template files (supports syntax for .gwl scripts).
- **Save Controls**: `Save` and `Save As` buttons to update or create new template files.


## 3. Purpose
The `CT Reconstruction` module is designed to:
- Centralize configuration of CT reconstruction parameters (image dimensions, layer thickness, position offsets).
- Support selection of reconstruction algorithms, kernels, and geometry files.
- Enable both graphical (UI) and text-based (Detailed) editing for flexibility.
- Manage template files (system-provided and user-created) to standardize reconstruction workflows.
- Initiate CT reconstruction simulations and integrate with the terminal for progress monitoring.


## 4. Operation Steps

### 4.1 Switching Views
- **UI View**: Click `UI` to access graphical parameter inputs. Best for modifying individual parameters with unit guidance.
- **Detailed View**: Click `Det` to edit raw template text. Suitable for advanced users adjusting script logic.
- **AI View**: Click `AI` to open the `ChatPanel` for AI-assisted configuration.


### 4.2 Configuring Parameters in UI View
1. **Load a Template**: Use `fileUICombo` to select a base template (e.g., `recon_default.gwl`). Parameters will auto-populate with template values.
2. **Edit Parameters**: Modify values in the input fields. Units are pre-defined (e.g., `PixelSizeX` uses mm).
3. **Set Algorithm/Kernel**: Enter `ReconAlgorithm` (e.g., "Iterative") and `ReconKernel` (e.g., "Sharp").
4. **Update Paths**:
   - Select `DetectorGeometry` and `CTGeometry` files using their browse buttons `...`.
   - Specify `InputPath` (projection data) and `OutputPath` (results) using their browse buttons `...`.
5. **Save Changes**:
   - Click `Save` to update the current template (overwrites if it’s a user file; prompts to create a user copy for system files).
   - Click `Save As` to create a new template. Enter a name in the dialog, and the file will be saved to the project’s `recon` folder.


### 4.3 Editing Templates in Detailed View
1. **Load a Template**: Use `fileCombo` to select a template. The text editor will display its content.
2. **Modify Content**: Edit the text directly (e.g., adjust regularization parameters or add custom algorithm logic).
3. **Save Changes**: Use `Save` (update existing) or `Save As` (new file) as in UI view.


### 4.4 Running the Reconstruction
1. **Select Target Script**: Use `TargetCombo` to choose the .gwl file that executes the reconstruction (matches the template used for parameter configuration).
2. **Initiate Reconstruction**: Click the `Simulation` button. The terminal will display progress updates (e.g., `CTISS:1`, `CTISS:2`...).


## 5. Notes
- **Template Sources**: System templates are pre-provided (read-only) and stored in `SystemScriptPath/recon/`. User templates are saved in `ProjectPath/[ProjectName]/recon/`.
- **Parameter Consistency**: Ensure values in UI view match the corresponding template in Detailed view. Mismatches may cause reconstruction errors.
- **File Extensions**: Templates are saved as `.gwl` files. The module auto-appends this extension if omitted in `Save As`.
- **Dependency Files**: `DetectorGeometry` and `CTGeometry` must point to valid geometry files, and `InputPath` must contain valid projection data for successful reconstruction.


## 6. Troubleshooting
- **Reconstruction Fails to Start**: Ensure a `Target Path` is selected in `TargetCombo`. If empty, populate templates by saving a file in UI/Detailed view.
- **Cannot Save Files**: Check write permissions for the project’s `recon` folder (`ProjectPath/[ProjectName]/recon/`).
- **Parameters Not Updating**: Verify changes are saved before running the reconstruction. Use `Save` after editing.
- **Path Errors**: If reconstruction fails due to missing files, confirm `DetectorGeometry`, `CTGeometry`, and `InputPath` point to valid locations.
- **Template Not Loading**: If a template is missing from the dropdown, refresh by saving any file (triggers re-scan of user/system directories).
