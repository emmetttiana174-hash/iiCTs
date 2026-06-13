# Geant4 X射线光谱仿真项目 - 宏命令手册

## 📋 概述
本文档详细介绍了当前Geant4 X射线光谱仿真项目中所有可用的宏命令，包括系统内置命令和自定义命令。

---

## 🔧 自定义宏命令

### 1. 步长限制控制
```bash
/testem/phys/fStepMax [长度值] [单位]
```
- **功能**: 设置默认区域的最大允许步长
- **参数**: 长度值 > 0，单位（如：mm, cm, m）
- **示例**: `/testem/phys/fStepMax 0.1 mm`

### 2. 物理过程配置

#### 2.1 添加物理模块
```bash
/testem/phys/addPhysics [物理模块名]
```
- **功能**: 添加指定的物理过程模块
- **可用模块**:
  - `emstandard` - 标准电磁过程
  - `emstandard_opt2` - 优化的电磁过程选项2
  - `emlivermore` - Livermore低能电磁模型
  - `empenelope` - Penelope低能电磁模型
  - `elastic` - 弹性散射过程
  - `binary` - 二元级联模型
  - `binary_ion` - 离子二元级联模型
  - `gamma_nuc` - 伽马核相互作用
  - `step_limit` - 步长限制过程
- **示例**: `/testem/phys/addPhysics emstandard_opt2`

#### 2.2 物理过程详细程度
```bash
/testem/phys/verbose [整数值]
```
- **功能**: 设置物理过程的详细输出级别
- **参数**: 0-5的整数，数值越大输出越详细
- **示例**: `/testem/phys/verbose 1`

#### 2.3 物理过程开关控制
```bash
# 电离过程控制
/testem/phys/EnableIonisation [0|1]
```
- **功能**: 启用/禁用电子电离过程 (G4eIonisation)
- **参数**: 1 = 启用, 0 = 禁用
- **示例**: `/testem/phys/EnableIonisation 1`

```bash
# 韧致辐射过程控制
/testem/phys/EnableBremsstrahlung [0|1]
```
- **功能**: 启用/禁用韧致辐射过程 (G4eBremsstrahlung)，X射线产生的主要机制
- **参数**: 1 = 启用, 0 = 禁用
- **示例**: `/testem/phys/EnableBremsstrahlung 1`

```bash
# 多重散射过程控制
/testem/phys/EnableScatter [0|1]
```
- **功能**: 启用/禁用电子多重散射过程 (G4eMultipleScattering)
- **参数**: 1 = 启用, 0 = 禁用
- **示例**: `/testem/phys/EnableScatter 1`

```bash
# 正电子湮灭过程控制
/testem/phys/EnableAnnihilation [0|1]
```
- **功能**: 启用/禁用正电子湮灭过程 (G4eplusAnnihilation)
- **参数**: 1 = 启用, 0 = 禁用
- **示例**: `/testem/phys/EnableAnnihilation 1`

### 3. 文件路径控制

#### 3.1 输入文件路径
```bash
/Input/setInputFilePath [文件路径]
```
- **功能**: 设置粒子源输入文件的完整路径
- **参数**: 文件的绝对路径（包含文件名和扩展名）
- **时机**: 必须在 `/run/initialize` 之前设置
- **示例**: `/Input/setInputFilePath /mnt/kunlun/users/LYL/geant4/educatect/input/W_5_80kev_educate.txt`

#### 3.2 输出文件路径
```bash
/Output/setOutputFilePath [文件路径前缀]
```
- **功能**: 设置能谱输出文件的路径前缀（不包含角度和.root扩展名）
- **参数**: 文件路径前缀，系统会自动添加角度值和.root扩展名
- **时机**: 可在任何时候设置，在下次 `/run/beamOn` 时生效
- **示例**: `/Output/setOutputFilePath /mnt/kunlun/users/LYL/geant4/educatect/xenergyoutput/540b_5_80kev_educate`
- **结果**: 生成文件如 `540b_5_80kev_educate15.0.root`（15.0为角度值）

