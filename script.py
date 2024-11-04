# Parse the script,
# conducting all story and level events.

def events_batcher(game):
    """ Dispatches events to the game, as needed,
    whenever the returned generator is iterated.
    """
    events = iterate_events(game)
    while True:
        while game.ready_for_events():
            next(events)
        yield None

def script_reader(jump=None):
    """ Interator for each line in the script.
    @param jump: Jump to label in the script, if given. Include prefix and newline.
    """
    with open("script.txt") as fp:
        for line in fp:
            if jump:
                if line != jump:
                    continue
                jump = None
            if line[0] == "#" or line[0] == "\n": # Ignore comments and blank lines.
                continue
            yield line
feed = script_reader()

def iterate_events(game):
    """ Dispatches an event to the game,
    whenever the returend generator is iterated.
    """
    global feed
    empty = bytearray("\x00", "utf8")
    while True:
        line = next(feed)
        t = line[0]
        line, _, _ = line.partition("#") # Strip comments.

        # 1,2,3=CHECKPOINT, 15=SPAWNER
        if t == "@" or t == "{":
            i1, _, i2= line[1:].partition(",")
            type = 1 if t == "@" else 15
            # 1=CHECKPOINT(POSITIONAL), 2=CHECKPOINT(LATERAL), 3=CHECKPOINT(DEPTH)
            if type == 1:
                if not i1.strip():
                    type = 3
                    i1 = 0
                elif not i2.strip():
                    type = 2
                    i2 = 0
            try:
                i1 = int(i1)
                i2 = int(i2)
            except ValueError:
                raise ValueError("Script Parse (%s)", line)
            game.send_event(type, i1, i2, 0, 0, empty)

        # 4,5=BACKGROUND, 6,7=FOREGROUND, 8,9=TILE
        elif t == "_" or t == "'" or t == "*":
            pattern, _, l = line[2:].rpartition(":")
            offsetX, _, l = l.rpartition(",")
            offsetY, _, distance = l.rpartition("@")
            try:
                pattern = int(pattern)
                offsetX = int(offsetX)
                offsetY = int(offsetY)
                distance = int(distance)
            except ValueError:
                raise ValueError("Script Parse (%s)", line)
            # 4,6,8=(VERTICAL_CHANGE)
            # 5,7,9=(LATERAL_CHANGE)
            type = 4 if t == "_" else (6 if t == "'" else 9)
            if line[1] == "|":
                type += 1
            game.send_event(type, pattern, offsetX, offsetY, distance, empty)

        # 10=DIALOG, 11=CHOICE
        elif t == '"' or t == "?":
            i, _, txt = line[1:].partition(",")
            try:
                i = int(i)
            except ValueError:
                raise ValueError("Script Parse (%s)", line)
            if txt[-1] != "\n":
                txt += "\x00"
            else:
                txt[-1] = "\x00"
            if len(txt) > 256:
                raise ValueError("Script Parse (%s)", line)
            type = 10 if t == '"' else 11
            game.send_event(type, i, 0, 0, 0, txt)

        # 12=SECRET
        elif t == "&":
            x, _, l = line[1:].partition(",")
            y, _, j = l.partition("?")
            try:
                x = int(x)
                y = int(y)
                j = int(j)
            except ValueError:
                raise ValueError("Script Parse (%s)", line)
            game.send_event(12, x, y, j, 0, empty)

        # JUMP POINT
        elif t == ">":
            continue

        # GOTO
        elif t == "<":
            feed = script_reader(">"+line[1:])

        # SAVE POINT
        elif t == "+":
            print(line,) # XXX TODO Save game.

        # 13=BOSS, 14=SPAWN
        elif t == "!":
            x, _, l = line[1:].partition(",")
            y, _, n = l.partition("?")
            try:
                x = int(x)
                y = int(y)
                n = int(n)
            except ValueError:
                raise ValueError("Script Parse (%s)", line)
            type = 13 if t == "!" else 14
            game.send_event(type, x, y, n, 0, empty)

        else:
            raise ValueError("Script Parse (%s)", line)
        yield None