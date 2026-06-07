#!/usr/bin/env bash
set -euo pipefail

# ─── 基本配置 ────────────────────────────────────────────────
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
APP_NAME="iiCTs"
PRO_FILE="$SCRIPT_DIR/${APP_NAME}.pro"
ICON_SRC="$SCRIPT_DIR/images/${APP_NAME}.png"

# 构建、输出目录
BUILD_ROOT="$(mktemp -d)"
trap 'rm -rf "$BUILD_ROOT"' EXIT
BUILD_DIR="$BUILD_ROOT/build"
APPDIR="$BUILD_ROOT/${APP_NAME}.AppDir"
DIST_DIR="$SCRIPT_DIR/dist"

# 找到 qmake（优先 Qt5）
QMAKE="$(which qmake-qt5 2>/dev/null || which qmake)"

# ─── 过滤 GLib/GIO 系列库，不打包进 AppDir ──────────────────────
skip_glib() {
  local base="$(basename "$1")"
  case "$base" in
    libglib-2.0.so.*|libgio-2.0.so.*|libgobject-2.0.so.* \
    |libgthread-2.0.so.*|libatk-bridge-2.0.so.*)
      return 0
      ;;
    *)
      return 1
      ;;
  esac
}

# ─── 1. 清理旧产物 & 创建目录 ─────────────────────────────────
rm -rf "$DIST_DIR"
mkdir -p "$BUILD_DIR" "$DIST_DIR" "$APPDIR"

# ─── 2. 编译 Release 可执行文件 ────────────────────────────────
pushd "$BUILD_DIR" >/dev/null
  "$QMAKE" "$PRO_FILE" CONFIG+=release
  make -j"$(nproc)"
popd >/dev/null

# ─── 3. 搭建 AppDir 目录结构 ──────────────────────────────────
mkdir -p \
  "$APPDIR/usr/bin" \
  "$APPDIR/usr/lib" \
  "$APPDIR/usr/share/applications" \
  "$APPDIR/usr/share/icons/hicolor/256x256/apps"

for sub in platforms iconengines imageformats sqldrivers xcbglintegrations platformthemes charts; do
  mkdir -p "$APPDIR/usr/plugins/$sub"
done

# ─── 4. 拷贝主程序与图标 ────────────────────────────────────────
cp "$BUILD_DIR/${APP_NAME}" "$APPDIR/usr/bin/"
chmod +x "$APPDIR/usr/bin/${APP_NAME}"
cp "$ICON_SRC" "$APPDIR/usr/share/icons/hicolor/256x256/apps/${APP_NAME}.png"

# ─── 5. 生成 .desktop 文件 ────────────────────────────────────
cat > "$APPDIR/usr/share/applications/${APP_NAME}.desktop" <<EOF
[Desktop Entry]
Type=Application
Name=${APP_NAME}
Exec=${APP_NAME}
Icon=${APP_NAME}
Categories=Utility;
EOF

# ─── 6. 生成 AppRun ───────────────────────────────────────────
cat > "$APPDIR/AppRun" <<'EOF'
#!/usr/bin/env bash
HERE="$(cd "$(dirname "$0")" && pwd)"

# 优先使用包内库和插件
export LD_LIBRARY_PATH="$HERE/usr/lib${LD_LIBRARY_PATH:+:}$LD_LIBRARY_PATH"
export QT_PLUGIN_PATH="$HERE/usr/plugins"
export QT_QPA_PLATFORM_PLUGIN_PATH="$HERE/usr/plugins/platforms"

# 不加载包内的 glib/GIO 库，避免版本冲突
unset GIO_MODULE_DIR

# 指定包内 dri 驱动目录
export LIBGL_DRIVERS_PATH="$HERE/usr/lib/dri"

exec "$HERE/usr/bin/iiCTs" "$@"
EOF
chmod +x "$APPDIR/AppRun"

# ─── 7. 收集主可执行文件依赖 ────────────────────────────────
echo "📦 收集主程序依赖库…"
ldd "$APPDIR/usr/bin/${APP_NAME}" \
  | awk '/=>/ {print $3}' \
  | sort -u \
  | while read -r lib; do
      [ -z "$lib" ] && continue
      basename="$(basename "$lib")"

      # 跳过核心系统库与 GLib/GIO
      case "$basename" in
        libc.so.*|libm.so.*|libpthread.so.*|libdl.so.*|librt.so.* \
        |libgcc_s.so.*|libstdc++.so.*|ld-linux*.so.*) continue ;;
      esac
      skip_glib "$lib" && continue

      echo "  → 拷贝 $basename"
      cp -u "$lib" "$APPDIR/usr/lib/"
    done