### 4. 几何配置命令

#### 3.1 待测物体控制
```bash
/detector/setObjectAngle [角度值]
```
- **功能**: 设置待测物体的旋转角度
- **参数**: 角度值（度），可以是负数
- **示例**: `/detector/setObjectAngle 45.0`

#### 3.2 阳极靶配置
```bash
# 阳极靶角度
/Anode/setAnodeAngle [角度值]
```
- **功能**: 设置阳极靶的倾斜角度
- **参数**: 角度值（度），范围：-180 到 180
- **示例**: `/Anode/setAnodeAngle 15.0`

```bash
# 阳极靶材料
/Anode/setAnodeMaterial [材料名]
```
- **功能**: 设置阳极靶材料，自动添加G4_前缀
- **参数**: Geant4材料库中的任意材料名（不需要G4_前缀）
- **常用材料**: W（钨）, Mo（钼）, Cu（铜）, Galactic（真空）
- **示例**: 
  - `/Anode/setAnodeMaterial W` → 实际使用G4_W
  - `/Anode/setAnodeMaterial Mo` → 实际使用G4_Mo

#### 3.3 滤过器配置
```bash
# 滤过器材料
/Filter/setFilterMaterial [材料名]
```
- **功能**: 设置滤过器材料，自动添加G4_前缀
- **参数**: Geant4材料库中的任意材料名
- **常用材料**: Al（铝）, Cu（铜）, Be（铍）, Galactic（无滤过）
- **示例**: 
  - `/Filter/setFilterMaterial Al` → 实际使用G4_Al
  - `/Filter/setFilterMaterial Cu` → 实际使用G4_Cu

```bash
# 滤过器尺寸
/Filter/setFilterX [尺寸] [单位]    # X方向半长度
/Filter/setFilterY [尺寸] [单位]    # Y方向半长度  
/Filter/setFilterZ [尺寸] [单位]    # Z方向半长度（厚度）
```
- **功能**: 设置滤过器的几何尺寸
- **参数**: 尺寸值 > 0，单位（mm, cm, m等）
- **示例**: 
  - `/Filter/setFilterX 2.0 cm`
  - `/Filter/setFilterY 2.0 cm`
  - `/Filter/setFilterZ 0.5 mm`

#### 3.4 探测器配置
```bash
# 探测器材料
/detector/setDetectorMaterial [材料名]
```
- **功能**: 设置探测器材料，自动添加G4_前缀
- **常用材料**: Si（硅）, CESIUM_IODIDE（碘化铯）, Galactic（真空）
- **示例**: `/detector/setDetectorMaterial CESIUM_IODIDE`

```bash
# 探测器尺寸
/detector/setDetectorX [尺寸] [单位]    # X方向半长度
/detector/setDetectorY [尺寸] [单位]    # Y方向半长度
/detector/setDetectorZ [尺寸] [单位]    # Z方向半长度（厚度）
```
- **功能**: 设置探测器的几何尺寸
- **参数**: 尺寸值 > 0，单位建议使用cm或mm
- **示例**: 
  - `/detector/setDetectorX 5.0 cm`
  - `/detector/setDetectorY 5.0 cm` 
  - `/detector/setDetectorZ 2.0 mm`

---

## 🎯 标准Geant4系统命令

### 5. 运行控制
```bash
/run/initialize                        # 初始化几何和物理过程
/run/setCut [距离] [单位]               # 设置二次粒子产生阈值
/run/beamOn [粒子数]                   # 开始模拟指定数量的事例
```

**运行时间输出**:
- 仿真开始时输出: `Starting simulation...`
- 仿真结束时输出: `Simulation complete!`、运行时间和输出文件路径
- 时间格式自动调整: 
  - 少于1秒: `523ms`
  - 少于1分钟: `45.234s`
  - 少于1小时: `5m 32s`
  - 超过1小时: `2h 15m 42s`

