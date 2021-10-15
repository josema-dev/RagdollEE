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
