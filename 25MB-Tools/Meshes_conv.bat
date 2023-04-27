echo OFF

for /D %%d in (Data/Meshes/*) do (
    for %%f in (Data/Meshes/%%d/*.bin) do (
        mesh-compress Data/Meshes/%%d/%%f ../Data-Mod/Data/Meshes/%%d/%%f
    )
)