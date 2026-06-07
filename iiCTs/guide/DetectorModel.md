# Detector Model Module

## 1. Overview
The `Detector Model` module is a specialized component within the CTISS software suite, designed to configure and manage detector model parameters for CT simulations. It allows users to define detector-specific properties (e.g., dimensions, pixel count, material), edit template files in both graphical and text modes, and initiate the detector model building process. This module integrates with project information to ensure consistent parameter management and seamless workflow execution.


## 2. Module Content
The `Detector Model` module includes several key components organized to support comprehensive detector model configuration:

### 2.1 View Toggle Buttons
Located in the top-right corner, these buttons switch between different operational views:
- **UI**: Graphical interface for parameter editing (default view).
- **Det (Detailed)**: Text-based editor for direct modification of template files.
- **AI**: AI-assisted panel (`ChatPanel`) for guidance and automation (if available).


### 2.2 Main Operation Controls
- **Build Button**: A prominent button (middle-upper section) to initiate the detector model building process using current parameters and selected target files.
- **Target Path Selector**: A dropdown menu (`TargetCombo`) to choose the script file (.gwl) that defines the model building workflow. Displays user-created and system-provided scripts with labels (`(user)` or `(system)`).


### 2.3 Parameter Configuration Panels
#### 2.3.1 UI View Panel
A structured interface for editing detector parameters, divided into:
- **Parameter Settings**: Input fields for numerical parameters with units:
  - Detector dimensions: `DetUnitSizeX` (mm), `DetUnitSizeY` (mm), `DetUnitThickness` (mm), `DetectorRadius` (mm).
  - Pixel count: `DetCountX`, `DetCountY` (no units).
  - Photon counting settings: `PCCT` (no unit), `PCCTTh1` to `PCCTTh7` (KeV, threshold energies).

- **Path Settings**: Fields for file/directory paths:
  - `OutputFilename`: Name of the detector model output file.
  - `DetectorMaterial`: Material of the detector (text input).
  - `OutputFolder`: Directory for model output (with a browse button `...` to select folders).

- **Template File Controls**: A dropdown (`fileUICombo`) to load system/user templates, plus `Save` and `Save As` buttons to store modified parameters.


#### 2.3.2 Detailed View Panel
A text editor for direct modification of template files:
- **Template File Selector**: A dropdown (`fileCombo`) to load templates (same as `fileUICombo` in UI view).
- **Text Editor**: A large input area to edit the raw content of template files (supports syntax for .gwl scripts).
- **Save Controls**: `Save` and `Save As` buttons to update or create new template files.


## 3. Purpose
The `Detector Model` module is designed to:
- Centralize configuration of detector model parameters (dimensions, pixel layout, materials, photon counting thresholds).
- Support both graphical (UI) and text-based (Detailed) editing for flexibility.
- Manage template files (system-provided and user-created) to standardize detector model workflows.
- Enable initiation of detector model building and integration with the terminal for progress monitoring.


## 4. Operation Steps

### 4.1 Switching Views
- **UI View**: Click `UI` to access graphical parameter inputs. Best for modifying individual parameters with unit guidance.
- **Detailed View**: Click `Det` to edit raw template text. Suitable for advanced users adjusting script logic.
- **AI View**: Click `AI` to open the `ChatPanel` for AI-assisted configuration.


### 4.2 Configuring Parameters in UI View
1. **Load a Template**: Use `fileUICombo` to select a base template (e.g., `detector_default.gwl`). Parameters will auto-populate with template values.
2. **Edit Parameters**: Modify values in the input fields. Units are pre-defined (e.g., `DetUnitSizeX` uses mm).
3. **Update Paths**:
   - Enter `OutputFilename` (e.g., `my_detector_model`).
   - Specify `OutputFolder` using the browse button `...` to select a result directory.
   - Define `DetectorMaterial` (e.g., `CesiumIodide`).
4. **Save Changes**:
   - Click `Save` to update the current template (overwrites if it’s a user file; prompts to create a user copy for system files).
   - Click `Save As` to create a new template. Enter a name in the dialog, and the file will be saved to the project’s `detector` folder.


### 4.3 Editing Templates in Detailed View
1. **Load a Template**: Use `fileCombo` to select a template. The text editor will display its content.
2. **Modify Content**: Edit the text directly (e.g., adjust pixel grid dimensions or add custom material properties).
3. **Save Changes**: Use `Save` (update existing) or `Save As` (new file) as in UI view.


### 4.4 Building the Detector Model
1. **Select Target Script**: Use `TargetCombo` to choose the .gwl file that executes the model building (matches the template used for parameter configuration).
2. **Initiate Building**: Click the `Build` button. The terminal will display progress updates (e.g., `CTISS:1`, `CTISS:2`...).


## 5. Notes
- **Template Sources**: System templates are pre-provided (read-only) and stored in `SystemScriptPath/detector/`. User templates are saved in `ProjectPath/[ProjectName]/detector/`.
- **Parameter Consistency**: Ensure values in UI view match the corresponding template in Detailed view. Mismatches may cause model building errors.
- **File Extensions**: Templates are saved as `.gwl` files. The module auto-appends this extension if omitted in `Save As`.
- **Material Compatibility**: Use materials available in the system’s material library (defined in `SystemMaterialPath` from the ParaHome module).


## 6. Troubleshooting
- **Build Fails to Start**: Ensure a `Target Path` is selected in `TargetCombo`. If empty, populate templates by saving a file in UI/Detailed view.
- **Cannot Save Files**: Check write permissions for the project’s `detector` folder (`ProjectPath/[ProjectName]/detector/`).
- **Parameters Not Updating**: Verify changes are saved before building. Use `Save` after editing.
- **Template Not Loading**: If a template is missing from the dropdown, refresh by saving any file (triggers re-scan of user/system directories).
