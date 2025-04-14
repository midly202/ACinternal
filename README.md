# AssaultCube Internal Cheat

An internal cheat for AssaultCube, designed for educational and game-hacking practice purposes. 

Thread-based modular system for each feature toggle
Console UI shows current status of all toggles (ON/OFF)

---

## 🔧 Features

A simple in-game menu toggleable via hotkeys:

| Keybind | Feature           | Description                                                                 |
|--------:|-------------------|-----------------------------------------------------------------------------|
|   [1]   | Infinite Ammo      | Never run out of bullets — removes ammo decrement logic.                    |
|   [2]   | NoClip             | Move through walls and obstacles freely.                                   |
|   [3]   | Godmode            | Become invulnerable to all damage.                                         |
|   [4]   | Rapid Fire         | Removes fire rate limitation for fast shooting.                            |
|   [5]   | No Recoil          | Eliminates camera shake and aim distortion while firing.                   |
|   [6]   | No Kickback        | Disables weapon knockback animation.                                       |
|   [7]   | Invisible          | Hide from enemies (WIP: disables enemy awareness).                         |
|   [8]   | Aimbot             | Automatically aim at the nearest enemy.                                    |
|   [9]   | ESP                | Work-in-progress: draws boxes around enemies and shows health, distance.   |

---

## 💡 How it works

- Injected via a custom DLL injector
- Uses pointer dereferencing and memory patching to modify in-game behavior

