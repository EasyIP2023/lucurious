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
cc -Wall -Wextra `pkgconf --cflags --libs lucurious` simple_example.c -o se
./se
```
**OR!**
```bash
make
./se
```

**Commands Line Usage**

Pull up man pages
```bash
man lucur
```
Print help message
```bash
lucur --help
```
Print instance extensions
```bash
lucur --pie
```
