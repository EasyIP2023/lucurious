# Testing

NOTE: The wayland client code is a modified version of what [Simon Ser did here](https://github.com/emersion/hello-wayland/).

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

**Valgrind reported leaks supression**

There are a few valgrind reported leaks that the API has no control over.
```
valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --undef-value-errors=no --trace-children=yes --gen-suppressions=all  <executable> &> <filename>.supp
```
 
These are potential commands one could type to delete lines after the top
```
sed '/==/d' <filename>.supp > <new_filname>.supp 
sed '/1m/d' <filename>.supp > <new_filname>.supp
sed '/0m/d' <filename>.supp > <new_filname>.supp 
```

```
valgrind --suppressions=<new_filename>.supp
```