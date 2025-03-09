extends Node2D

enum {
	EMPTY = 0, BLUE, RED, BWALL, WWALL
}

var BD_WIDTH = 3
var bd
var gbd
var next = Board.BLUE
var game_over = false
var move_hist : PackedVector3Array = []		# 着手履歴 for Undo、要素：(x, y, col)

func _ready() -> void:
	#gbd = Board.new()
	#$BoardRect.gbd = gbd
	bd = CBoard.new()
	bd.set_width(BD_WIDTH)
	$BoardRect.bd = bd
	$BoardRect.set_width(BD_WIDTH)
	#var f : bool = bd.put_color(0, 0, 1)
	#f = bd.put_color(0, 1, 1)
	#f = bd.put_color(0, 2, 1)
	init_board()
	print_board()
func init_board():
	game_over = false
	next = Board.BLUE
	move_hist.clear()
	bd.init()
	$BoardRect.queue_redraw()
func print_board():
	for y in range(BD_WIDTH):
		var txt = ""
		for x in range(BD_WIDTH):
			txt += "%4d"%bd.get_color(x, y)
		print(txt)
	pass
