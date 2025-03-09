extends Node2D

enum {
	EMPTY = 0, BLUE, RED, BWALL, WWALL
}

var BD_WIDTH = 11
var gbd
var next = Board.BLUE
var game_over = false
var move_hist : PackedVector3Array = []		# 着手履歴 for Undo、要素：(x, y, col)

func _ready() -> void:
	gbd = Board.new()
	$BoardRect.gbd = gbd
	var bd = CBoard.new()
	bd.set_width(BD_WIDTH)
	$BoardRect.bd = bd
	$BoardRect.set_width(BD_WIDTH)
	#var f : bool = bd.put_color(0, 0, 1)
	#f = bd.put_color(0, 1, 1)
	#f = bd.put_color(0, 2, 1)
	
	init_board()
func init_board():
	game_over = false
	next = Board.BLUE
	move_hist.clear()
	gbd.init()
	$BoardRect.queue_redraw()

	pass