# ─── 8. 拷贝 Qt 库（含 QtConcurrent、QtCharts） ────────────────
QT_LIB_DIR="$("$QMAKE" -query QT_INSTALL_LIBS)"
echo "📦 拷贝 Qt 库…"
for mod in Core Gui Widgets Sql Network DBus Concurrent Charts; do
  cp -u "$QT_LIB_DIR/libQt5${mod}.so"* "$APPDIR/usr/lib/"
done

# ─── 9. 拷贝 Qt 插件 ─────────────────────────────────────────
QT_PLUGINS_DIR="$("$QMAKE" -query QT_INSTALL_PLUGINS)"
for sub in platforms iconengines imageformats sqldrivers xcbglintegrations platformthemes charts; do
  [ -d "$QT_PLUGINS_DIR/$sub" ] \
    && cp -r "$QT_PLUGINS_DIR/$sub/"* "$APPDIR/usr/plugins/$sub/"
done

# ─── 10. 收集插件依赖 ────────────────────────────────────────
echo "📦 收集插件依赖库…"
find "$APPDIR/usr/plugins" -type f -name "*.so*" | while read -r plugin; do
  ldd "$plugin" \
    | awk '/=>/ {print $3}' \
    | sort -u \
    | while read -r lib; do
        [ -z "$lib" ] && continue
        basename="$(basename "$lib")"

        # 跳过核心系统库与 GLib/GIO
        case "$basename" in
          libc.so.*|libm.so.*|libpthread.so.*|libdl.so.*|librt.so.* \
          |libgcc_s.so.*|libstdc++.so.*|ld-linux*.so.*) continue ;;
        esac
        skip_glib "$lib" && continue

        echo "  → 拷贝插件依赖 $basename"
        cp -u "$lib" "$APPDIR/usr/lib/"
      done
done

# ─── （可选）专门拷贝 libmd4c ────────────────────────────────
echo "📦 确保包含 libmd4c…"
for so in /usr/lib*/libmd4c.so.0*; do
  [ -e "$so" ] && cp -u "$so" "$APPDIR/usr/lib/"
done

# ─── 11. 补充分散的 X11/OpenGL 等系统库 ────────────────────────
echo "📦 补充分散的系统库…"
for dir in /usr/lib64 /usr/lib/x86_64-linux-gnu /usr/lib; do
  for lib in libxcb libX11 libXrender libXext libXfixes libxkbcommon libXcursor libXinerama libXrandr libXi libXss libGL; do
    cp -u $dir/${lib}*.so* "$APPDIR/usr/lib/" 2>/dev/null || :
  done
done

# ─── 11.1 拷贝 Mesa DRI 驱动 ─────────────────────────────────
echo "📦 拷贝 Mesa DRI 驱动…"
mkdir -p "$APPDIR/usr/lib/dri"
cp -u /usr/lib/x86_64-linux-gnu/dri/*.so* "$APPDIR/usr/lib/dri/" 2>/dev/null || :

# ─── 11.2 拷贝 libLLVM ───────────────────────────────────
echo "📦 拷贝 libLLVM…"
# Ubuntu/Debian 默认路径下的 libLLVM-15
for llvm in /usr/lib/x86_64-linux-gnu/libLLVM-15.so.1* \
            /usr/lib/libLLVM-15.so.1*; do
  [ -e "$llvm" ] && cp -u "$llvm" "$APPDIR/usr/lib/"
done

# ─── 12. 打包 AppDir ───────────────────────────────────────────
pushd "$BUILD_ROOT" >/dev/null
tar --dereference --hard-dereference -czf \
    "${DIST_DIR}/${APP_NAME}-AppDir.tar.gz" \
    "$(basename "$APPDIR")"
popd >/dev/null

# ─── 完成提示 ────────────────────────────────────────────────
cat <<EOF

✅ 打包完成！

分发文件：$DIST_DIR/${APP_NAME}-AppDir.tar.gz

使用方法：
  tar xzf ${APP_NAME}-AppDir.tar.gz
  cd ${APP_NAME}.AppDir
  ./AppRun

如果目标机缺少以下系统库，请先安装：
  libxcb1  libxcb-render0 libxcb-shm0 libx11-6 \
  libxrender1 libxkbcommon0 libx11-xcb1 libglu1-mesa
EOF
