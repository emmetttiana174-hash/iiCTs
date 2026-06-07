#!/usr/bin/env bash
set -euo pipefail

# ─── 基本配置 ────────────────────────────────────────────────
APP_NAME="iiCTs"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/build"
DIST_DIR="$SCRIPT_DIR/dist"
APPDIR="$DIST_DIR/${APP_NAME}.AppDir"
ICON_SRC="$SCRIPT_DIR/images/${APP_NAME}.png"

# ─── 忽略 GLib/GIO，不打包 ─────────────────────────────────────
skip_glib() {
  local base
  base="$(basename "$1")"
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

# ─── 1. 如未编译，先 CMake && make ────────────────────────────
if [ ! -x "$BUILD_DIR/$APP_NAME" ]; then
  echo "🔨 编译项目..."
  mkdir -p "$BUILD_DIR"
  pushd "$BUILD_DIR" >/dev/null
    cmake .. -DCMAKE_BUILD_TYPE=Release
    cmake --build . --parallel
  popd >/dev/null
fi

# ─── 2. 清理旧产物 & 搭建 AppDir 结构 ───────────────────────────
if [ -d "$DIST_DIR" ]; then
  find "$DIST_DIR" -mindepth 1 -maxdepth 1 -exec rm -rf {} +
else
  mkdir -p "$DIST_DIR"
fi

mkdir -p \
  "$APPDIR/usr/bin" \
  "$APPDIR/usr/lib" \
  "$APPDIR/usr/lib/dri" \
  "$APPDIR/usr/share/applications" \
  "$APPDIR/usr/share/icons/hicolor/256x256/apps"

for sub in platforms iconengines imageformats sqldrivers xcbglintegrations platformthemes charts; do
  mkdir -p "$APPDIR/usr/plugins/$sub"
done

# ─── 3. 拷贝可执行 & 图标 ─────────────────────────────────────
cp "$BUILD_DIR/$APP_NAME" "$APPDIR/usr/bin/"
chmod +x "$APPDIR/usr/bin/$APP_NAME"
if [ -f "$ICON_SRC" ]; then
  cp "$ICON_SRC" "$APPDIR/usr/share/icons/hicolor/256x256/apps/${APP_NAME}.png"
fi

# ─── 4. 生成 .desktop ─────────────────────────────────────────
cat > "$APPDIR/usr/share/applications/${APP_NAME}.desktop" <<EOF
[Desktop Entry]
Type=Application
Name=${APP_NAME}
Exec=${APP_NAME}
Icon=${APP_NAME}
Categories=Utility;
EOF

# ─── 5. 生成启动脚本（根目录下同名二进制）──────────────────────
cat > "$APPDIR/$APP_NAME" <<EOF
#!/usr/bin/env bash
HERE="\$(cd "\$(dirname "\$0")" && pwd)"
export LD_LIBRARY_PATH="\$HERE/usr/lib\${LD_LIBRARY_PATH:+:}\$LD_LIBRARY_PATH"
export QT_PLUGIN_PATH="\$HERE/usr/plugins"
export QT_QPA_PLATFORM_PLUGIN_PATH="\$HERE/usr/plugins/platforms"
unset GIO_MODULE_DIR
export LIBGL_DRIVERS_PATH="\$HERE/usr/lib/dri"
exec "\$HERE/usr/bin/${APP_NAME}" "\$@"
EOF
chmod +x "$APPDIR/$APP_NAME"

# ─── 6. 收集主程序依赖 ─────────────────────────────────────────
echo "📦 收集主程序依赖..."
ldd "$APPDIR/usr/bin/$APP_NAME" \
  | awk '/=>/ {print $3}' \
  | sort -u \
  | while read -r lib; do
      [ -z "$lib" ] && continue
      bn="$(basename "$lib")"
      case "$bn" in
        libc.so.*|libm.so.*|libpthread.so.*|libdl.so.*|librt.so.* \
        |libgcc_s.so.*|libstdc++.so.*|ld-linux*.so.*) continue ;;
      esac
      skip_glib "$lib" && continue
      echo "  → $bn"
      cp -u "$lib" "$APPDIR/usr/lib/"
    done

# ─── 7. 拷贝 Qt 库 ─────────────────────────────────────────────
QT_PREFIX="$(pkg-config --variable=prefix Qt5Core 2>/dev/null || echo /usr)"
QT_LIB_DIR="$QT_PREFIX/lib"
echo "📦 拷贝 Qt 库..."
for mod in Qt5Core Qt5Gui Qt5Widgets Qt5Sql Qt5Concurrent Qt5Charts; do
  cp -u "$QT_LIB_DIR/lib${mod}.so"* "$APPDIR/usr/lib/" 2>/dev/null || :
