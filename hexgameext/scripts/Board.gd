class_name Board
extends Node

enum {
	#EMPTY = 0, BLACK, WHITE, BWALL, WWALL
	EMPTY = 0, BLUE, RED, BWALL, WWALL
}
const N_HORZ = 7
const ARY_WIDTH = N_HORZ + 1
const ARY_HEIGHT = N_HORZ + 2
const ARY_SIZE = ARY_WIDTH * ARY_HEIGHT
#const col_str = ".XO#"
const col_str = ". X O # "
#const col_str = "・●◯＃"

#var m_cells = []
var m_next_id = 1
var win_rate = 0.0
var m_cells : PackedByteArray
var m_gid : PackedByteArray	# 0 for 未探索 or 空欄、1以上 for 連結石群 id
var m_gid_ex : PackedByteArray	# 0 for 未探索 or 空欄、1以上 for 連結石群 id
var m_dist : PackedByteArray		# 0 for 未探索、1以上 for 距離+1
var m_path : PackedByteArray		# 1以上：最短パス
var m_ter_lst : PackedByteArray		# 末端位置リスト
var m_next_ter : PackedByteArray	# 次の末端位置リスト
var m_eval : PackedInt32Array		# 各位置の着手価値

var policy_text = ""

func xyToIndex(x, y): return (y+1)*ARY_WIDTH + x
func ixToX(ix): return ix%ARY_WIDTH

func _init():
	m_cells.resize(ARY_SIZE)
	m_gid.resize(ARY_SIZE)
	m_gid_ex.resize(ARY_SIZE)
	m_dist.resize(ARY_SIZE)
	m_path.resize(ARY_SIZE)
	m_eval.resize(ARY_SIZE)
	m_cells.fill(BWALL)			# 青壁 for 上下
	init()
func init():
	m_gid.fill(0)
	m_gid_ex.fill(0)
	for y in range(N_HORZ):
		m_cells[xyToIndex(-1, y)] = WWALL		# 赤壁 for 左右
		for x in range(N_HORZ):
			m_cells[xyToIndex(x, y)] = EMPTY
	if false:
		for y in range(1, N_HORZ-1, 2):
			for x in range(1, N_HORZ-1, 2):
				m_cells[xyToIndex(x, y)] = RED
				m_next_id += 1
	if false:
		var lst = [[1, 1], [3, 3]]
		for pos in lst:
			m_cells[xyToIndex(pos[0], pos[1])] = RED
			m_next_id += 1
	check_connected()
func copy_from(s):
	m_next_id = s.m_next_id
	m_cells = s.m_cells.duplicate()
	m_gid = s.m_gid.duplicate()			# グループid 配列

func print():
	for y in range(N_HORZ):
		var txt = ""
		for x in range(N_HORZ):
			txt += col_str.substr(m_cells[xyToIndex(x, y)]*2, 2)
			#txt += "%d"%m_cells[xyToIndex(x, y)]
			#print(m_cells[i])
		print(txt)
func print_gid():
	print("m_gid[] = ")
	for y in range(N_HORZ):
		var txt = ""
		for x in range(N_HORZ):
			txt += "%3d"%m_gid[xyToIndex(x, y)]
		print(txt)
func print_gid_ex():
	print("m_gid_ex[] = ")
	for y in range(N_HORZ):
		var txt = ""
		for x in range(N_HORZ):
			txt += "%3d"%m_gid_ex[xyToIndex(x, y)]
		print(txt)
func print_dist():
	print("m_dist[] = ")
	for y in range(N_HORZ):
		var txt = ""
		for x in range(N_HORZ):
			txt += "%3d"%m_dist[xyToIndex(x, y)]
		print(txt)
func print_eval():
	print("m_eval[] = ")
	for y in range(N_HORZ):
		var txt = ""
		for x in range(N_HORZ):
			txt += "%4d"%m_eval[xyToIndex(x, y)]
		print(txt)
func get_col(x, y):
	return m_cells[xyToIndex(x, y)]
func find_horz(id, y):
	for x in range(N_HORZ):
		if m_gid[xyToIndex(x, y)] == id:
			return true
	return false
func find_vert(id, x):
	for y in range(N_HORZ):
		if m_gid[xyToIndex(x, y)] == id:
			return true
	return false
func put_col(x, y, col) -> bool:	# col: BLUE or RED
	var ix = xyToIndex(x, y)
	m_cells[ix] = col
	#check_connected()
	update_gid(x, y)
	var id = m_gid[ix]
	if col == BLUE:
		return find_horz(id, 0) && find_horz(id, N_HORZ-1)	# 上辺・下辺に打った箇所と同じ gid の石があるか？
	else:
		return find_vert(id, 0) && find_vert(id, N_HORZ-1)
func put_black(x, y):
	m_cells[xyToIndex(x, y)] = BLUE
