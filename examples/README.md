# Lucurious Examples
Folder with a dump of examples

**To install**
```bash
mkdir -v build
meson build
ninja install -C build

# Encase of PolicyKit daemon errors
pkttyagent -p $(echo $$) | pkexec ninja install -C $(pwd)/build/
```

**To Uninstall**
```bash
ninja uninstall -C build

# Encase of PolicyKit daemon errors
pkttyagent -p $(echo $$) | pkexec ninja uninstall -C $(pwd)/build/
```

**Usage**
```bash
# This is just an extra step
export LD_LIBRARY_PATH=/usr/local/lib/:$LD_LIBRARY_PATH
cc -L /usr/local/lib/ -l lucurious simple_example.c -o se
./se
```
