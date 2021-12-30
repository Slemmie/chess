# s = "abc"

# val = s[2].upper()

# s = s[:-1]
# s += val

# print(s)

# exit(0)

import chess

lines = []

with open("games_san") as f:
	lines = f.readlines()

# print(lines)

moves = []

for line in lines:
	moves.append(line.split())

# print(moves)

result = []

for seq in moves:
	board = chess.Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")
	result.append([])
	for move in seq:
		if move == "1-0" or move == "0-1" or move == "1/2-1/2":
			result[-1].append(move)
			break
		result[-1].append(str(board.parse_san(move)))
		board.push_san(move)

f = open("games_coord", "w")
for l in result:
	cur = ""
	move = 1
	for s in l:
		t = s
		if move == 1 and len(t) == 5:
			cha = t[4].upper()
			t = t[:-1]
			t += cha
		cur += t
		cur += " "
		move ^= 1
	cur = cur[:-1]
	cur += "\n"
	f.write(cur)
f.close()
