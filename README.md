## General info
Ragdoll edytor based on build in Esenthel Engine classes Ragdoll and Chr.
![MainWindow](./images/mainWindow.png)
## Technologies
* [Esenthel Engine](http://www.esenthel.com)
* Microsoft Visual Studio
* C++

## Getting Started
* Open Project.sln
* Build
* Run MyRagdoll.exe

## Usage
Version 0.0.1 alpha. For test purpose only!
#### All modes:
	ESC - Exit editor
	Left CTRL - Hold to rotate camera with mouse. Hold right mouse button to move camera.
	Mouse scroll - Zoom camera in and out. (Works also with LCTRL)
#### Simulation mode:
	Right mouse button hold on ragdoll body: drag body. (Causes errors!)
#### View mode:
	Left Shift + Left mouse button click: Select ragdoll actor and display parameters in window.
	(IMPORTAND! Change actor or click on viewport to update parameters and deselect active actor!)
	Pink actor: Parent to selected actor to which joint is created.
	Yellow actor: Selected actor.
	Black dot: Joint start pos.
	Green line: Joint.
	
* Disable Ragdoll Draw: Do not draw ragdoll and all related things.
* Start Simulation: Start physics update.
* Disable mesh draw: Do not draw mesh.
* Save params: Save ragdoll params to ragdoll_params.txt
* Load params: Load ragdoll params from ragdoll_params.txt and set it into current ragdoll. IMPORTAND! Deselect all actors to enable load button.

## ChangeLog
### Version 0.0.2
* Save selected actor and joint settings when switch too simulation mode or params are saved to file.

## Use with Esenthel Editor Code
After saving parameters for your ragdoll you can load it and use in existing project.
### Basic example
For basic example we will use application from Tutorial 11 - Physics/11 - Ragdoll.
* Import text file with ragdoll parameters to your project (Just drag and drop for example to Objects folder).
* Open application 11 - Physics/11 - Ragdoll.
To make it work we need some extra code that can be found in EsenthelProjectSource in repository.
* Create new code with name Helpers and copy code from ![EsenthelProjectSource/Helpers.h](.EsenthelProjectSource/Helpers.h)
* Create new code with name MyRagdoll and copy code from ![EsenthelProjectSource/MyRagdoll.h](.EsenthelProjectSource/MyRagdoll.h)
* Create new code with name MyRagdollParams and copy code from ![EsenthelProjectSource/MyRagdollParams.h](.EsenthelProjectSource/MyRagdollParams.h)
You should now have all needed source code and project must build. If no report bug.
To use new created class we need to make some changes in Main file.
* Change line 
```cpp
	Ragdoll ragdoll;
```
to
```cpp
	MyRagdoll ragdoll;
```
Now our class is used for ragdoll but it is created with default parameters.
* In Init() search for:
```cpp
 ragdoll.create(skel);                      // create ragdoll from skeleton
```
 and change it to
```cpp
	Flt densityOut;
   Mems<RagdollActorData> rad = RagdollDataHelpers::LoadRagdollData(UID(847958401, 1166262589, 4176582802, 1499001204), densityOut);
   RagdollData rd(densityOut,  rad);
   ragdoll.create(skel, rd,  1.7,  densityOut);
```
After that the ragdoll will be created with saved parameters.