### 6. 详细程度控制
```bash
/control/verbose [级别]                # 控制系统输出详细程度
/run/verbose [级别]                    # 运行管理器输出详细程度
/tracking/verbose [级别]               # 粒子跟踪输出详细程度
/event/verbose [级别]                  # 事例处理输出详细程度
```

### 7. 传统粒子枪设置
```bash
/gun/particle [粒子类型]               # 设置粒子类型（e-, gamma, proton等）
/gun/energy [能量] [单位]              # 设置粒子能量
/gun/position [x] [y] [z] [单位]       # 设置发射位置
/gun/direction [dx] [dy] [dz]          # 设置发射方向（单位矢量）
```

### 8. GPS通用粒子源设置
```bash
# 基本设置
/gps/particle [粒子类型]               # 粒子类型
/gps/ene/type [能谱类型]               # 能量分布类型
/gps/ene/mono [能量] [单位]            # 单能设置

# 位置和几何
/gps/pos/type [源类型]                 # 源的几何类型
/gps/pos/shape [形状]                  # 源的形状
/gps/pos/centre [x] [y] [z] [单位]     # 源的中心位置
/gps/pos/halfx [半长] [单位]           # X方向半长度
/gps/pos/halfy [半长] [单位]           # Y方向半长度

# 方向设置
/gps/direction [dx] [dy] [dz]          # 粒子发射方向
```

### 9. 可视化命令
```bash
# 基本可视化
/vis/open [渲染器]                     # 打开可视化窗口
/vis/drawVolume                        # 绘制几何体
/vis/scene/add/trajectories            # 添加粒子轨迹显示
/vis/scene/add/axes [x] [y] [z] [长度] [单位]  # 添加坐标轴

# 视角控制
/vis/viewer/set/viewpointThetaPhi [theta] [phi]  # 设置观察角度
/vis/viewer/zoom [缩放倍数]            # 缩放视图
/vis/viewer/set/style [样式]           # 设置显示样式（surface/wireframe）
```

---

## 📝 完整配置示例

### 基础X射线管配置
```bash
# 设置详细程度
/control/verbose 2
/run/verbose 1
/tracking/verbose 0

# 配置物理过程
/testem/phys/addPhysics emstandard_opt2
/testem/phys/verbose 1

# 控制特定物理过程（可选）
/testem/phys/EnableIonisation 1
/testem/phys/EnableBremsstrahlung 1
/testem/phys/EnableScatter 1  
/testem/phys/EnableAnnihilation 1

# 配置文件路径
/Input/setInputFilePath /mnt/kunlun/users/LYL/geant4/educatect/input/W_5_80kev_educate.txt
/Output/setOutputFilePath /mnt/kunlun/users/LYL/geant4/educatect/xenergyoutput/540b_5_80kev_educate

# 配置阳极靶（钨靶，15度倾斜）
/Anode/setAnodeMaterial W
/Anode/setAnodeAngle 15.0

# 配置铝滤过器
/Filter/setFilterMaterial Al
/Filter/setFilterX 2.0 cm
/Filter/setFilterY 2.0 cm  
/Filter/setFilterZ 0.5 mm

# 配置CsI探测器
/detector/setDetectorMaterial Galactic
/detector/setDetectorX 5.0 cm
/detector/setDetectorY 5.0 cm
/detector/setDetectorZ 2.0 mm

# 初始化
/run/initialize
/run/setCut 0.01 mm

# 设置120keV电子束
/gps/particle e-
/gps/ene/type Mono
/gps/ene/mono 120 keV
/gps/pos/type Plane
/gps/pos/shape Square
/gps/pos/centre 3.0 0.0 0.0 cm
/gps/pos/halfx 0.5 cm
/gps/pos/halfy 0.5 cm
/gps/direction -1.0 0.0 0.0

# 开始模拟
/run/beamOn 100000

# 预期输出:
# =====================================
# Starting simulation...
# =====================================
# Output file: /path/to/output_file15.0.root
# ... (仿真过程) ...
# =====================================
# Simulation complete!
# Output file saved: /path/to/output_file15.0.root
# Simulation elapsed time: 2m 35s
# =====================================
```

