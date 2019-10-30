# Testing

**Running test**
```bash
meson build/
ninja test -C build
cat build/meson-logs/testlog.txt
```

**To test Wayland Client Images**
```bash
meson test -C build/ --suite images
```
