extends Node2D

enum {
	EMPTY = 0, BLUE, RED, BWALL, WWALL
}

var bd
var next = Board.BLUE
var game_over = false
var move_hist : PackedVector3Array = []		# 着手履歴 for Undo、要素：(x, y, col)

func _ready() -> void:
	bd = Board.new()
	$BoardRect.bd = bd
	var bd2 = CBoard.new()
	bd2.set_width(3)
	var f : bool = bd2.put_color(0, 0, 1)
	f = bd2.put_color(0, 1, 1)
	f = bd2.put_color(0, 2, 1)
	
	#print("board width = ", bd.get_width())
	#print("n_empty = ", bd.get_n_empty())
	#bd.put_color(1, 1, 2)
	#print("(1, 1) = ", bd.get_color(1, 1))
	#print("n_empty = ", bd.get_n_empty())
	init_board()
func init_board():
	game_over = false
	next = Board.BLUE
	move_hist.clear()
	bd.init()
	$BoardRect.queue_redraw()

	pass
