# CT Scan Simulation Module

## 1. Overview
The `CT Scan Simulation` module is a core component within the CTISS software suite, designed to configure and manage parameters for CT scan simulations. It enables users to define scan-specific properties (e.g., photon count, detector geometry, phantom settings), edit template files in both graphical and text modes, and initiate CT scan simulations. This module integrates with project information to ensure consistent parameter management and seamless workflow execution.


## 2. Module Content
The `CT Scan Simulation` module includes several key components organized to support comprehensive CT scan configuration:

### 2.1 View Toggle Buttons
Located in the top-right corner, these buttons switch between different operational views:
- **UI**: Graphical interface for parameter editing (default view).
- **Det (Detailed)**: Text-based editor for direct modification of template files.
- **AI**: AI-assisted panel (`ChatPanel`) for guidance and automation (if available).


### 2.2 Main Operation Controls
- **Simulation Button**: A prominent button (middle-upper section) to initiate the CT scan simulation using current parameters and selected target files.
- **Target Path Selector**: A dropdown menu (`TargetCombo`) to choose the script file (.gwl) that defines the simulation workflow. Displays user-created and system-provided scripts with labels (`(user)` or `(system)`).


### 2.3 Parameter Configuration Panels
#### 2.3.1 UI View Panel
A structured interface for editing CT scan parameters, divided into:
- **Parameter Settings**: Input fields for numerical parameters with units:
  - Scan properties: `PhotonCount` (no unit), `BedDirection` (no unit), `StartIndex` (no unit).
  - Geometry: `FanAngle` (°), `DetModuleNum` (no unit), `DetDistance` (mm), `TubeDistance` (mm).
  - Phantom settings: `PhantomScale` (no unit), `PhantomOffsetX` (mm), `PhantomOffsetY` (mm).
  - Tube offsets: `TubeOffsetX` (mm), `TubeOffsetY` (mm).

- **Path Settings**: Fields for file/directory paths:
  - `DetectorPath`: Path to detector model files (with a browse button `...` to select files).
  - `SpectrumPath`: Directory for spectrum data (with a browse button `...` to select folders).
  - `PhantomPath`: Directory for phantom models (with a browse button `...` to select folders).
  - `OutputPath`: Directory for simulation results (with a browse button `...` to select folders).

- **Template File Controls**: A dropdown (`fileUICombo`) to load system/user templates, plus `Save` and `Save As` buttons to store modified parameters.


#### 2.3.2 Detailed View Panel
A text editor for direct modification of template files:
- **Template File Selector**: A dropdown (`fileCombo`) to load templates (same as `fileUICombo` in UI view).
- **Text Editor**: A large input area to edit the raw content of template files (supports syntax for .gwl scripts).
- **Save Controls**: `Save` and `Save As` buttons to update or create new template files.


## 3. Purpose
The `CT Scan Simulation` module is designed to:
- Centralize configuration of CT scan parameters (geometry, photon count, phantom positioning, detector/tube settings).
- Support both graphical (UI) and text-based (Detailed) editing for flexibility.
- Manage template files (system-provided and user-created) to standardize CT simulation workflows.
- Enable initiation of CT scan simulations and integration with the terminal for progress monitoring.


## 4. Operation Steps

### 4.1 Switching Views
- **UI View**: Click `UI` to access graphical parameter inputs. Best for modifying individual parameters with unit guidance.
- **Detailed View**: Click `Det` to edit raw template text. Suitable for advanced users adjusting script logic.
- **AI View**: Click `AI` to open the `ChatPanel` for AI-assisted configuration.


### 4.2 Configuring Parameters in UI View
1. **Load a Template**: Use `fileUICombo` to select a base template (e.g., `ct_scan_default.gwl`). Parameters will auto-populate with template values.
2. **Edit Parameters**: Modify values in the input fields. Units are pre-defined (e.g., `FanAngle` uses degrees).
3. **Update Paths**:
   - Specify `DetectorPath` using the browse button `...` to select detector model files.
   - Set `SpectrumPath`, `PhantomPath`, and `OutputPath` using their respective browse buttons `...` to select directories.
4. **Save Changes**:
   - Click `Save` to update the current template (overwrites if it’s a user file; prompts to create a user copy for system files).
   - Click `Save As` to create a new template. Enter a name in the dialog, and the file will be saved to the project’s `ct` folder.


### 4.3 Editing Templates in Detailed View
1. **Load a Template**: Use `fileCombo` to select a template. The text editor will display its content.
2. **Modify Content**: Edit the text directly (e.g., adjust scan angles or add custom phantom logic).
3. **Save Changes**: Use `Save` (update existing) or `Save As` (new file) as in UI view.


### 4.4 Running the CT Simulation
1. **Select Target Script**: Use `TargetCombo` to choose the .gwl file that executes the simulation (matches the template used for parameter configuration).
2. **Initiate Simulation**: Click the `Simulation` button. The terminal will display progress updates (e.g., `CTISS:1`, `CTISS:2`...).


## 5. Notes
- **Template Sources**: System templates are pre-provided (read-only) and stored in `SystemScriptPath/ct/`. User templates are saved in `ProjectPath/[ProjectName]/ct/`.
- **Parameter Consistency**: Ensure values in UI view match the corresponding template in Detailed view. Mismatches may cause simulation errors.
- **File Extensions**: Templates are saved as `.gwl` files. The module auto-appends this extension if omitted in `Save As`.
- **Dependency Paths**: `DetectorPath`, `SpectrumPath`, and `PhantomPath` must point to valid files/directories for the simulation to run successfully.


## 6. Troubleshooting
- **Simulation Fails to Start**: Ensure a `Target Path` is selected in `TargetCombo`. If empty, populate templates by saving a file in UI/Detailed view.
- **Cannot Save Files**: Check write permissions for the project’s `ct` folder (`ProjectPath/[ProjectName]/ct/`).
- **Parameters Not Updating**: Verify changes are saved before running the simulation. Use `Save` after editing.
- **Path Errors**: If the simulation fails due to missing files, confirm `DetectorPath`, `SpectrumPath`, and `PhantomPath` point to valid locations.
- **Template Not Loading**: If a template is missing from the dropdown, refresh by saving any file (triggers re-scan of user/system directories).
