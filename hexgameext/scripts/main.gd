extends Node2D

var bd
var next = Board.BLUE
var game_over = false
var move_hist : PackedVector3Array = []		# 着手履歴 for Undo、要素：(x, y, col)

func _ready() -> void:
	var cbd = CBoard.new()
	cbd.set_width(3)
	#print("board width = ", bd.get_width())
	#print("n_empty = ", bd.get_n_empty())
	#bd.put_color(1, 1, 2)
	#print("(1, 1) = ", bd.get_color(1, 1))
	#print("n_empty = ", bd.get_n_empty())
	bd = Board.new()
	$BoardRect.bd = bd
	init_board()
func init_board():
	game_over = false
	next = Board.BLUE
	move_hist.clear()
	bd.init()
	$BoardRect.queue_redraw()

	pass
