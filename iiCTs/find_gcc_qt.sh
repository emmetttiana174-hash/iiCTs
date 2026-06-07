###
 # @Description  : 
 # @Author       : Guo Yi
 # @Date         : 2025-08-22 03:13:43
 # @LastEditors  : Guo Yi
 # @LastEditTime : 2025-08-22 03:24:53
 # @FilePath     : \\iiCTs\\find_gcc_qt.sh
 # Copyright (C) 2025 Guo Yi. All rights reserved.
### 
# *****************************************************
# 1. chmod +x find_gcc_qt.sh
# 2. ./find_gcc_qt.sh
# *****************************************************
#!/usr/bin/env bash
set -euo pipefail
IFS=$'\n\t'

#
# find_gcc_qt.sh
# 查找系统中所有可用的 GCC 可执行文件及 Qt 路径（包括 qmake 和 CMake 配置）
#

echo "==== GCC executables in PATH ===="
mapfile -t gcc_in_path < <(which -a gcc 2>/dev/null | sort -u)
if (( ${#gcc_in_path[@]} )); then
  for gcc in "${gcc_in_path[@]}"; do
    version=$("$gcc" --version 2>/dev/null | head -n1)
    printf "%-40s %s\n" "$gcc" "$version"
  done
else
  echo "No gcc found in PATH"
fi

echo
echo "==== Extra GCC under /usr/{,local}/bin ===="
mapfile -t gcc_extra < <(
  find /usr/bin /usr/local/bin -maxdepth 1 -type f -name 'gcc*' -executable 2>/dev/null | sort -u
)
for gcc in "${gcc_extra[@]}"; do
  if ! printf '%s\n' "${gcc_in_path[@]}" | grep -qx "$gcc"; then
    version=$("$gcc" --version 2>/dev/null | head -n1)
    printf "%-40s %s\n" "$gcc" "$version"
  fi
done

echo
echo "==== qmake executables in PATH ===="
declare -a qmake_cmds=(qmake qmake-qt5 qmake-qt6 qmake6)
mapfile -t qmake_in_path < <(
  for cmd in "${qmake_cmds[@]}"; do
    which -a "$cmd" 2>/dev/null || :
  done | sort -u
)
if (( ${#qmake_in_path[@]} )); then
  for qmake in "${qmake_in_path[@]}"; do
    prefix=$("$qmake" -query QT_INSTALL_PREFIX 2>/dev/null || echo "<unknown>")
    ver=$("$qmake" -query QT_VERSION         2>/dev/null || echo "<unknown>")
    printf "%-40s prefix=%-20s version=%s\n" "$qmake" "$prefix" "$ver"
  done
else
  echo "No qmake found in PATH"
fi

echo
echo "==== Extra qmake under /opt, /usr/local, /usr/lib ===="
mapfile -t qmake_extra < <(
  find /opt /usr/local /usr/lib -type f -name 'qmake*' -executable 2>/dev/null | sort -u
)
for qmake in "${qmake_extra[@]}"; do
  if ! printf '%s\n' "${qmake_in_path[@]}" | grep -qx "$qmake"; then
    prefix=$("$qmake" -query QT_INSTALL_PREFIX 2>/dev/null || echo "<unknown>")
    ver=$("$qmake" -query QT_VERSION         2>/dev/null || echo "<unknown>")
    printf "%-40s prefix=%-20s version=%s\n" "$qmake" "$prefix" "$ver"
  fi
done

echo
echo "==== Searching for Qt CMake config files ===="
mapfile -t cmake_cfgs < <(
  find /opt /usr /usr/local -type f \
    \( -name "Qt6Config.cmake" -o -name "Qt5Config.cmake" -o -name "qt6-config.cmake" \) \
    2>/dev/null | sort -u
)
if (( ${#cmake_cfgs[@]} )); then
  for cfg in "${cmake_cfgs[@]}"; do
    prefix=$(dirname "$(dirname "$cfg")")
    printf "%-60s -> prefix: %s\n" "$cfg" "$prefix"
  done
else
  echo "No Qt CMake config files found under /opt, /usr, /usr/local"
fi

echo
echo "Done. 将上面输出中带 'prefix:' 的路径拷贝到 CMAKE_PREFIX_PATH 即可。"
