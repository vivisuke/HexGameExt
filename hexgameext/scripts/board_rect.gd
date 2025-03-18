extends ColorRect

#const WHITE_COL = Color("pink")
const WHITE_COL = Color.WHITE
const BLACK_COL = Color.BLACK
#const WHITE_COL = Color("#f00000")		# 赤系
#const BLACK_COL = Color("#6060ff")		# 青系
var N_HORZ = 3
var N_VERT = 3
var CELL_WD = self.size.x / (Board.N_HORZ+1)
var X0 = CELL_WD
var Y0 = CELL_WD
var GRID_WD = CELL_WD * (Board.N_HORZ-1)
var GRID_HT = CELL_WD * (Board.N_HORZ-1)

var bd			# C++ CBorad オブジェクト
var gbd
var put_pos = Vector2(-1, -1)
var view_path = false			# gbd.m_dist != 0 部分強調

# Called when the node enters the scene tree for the first time.
func _ready():
	#print("CELL_WD = ", CELL_WD)
	pass # Replace with function body.
func init():
	put_pos = Vector2(-1, -1)
	view_path = false			# gbd.m_dist != 0 部分強調
func set_width(wd):
	N_HORZ = wd
	N_VERT = wd
	CELL_WD = self.size.x / (N_HORZ+1)
	X0 = CELL_WD + 15
	Y0 = CELL_WD + 15
	GRID_WD = CELL_WD * (N_HORZ-1)
	GRID_HT = CELL_WD * (N_HORZ-1)
	queue_redraw()
func xyToPos(x, y):
	return Vector2(X0+CELL_WD*x, Y0+CELL_WD*y+CELL_WD*x/2.0)
func vec2ToPos(vec2):
	return xyToPos(vec2.x, vec2.y)
func posToXY(pos : Vector2):
	var x = pos.x - X0
	var y = pos.y - Y0 - x/2.0
	x = floor((x + CELL_WD/2.0)/CELL_WD)
	y = floor((y + CELL_WD/2.0)/CELL_WD)
	return Vector2(x, y)
# 目盛り値ラベル設置
func add_axis_label(pos, txt):
	var lbl = Label.new()
	lbl.add_theme_color_override("font_color", Color.BLACK)
	lbl.add_theme_font_size_override("font_size", 24)
	lbl.text = txt
	lbl.position = pos + Vector2(-CELL_WD/10.0, 0)
	add_child(lbl)
	return lbl
func draw_stone(x, y, b):
	#var col = Color("#6060ff") if b else Color("pink")
	var col = BLACK_COL if b else WHITE_COL
	draw_circle(xyToPos(x, y), CELL_WD*0.4, col)
	draw_circle(xyToPos(x, y), CELL_WD*0.4, Color.BLACK, false, 1.0, true)	# 枠線
func _draw():
	for i in range(N_HORZ):
		add_axis_label(xyToPos(i, -1.25)+Vector2(0, CELL_WD*(11-N_HORZ)/16), "%c"%(0x61+i))		# 上辺
		add_axis_label(xyToPos(-1.15, i)+Vector2(CELL_WD*(11-N_HORZ)/16, CELL_WD/2), "%2d"%(i+1))		# 左辺
	#draw_rect(Rect2(X0, Y0, GRID_WD, GRID_HT), Color.BLACK, false, 3.0)
	var BWD = N_HORZ
	for i in range(1, BWD-1):
		var t = Y0 + i * CELL_WD
		draw_line(xyToPos(0, i), xyToPos(N_HORZ-1, i), Color.BLACK)
		draw_line(xyToPos(i, 0), xyToPos(i, N_VERT-1), Color.BLACK)
		draw_line(xyToPos(0, i), xyToPos(i, 0), Color.BLACK)
		draw_line(xyToPos(i, N_VERT-1), xyToPos(N_HORZ-1, i), Color.BLACK)
		#draw_line(Vector2(X0, t), Vector2(X0+GRID_WD, t), Color.BLACK)
		#draw_line(Vector2(t, Y0), Vector2(t, Y0+GRID_HT), Color.BLACK)
		#draw_line(Vector2(X0, t), Vector2(t, Y0), Color.BLACK)
		#draw_line(Vector2(t, Y0+GRID_HT), Vector2(X0+GRID_WD, t), Color.BLACK)
	draw_line(xyToPos(0, N_VERT-1), xyToPos(N_HORZ-1, 0), Color.BLACK)		# 主対角線
	#draw_line(Vector2(X0, Y0+GRID_HT), Vector2(X0+GRID_WD, Y0), Color.BLACK)		# 主対角線
	const FR_WD = 10.0
	draw_line(xyToPos(0, 0), xyToPos(N_HORZ-1, 0), BLACK_COL, FR_WD)
	draw_line(xyToPos(0, N_VERT-1), xyToPos(N_HORZ-1, N_VERT-1), BLACK_COL, FR_WD)
	#draw_line(xyToPos(0, 0), xyToPos(0, N_VERT-1), WHITE_COL, FR_WD)
	#draw_line(xyToPos(N_HORZ-1, 0), xyToPos(N_HORZ-1, N_VERT-1), WHITE_COL, FR_WD)
	#draw_rect(Rect2())
	draw_line(xyToPos(0, 0), xyToPos(0, N_VERT-1), BLACK_COL, 1)
	draw_line(xyToPos(N_HORZ-1, 0), xyToPos(N_HORZ-1, N_VERT-1), BLACK_COL, 1)
	#draw_stone(1, 0, true)
	#draw_stone(1, 1, true)
	#draw_stone(2, 0, false)
	#draw_stone(0, 2, false)
	# 石描画
	for y in range(BWD):
		for x in range(BWD):
			var col = bd.get_color(x, y)
			if col != Board.EMPTY:
				draw_stone(x, y, col == Board.BLUE)
	# 着手箇所強調
	const R = 6
	if put_pos.x >= 0:
		var p = vec2ToPos(put_pos)
		#draw_rect(Rect2(p-Vector2(R, R), Vector2(2*R, 2*R)), Color.GREEN)
		#var col = Color.BLACK if bd.get_color(put_pos.x, put_pos.y) == 2 else Color.WHITE
		var col = Color.DARK_GREEN if bd.get_color(put_pos.x, put_pos.y) == 2 else Color.GREEN
		draw_circle(p, CELL_WD*0.15, col, false, 2.0, true)	# 枠線
	# 連結パス強調
	if view_path:
		for y in range(BWD):
			for x in range(BWD):
				var ix = bd.xyToIndex(x, y)
				if bd.get_path(x, y) != 0:
					var p = xyToPos(x, y)
					#draw_circle(p, CELL_WD*0.1, Color.WHITE)
					#draw_rect(Rect2(p-Vector2(R, R), Vector2(2*R, 2*R)), Color.GREEN)
					var col = Color.BLACK if bd.get_color(x, y) == 2 else Color.WHITE
					draw_circle(p, CELL_WD*0.15, col, false, 2.0, true)	# 枠線
	pass

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
