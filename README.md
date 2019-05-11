# boost.out_ptr

`boost.out_ptr` is a simple parameter wrapper for output pointers.

| Appveyor | Travis CI |
| -------- | --------- |
|          |           |
|          |           |
|          |           |

# Quick Comparison and Example

```cpp
// Before
std::unique_ptr<Obj, ObjDeleter> ptr;

if (T* tmp_ptr; !c_api_get_obj(&tmp_ptr, ...)) {
  throw std::runtime_error(...);
}
else {
  ptr.reset(tmp_ptr);
}

use(ptr);
```

```cpp
// After
std::unique_ptr<Obj, ObjDeleter> ptr;

if (!c_api_get_obj(std::inout_ptr(ptr), ...)) {
  throw std::runtime_error(...);
}

use(ptr);
```


# Full Examples and Documentation

There are examples and documentation contained in this repository: please, peruse them as much as you need to! Some interesting/illuminating ones:

- It works with [custom unique pointers just fine](examples/source/std.custom_unique_ptr.cpp)
- It is [customizable to your own pointer types](examples/source/custom.handle.cpp), if you need performance or different semantics
- It works with [Boost](examples/source/boost.shared_ptr.cpp) and [Standard](examples/source/std.shared_ptr.cpp) shared pointers.
- It works with things like https://github.com/okdshin/unique_resource[unique_resource] out of the box.
