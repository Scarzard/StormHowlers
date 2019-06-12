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

Every button or window in the main UI has a description about what it does and the stats for the troop/building to be build or deployed.

  
### Installation

Decompress the .zip file in any directory. Open the file directoy and double click on the executable.
Download [here](https://github.com/stormhowlers/Command_and_Conquer_WarZone/releases/download/v1.0/Command.Conquer_WarZone_v1.0.Gold.zip) 

or download the desired release here and follow the same steps:
<https://github.com/stormhowlers/Command_and_Conquer_WarZone/releases>
       

### Buildings and troops implemented
#### Buildings

- Non-buildable: TownHall, CommandCenter, Tesla
- Buildable: AoE Tower, Single-target tower, mines, barracks.

#### Troops 

Soldier, WarHound, Engineer

## Game controls
### Controller
#### Main menu/ingame menu
- Dpad: Cycle through options.
- A: Use current button.
- B: Go back to previous window.
- Dpad right(on a slider): Increase value.
- Dpad left(on a slider): Decrease value.

#### In-game
- RB/Right/Joystick->: Go to button in the right side.
- LB/Left/<-Joystick: Go to button in the left side.
- A: Use current button.
- A(with building selected): Place it in the current location.
- B: Go back to previous window/options.
- Dpad(with building selected): Slow movement through the map.
- Left Joystick(with building selected): Quick movement through the map.
- Right Joystick: Change building type.
- X (in troop menu): Change all troops states.
- Y (in troop menu): Change the state for the selected troop.
- A (in troop menu): Spawn selected troop.
- RT: Shortcut to change all troop states. (Anywhere)
- Start: Trigger pause menu

## Debug functionalities
### Controller ingame
- Dpad UP: -1000 HP to your own TownHall.
- Dpad DOWN: -1000 HP to your own Command Center.
### Keyboard
#### Main menu/ingame menu
Space: Transition from main menu to the game

#### In-game
This functionalities can be used anytime
- F7: Map Debug
- F8: UI Debug
- F10: Toggle Godmode. 

**Godmode must be on to access debug functionalities below with F10**

- F1: Set timer to 10min

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

- 8: Change troop states for player 1
- 9: Change troop states for player 2

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

## Version changelog

### **v0.05**
#### Implementations
- Simple UI for both players
- Map basic design
- Controller support

### **v0.1**
#### New Implementations
- Timer
- Update walkability map when a building is created

### **v0.2**
#### New Implementations
- Fully functional resource system
- Fully functional building system
- Fully functional UI system

### **v0.3**
#### New Implementations
- Semi-functional troop unit (only solider, for now)
- Map design
- Art and FX implemented

### **v0.4**
#### New Implementations
- An improved version of 0.3. 
- Fully functional troop unit
- Map design
- Art and FX implemented

### **v0.5 (Vertical Slice)**
#### New implementations
- Fully functional resource system
- Fully functional building system
- Fully functional UI system
- Fully functional troop unit
- Map design
- Art and FX implemented
- Improved UI

### **v0.6**
#### New implementations
- Reworked troops AI
- Major improvements in the UI

### **v0.7**
#### New implementations
- Auto deploy for troops
- More improvements on troop AI
- More work on UI manageability
- Audio feedback on what's happening

### **v0.8 (Alpha)**
#### New implementations
- Improved auto deploy for all troops
- Major improvements on UI
- Overhaul of troop logic
- New states for troops
- New troops!
- Balancing
- New sprites for different troops, building and UI
- UI windows to show information about current button/window
- Showing tower ranges while having the building selected

### **v0.9**
#### New implementations
- Keybindings
- Intro video of the team's logo
- Smooth transitions between scenes
- More work on UI features
- All troops implemented!

### **v1.0 (Gold)**
#### New implementations
- Option to remap controller buttons (ingame pause menu)
- Troop state shortcut
- Troops invincibility sprite
- Tankman and inflitrator sprite implemented
- Balancing!

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