func put_white(x, y):
	m_cells[xyToIndex(x, y)] = RED
func sel_move_random() -> Vector2:
	var lst : PackedVector2Array
	for y in range(N_HORZ):
		for x in range(N_HORZ):
			if m_cells[xyToIndex(x, y)] == EMPTY: lst.push_back(Vector2(x, y))
	#print("lst.size = ", lst.size())
	if lst.size() != 0:
		return lst[randi() % lst.size()]
	else:
		return Vector2(-1, -1)
func sel_move_maxeval() -> Vector2:
	var lst : PackedVector2Array
	var mxev = -1
	for y in range(N_HORZ):
		for x in range(N_HORZ):
			var ix = xyToIndex(x, y)
			if m_eval[ix] > mxev:
				mxev = m_eval[ix]
				lst.clear()
				lst.push_back(Vector2(x, y))
			elif m_eval[ix] == mxev:
				lst.push_back(Vector2(x, y))
	if lst.size() != 0:
		return lst[randi() % lst.size()]
	else:
		return Vector2(-1, -1)
func update_gid_sub(ix, ix2):
	if m_cells[ix] != m_cells[ix2]:		# 同じ石色でない場合
		return
	if m_gid[ix] == 0:		# 最初の連結の場合
		m_gid[ix] = m_gid[ix2]
	elif m_gid[ix] != m_gid[ix2]:		# グループid のマージが必要
		var o = m_gid[ix2]
		var n = m_gid[ix]
		for i in range(xyToIndex(0, 0), xyToIndex(N_HORZ-1, N_HORZ-1)+1):
			if m_gid[i] == o:
				m_gid[i] = n
func update_gid(x, y):		# put_col() からコールされる
	var ix = xyToIndex(x, y)
	update_gid_sub(ix, ix-ARY_WIDTH)
	update_gid_sub(ix, ix-ARY_WIDTH+1)
	update_gid_sub(ix, ix-1)
	update_gid_sub(ix, ix+1)
	update_gid_sub(ix, ix+ARY_WIDTH-1)
	update_gid_sub(ix, ix+ARY_WIDTH)
	if m_gid[ix] == 0:
		m_gid[ix] = m_next_id
		m_next_id += 1
	pass
func check_connected_sub(ix, col, id):		# 深さ優先探索
	m_gid[ix] = id
	if m_cells[ix-ARY_WIDTH] == col && m_gid[ix-ARY_WIDTH] == 0:
		check_connected_sub(ix-ARY_WIDTH, col, id)
	if m_cells[ix-ARY_WIDTH+1] == col && m_gid[ix-ARY_WIDTH+1] == 0:
		check_connected_sub(ix-ARY_WIDTH+1, col, id)
	if m_cells[ix-1] == col && m_gid[ix-1] == 0:
		check_connected_sub(ix-1, col, id)
	if m_cells[ix+1] == col && m_gid[ix+1] == 0:
		check_connected_sub(ix+1, col, id)
	if m_cells[ix+ARY_WIDTH-1] == col && m_gid[ix+ARY_WIDTH-1] == 0:
		check_connected_sub(ix+ARY_WIDTH-1, col, id)
	if m_cells[ix+ARY_WIDTH] == col && m_gid[ix+ARY_WIDTH] == 0:
		check_connected_sub(ix+ARY_WIDTH, col, id)
func check_connected():
	m_gid.fill(0)
	var id = 0
	for y in range(N_HORZ):
		for x in range(N_HORZ):
			var ix = xyToIndex(x, y)
			var col = m_cells[ix]
			if (col == BLUE || col == RED) && m_gid[ix] == 0:
				id += 1
				check_connected_sub(ix, col, id)
func is_connected_to_right(ix):
	var x = ixToX(ix)
	if x == N_HORZ - 1:
		return true
	if x == N_HORZ - 2 && m_cells[ix-ARY_WIDTH+1] == EMPTY && m_cells[ix+1] == EMPTY:
		return true
	return false
