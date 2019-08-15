# Testing

**Running test**
```bash
mkdir -v build/
meson build/ -Dc_args=-DDEBUG
ninja test -C build
cat build/meson-logs/testlog.txt
```
