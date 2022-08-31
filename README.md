# Advanced Sessions Plugin plugin for Unreal Engine 5

This plugin is designed for users of Unreal Engine 5. It will help you to create/join & invite your friends with steam (or not) for your game.


## Installation

1) Dowload this repostory
2) Extract the repo
3) Copy all content to your plugin project (YourProject/Plugins)
4) Open your project and enable the plugin

## Initialization

For a good functioning of the plugin you will have to add some lines in your **DefaultEngine.ini** :

```
[/Script/Engine.GameEngine]
+NetDriverDefinitions=(DefName="GameNetDriver",DriverClassName="OnlineSubsystemSteam.SteamNetDriver",DriverClassNameFallback="OnlineSubsystemUtils.IpNetDriver")

[OnlineSubsystem]
DefaultPlatformService=Steam

[OnlineSubsystemSteam]
bEnabled=true
SteamDevAppId=480

; If using Sessions
; bInitServerOnClient=true

[/Script/OnlineSubsystemSteam.SteamNetDriver]
NetConnectionClassName="OnlineSubsystemSteam.SteamNetConnection"
```

## Nodes 

For Create a Advanced Session use this node : 

![Create advanced session](https://user-images.githubusercontent.com/47295080/149629554-a286780f-ab64-49f9-8cb9-1e00616c2544.png)

For Finding some Advanced Session use this node : 

![Find Sessions](https://user-images.githubusercontent.com/47295080/149629664-f1f487aa-18a8-46a2-9e63-df9636b6d240.png)

For Join a session use this node : 

![Join Session](https://user-images.githubusercontent.com/47295080/149629583-3792f87c-41c7-470e-b579-d676ebccd05b.png)


### **More advanced documentation coming soon.**
