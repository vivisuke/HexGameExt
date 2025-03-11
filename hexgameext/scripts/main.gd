extends Node2D

enum {
	EMPTY = 0, BLACK, WHITE, BWALL, WWALL
}

var BD_WIDTH = 3
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
	print_next()
	#
	if false:
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
func print_next():
	if next == Board.BLUE:
		$MessLabel.text = "青の手番です。"
	else:
		$MessLabel.text = "赤の手番です。"
func print_board():
	for y in range(BD_WIDTH):
		var txt = ""
		for x in range(BD_WIDTH):
			txt += "%4d"%bd.get_color(x, y)
		print(txt)
	pass
func _input(event: InputEvent) -> void:
	if event is InputEventMouseButton && event.is_pressed():
		var pos = get_global_mouse_position() - $BoardRect.position
		print("pos = ", pos)
		var xy = $BoardRect.posToXY(pos)
		print("xy = ", xy)
		if xy.x >= 0 && xy.x < BD_WIDTH&& xy.y >= 0 && xy.y < BD_WIDTH:
			if bd.get_color(xy.x, xy.y) == EMPTY:
				do_put(xy)
		pass
func do_put(xy: Vector2):
	bd.put_color(xy.x, xy.y, next)
	$BoardRect.put_pos = xy
	$BoardRect.queue_redraw()
	next = (BLACK + WHITE) - next;
	print_next()
