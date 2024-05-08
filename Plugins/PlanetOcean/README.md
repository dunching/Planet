# PlanetOcean Plugin
Gerstner waves for planets and flat worlds for Unreal Engine 5.3+

![PlanetOcean](https://github.com/script526/WavesGenerator/assets/32175853/5491053b-44b8-4ed3-be87-ea0fd7f21d0e)
![FlatOcean](https://github.com/script526/WavesGenerator/assets/32175853/fc2e2377-ac1b-4866-a141-28bae14708a2)
# Warning
- This plugin doesn't provide any mesh quadtree/LOD system. You have to use your own, or take advantage of other plugins like Voxel Plugin or Worldscape.
- Meshes that are used in the example scenes were just subdivided to achieve the desirable level of detail.
# Features
- Spherical oceans can be moved and rotated (check example map)
- You can have more than one planet with a spherical ocean in the scene
- Wave foam
- Texture-based scrolling normals on top of displacement normals
- Additional masks that you might want to use:
	- Distance to vertex
	- Distance field mask (can be used to fade off World Position Offset and normals)
	- Beach mask based on depth (for beach foam, only pixel shader)
	- Specular highlight mask
	- Sun reflection mask
# Planned
- Distance based wave count reduction
- Wave presets blending
- Buoyancy
- Planet gravity using Unreal's native movement component
- Subsurface scattering
- Better foam masking and fading out
- Custom screen-space reflections based on ocean shader with greater control compared to standard SSR
- Playable character
- Drivable boat
- (Possibly) CPU async or shader based mesh LOD system
# Support
  - https://www.facebook.com/aaalikin/
  - Discord: scrappy526
  - I'm open for contributors
