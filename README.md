Unreal C++ Coding
=================

Repo for tracking and backing up my experimental Unreal C++ coding.

# Aim

The purpose is to get to know the Unreal C++ framework and learn all the basics such as camera work, player control, HUDs, interaction, multiplayer and AI.

Level design and content such as meshes will not be committed and so while the source may be educational it is not a complete project. However, if you wish to plug this source into your own project then starting with the Unreal Code Flying (C++) template project is the best starting point. Naming it SpaceExplorer will make the integration much easier.

You will need of course to assign inputs which may look something like this:

![](http://i955.photobucket.com/albums/ae34/Bornich/SpaceExplorerInput_zpsc1672e10.png)

# Blueprints

Update: Because blueprints have turned out to be so fun and powerful I have developed all of the HUD and inventory system in blueprints. I've tried to include all the relevant uassets, materials and textures but this is still not a complete project.

Some blueprints are committed, but will generally be based on C++ custom classes so that the underlying code can easily be modified. Such as the CustomHud class which is the basis for the blueprint HUD.

The C++ classes will generally be exposed to blueprints for easy in-editor tweaks.

# Learning Sources

## C++

* [HUD using Canvas](https://wiki.unrealengine.com/HUD,_Canvas,_Code_Sample_of_800%2B_Lines,_Create_Buttons_%26_Draw_Materials)
* [C++ FPS Tutorial](https://wiki.unrealengine.com/First_Person_Shooter_C%2B%2B_Tutorial)

## Tutorials

* [YouTube Channel - Tesla Dev](https://www.youtube.com/channel/UC3QBWg9pMnaFF-q0qjXPDEg)
* [YouTube Inventoy System - Savai Kun](https://www.youtube.com/channel/UCLCVZLOs3tdcKYGjmXZWK2Q)
* [How to creat a space skybox](https://answers.unrealengine.com/questions/15307/how-to-create-simple-space-skybox.html)

## Official

* [Unreal Engine Docs](https://docs.unrealengine.com)
* [Unreal Blueprint Content Example](https://docs.unrealengine.com/latest/INT/Resources/ContentExamples/Blueprints/index.html)
* [Unreal Wiki Tutorials](https://wiki.unrealengine.com/Category:Tutorials)
* [Damage](https://www.unrealengine.com/blog/damage-in-ue4)

## Tools

* [The excellent Spacescape space skybox creator by Alex C Peterson](http://alexcpeterson.com/spacescape)

Technology details
-----------------

This project is free and open source for you to use as you see fit, but uses the Unreal® Engine and Unreal® is a trademark or registered trademark of Epic Games, Inc. in the United States of America and elsewhere.

Unreal® Engine, Copyright 1998 – 2014, Epic Games, Inc. All rights reserved.
