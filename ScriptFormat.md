
# Guide for script.txt

The script.txt has a custom format which describes the levels and story of the game.

Used together, these directives support dialog, narration, optional side quests, secrets,
quest items, monsters, bosses, story arcs, and evolving level designs.

Comments (#) are supported at the start of a line or at the end of a directive.

I call this format UmbyScript.

## CHECKPOINT

* Arrows will point the player in this direction, if there is no boss loaded.
* Checkpoints will not activate while a boss is still loaded.
* When a checkpoint is activated, all further directives are loaded until the next checkpoint.
* X or Y can be omitted to indicate the checkpoint is a line to cross, rather than position.
* Positions of other directives are relative to the last checkpoint.
* It is recommended to SPAWN something to indicate the location of position checkpoints.
* Positional triggers are activated by the player being less than 8 pixels away in both directions.
* Constraints per checkpoint:
  * Up to 1 background pattern directive.
  * Up to 1 foreground pattern directive.
  * Up to 8 channels of tile pattern directives (superimposed).
  * Up to 1 boss directive.
  * Up to 1 secret directive (unloaded on next checkpoint).
  * Up to 32 spawn directives (unloaded on next checkpoint).
  * Up to 4 spawner directives (persistant FIFO).

```
@X,Y
```

## BACKGROUND, FOREGROUND, TILE

* Loads a pattern to the background, foreground, or tile-map layer.
* The pattern is defined relative to cordinates that can by offset by X,Y.
* Setting the distance to 0 will immetiately reload the layer.
* There are 8 tile layers, each with their own pattern (N=1-8)

Load a background change at a distance vertically away:
```
_/PATTERN:X,Y@DISTANCE
```

Load a background change at a distance laterally away:
```
_|PATTERN:X,Y@DISTANCE
```

Load a foreground change at a distance vertically away:
```
`/PATTERN:X,Y@DISTANCE
```

Load a foreground change at a distance laterally away:
```
`|PATTERN:X,Y@DISTANCE
```

Load a tile-map change on tile layer N at a distance vertically away:
```
*N/PATTERN:X,Y@DISTANCE
```

Load a tile-map change on tile layer N at a distance laterally away:
```
*N|PATTERN:X,Y@DISTANCE
```

## DIALOG

* Displays dialog and idicates the actor speaking.
* Pauses until the user continues.
* Dialog must not be over 255 bytes.

```
"ACTOR,DIALOG(255)####################################################################################################################
```

## CHOICE

* Asks the user a YES/NO question and pauses until there is a response.
* JUMPs to another position in the script if the response is afirmative.
* Question must not be over 255 bytes.

```
?JUMP,QUESTION(255)##################################################################################################################
```

## SECRET

* Loads a positional trigger that, if activated, will JUMP to another point in the script.
* Activated by the player being less than 8 pixels away in both directions.
* It is recommended to SPAWN something subtle to indicate the location of positional secrets.

```
&X,Y?JUMP
```

## JUMP POINT

* Labels a position in the script so other directives can make a JUMP here.
* JUMP POINT labels must be unique.
* When another directive JUMPs here, all further directives are loaded until the next checkpoint.
* Comments are forbidden at the end of JUMP directives.

```
>JUMP
```

## GOTO

* Go to the labelled JUMP point.

```
<JUMP
```

## SAVE POINT

* Labels a position in the script where a save occurs.
* SAVE POINT labels must be unique.
* SAVE POINT labels must not be more than 32 bytes.
* Comments are forbidden at the end of SAVE POINT directives.

```
+LABEL(32)#######################
```

## BOSS

* Spawn something at a location that acts as a boss.
* Arrows will point the player in the direction of the boss, if one is loaded.
* Checkpoints will not trigger while a boss is loaded.
* Bosses may not always be monsters, and despawn can be triggered by things other than death.

```
!X,Y,NPC
```

## SPAWN

* Spawn something at a location.
* There is a SPAWN buffer that is capped at 32.
* On a new checkpoint, previously loaded NPCs will despawn.
* Attempts to despawn onscreen NPCs, may instead move them to the DYNAMIC buffer.
* Bosses may not always be monsters, and despawn can be triggered by things other than death.

```
:X,Y,NPC
```

## DYNAMIC SPAWNER

* Loads a dynamic spawner that randomly spawns NPCs at a given rate.
* Loads as the player moves in the dominant direction towards the next checkpoint.
* Automatically despawns NPCs that fall off the screen buffer towards the last checkpoint.
* A rate of 0 never spawns anything, and a rate of 255 creates the most spawns.
* Spawns into a separate a DYNAMIC buffer that caps the concurrent number of dynamic spawns to 32.

```
{NPC,RATE(0-255)
```

