extends Node2D

enum {
	EMPTY = 0, BLACK, WHITE, BWALL, WWALL
}

var BD_WIDTH = 9
var bd
var gbd
var next = BLACK
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
	#
	while true:
		var ix = bd.sel_move_random()
		if( bd.put_ix_color(ix, next) ): break;
		next = (BLACK + WHITE) - next;
	if next == BLACK:
		$MessLabel.text = "黒の勝ちです"
	else:
		$MessLabel.text = "白の勝ちです"
	$BoardRect.queue_redraw()
func init_board():
	game_over = false
	next = BLACK
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
