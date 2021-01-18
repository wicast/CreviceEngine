## <center>Crevice Engine (Still Under Construction)</center>
![](doc/pic/crevice_logo.svg)

### Intro
Crevice is a 'ecs-driven', 'vulkan-first' Game Engine which also aims to be highly customizable.
'ecs-driven' means the architecture design is heavily ecs-style under the hood, lots of components are designed specially for ecs-style usage.
Vulkan is the primary render backend api.

### Third party library used
- [flecs](https://github.com/SanderMertens/flecs) for ecs implementation.
- [eastl](https://github.com/electronicarts/EASTL) for stl implementation.
- [glm](https://github.com/g-truc/glm) for graphic math.
- [glfw](https://github.com/glfw/glfw)(sdl2 in plan) for game window container.
- [QT5](https://www.qt.io) for editor.
- [volk](https://github.com/zeux/volk) for vulkan loader.

### Build&Run

**Notice: this project is windows msvc only for now**

1) Get the repo
you need git and git lfs for clone the repo
```shell
git clone https://www.github.com/wicast/CreviceEngine
git submodule init
git submodule update --depth 1
```

2) Install [xmake](https://xmake.io/#/)

you can also use [scoop](https://scoop.sh/) to install xmake
```shell
scoop install xmake
```

3) Install Vulkan SDK  
[Vulkan SDK](https://www.lunarg.com/vulkan-sdk/) should be installed for validation layer is required. Shader compiling also needs this sdk.

4) Build and run  

It is recommended to use vscode + xmake plugin. Everything will automatically configure.
There are two runnable target:
- vkMain
- qtEditor  

one for glfw based window,one for editor.

Or you can use xmake directory in command line.
glfw based window
```shell
xmake f -p windows -a x64
xmake build -w vkMain
xmake r vkMain
```

editor

```shell
xmake f -p windows -a x64 --qt="directory to your qt sdk"
xmake build -w qtEditor
xmake r qtEditor
```  

Also don't forget to compile shaders to spir-v
```
cd shaders
compile.bat
```

In vscode, there is a task for this.

### Screenshot

###### glfw window
![vmMain](doc/pic/vkMain.jpg)


###### editor docking
![editor](doc/pic/editor.jpg)
![editor docking](doc/pic/qt_editor_dock.gif)


###### Large file async loading.
The example using [Happy Buddha](http://graphics.stanford.edu/pub/3Dscanrep/happy/happy_recon.tar.gz) from Stanford University Computer Graphics Laboratory which has 1,087,716 triangles.

![large file loading](doc/pic/large_file.gif)

### Api documentation
you need to install [doxygen](https://github.com/doxygen/doxygen) and [grphiviz](https://graphviz.org)(optional for dependency graph)

```shell
cd doc
doxygen
```
after that the doc will appear in `build/html/index.html`


### THE GOOD, THE BAD AND THE UGLY
This is a brief on design and implementation status.

##### THE GOOD
*"Such ingratitude. After all the times I've saved your life."*


- With the power of flecs, dynamically attach system function is possible.
- Resource is specially designed for ecs and flexibility. Resource object is different component on different dedicate servers, like image is pixels on memory, texture is pixels on gpu, this is not tradition way of inheritance of classes. This is useful for resource loading with GPUDirect, you just need to disable default system and impl your custom system, loading process can skip creating resources on memory and you don't have to modify the resource class.
- RenderGraph driven render pipeline, vulkan impl is done.
- The RenderServer(handle render backend like vk,dx12) can coexist in a instance by design. Although other server like dx12 is not impl yet, but it will be useful in future. Like using DXVA(DirectX Video Acceleration) in dx and reading in vulkan through external memory or GPUDirect in dx12 and used in vulkan.
- Async resource loading is done in ecs, with flecs's pipeline system.
- Scene tree dumping to editor is impl with diff tree. Remote inspection can be easily done.

##### THE UGLY
*"When you have to shoot, shoot. Don't talk."*

- Resource dependency is now impl via same resource id, but one asset might derive to two different resources in some circumstance. So it's better with different resource id with resource dependency management.
- Third party library is now used directly, wrapping a lib is time consuming, some of them are partially done (eastl), weather it is really necessary should be discussed.
- Resource loading still needs to improve, Checking every future is not efficient, a epoll style event query system might use for notifying the resource creation procedure.
- SwapChain recreation is a mess, with hardcoded code. The `swapchain out of date` event is caused by window container, but triggered by acquire swapchain image and executed recreation in render loop. While recreating, the render server have to access a lot of external object. This might improve by [signal/slots mechanism](https://github.com/TheWisp/signals).
- Windows only and MSVC only, even mingw is not working. Template Specialization should in namespace when using gcc. GLM has some bug when using mingw.
- Shader should be included in resource loading system.

##### THE BAD
*"That an accusation?"*

- Code style and naming convention(including directory name) is totally a disaster. Some class should also under crevice namespace. 
- There are too many header only files just for quick impl, this will cause redefine problem etc, it's better split in to cpp file. Shame on c++ anyway.
- Resource manager is to using shared_ptr for deferred release(kind of grabage collection, not impl yet), but code is kinda messy. Using shared_ptr is also tedious. Resource handle should place in the dedicate server by design, but the code is totally another story.

### TODO
- [ ] scene tree view in editor, it's almost done.
- [ ] RenderGraph editor.
- [ ] multi render window(multiple surface) for editor.
- [ ] editor state and play state in engine core.
- [ ] metadata system for editor entity inspection.
- [ ] code gen for reflection to get the metadata of component and shader.
- [ ] more 3d formats rather than simple obj format. [glTF](https://www.khronos.org/gltf) is considered for primary 3d format.
- [ ] [vma](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator) for vulkan memory management.
- [ ] better material definition.
- [ ] complex render technic impl like pbr, ssao etc.
- [ ] scene edit, component edit and all editor's work.
- [ ] luajit script system.
- [ ] [WAMR](https://github.com/bytecodealliance/wasm-micro-runtime) and [AssemblyScript](https://www.assemblyscript.org/) based webassembly script system.
- [ ] animation system.
- [ ] [bullet physics](https://pybullet.org) integration
- [ ] [soloud](https://github.com/jarikomppa/soloud) sound system.


### LICENSE

Most of the code is under MIT except qt editor is under LGPLv3