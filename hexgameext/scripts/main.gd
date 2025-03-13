extends Node2D

enum {
	EMPTY = 0, BLACK, WHITE, BWALL, WWALL
}

var BD_WIDTH = 4
var ARY_WIDTH = BD_WIDTH + 1
var bd
var gbd
var next = BLACK
var game_over = false
var move_hist : PackedVector3Array = []		# 着手履歴 for Undo、要素：(x, y, col)

func _ready() -> void:
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
	bd.sel_move_PMC(BLACK);
	#
	if false:
		var ix
		while true:
			ix = bd.sel_move_random()
			if( bd.put_ix_color(ix, next) ): break;
			next = (BLACK + WHITE) - next;
		game_over = true
		if next == BLACK:
			$MessLabel.text = "黒の勝ちです"
		else:
			$MessLabel.text = "白の勝ちです"
		var x = ix % ARY_WIDTH
		var y = ix / ARY_WIDTH - 1
		print("x, y = %d, %d, next = %d"%[x, y, next])
		bd.BFS(x, y)	# 幅優先探索：pos からの距離計算
		bd.get_shortest_path(next)
		print_dist()
		print_path()
		$BoardRect.view_path = true
	$BoardRect.queue_redraw()
func init_board():
	game_over = false
	next = BLACK
	move_hist.clear()
	bd.init()
	$BoardRect.queue_redraw()
func print_next():
	if next == BLACK:
		$MessLabel.text = "黒の手番です。"
	else:
		$MessLabel.text = "白の手番です。"
func print_board():
	for y in range(BD_WIDTH):
		var txt = ""
		for x in range(BD_WIDTH):
			txt += "%4d"%bd.get_color(x, y)
		print(txt)
func print_dist():
	print("dist[]:")
	for y in range(BD_WIDTH):
		var txt = "  "
		for x in range(BD_WIDTH):
			txt += "%4d"%bd.get_dist(x, y)
		print(txt)
func print_path():
	print("path[]:")
	for y in range(BD_WIDTH):
		var txt = "  "
		for x in range(BD_WIDTH):
			txt += "%4d"%bd.get_path(x, y)
		print(txt)
func _input(event: InputEvent) -> void:
	if event is InputEventMouseButton && event.is_pressed():
		if game_over: return
		var pos = get_global_mouse_position() - $BoardRect.position
		#print("pos = ", pos)
		var xy = $BoardRect.posToXY(pos)
		#print("xy = ", xy)
		if xy.x >= 0 && xy.x < BD_WIDTH&& xy.y >= 0 && xy.y < BD_WIDTH:
			if bd.get_color(xy.x, xy.y) == EMPTY:
				do_put(xy)
		pass
func do_put(xy: Vector2):
	$BoardRect.put_pos = xy
	if !bd.put_color(xy.x, xy.y, next):		# 終局でない場合
		next = (BLACK + WHITE) - next;
		print_next()
	else:	# 終局した場合
		game_over = true
		if next == BLACK:
			$MessLabel.text = "黒の勝ちです。"
		else:
			$MessLabel.text = "白の勝ちです。"
		print("x, y = %d, %d, next = %d"%[xy.x, xy.y, next])
		bd.BFS(xy.x, xy.y)	# 幅優先探索：pos からの距離計算
		bd.get_shortest_path(next)
		$BoardRect.view_path = true
	$BoardRect.queue_redraw()


func _on_next_button_pressed() -> void:
	if game_over: return
	var ix
	if next == BLACK:
		ix = bd.sel_move_random()
		#pos = bd.sel_move_PMC(next)
		#pos = bd.sel_move_MCTS(next)
	else:
		ix = bd.sel_move_random()
		#pos = bd.sel_move_PMC(next)
		#pos = bd.sel_move_MCTS(next)
	var x = ix % ARY_WIDTH
	var y = ix / ARY_WIDTH - 1
	var pos : Vector2 = Vector2(x, y)
	print("put pos = ", pos)
	do_put(pos)
	pass # Replace with function body.
