# Command and Conquer: WarZone 
by StormHowlers

<p align="center">
  <img  src="https://raw.githubusercontent.com/Scarzard/StormHowlers/Wiki/Wiki%20Images/Home%20Page/goldstar.png" width="100">
</p>


![game logo]( change1)

## Game info

The game we're developing is a mix between the classic RTS games and the tower defense genre.
It has one very clear objective for both players: "Crush your opposition without showing any mercy!"

Command and Conquer: WarZone, features a streamlined resource system that allows the player to focus on more important tasks such as fighting and defending your precious base.
We have also implemented an automatic troop AI, as well as an auto-deploy from the barracks they have been created. 
This means that once bought, they will automatically be deployed and they will go to their main objective. 
It can be everything, enemy troops, defensive structures or the Town Hall itself.

The game also features a polished UI so any new players can quickly grasp how to navigate through the UI and allows seasoned players to execute more efficiently.
The game also communicates with the players to notify them about the game state via voicelines.

The game is intended to be player with controllers for 2 people at a time. 

## Team

![StormHowlers Logo](https://raw.githubusercontent.com/Scarzard/StormHowlers/Wiki/Wiki%20Images/Home%20Page/stormhowlers%20with%20catchphrase.png)

We are an indie game development team formed by 7 students taking the bachelor's degree in Videogame design and development in the UPC.
The game will be developed for the subject Project II. We will be making a tribute to the well-known RTS game: Command&Conquer: Red Alert 2.


![Team photo](https://raw.githubusercontent.com/Scarzard/StormHowlers/Wiki/Wiki%20Images/Home%20Page/team_photo.jpeg)

- ![](https://raw.githubusercontent.com/Scarzard/StormHowlers/Wiki/Wiki%20Images/Home%20Page/COMMANDO.png) **Lead:** Andrés Saladrigas
   - [Andrés' GitHub Link](https://github.com/TheArzhel)
   
- ![](https://raw.githubusercontent.com/Scarzard/StormHowlers/Wiki/Wiki%20Images/Home%20Page/SPY.png) **Management:** Òscar Pons
   - [Òscar's GitHub Link](https://github.com/ponspack9)
   
- ![](https://raw.githubusercontent.com/Scarzard/StormHowlers/Wiki/Wiki%20Images/Home%20Page/G.I.png) **Code:** David Tello
   - [David's GitHub Link](https://github.com/DavidTello1)
   
- ![](https://raw.githubusercontent.com/Scarzard/StormHowlers/Wiki/Wiki%20Images/Home%20Page/ENGNEER.png) **Art/Audio:** Joan Barduena
   - [Joan's GitHub Link](https://github.com/JoanBarduena)
   
- ![](https://raw.githubusercontent.com/Scarzard/StormHowlers/Wiki/Wiki%20Images/Home%20Page/SABOTAGER.png) **Design:** Arnau Gallofré
   - [Arnau's GitHub Link](https://github.com/ggaka6god)
   
- ![](https://raw.githubusercontent.com/Scarzard/StormHowlers/Wiki/Wiki%20Images/Home%20Page/ROCKETEER.png) **QA:** Víctor Chen
   - [Victor's GitHub Link](https://github.com/Scarzard)
   
- ![](https://raw.githubusercontent.com/Scarzard/StormHowlers/Wiki/Wiki%20Images/Home%20Page/MEDIC.png) **UI:** Josep Lleal
   - [Josep's GitHub Link](https://github.com/JosepLleal)

## How to play

As stated in the game info, each player will have one controller where they can control the UI from his faction.
The controls of the controllers as well as debugging functions with keyboard/mouse can be found in the next section below.

- Allies(Player1): Blue. Base located in the left side of the screen. UI located on the bottom-right corner

- Soviets(Player2): Red. Base located in the left side of the screen. UI located on the top-left corner

The main objective of the game is **to destroy the opponents' TownHall.**

In order to achieve this, the player must manage resources via gold mines, create troops via barracks and use the defensive buildings(AoE and single-target) 
to defend all the other buildings. And more importantly, the main base which can't be built once destroyed nor repaired under any circumstance.

Every button or window in the main UI has a decription about what it does and the stats for the troop/building to be build or deployed.

### Buildings and troops
#### Buildings

- Non-buildable: TownHall, CommandCenter, Tesla
- Buildable: AoE Tower, Single-target tower, mines, barracks.

#### Troops 

Soldier, WarHound, Engineer, Spy, Tankman

## Game controls
### Controller
#### Main menu/ingame menu
- Dpad/Left Joystick: Cycle through options
- A: Use current button
- B: Go back to previous window
- Dpad right(on a slider): Increase value
- Dpad left(on a slider): Decrease value

#### In-game
- X: Select buildings
- RB/Right/Joystick->: Go to button/building in the right side
- LB/Left/<-Joystick: Go to button/building in the left side
- **Note**: To cycle between buildings, only with RB/LB
- A: Use current button
- A(with building selected): Place it in the current location
- A(with a building selected with X): Show building options
- B: Go back to previous window/options
- Dpad(with troop/building selected): Slow movement through the map
- Left Joystick(with troop/building selected): Quick movement through the map
- Start: Trigger pause menu

For Debug functionalities check the Games Readme [here](https://github.com/stormhowlers/Command_and_Conquer_WarZone/blob/master/README.md)
   
## Contact us!
- [StormHowlers Twitter](https://twitter.com/StormHowlers)
- StormHowlersDevelopers@gmail.com

#### For more information about the game, head to the link below:

<https://github.com/stormhowlers/Command_and_Conquer_WarZone/wiki>

## Github

As stated on the license, anyone can look at or modify the code of this project. 
It can be found here: https://github.com/stormhowlers/Command_and_Conquer_WarZone

## Disclosure 

```
We do not own any of the art such as animations, tilesets or music/SFX.

Everything belongs to the creators of the original game: Westwood Studios

However, there are some art pieces that have been made or edited by the team. 
```

## License

Licensed under the [MIT License](https://github.com/stormhowlers/Command_and_Conquer_WarZone/blob/master/LICENSE.md)

https://github.com/stormhowlers/Command_and_Conquer_WarZone/blob/master/LICENSE.md



### Code Features
Some special features included in the code are:
```markdown
- Entity IA: each entity manages his own depending his state and needs.

- Animations Loaded from Tiled: loaded animations from xml document save from Tiled.

- Construction system: collissions and previews to build inside your zone.

- Entity Update Area: enemies update depending on the Zone they are, and the mode the player selects.

- Tiled: Animations, collisions, entities, and areas loaded from tiled.

- UI with gamepads: UI designed for gamepads.

- Entity managment: avoid destroying and creating entities by active or inactive system.

- Video managment: start the game with the teams intro.

```
  
### Installation

Decompress the .zip file in any directory. Open the file directoy and double click on the executable.
Download [Here](https://github.com/stormhowlers/Command_and_Conquer_WarZone/releases/tag/v0.9) 
    
or dowload the desired release here and follow the same steps:
https://github.com/stormhowlers/Command_and_Conquer_WarZone/releases
       
## Game Video

<html>
<body>

<iframe width="590" height="520" src="" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

</body>
</html>


## Tasks

### Andres


### Josep


### Joan


### Victor


### David


### Arnau


### Oscar