func BFS_ex(ix) -> bool:
	if m_gid_ex[ix-ARY_WIDTH] == 0 && m_cells[ix-ARY_WIDTH] == RED:
		m_gid_ex[ix-ARY_WIDTH] = 1
		if is_connected_to_right(ix-ARY_WIDTH): return true
		if BFS_ex(ix-ARY_WIDTH): return true
	if m_gid_ex[ix-ARY_WIDTH+1] == 0 && m_cells[ix-ARY_WIDTH+1] == RED:
		m_gid_ex[ix-ARY_WIDTH+1] = 1
		if is_connected_to_right(ix-ARY_WIDTH+1): return true
		if BFS_ex(ix-ARY_WIDTH+1): return true
	if m_gid_ex[ix-1] == 0 && m_cells[ix-1] == RED:
		m_gid_ex[ix-1] = 1
		if is_connected_to_right(ix-1): return true
		if BFS_ex(ix-1): return true
	if m_gid_ex[ix+1] == 0 && m_cells[ix+1] == RED:
		m_gid_ex[ix+1] = 1
		if is_connected_to_right(ix+1): return true
		if BFS_ex(ix+1): return true
	if m_gid_ex[ix+ARY_WIDTH-1] == 0 && m_cells[ix+ARY_WIDTH-1] == RED:
		m_gid_ex[ix+ARY_WIDTH-1] = 1
		if is_connected_to_right(ix+ARY_WIDTH-1): return true
		if BFS_ex(ix+ARY_WIDTH-1): return true
	if m_gid_ex[ix+ARY_WIDTH] == 0 && m_cells[ix+ARY_WIDTH] == RED:
		m_gid_ex[ix+ARY_WIDTH] = 1
		if is_connected_to_right(ix+ARY_WIDTH): return true
		if BFS_ex(ix+ARY_WIDTH): return true
	#
	if (m_cells[ix-ARY_WIDTH] == EMPTY && m_cells[ix-ARY_WIDTH+1] == EMPTY &&
		m_gid_ex[ix-ARY_WIDTH*2+1] == 0 && m_cells[ix-ARY_WIDTH*2+1] == RED):
			m_gid_ex[ix-ARY_WIDTH*2+1] = 1
			if is_connected_to_right(ix-ARY_WIDTH*2+1): return true
			if BFS_ex(ix-ARY_WIDTH*2+1): return true
	return false
func check_h_connected_ex() -> bool:	# 弱連結考慮 左右辺連結判定
	m_gid_ex.fill(0)
	var lst = []
	for y in range(N_HORZ):
		var ix = xyToIndex(0, y)
		if m_cells[ix] == RED:
			m_gid_ex[ix] = 1
			lst.push_back(ix)
	for y in range(N_HORZ):
		var ix = xyToIndex(1, y)
		if m_cells[ix] == RED && m_cells[ix-1] == EMPTY && m_cells[ix-ARY_WIDTH-1] == EMPTY:
			m_gid_ex[ix] = 1
			lst.push_back(ix)
	for ix in lst:
		if BFS_ex(ix): return true
	return false
func BFS_sub(dist, col):
	#m_next_ter.clear()
	m_next_ter = PackedByteArray()
	for ix in m_ter_lst:
		if m_cells[ix-ARY_WIDTH] == col && m_dist[ix-ARY_WIDTH] == 0:
			m_dist[ix-ARY_WIDTH] = dist
			m_next_ter.push_back(ix-ARY_WIDTH)
		if m_cells[ix-ARY_WIDTH+1] == col && m_dist[ix-ARY_WIDTH+1] == 0:
			m_dist[ix-ARY_WIDTH+1] = dist
			m_next_ter.push_back(ix-ARY_WIDTH+1)
		if m_cells[ix-1] == col && m_dist[ix-1] == 0:
			m_dist[ix-1] = dist
			m_next_ter.push_back(ix-1)
		if m_cells[ix+1] == col && m_dist[ix+1] == 0:
			m_dist[ix+1] = dist
			m_next_ter.push_back(ix+1)
		if m_cells[ix+ARY_WIDTH-1] == col && m_dist[ix+ARY_WIDTH-1] == 0:
			m_dist[ix+ARY_WIDTH-1] = dist
			m_next_ter.push_back(ix+ARY_WIDTH-1)
		if m_cells[ix+ARY_WIDTH] == col && m_dist[ix+ARY_WIDTH] == 0:
			m_dist[ix+ARY_WIDTH] = dist
			m_next_ter.push_back(ix+ARY_WIDTH)
	if m_next_ter.is_empty(): return
	m_ter_lst = m_next_ter.duplicate()
	BFS_sub(dist+1, col)
func BFS(x, y):		# (x, y) から幅優先探索
	m_dist.fill(0)
	var ix = xyToIndex(x, y)
	m_dist[ix] = 1
	var col = m_cells[ix]
	m_ter_lst = [ix]
	BFS_sub(2, col)
	pass
func min_dist_y(x):
	var mnv = 9999
	var mny = -1
	for y in range(N_HORZ):
		var ix = xyToIndex(x, y)
		if m_dist[ix] != 0 && m_dist[ix] < mnv:
			mnv = m_dist[ix]
			mny = y
	return mny
func min_dist_x(y):
	var mnv = 9999
	var mnx = -1
	for x in range(N_HORZ):
		var ix = xyToIndex(x, y)
		if m_dist[ix] != 0 && m_dist[ix] < mnv:
			mnx = m_dist[ix]
			mnx = x
	return mnx