### 材质研究配置
```bash
# 测试不同阳极材料
/Anode/setAnodeMaterial Mo              # 钼靶
# /Anode/setAnodeMaterial W             # 钨靶
# /Anode/setAnodeMaterial Cu            # 铜靶

# 测试不同滤过材料
/Filter/setFilterMaterial Be           # 铍滤过
# /Filter/setFilterMaterial Al         # 铝滤过
# /Filter/setFilterMaterial Cu         # 铜滤过

# 不同滤过厚度
/Filter/setFilterZ 0.1 mm              # 薄滤过
# /Filter/setFilterZ 1.0 mm            # 厚滤过
```

---

## ⚠️ 注意事项

1. **材料名称**: 所有材料命令会自动添加"G4_"前缀，请直接输入材料名（如Cu而非G4_Cu）

2. **角度单位**: 角度参数默认为度（degree），不需要指定单位

3. **长度单位**: 长度参数必须指定单位，建议使用mm或cm

4. **初始化顺序**: 必须在设置完所有几何和物理参数后调用`/run/initialize`

5. **材料有效性**: 系统会自动验证材料是否存在于Geant4材料库中

6. **参数范围**: 所有尺寸参数必须大于0，角度参数有相应的合理范围限制

7. **物理过程控制**: 
   - 物理过程开关命令必须在`/run/initialize`之前设置
   - 默认情况下所有物理过程都是启用的
   - 禁用韧致辐射会显著影响X射线产生
   - 建议在研究特定物理机制时有选择地禁用某些过程

---

## 🔍 常用材料参考

### 阳极靶材料
- `W` - 钨（高原子序数，适合高能X射线）
- `Mo` - 钼（中等原子序数，适合乳腺摄影）
- `Cu` - 铜（低成本选择）
- `Rh` - 铑（特殊应用）

### 滤过器材料  
- `Al` - 铝（最常用的滤过材料）
- `Cu` - 铜（硬化射线束）
- `Be` - 铍（低衰减滤过）
- `Mo` - 钼（匹配钼靶使用）

### 探测器材料
- `Si` - 硅（半导体探测器）
- `CESIUM_IODIDE` - 碘化铯（闪烁体探测器）
- `SODIUM_IODIDE` - 碘化钠（传统闪烁体）

---

## 💡 文件路径控制完整示例

### 基本文件路径设置
```bash
# 在初始化之前设置输入文件路径
/Input/setInputFilePath /mnt/kunlun/users/LYL/geant4/educatect/input/W_5_80kev_educate.txt

# 设置输出文件路径前缀
/Output/setOutputFilePath /mnt/kunlun/users/LYL/geant4/educatect/xenergyoutput/W_target_result

# 然后进行正常的初始化和运行
/run/initialize
/run/beamOn 10000
# 输出文件: W_target_result15.0.root (假设角度为15度)
```

### 物理过程控制示例
```bash
# 基础设置
/testem/phys/addPhysics emstandard_opt2

# 研究韧致辐射的贡献 - 只启用韧致辐射
/testem/phys/EnableIonisation 0         # 禁用电离
/testem/phys/EnableBremsstrahlung 1     # 启用韧致辐射
/testem/phys/EnableScatter 0            # 禁用散射
/testem/phys/EnableAnnihilation 0       # 禁用湮灭

/run/initialize
/run/beamOn 50000

# 研究所有过程的综合效果 - 全部启用
/testem/phys/EnableIonisation 1
/testem/phys/EnableBremsstrahlung 1
/testem/phys/EnableScatter 1
/testem/phys/EnableAnnihilation 1
```

---

*最后更新: 2025年7月30日*
