# StormHowlers

## Objectives

We are an indie game development team formed by 7 students taking the bachelor's degree in Videogame design and development in the UPC.
The game will be developed for the subject Project II. We will be making a tribute to the well-known RTS game: Command&Conquer: Red Alert 2.

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

## How to play

As stated in the game info, each player will have one controller where they can control the UI from his faction.
The controls of the controllers as well as debugging functions with keyboard/mouse can be found in the next section below.

- Allies(Player1): Blue. Base located in the left side of the screen. UI located on the bottom-right corner

- Soviets(Player2): Red. Base located in the left side of the screen. UI located on the top-left corner

The main objective of the game is **to destroy the opponents' TownHall.**

In order to achieve this, the player must manage resources via gold mines, create troops via barracks and use the defensive buildings(AoE and single-target) 
to defend all the other buildings. And more importantly, the main base which can't be built once destroyed nor repaired under any circumstance.

Every button or window in the main UI has a decription about what it does and the stats for the troop/building to be build or deployed.

### Buildings and troops implemented
#### Buildings

- Non-buildable: TownHall, CommandCenter, Tesla
- Buildable: AoE Tower, Single-target tower, mines, barracks.

#### Troops 

Soldier, WarHound, Engineer

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

## Debug functionalities
### Keyboard
#### Main menu/ingame menu
N/A

#### In-game
This functionalities can be used anytime
- F7: Map Debug
- F8: UI Debug
- F10: Toggle Godmode. 

**Godmode must be on to access debug functionalities below**

- F1: Set timer to 10min

- 1: Spawn soldier from player 1 in the location of the mouse
- Q: Spawn soldier from player 2 in the location of the mouse

- 2: Add main defense from player 1 in the location of the mouse
- W: Add main defense from player 2 in the location of the mouse

- 3: Add command center from player 1 in the location of the mouse
- E: Add command center from player 2 in the location of the mouse

- 4: Add barrack from player 1 in the location of the mouse
- R: Add bararck from player 2 in the location of the mouse

- 5: Add AoE defense (tesla) from player 1 in the location of the mouse
- T: Add AoE defense (tesla) from player 2 in the location of the mouse

- 6: Add single-target defense (sentry gun) from player 1 in the location of the mouse
- Y: Add single-target defense (sentry gun) from player 2 in the location of the mouse

- 7: Add mines from player 1 in the location of the mouse
- U: Add mines from player 2 in the location of the mouse

- 0: Add walls from player 1 in the location of the mouse
- P: Add walls from player 2 in the location of the mouse

- X: Spawn warhound from player 1 in the location of the mouse

- Arrow keys: Move camera

- I: Increase zoom
- K: Decrease zoom

- D: Toggle timer

### Mouse
- Middle click(wheel, while clicked): Drag map
- Left click: Set goal for soldiers, begin pathfinding.

## The team
   
- **Lead:** Andrés Saladrigas
   - [Andrés' GitHub Link](https://github.com/TheArzhel)
   
- **Management:** Òscar Pons
   - [Òscar's GitHub Link](https://github.com/ponspack9)
   
- **Code:** David Tello
   - [David's GitHub Link](https://github.com/DavidTello1)
   
- **Art/Audio:** Joan Barduena
   - [Joan's GitHub Link](https://github.com/JoanBarduena)
   
- **Design:** Arnau Gallofré
   - [Arnau's GitHub Link](https://github.com/ggaka6god)
   
- **QA:** Víctor Chen
   - [Victor's GitHub Link](https://github.com/Scarzard)
   
- **UI:** Josep Lleal
   - [Josep's GitHub Link](https://github.com/JosepLleal)
   
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

Copyright 2019 StormHowlers

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

