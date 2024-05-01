## What is it?
A tool for simple cleanup operations like finding and removing unused assets, renaming and such.

This is a WIP but im trying to ensure that whats on the repo is always stable.

## Engine / Languages
- Unreal Engine 5.3
- C++

> [!WARNING]
> Since this is written for UE 5.3 and the functionality/syntax of needed modules changed quite a bit during UE5 evolution, this will not work on other versions out of the box.

## How to install

- Put the "UnrealCleanup folder into the plugin folder of the poject where you want to use the plugin
  
  this should be located in the root folder of your project. If you dont have it, just create one.

  ![Screenshot 2024-05-01 110733 _sstep1png](https://github.com/Finderadeis/UnrealCleanup/assets/79834166/0b8f6466-2ca4-4edc-bca7-0ef76da1d087)


- Delete "Binaries" and "Intermediate" folders (it needs to recompile for the plugin to work)

  ![Screenshot 2024-05-01 110733 _sstep2](https://github.com/Finderadeis/UnrealCleanup/assets/79834166/f8e628e3-11ba-4da7-a6d4-70bab582dc61)

- Double click the .uproject file and unreal should prompt you to rebuild

![Screenshot 2024-05-01 111921](https://github.com/Finderadeis/UnrealCleanup/assets/79834166/9510f626-f19f-4175-ba16-9a2862463af2)

- The plugin should be automatically activated. If you want to double-check, it should be listed under Plugins-Installed-Other
  
![Screenshot 2024-05-01 112137](https://github.com/Finderadeis/UnrealCleanup/assets/79834166/37791c80-9588-4b63-b2ac-a77c61c49799)

> [!TIP]
> If you want to use the plugin in multiple project without copying it around all the time, you can put a copy of the compiled version into the engines plugin folder
>
>   ![Screenshot 2024-05-01 112735](https://github.com/Finderadeis/UnrealCleanup/assets/79834166/a4452161-5ee4-47ce-b0fb-be7cf24649ba)

## How to use

- Rightclick any folder in yout content browser and select the "Project Cleanup" option

  ![Screenshot 2024-05-01 113045](https://github.com/Finderadeis/UnrealCleanup/assets/79834166/cf4a3216-adef-43fc-8950-e7a1c9c91994)

- A window will pop up with all the options available (for now just asset deletion is ready)
- You can show either all assets in the selected folder and delete them manually, or get just the unused assets to easily get rid of those

Folder deletion will be up next.