done

# ─── 8. 拷贝 Qt 插件 ───────────────────────────────────────────
QT_PLUGINS_DIR="$(qtpaths --plugin-dir 2>/dev/null || echo "$QT_PREFIX/lib/qt5/plugins")"
echo "📦 拷贝 Qt 插件..."
for sub in platforms iconengines imageformats sqldrivers xcbglintegrations platformthemes charts; do
  if [ -d "$QT_PLUGINS_DIR/$sub" ]; then
    cp -r "$QT_PLUGINS_DIR/$sub/"* "$APPDIR/usr/plugins/$sub/"
  fi
done

# ─── 9. 收集插件依赖 ───────────────────────────────────────────
echo "📦 收集插件依赖..."
find "$APPDIR/usr/plugins" -type f -name '*.so*' | while read -r plugin; do
  ldd "$plugin" \
    | awk '/=>/ {print $3}' \
    | sort -u \
    | while read -r lib; do
        [ -z "$lib" ] && continue
        bn="$(basename "$lib")"
        case "$bn" in
          libc.so.*|libm.so.*|libpthread.so.*|libdl.so.*|librt.so.* \
          |libgcc_s.so.*|libstdc++.so.*|ld-linux*.so.*) continue ;;
        esac
        skip_glib "$lib" && continue
        echo "  → 插件 $bn"
        cp -u "$lib" "$APPDIR/usr/lib/"
      done
done

# ─── 10. 补充分散系统库、DRI、libLLVM ───────────────────────────
echo "📦 补全系统库..."
for dir in /usr/lib64 /usr/lib/x86_64-linux-gnu /usr/lib; do
  for lib in libxcb libX11 libXrender libXext libXfixes libxkbcommon \
             libXcursor libXinerama libXrandr libXi libXss libGL; do
    cp -u "$dir/${lib}"*.so* "$APPDIR/usr/lib/" 2>/dev/null || :
  done
done

echo "📦 Mesa DRI 驱动..."
cp -u /usr/lib/x86_64-linux-gnu/dri/*.so* "$APPDIR/usr/lib/dri/" 2>/dev/null || :

echo "📦 libLLVM..."
for llvm in /usr/lib/x86_64-linux-gnu/libLLVM-*.so* /usr/lib/libLLVM-*.so*; do
  [ -e "$llvm" ] && cp -u "$llvm" "$APPDIR/usr/lib/"
done

# ─── 强制落盘 ───────────────────────────────────────────────────
sync

# ─── 11. 打包并去掉 “.AppDir” 后缀 ───────────────────────────────
echo "📦 打包到 ${DIST_DIR}/${APP_NAME}.tar.gz ..."

# tar --warning=no-file-changed \
#     --transform "s|^${APP_NAME}\.AppDir|${APP_NAME}|" \
#     -C "$DIST_DIR" \
#     -czf "${DIST_DIR}/${APP_NAME}.tar.gz" \
#     "${APP_NAME}.AppDir"


set +e
tar -C "$DIST_DIR" \
    --warning=no-file-changed \
    --transform "s|^${APP_NAME}\.AppDir|${APP_NAME}|" \
    -czf "${DIST_DIR}/${APP_NAME}.tar.gz" \
    "${APP_NAME}.AppDir"
TAR_EXIT=$?
set -e
if [ $TAR_EXIT -ne 0 ]; then
  echo "⚠️ 打包过程中出现警告或错误，但脚本将继续" 
fi

# 删除临时 AppDir
rm -rf "$APPDIR"

# ─── 完成提示 ───────────────────────────────────────────────────
printf "✅ 打包完成：%s\n" "$DIST_DIR/${APP_NAME}.tar.gz"
# popd >/dev/null

# ─── 完成提示 ───────────────────────────────────────────────────
# echo
# echo "使用方法："
# echo "  tar xzf ${APP_NAME}.tar.gz"
# echo "  cd ${APP_NAME}"
# echo "  ./${APP_NAME}"
# echo
# echo "如遇找不到系统库，请先安装："
# echo "  libxcb1 libxcb-render0 libxcb-shm0 libx11-6 \\"
# echo "  libxrender1 libxkbcommon0 libx11-xcb1 libglu1-mesa"
# echo
