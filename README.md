<p align="center"><img src="exports_shared/tea_cerem_2048.png"></p>

---
## Rendering Steps
- Direct Illumination and Specular via (Distributed) whitted-style raytracing.
- Indirect Illumination and Caustics via photon map.

---
## More Info
Please refer to [this blog post](https://skittss.github.io/PortfolioWebsite/#/projects/RayTracer) where I describe the features of this raytracer in more detail, and outline how each one was achieved.

---
## Compiling and Running
The source is compiled using CMake and g++.
\
\
`build_win.bat` and `build_unix.sh` are provided which will build the project for windows and unix (linux/MacOS) respectively. (note: unix building is untested)
\
\
If using windows, specify a path before building for MinGW in `.env.cmake` using the template `env_template.cmake`.
\
\
After the project is built, ensure you run the executable <b><u>from the build directory</u></b> or else
relative paths will break.
\
\
Note, the executable will *not* output a file if the output directories do not exist (the above build files do handle making these directories, however).
\
\
Vscode build tasks are also included for running & debugging, but will need to have their compiler paths updated if you plan to use them.

---
## Project Structure



|Name|Description|
|:--|:--|
|`./src`|Program entry point and Scene Compositing|
|`./lib`|Core Raytracer functionality|
|`./bin`|Default build directory
|`./exports`|Default render directory

|SubDir|Description
|:--|:--|
|`lib/.../config`|Raytracer config and general parameters (e.g. render output path).|
|`lib/.../data`|Data types including kd-tree and matrices.|
|`lib/.../geometry`|3D Objects|
|`lib/.../lights`|Point and area lights|
|`lib/.../materials`|3D Object Materials (Implementation of BDRFs)|
|`lib/.../rendering`|Core rending code - camera, ray casting and photon mapping|
|`lib/.../textures`|UV-mapped and procedural textures for materials.|
|`lib/.../util`|Various helper functions|
|`lib/include/engine.h`|Lib include manifest (must be edited if new files added)|

<details>
<summary>Full tree view of ./lib</summary>

```
📂lib
 ┣ 📂include
 ┃ ┣ 📂config
 ┃ ┃ ┗ 📜config.h
 ┃ ┣ 📂data
 ┃ ┃ ┣ 📜bounding_box.h
 ┃ ┃ ┣ 📜bounding_volume.h
 ┃ ┃ ┣ 📜colour.h
 ┃ ┃ ┣ 📜framebuffer.h
 ┃ ┃ ┣ 📜hit.h
 ┃ ┃ ┣ 📜kd_tree.h
 ┃ ┃ ┣ 📜object.h
 ┃ ┃ ┣ 📜photon.h
 ┃ ┃ ┣ 📜ray.h
 ┃ ┃ ┣ 📜transform.h
 ┃ ┃ ┗ 📜vector.h
 ┃ ┣ 📂geometry
 ┃ ┃ ┣ 📜csg.h
 ┃ ┃ ┣ 📜cube.h
 ┃ ┃ ┣ 📜object3D.h
 ┃ ┃ ┣ 📜plane.h
 ┃ ┃ ┣ 📜polymesh.h
 ┃ ┃ ┣ 📜quadric.h
 ┃ ┃ ┗ 📜sphere.h
 ┃ ┣ 📂lights
 ┃ ┃ ┣ 📜directional_light.h
 ┃ ┃ ┣ 📜light.h
 ┃ ┃ ┣ 📜point_light.h
 ┃ ┃ ┗ 📜square_light.h
 ┃ ┣ 📂materials
 ┃ ┃ ┣ 📜dielectric.h
 ┃ ┃ ┣ 📜emissive.h
 ┃ ┃ ┣ 📜global_material.h
 ┃ ┃ ┣ 📜lambertian.h
 ┃ ┃ ┣ 📜material.h
 ┃ ┃ ┣ 📜mirror.h
 ┃ ┃ ┣ 📜normat.h
 ┃ ┃ ┗ 📜phong.h
 ┃ ┣ 📂rendering
 ┃ ┃ ┣ 📜camera.h
 ┃ ┃ ┗ 📜scene.h
 ┃ ┣ 📂textures
 ┃ ┃ ┣ 📜block_texture.h
 ┃ ┃ ┣ 📜checker_texture.h
 ┃ ┃ ┣ 📜image_texture.h
 ┃ ┃ ┗ 📜texture.h
 ┃ ┣ 📂util
 ┃ ┃ ┗ 📜util.h
 ┃ ┗ 📜engine.h
 ┗ 📂src
 ┃ ┣ 📂data
 ┃ ┃ ┣ 📜bounding_volume.cpp
 ┃ ┃ ┗ 📜framebuffer.cpp
 ┃ ┣ 📂geometry
 ┃ ┃ ┣ 📜cube.cpp
 ┃ ┃ ┣ 📜plane.cpp
 ┃ ┃ ┣ 📜polymesh.cpp
 ┃ ┃ ┗ 📜sphere.cpp
 ┃ ┣ 📂materials
 ┃ ┃ ┣ 📜lambertian.cpp
 ┃ ┃ ┗ 📜phong.cpp
 ┃ ┣ 📂rendering
 ┃ ┃ ┣ 📜camera.cpp
 ┃ ┃ ┗ 📜scene.cpp
 ┃ ┗ 📂util
 ┃ ┃ ┗ 📜util.cpp
```
</details>

---
## Improvements

- [ ] Periodic autosave for large renders.