func get_shortest_path_sub(ix):
	while m_dist[ix] != 0:
		var dist = m_dist[ix]
		m_path[ix] = dist
		dist -= 1
		if m_dist[ix-ARY_WIDTH] == dist:
			ix = ix - ARY_WIDTH
		elif m_dist[ix-ARY_WIDTH+1] == dist:
			ix = ix - ARY_WIDTH + 1
		elif m_dist[ix-1] == dist:
			ix = ix - 1
		elif m_dist[ix+1] == dist:
			ix = ix + 1
		elif m_dist[ix+ARY_WIDTH-1] == dist:
			ix = ix + ARY_WIDTH - 1
		elif m_dist[ix+ARY_WIDTH] == dist:
			ix = ix + ARY_WIDTH
		else:
			return
func get_shortest_path(black):
	m_path.fill(0)
	if !black:
		var y = min_dist_y(0)
		get_shortest_path_sub(xyToIndex(0, y))
		y = min_dist_y(N_HORZ-1)
		get_shortest_path_sub(xyToIndex(N_HORZ-1, y))
	else:
		var x = min_dist_x(0)
		get_shortest_path_sub(xyToIndex(x, 0))
		x = min_dist_x(N_HORZ-1)
		get_shortest_path_sub(xyToIndex(x, N_HORZ-1))
func is_black_or_white(ix):
	return m_cells[ix] == BLUE || m_cells[ix] == RED
func eval_empty():
	m_eval.fill(0)
	for y in range(N_HORZ):
		for x in range(N_HORZ):
			var ix = xyToIndex(x, y)
			if m_cells[ix] == EMPTY:
				m_eval[ix] += 1
				if m_cells[ix-ARY_WIDTH] != EMPTY && m_cells[ix-ARY_WIDTH] == m_cells[ix+ARY_WIDTH]:
					m_eval[ix] += 100
				if m_cells[ix-ARY_WIDTH+1] != EMPTY && m_cells[ix-ARY_WIDTH+1] == m_cells[ix+ARY_WIDTH-1]:
					m_eval[ix] += 100
				if m_cells[ix-1] != EMPTY && m_cells[ix-1] == m_cells[ix+1]:
					m_eval[ix] += 100
				#
				if is_black_or_white(ix-ARY_WIDTH) && m_cells[ix+ARY_WIDTH] == EMPTY:
					m_eval[ix] += 10
				elif is_black_or_white(ix-ARY_WIDTH+1) && m_cells[ix+ARY_WIDTH-1] == EMPTY:
					m_eval[ix] += 10
				elif is_black_or_white(ix-1) && m_cells[ix+1] == EMPTY:
					m_eval[ix] += 10
				elif is_black_or_white(ix+1) && m_cells[ix-1] == EMPTY:
					m_eval[ix] += 10
				elif is_black_or_white(ix+ARY_WIDTH-1) && m_cells[ix-ARY_WIDTH+1] == EMPTY:
					m_eval[ix] += 10
				elif is_black_or_white(ix+ARY_WIDTH) && m_cells[ix-ARY_WIDTH] == EMPTY:
					m_eval[ix] += 10
# ランダム着手によるプレイアウト
# return 
# return 勝者 BLUE or RED
func playout_random(next, x, y) -> int:		# 次を (x, y) に打ち、その後はランダム
	var b2 = Board.new()
	b2.copy_from(self)
	if b2.put_col(x, y, next):
		return next
	while true:
		next = (BLUE + RED) - next
		var mv = b2.sel_move_random()
		if b2.put_col(mv.x, mv.y, next):
			return next
	return 0
func sel_move_PMC(next) -> Vector2:			# 純粋モンテカルロ法による着手選択
	var wc = PackedByteArray()
	wc.resize(ARY_SIZE)
	wc.fill(0)
	const CNT = 100
	for y in range(N_HORZ):
		for x in range(N_HORZ):
			var ix = xyToIndex(x, y)
			if m_cells[ix] == EMPTY:
				for i in range(CNT):
					if playout_random(next, x, y) == next:
						wc[ix] += 1
	print("wc[] = ")
	for y in range(N_HORZ):
		var txt = ""
		for x in range(N_HORZ):
			txt += " %3d%%"%(wc[xyToIndex(x, y)]*100/CNT)
		print(txt)
	var mxc = 0
	var v2 = Vector2(-1, -1)
	for y in range(N_HORZ):
		for x in range(N_HORZ):
			var ix = xyToIndex(x, y)
			if wc[ix] > mxc:
				mxc = wc[ix]
				v2 = Vector2(x, y)
	return v2
func sel_move_MCTS(next):
	var mcts = MCTS.new(self, next)
	#mcts.add_children()
	var mv = mcts.do_search(1000)
	win_rate = mcts.win_rate
	#mcts.print()
	mcts.print_top_children()
	mcts.print_policy()
	policy_text = mcts.policy_text
	return mv
func _ready():
	pass # Replace with function body.
func _process(delta):
	pass
